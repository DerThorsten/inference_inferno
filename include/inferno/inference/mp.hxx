/** \file mp.hxx 
    \brief  loop-bp is implemented here

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_MP_HXX
#define INFERNO_INFERENCE_MP_HXX

#include <functional>

#include "boost/format.hpp"
#include "inferno/inferno.hxx"
#include "inferno/utilities/delegate.hxx"
#include "inferno/utilities/parallel/pool.hxx"
#include "inferno/inference/discrete_inference_base.hxx"
#include "inferno/inference/utilities/movemaker.hxx"
#include "inferno/model/factors_of_variables.hxx"


namespace inferno{


namespace inference{


    struct Msg{
        ValueType * msg_;
        ValueType * oMsg_;
    };

    template<class MODEL>
    class MessageStoring{
    public:
        typedef MODEL Model;
        typedef typename Model::FactorDescriptor FactorDescriptor;
        typedef typename Model::VariableDescriptor VariableDescriptor;
        typedef typename MODEL:: template FactorMap<uint64_t> FacToVarMsgOffset;
        typedef typename MODEL:: template VariableMap<uint64_t>   VarToFacMsgOffset;
        typedef typename MODEL:: template FactorMap< std::array<uint32_t,2> >   ReverseMsgIndex;
     

        MessageStoring(const Model & model, const models::HigherOrderAndUnaryFactorsOfVariables<Model> & factorsOfVariables)
        :   model_(model),
            factorsOfVariables_(factorsOfVariables),
            msgPtrs_(),
            facToVarOffset_(model,std::numeric_limits<uint64_t>::max()),
            varToFacOffset_(model,std::numeric_limits<uint64_t>::max()),
            nLabels_()
        {
            const bool simpleLabelSpace = model_.hasSimpleLabelSpace(nLabels_);


            uint64_t nFacToVar = 0;
            uint64_t msgSpace = 0;
            // count messages
            for(const auto factor : model_.factors()){
                const auto arity = factor->arity();
                if(arity>1){
                    nFacToVar +=arity;
                    msgSpace += factor->accumulateShape();
                }
            }

            msgStorage_.resize(2*msgSpace,0.0);
            msgPtrs_.reserve(2*nFacToVar);

            // setup offsets
            uint64_t vOffset = 0;
            uint64_t mIndex = 0;

            for(auto varDesc: model_.variableDescriptors()){              
                const DiscreteLabel nl = model_.nLabels(varDesc);
                const auto & facsOfVar = factorsOfVariables_[varDesc];
                varToFacOffset_[varDesc] = mIndex;
                for(auto facDesc : facsOfVar.higherOrderFactors()){
                    const auto factor = model_[facDesc];
                    if(factor->arity()>1){
                        msgPtrs_.push_back(Msg{msgStorage_.data()+ vOffset , NULL});
                        vOffset += nl;
                        ++mIndex;
                    }
                }
            }

            for(const auto fDesc : model_.factorDescriptors()){
                const auto factor = model_[fDesc];
                const auto arity = factor->arity();
                if(arity>1){
                    // set vOffset
                    for(auto a=0; a<arity; ++a){

                        // find out at which position fDesc is in hfacs
                        const VariableDescriptor var = factor->variable(a);
                        const DiscreteLabel nl = model_.nLabels(var);
                        const auto  & hfacs = factorsOfVariables_[var].higherOrderFactors();
                        const auto pos = std::distance(hfacs.begin(), hfacs.find(fDesc));

                        auto  & varToFacMsgHolder = msgPtrs_[varToFacOffset_[var]+pos];

                        const auto facToVarMsgHolder  = Msg{msgStorage_.data()+ vOffset, varToFacMsgHolder.msg_};
                        varToFacMsgHolder.oMsg_ = facToVarMsgHolder.msg_;
                        msgPtrs_.push_back(facToVarMsgHolder);
                        vOffset += nl;
                    }
                    facToVarOffset_[fDesc] = mIndex;
                    mIndex += arity; 
                }
            }
        }

        ValueType * facToVarMsg(const FactorDescriptor facDesc, const size_t mi){
            const uint64_t offset = facToVarOffset_[facDesc];
            return msgPtrs_[offset + mi].msg_;
        }

        ValueType * oppToFacToVarMsg(const FactorDescriptor facDesc, const size_t mi){
            const uint64_t offset = facToVarOffset_[facDesc];
            return msgPtrs_[offset + mi].oMsg_;
        }

        ValueType * varToFacMsg(const VariableDescriptor var, const size_t mi){
            const uint64_t offset = varToFacOffset_[var];
            return msgPtrs_[offset + mi].msg_;
        }

        ValueType * oppToVarToFacMsg(const VariableDescriptor var, const size_t mi){
            const uint64_t offset = varToFacOffset_[var];
            return msgPtrs_[offset + mi].oMsg_;
        }

        DiscreteLabel nLabels()const{
            return nLabels_;
        }
        uint64_t nMsg()const{
            return msgPtrs_.size();
        }

    private:
        const Model model_;
        const models::HigherOrderAndUnaryFactorsOfVariables<Model> factorsOfVariables_;
        std::vector<Msg> msgPtrs_;

        FacToVarMsgOffset facToVarOffset_;
        VarToFacMsgOffset varToFacOffset_;
        std::vector<ValueType> msgStorage_;
        DiscreteLabel nLabels_;
        // uint64_t lastVarOffset_;
        
    };


    
    template<class MODEL>
    class MessagePassing  : public DiscreteInferenceBase<MODEL> {
    public:
        typedef MODEL Model;
        typedef typename Model::FactorDescriptor FactorDescriptor;
        typedef typename Model::VariableDescriptor VariableDescriptor;
        typedef MessagePassing<MODEL> Self;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor Visitor;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;
        typedef typename MODEL:: template VariableMap<uint8_t> UIn8VarMap;
    private:
        typedef MessageStoring<MODEL> MsgStorage;
        typedef typename MODEL::FactorIdIter FactorIdIter;
        typedef typename MODEL::VariableIdIter VariableIdIter;
        typedef typename std::iterator_traits<FactorIdIter>::iterator_category FactorIdIterCategory;
        typedef typename std::iterator_traits<VariableIdIter>::iterator_category VariableIdIterCategory;
    public:
        struct Options{
            Options(           
            ){
                nSteps_ = 1000;
                damping_ = 0.95;
                eps_ = 1.0e-07 ;
                saveMem_ = false;
                nThreads_ = 0;     
            }
            ValueType damping_;
            uint64_t nSteps_;
            ValueType eps_;
            uint64_t nThreads_;
            bool saveMem_;
        };


 

        MessagePassing(const Model & model, const Options & options = Options())
        :   BaseInf(),
            model_(model),
            options_(options),
            factorsOfVariables_(model),
            msg_(model,factorsOfVariables_),
            maxNumLabels_(),
            sMsgBuffer_(),
            conf_(model),
            stopInference_(false),
            pool_(options_.nThreads_ == 0 ? std::thread::hardware_concurrency() : options_.nThreads_),
            nThreads_(options_.nThreads_ == 0 ? std::thread::hardware_concurrency() : options_.nThreads_)
        {
            DiscreteLabel minNumLabels;
            model_.minMaxNLabels(minNumLabels, maxNumLabels_);


            sMsgBuffer_.resize(maxNumLabels_*nThreads_);

            for(const auto var : model_.variableDescriptors())
                conf_[var]  = 0;
        }


        // MUST HAVE INTERACE
        virtual std::string name() const {
            return "MessagePassing";
        }
        // inference
        virtual void infer( Visitor  * visitor  = NULL) {

            if(visitor!=NULL)
                visitor->begin(this);

            // outer loop
            for(size_t i=0; i<options_.nSteps_; ++i){

                // reset epsilon for convergence 
                eps_ = 0;
                sendAllFacToVar();
                sendAllVarToFac();
                eps_ /= (msg_.nMsg()/2);

                // call visitor 
                if(visitor!=NULL){
                    visitor->logging(this,std::string("convergence"), eps_);
                    visitor->visit(this);
                }
                // check for termination (by visitors)
                if(stopInference_)
                    break;

                // check for termination by convergence
                if(eps_ < options_.eps_)
                    break;
            }

            if(visitor!=NULL)
                visitor->end(this);
        }



        // send varToFac parallel for
        // random access iterator
        void sendAllVarToFac(){
            std::vector<ValueType> sum(nThreads_, 0.0);
            utilities::parallel_foreach(pool_, model_.nVariables(), 
                model_.variableDescriptorsBegin(), model_.variableDescriptorsEnd(),
                [this, &sum] (int id,VariableDescriptor var) {
                    sum[id] += this->sendVarToFac(this->model().variableId(var),id);
                }
            );

            eps_ = 0;
            for(const auto s : sum){
                eps_+=s;
            }
        }
     
        // send facToVar parallel for
        // NON random access iterator
        void sendAllFacToVar(){
            utilities::parallel_foreach(pool_, model_.nFactors(),
                model_.factorDescriptorsBegin(),model_.factorDescriptorsEnd(),
                [this] (int id, FactorDescriptor fDesc) {
                    const auto facDesc  = this->model().factorId(fDesc);
                    this->sendFacToVar(facDesc);
                }
            );  
        }

        void sendFacToVar(const FactorDescriptor facDesc){
            const auto factor = model_[facDesc];
            const auto arity  = factor->arity();

            #define SEND_FAC_TO_VAR_MS(ARITY) \
                ValueType * facToVar[ARITY]; \
                const ValueType * varToFac[ARITY]; \
                for(auto i=0; i<ARITY; ++i){ \
                    facToVar[i] = msg_.facToVarMsg(facDesc, i); \
                    varToFac[i] = msg_.oppToFacToVarMsg(facDesc, i); \
                } \
                factor->facToVarMsg(varToFac, facToVar); \
                break

            switch(arity){
                case 1 : {break;}
                case 2 : {
                    ValueType * facToVar[2] = {msg_.facToVarMsg(facDesc, 0), msg_.facToVarMsg(facDesc, 1)};
                    const ValueType * varToFac[2] = {msg_.oppToFacToVarMsg(facDesc, 0), msg_.oppToFacToVarMsg(facDesc, 1)}; 
                    factor->facToVarMsg(varToFac, facToVar);
                    break;
                }
                case  3 : {SEND_FAC_TO_VAR_MS( 3);}
                case  4 : {SEND_FAC_TO_VAR_MS( 4);}
                case  5 : {SEND_FAC_TO_VAR_MS( 5);}
                case  6 : {SEND_FAC_TO_VAR_MS( 6);}
                case  7 : {SEND_FAC_TO_VAR_MS( 7);}
                case  8 : {SEND_FAC_TO_VAR_MS( 8);}
                case  9 : {SEND_FAC_TO_VAR_MS( 9);}
                case 10 : {SEND_FAC_TO_VAR_MS(10);}
                case 11 : {SEND_FAC_TO_VAR_MS(11);}
                case 12 : {SEND_FAC_TO_VAR_MS(12);}
                case 13 : {SEND_FAC_TO_VAR_MS(13);}
                case 14 : {SEND_FAC_TO_VAR_MS(14);}
                case 15 : {SEND_FAC_TO_VAR_MS(15);}
                case 16 : {SEND_FAC_TO_VAR_MS(16);}
                default : {
                    std::vector<ValueType *>       facToVar(arity);
                    std::vector<const ValueType *> varToFac(arity);
                    for(auto i=0; i<arity; ++i){ 
                        facToVar[i] = msg_.facToVarMsg(facDesc, i); 
                        varToFac[i] = msg_.oppToFacToVarMsg(facDesc, i); 
                    } 
                    factor->facToVarMsg(varToFac.data(), facToVar.data()); 
                    break;
                }
            }
            #undef SEND_FAC_TO_VAR_MS
        }

        ValueType sendVarToFac(const Vi vi, const int tid){
            
            auto buffer  =  sMsgBuffer_.data() +  (options_.nThreads_!= 1 ? tid*maxNumLabels_ : 0);
            ValueType msgSquaredDiff = 0.0;

            // how many labels
            const DiscreteLabel nl = model_.nLabels(vi);

            // how many higher order factors
            // has this variable (higher order includes order = 2)
            const auto & uFacs = factorsOfVariables_[vi].unaryFactors();
            const auto & hoFacs = factorsOfVariables_[vi].higherOrderFactors();

            if(uFacs.size() + hoFacs.size() > 0 ){

                // fill buffer
                std::fill(buffer, buffer + nl, 0.0);

                // unary factors
                for(const auto facDesc : uFacs)
                    model_[facDesc]->addToBuffer(buffer);

                // nHigher order factors
                const auto nHo = hoFacs.size();
                for(size_t i=0; i<nHo; ++i){
                    // get the factor msg
                    const auto facToVar = msg_.oppToVarToFacMsg(vi,i);
                    for(DiscreteLabel l=0; l<nl; ++l)
                        buffer[l] +=facToVar[l]; 
                }

                // all msg are summed up now
                // therefore sMsgBuffer is 
                // the actual belief vector
                conf_[vi] = std::distance(buffer, std::min_element(buffer,buffer+nl));
                
                // compute outgoing messages
                // - we subtract a single varToFac msg 
                //   from belief vector 
                for(size_t i=0; i<nHo; ++i){
                    // get the factor msg
                    const auto facToVar = msg_.oppToVarToFacMsg(vi,i);
                    auto varToFac = msg_.varToFacMsg(vi,i);

                    // calculate the mean of the new message
                    ValueType mean = 0.0;
                    for(DiscreteLabel l=0; l<nl; ++l){
                        mean += (buffer[l] - facToVar[l]);
                    }
                    mean/=nl;

                    // substract the mean and damp
                    for(DiscreteLabel l=0; l<nl; ++l){
                        const ValueType oldValue = varToFac[l];
                        const ValueType newUndampedValue = (buffer[l] - facToVar[l]) - mean;
                        const ValueType newDampedValue = options_.damping_ * oldValue + (1.0 - options_.damping_)*newUndampedValue;
                        varToFac[l] = newDampedValue;

                        // convergence accumulation
                        const ValueType diff = oldValue-newDampedValue;
                        msgSquaredDiff += diff*diff;
                    }
                }
            }
            return msgSquaredDiff;
        }



        // get result
        virtual void conf(Conf & confMap ) {
            confMap = conf_;
        }
        virtual DiscreteLabel label(const Vi vi) {
            return conf_[vi];
        }
        // get model
        virtual const Model & model() const{
            return model_;
        }
        // stop inference (via visitor)
        virtual void stopInference(){
            stopInference_ = true;
        }

        // OPTIONAL INTERFACE
        // warm start related (do nothing default)
        virtual void setConf(const Conf & conf){
            
        }

        // get results optional interface
        virtual ValueType upperBound(){
            return model_.eval(conf_);
        }
        virtual ValueType energy(){
            return model_.eval(conf_);
        }

    private:
        const Model & model_;
        Options options_;
        models::HigherOrderAndUnaryFactorsOfVariables<Model> factorsOfVariables_;
        MsgStorage msg_;
        DiscreteLabel maxNumLabels_;
        std::vector<ValueType> sMsgBuffer_;
        Conf conf_;
        ValueType eps_;
        bool stopInference_;
        utilities::ThreadPool pool_;
        size_t nThreads_;
    };
  
    

}
}


#endif /* INFERNO_INFERENCE_MP_HXX */
