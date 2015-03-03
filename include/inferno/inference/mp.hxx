/** \file base_discrete_inference.hxx 
    \brief  Functionality which is shared between all inference algorithms
    is implemented in this header.

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_MP_HXX
#define INFERNO_INFERENCE_MP_HXX

#include "boost/format.hpp"

#include "inferno/inferno.hxx"
#include "inferno/utilities/delegate.hxx"
#include "inferno/inference/base_discrete_inference.hxx"
#include "inferno/inference/utilities/movemaker.hxx"
#include "inferno/model/factors_of_variables.hxx"
namespace inferno{


namespace inference{


    struct Msg{
        ValueType * msg_;
        ValueType * oMsg_;
    };

    template<class MODEL>
    class SimpleMessageStoring{
    public:
        typedef MODEL Model;
        typedef typename MODEL:: template FactorMap<uint64_t> FacToVarMsgOffset;
        typedef typename MODEL:: template VariableMap<uint64_t>   VarToFacMsgOffset;
        typedef typename MODEL:: template FactorMap< std::array<uint32_t,2> >   ReverseMsgIndex;

        typedef typename MODEL:: template VariableMap< VectorSet<Fi>  >   VarsHoFacs;        

        SimpleMessageStoring(const Model & model, const HigherOrderAndUnaryFactorsOfVariables<Model> & factorsOfVariables)
        :   model_(model),
            factorsOfVariables_(factorsOfVariables),
            msgPtrs_(),
            facToVarOffset_(model,std::numeric_limits<uint64_t>::max()),
            varToFacOffset_(model,std::numeric_limits<uint64_t>::max()),
            nLabels_()
        {
            INFERNO_CHECK(model_.hasSimpleLabelSpace(nLabels_),"only for simple label spaces");
            INFERNO_CHECK_OP(model_.maxArity(),==,2,"only for 2.-order models");

            uint64_t nFacToVar = 0;
            // count messages
            for(const auto fi : model_.factorIds()){
                const auto factor = model_[fi];
                const auto arity = factor->arity();
                if(arity>1){
                    nFacToVar +=arity;
                }
            }
            const uint64_t nMsg = 2*nFacToVar;

            //std::cout<<" allocating space : " << (nMsg*nLabels_*sizeof(ValueType)*8)/(1024.0 * 1024.0) <<" MB \n";
            msgStorage_.resize(nMsg*nLabels_,0.0);
            msgPtrs_.reserve(nMsg);

            // setup offsets
            uint64_t vOffset = 0;
            uint64_t mIndex = 0;

            for(auto vi: model_.variableIds()){              
                const auto & facsOfVar = factorsOfVariables_[vi];
                varToFacOffset_[vi] = mIndex;
                size_t nMsgOut = 0;
                for(auto fi : facsOfVar.higherOrderFactors()){
                    const auto factor = model_[fi];
                    if(factor->arity()>1){
                        msgPtrs_.push_back(Msg{msgStorage_.data()+ vOffset +nMsgOut*nLabels_, NULL});
                        ++nMsgOut;
                        ++mIndex;
                    }
                }
                vOffset += nMsgOut*nLabels_;
            }

            for(const auto fi : model_.factorIds()){
                const auto factor = model_[fi];
                const auto arity = factor->arity();
                if(arity>1){
                    // set vOffset
                    for(auto a=0; a<arity; ++a){

                        // find out at which position fi is in hfacs
                        const Vi vi = factor->vi(a);
                        const auto  & hfacs = factorsOfVariables_[vi].higherOrderFactors();
                        INFERNO_CHECK(hfacs.find(fi)!=hfacs.end(),"");
                        const auto pos = std::distance(hfacs.begin(), hfacs.find(fi));

                        // get the message at pos  
                        auto  & varToFacMsgHolder = msgPtrs_[varToFacOffset_[vi]+pos];

                        //INFERNO_CHECK_OP(hfacs[pos], ==, fi, "internal error");

                        const auto facToVarMsgHolder  = Msg{msgStorage_.data()+ vOffset +a*nLabels_, varToFacMsgHolder.msg_};
                        varToFacMsgHolder.oMsg_ = facToVarMsgHolder.msg_;
                        msgPtrs_.push_back(facToVarMsgHolder);
                    }
                    facToVarOffset_[fi] = mIndex;
                    mIndex += arity;
                    vOffset += arity*nLabels_;

                }
            }

            //INFERNO_CHECK_OP(vOffset,==,msgStorage_.size(),"hups");
        }

        // uint64_t nVarToFac(const Vi vi)const{
        //     return factorsOfVariables_[vi].higherOrderFactors().size();
        // }

        ValueType * facToVarMsg(const Fi fi, const size_t mi){
            const uint64_t offset = facToVarOffset_[fi];
            INFERNO_CHECK(msgPtrs_[offset + mi].msg_!=NULL, "");
            return msgPtrs_[offset + mi].msg_;
        }

        ValueType * oppositeToFacToVarMsg(const Fi fi, const size_t mi){
            const uint64_t offset = facToVarOffset_[fi];
            INFERNO_CHECK(msgPtrs_[offset + mi].oMsg_!=NULL, "");
            return msgPtrs_[offset + mi].oMsg_;
        }


        ValueType * varToFacMsg(const Vi vi, const size_t mi){
            const uint64_t offset = varToFacOffset_[vi];
            INFERNO_CHECK(msgPtrs_[offset + mi].msg_!=NULL, "");
            return msgPtrs_[offset + mi].msg_;
        }

        ValueType * opposideToVarToFacMsg(const Vi vi, const size_t mi){
            const uint64_t offset = varToFacOffset_[vi];
            INFERNO_CHECK(msgPtrs_[offset + mi].oMsg_!=NULL, "");
            return msgPtrs_[offset + mi].oMsg_;
        }

        DiscreteLabel nLabels()const{
            return nLabels_;
        }

    private:
        const Model model_;
        const HigherOrderAndUnaryFactorsOfVariables<Model> factorsOfVariables_;
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
        typedef MessagePassing<MODEL> Self;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor Visitor;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;
        typedef typename MODEL:: template VariableMap<uint8_t> UIn8VarMap;

        typedef SimpleMessageStoring<MODEL> MsgStorage;

        struct Options{
            Options(){
                damping_  = 0.95;
                nSteps_  = 1000;
            }
            Options(const InferenceOptions & options)
            {
                if(options.checkOptions()){
                    auto keys = options.keys();
                    options.getOption("damping", 0.95, damping_,keys);
                    options.getOption("nSteps", 1000, nSteps_,keys);
                    options. template checkForLeftovers<Self>(keys);
                }
                else{
                    options.getOption("damping", 0.95, damping_);
                    options.getOption("nSteps", 1000, nSteps_);
                }
            }
            ValueType damping_;
            uint64_t nSteps_;
        };

        static void defaultOptions(InferenceOptions & options){
            Options opt;
            options.set("damping",opt.damping_);
            options.set("nSteps",opt.nSteps_);
        }

 

        MessagePassing(const Model & model, const InferenceOptions & infParam = InferenceOptions())
        :   BaseInf(),
            model_(model),
            options_(infParam),
            factorsOfVariables_(model),
            msg_(model,factorsOfVariables_),
            nLabels_(),
            sMsgBuffer_(),
            sBelBuffer_(),
            conf_(model),
            stopInference_(false)
        {

            nLabels_ = msg_.nLabels();
            sMsgBuffer_.resize(nLabels_);
            sBelBuffer_.resize(nLabels_);
            for(const auto vi : model_.variableIds())
                conf_[vi]  = 0;
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
                convergence_ = 0;
                // factor to variable message
                //std::cout<<"send fac to var \n";
                for(const auto fi : model_.factorIds()){
                    //std::cout<<"    -fi "<<fi<<"\n";
                    sendFacToVar(fi);
                }

                //std::cout<<"send var to fac \n";
                // variable to factor message
                for(const auto vi : model_.variableIds()){
                    sendVarToFac(vi);
                }
                //std::cout<<"convergence  "<<convergence_<<"\n";
                if(visitor!=NULL)
                    visitor->visit(this);
            }


            if(visitor!=NULL)
                visitor->end(this);
        }

        void sendFacToVar(const Fi fi){
            const auto factor = model_[fi];
            if(factor->arity()>1){
                ValueType * facToVar[2] ={
                    msg_.facToVarMsg(fi, 0),
                    msg_.facToVarMsg(fi, 1)
                };
                const ValueType * varToFac[2] ={
                    msg_.oppositeToFacToVarMsg(fi, 0),
                    msg_.oppositeToFacToVarMsg(fi, 1)
                }; 
                factor->facToVarMsg(varToFac, facToVar);
            }
        }
        void sendVarToFac(const Vi vi){
            
            // how many higher order factors
            // has this variable (higher order includes order = 2)
            const auto & uFacs = factorsOfVariables_[vi].unaryFactors();
            const auto & hoFacs = factorsOfVariables_[vi].higherOrderFactors();

            if(uFacs.size() + hoFacs.size() > 0 ){

                // fill buffer
                std::fill(sMsgBuffer_.begin(), sMsgBuffer_.end(), 0.0);

                // unary factors
                for(const auto fi : uFacs)
                    model_[fi]->addToBuffer(sMsgBuffer_.data());

                // nHigher order factors
                const auto nHo = hoFacs.size();
                for(size_t i=0; i<nHo; ++i){
                    // get the factor msg
                    const auto facToVar = msg_.opposideToVarToFacMsg(vi,i);
                    for(DiscreteLabel l=0; l<nLabels_; ++l)
                        sMsgBuffer_[l] +=facToVar[l]; 
                }

                // all msg are summed up now
                // therefore sMsgBuffer is 
                // the actual belief vector
                const auto & belief  = sMsgBuffer_;
                auto bBegin = belief.begin();
                conf_[vi] = std::distance( bBegin,std::min_element(bBegin,belief.end()));
                
                // compute outgoing messages
                // - we subtract a single varToFac msg 
                //   from belief vector 
                for(size_t i=0; i<nHo; ++i){
                    // get the factor msg
                    const auto facToVar = msg_.opposideToVarToFacMsg(vi,i);
                    auto varToFac = msg_.varToFacMsg(vi,i);

                    // calculate the mean of the new message
                    ValueType mean = 0.0;
                    for(DiscreteLabel l=0; l<nLabels_; ++l){
                        mean += (belief[l] - facToVar[l]);
                    }
                    mean/=nLabels_;

                    // substract the mean and damp
                    for(DiscreteLabel l=0; l<nLabels_; ++l){
                        const ValueType oldValue = varToFac[l];
                        const ValueType newUndampedValue = (belief[l] - facToVar[l]) - mean;
                        const ValueType newDampedValue = options_.damping_ * oldValue + (1.0 - options_.damping_)*newUndampedValue;
                        varToFac[l] = newDampedValue;

                        // convergence accumulation
                        const ValueType diff = oldValue-newDampedValue;
                        convergence_ += diff*diff;
                    }
                }
            }

            

            /*
            const auto facsOfVar = factorsOfVariables_[vi];
            std::fill(sMsgBuffer_.begin(), sMsgBuffer_.end(), 0.0);
            for(const auto fi : facsOfVar){
                const auto factor = model_[fi];
                const auto arity = factor->arity();

                if(arity==1){
                    factor->addToBuffer(sMsgBuffer_.data());
                }
                else if(arity>1){
                    const auto vi0 = factor->vi(0);
                    const auto vi1 = factor->vi(1);
                    //INFERNO_CHECK(vi0==vi || vi1==vi,"");
                    const ValueType * m = msg_.facToVarMsg(fi, vi0 == vi ? 0 : 1);
                    for(DiscreteLabel l=0; l<nLabels_; ++l){
                        sMsgBuffer_[l] += m[l];
                    }
                }
            }
            auto msgIndex = 0;

            // sum is tidy
            ValueType minVal = std::numeric_limits<ValueType>::infinity();
            DiscreteLabel minLabel = 0;
            for(DiscreteLabel l=0; l<nLabels_; ++l){
                if(sMsgBuffer_[l]<minVal){
                    minVal = sMsgBuffer_[l];
                    minLabel = l;
                }
            }
            conf_[vi] = minLabel;

            for(const auto fi : facsOfVar){
                const auto factor = model_[fi];
                const auto arity = factor->arity();
                if(arity>1){
                    const auto vi0 = factor->vi(0);
                    const auto vi1 = factor->vi(1);
                    //INFERNO_CHECK(vi0==vi || vi1==vi,"");
                    const ValueType * ftv = msg_.facToVarMsg(fi, vi0 == vi ? 0 : 1);
                    ValueType  * vtf = msg_.varToFacMsg(vi, msgIndex);
                    ValueType mean = 0;
                    for(DiscreteLabel l=0; l<nLabels_; ++l){
                        mean += (sMsgBuffer_[l]  -ftv[l]);
                    }
                    //INFERNO_CHECK_NUMBER(mean);
                    mean/=nLabels_;
                    //INFERNO_CHECK_NUMBER(mean);

                    for(DiscreteLabel l=0; l<nLabels_; ++l){
                        const ValueType oldValue = vtf[l];
                        const ValueType newVal  = (sMsgBuffer_[l]  -ftv[l]) - mean;
                        //INFERNO_CHECK_NUMBER(oldValue);
                        //INFERNO_CHECK_NUMBER(newVal);
                        const ValueType diff = oldValue-newVal;
                        convergence_ += diff*diff;
                        //INFERNO_CHECK_NUMBER(NAN);
                        //INFERNO_CHECK_NUMBER(convergence_);
                        //INFERNO_CHECK_NUMBER(options_.damping_);
                        vtf[l] = options_.damping_ * oldValue + (1.0 - options_.damping_)*newVal;
                    }
                    ++msgIndex;
                }
            }
            */
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
        HigherOrderAndUnaryFactorsOfVariables<Model> factorsOfVariables_;
        MsgStorage msg_;
        DiscreteLabel nLabels_;
        std::vector<ValueType> sMsgBuffer_;
        std::vector<ValueType> sBelBuffer_;
        Conf conf_;
        ValueType convergence_;
        bool stopInference_;
    };
  
    

}
}


#endif /* INFERNO_INFERENCE_MP_HXX */
