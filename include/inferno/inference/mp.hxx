/** \file base_discrete_inference.hxx 
    \brief  Functionality which is shared between all inference algorithms
    is implemented in this header.

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_MP_HXX
#define INFERNO_INFERENCE_MP_HXX

#include "boost/format.hpp"

#ifdef WITH_OPENMP
#include <omp.h>
#endif

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
            //INFERNO_CHECK_OP(model_.maxArity(),==,2,"only for 2.-order models");

            const bool simpleLabelSpace = model_.hasSimpleLabelSpace(nLabels_);


            uint64_t nFacToVar = 0;
            uint64_t msgSpace = 0;
            // count messages
            for(const auto fi : model_.factorIds()){
                const auto factor = model_[fi];
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

            for(auto vi: model_.variableIds()){              
                const DiscreteLabel nl = model_.nLabels(vi);
                const auto & facsOfVar = factorsOfVariables_[vi];
                varToFacOffset_[vi] = mIndex;
                for(auto fi : facsOfVar.higherOrderFactors()){
                    const auto factor = model_[fi];
                    if(factor->arity()>1){
                        msgPtrs_.push_back(Msg{msgStorage_.data()+ vOffset , NULL});
                        vOffset += nl;
                        ++mIndex;
                    }
                }
            }

            for(const auto fi : model_.factorIds()){
                const auto factor = model_[fi];
                const auto arity = factor->arity();
                if(arity>1){
                    // set vOffset
                    for(auto a=0; a<arity; ++a){

                        // find out at which position fi is in hfacs
                        const Vi vi = factor->vi(a);
                        const DiscreteLabel nl = model_.nLabels(vi);
                        const auto  & hfacs = factorsOfVariables_[vi].higherOrderFactors();
                        const auto pos = std::distance(hfacs.begin(), hfacs.find(fi));

                        auto  & varToFacMsgHolder = msgPtrs_[varToFacOffset_[vi]+pos];

                        const auto facToVarMsgHolder  = Msg{msgStorage_.data()+ vOffset, varToFacMsgHolder.msg_};
                        varToFacMsgHolder.oMsg_ = facToVarMsgHolder.msg_;
                        msgPtrs_.push_back(facToVarMsgHolder);
                        vOffset += nl;
                    }
                    facToVarOffset_[fi] = mIndex;
                    mIndex += arity; 
                }
            }
        }

        ValueType * facToVarMsg(const Fi fi, const size_t mi){
            const uint64_t offset = facToVarOffset_[fi];
            INFERNO_CHECK(msgPtrs_[offset + mi].msg_!=NULL, "");
            return msgPtrs_[offset + mi].msg_;
        }

        ValueType * oppToFacToVarMsg(const Fi fi, const size_t mi){
            const uint64_t offset = facToVarOffset_[fi];
            INFERNO_CHECK(msgPtrs_[offset + mi].oMsg_!=NULL, "");
            return msgPtrs_[offset + mi].oMsg_;
        }


        ValueType * varToFacMsg(const Vi vi, const size_t mi){
            const uint64_t offset = varToFacOffset_[vi];
            INFERNO_CHECK(msgPtrs_[offset + mi].msg_!=NULL, "");
            return msgPtrs_[offset + mi].msg_;
        }

        ValueType * oppToVarToFacMsg(const Vi vi, const size_t mi){
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
        typedef typename std::iterator_traits<typename MODEL::FactorIdIter>::iterator_category FactorIdIterCategory;
        typedef typename std::iterator_traits<typename MODEL::VariableIdIter>::iterator_category VariableIdIterCategory;

        struct Options{
            Options(){
                damping_  = 0.95;
                nSteps_  = 1000;
                nThreads_ = 0; 
            }
            Options(const InferenceOptions & options)
            {
                if(options.checkOptions()){
                    auto keys = options.keys();
                    options.getOption("damping", 0.95, damping_, keys);
                    options.getOption("nSteps", 1000, nSteps_, keys);
                    options.getOption("nThreads", 0, nThreads_, keys);
                    options. template checkForLeftovers<Self>(keys);
                }
                else{
                    options.getOption("damping", 0.95, damping_);
                    options.getOption("nSteps", 1000, nSteps_);
                    options.getOption("nThreads", 0, nThreads_);
                }
            }
            ValueType damping_;
            uint64_t nSteps_;
            uint64_t nThreads_;
        };

        static void defaultOptions(InferenceOptions & options){
            Options opt;
            options.set("damping",opt.damping_);
            options.set("nSteps",opt.nSteps_);
            options.set("nThreads",opt.nThreads_);
        }

 

        MessagePassing(const Model & model, const InferenceOptions & infParam = InferenceOptions())
        :   BaseInf(),
            model_(model),
            options_(infParam),
            factorsOfVariables_(model),
            msg_(model,factorsOfVariables_),
            maxNumLabels_(),
            sMsgBuffer_(),
            sBelBuffer_(),
            conf_(model),
            stopInference_(false)
        {
            DiscreteLabel minNumLabels;
            model_.minMaxNLabels(minNumLabels, maxNumLabels_);

            #ifdef WITH_OPENMP
                if(options_.nThreads_!=1){
                    if(options_.nThreads_!=0)
                        omp_set_num_threads(options_.nThreads_);
                    size_t usedThreads = omp_get_max_threads(); 
                    sMsgBuffer_.resize(maxNumLabels_*usedThreads);
                }
                else{
                    sMsgBuffer_.resize(maxNumLabels_);
                }
            #else 
                sMsgBuffer_.resize(maxNumLabels_);
            #endif


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

                // SEND FAC TO VAR
                if(options_.nThreads_ == 1){
                    sendFacToVarNonParallel();
                }
                else{
                    #ifdef WITH_OPENMP
                    sendFacToVarParallel(FactorIdIterCategory());
                    #else 
                    sendFacToVarNonParallel();
                    #endif
                }

                // SEND VAR TO FAC
                if(options_.nThreads_ == 1){
                    sendVarToFacNonParallel();
                }
                else{
                    #ifdef WITH_OPENMP
                    sendVarToFacParallel(VariableIdIterCategory());
                    #else 
                    sendVarToFacNonParallel();
                    #endif
                }


                if(visitor!=NULL)
                    visitor->visit(this);
            }

            if(visitor!=NULL)
                visitor->end(this);
        }


        template<class ITER_TAG>
        void sendVarToFacParallel(const ITER_TAG tag){
            ValueType sum = 0.0;
            #pragma omp parallel  reduction(+:sum)
            {
                for(auto varIter = model_.variableIdsBegin(); varIter< model_.variableIdsEnd(); ++varIter){
                    #pragma omp single nowait 
                    {
                        sum += sendVarToFac(*varIter);
                    }
                }
            }
            convergence_ = sum;
        }
        void sendVarToFacParallel(const std::random_access_iterator_tag tag){
            ValueType sum = 0.0;
            #pragma omp parallel for reduction(+:sum)
                for(auto varIter = model_.variableIdsBegin(); varIter< model_.variableIdsEnd(); ++varIter){
                    sum+=sendVarToFac(*varIter);
            }
            convergence_ = sum;
        }
        void sendVarToFacNonParallel(){
            ValueType sum = 0.0;
            for(auto varIter = model_.variableIdsBegin(); varIter< model_.variableIdsEnd(); ++varIter){
                sum += sendVarToFac(*varIter);
            }
            convergence_ = sum;
        }



        template<class ITER_TAG>
        void sendFacToVarParallel(const ITER_TAG tag){
            #pragma omp parallel
            {
                for(auto facIter = model_.factorIdsBegin(); facIter!=model_.factorIdsEnd(); ++facIter){
                    #pragma omp single nowait 
                    {
                        sendFacToVar(*facIter);
                    }
                }
            }
        }
        void sendFacToVarParallel(const std::random_access_iterator_tag tag){
            #pragma omp parallel for
            for(auto facIter = model_.factorIdsBegin(); facIter<model_.factorIdsEnd(); ++facIter){
                const auto fi = *facIter;
                sendFacToVar(fi);
            }
        }
        void sendFacToVarNonParallel(){
            for(auto facIter = model_.factorIdsBegin(); facIter<model_.factorIdsEnd(); ++facIter){
                const auto fi = *facIter;
                sendFacToVar(fi);
            }
        }



        void sendFacToVar(const Fi fi){
            const auto factor = model_[fi];
            const auto arity  = factor->arity();

            #define SEND_FAC_TO_VAR_MS(ARITY) \
                ValueType * facToVar[ARITY]; \
                const ValueType * varToFac[ARITY]; \
                for(auto i=0; i<ARITY; ++i){ \
                    facToVar[i] = msg_.facToVarMsg(fi, i); \
                    varToFac[i] = msg_.oppToFacToVarMsg(fi, i); \
                } \
                factor->facToVarMsg(varToFac, facToVar); \
                break

            switch(arity){
                case 1 : {break;}
                case 2 : {
                    ValueType * facToVar[2] = {msg_.facToVarMsg(fi, 0), msg_.facToVarMsg(fi, 1)};
                    const ValueType * varToFac[2] = {msg_.oppToFacToVarMsg(fi, 0), msg_.oppToFacToVarMsg(fi, 1)}; 
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
                        facToVar[i] = msg_.facToVarMsg(fi, i); 
                        varToFac[i] = msg_.oppToFacToVarMsg(fi, i); 
                    } 
                    factor->facToVarMsg(varToFac.data(), facToVar.data()); 
                    break;
                }

                #undef SEND_FAC_TO_VAR_MS
            }


            
        }
        ValueType sendVarToFac(const Vi vi){
            
            #ifdef WITH_OPENMP
                const auto tid = omp_get_thread_num();
                auto buffer  =  sMsgBuffer_.data() +  (options_.nThreads_!= 1 ? tid*maxNumLabels_ : 0);
            #else
                auto buffer  =  sMsgBuffer_.data();
            #endif

            ValueType res = 0.0;

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
                for(const auto fi : uFacs)
                    model_[fi]->addToBuffer(buffer);

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
                        res += diff*diff;
                    }
                }
            }
            return res;
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
        DiscreteLabel maxNumLabels_;
        std::vector<ValueType> sMsgBuffer_;
        std::vector<ValueType> sBelBuffer_;
        Conf conf_;
        ValueType convergence_;
        bool stopInference_;
    };
  
    

}
}


#endif /* INFERNO_INFERENCE_MP_HXX */
