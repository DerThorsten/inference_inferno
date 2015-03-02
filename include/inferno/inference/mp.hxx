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



    template<class MODEL>
    class SimpleMessageStoring{
    public:
        typedef MODEL Model;
        typedef typename MODEL:: template FactorMap<uint64_t> FacToVarMsgOffset;
        typedef typename MODEL:: template VariableMap<uint64_t>   VarToFacMsgOffset;

        typedef typename MODEL:: template FactorMap< std::array<uint32_t,2> >   ReverseMsgIndex;

        SimpleMessageStoring(const Model & model, const FactorsOfVariables<Model> & factorsOfVariables)
        :   model_(model),
            factorsOfVariables_(factorsOfVariables),
            facToVarOffset_(model,std::numeric_limits<uint64_t>::max()),
            varToFacOffset_(model,std::numeric_limits<uint64_t>::max()),
            reverseMsgIndex_(model),
            nLabels_()
        {
            INFERNO_CHECK(model_.hasSimpleLabelSpace(nLabels_),"only for simple label spaces");
            INFERNO_CHECK_OP(model_.maxArity(),==,2,"only for 2.-order models");

            uint64_t nFacToVar = 0;
            uint64_t facToVarSize = 0;

            uint64_t nVarToFac = 0;
            uint64_t varToFacSize = 0;

            // count messages
            for(const auto fi : model_.factorIds()){
                const auto factor = model_[fi];
                const auto arity = factor->arity();
                if(arity>1){
                    nFacToVar +=arity;
                    facToVarSize += arity*nLabels_;
                }
            }

            //std::cout<<"nFacToVar "<<nFacToVar<<" (size )"<<facToVarSize<<"\n";
            const uint64_t nMsg = 4*nFacToVar;
            std::cout<<" allocating space : " << (nMsg*nLabels_*sizeof(ValueType)*8)/(1024.0 * 1024.0) <<" MB \n";
            msgStorage_.resize(nMsg*nLabels_,0.0);

            // setup offsets
            uint64_t offset = 0;

            for(auto vi: model_.variableIds()){
           
                // calculate new offset
                size_t nMsgOut = 0;
                const auto facsOfVar = factorsOfVariables_[vi];
                INFERNO_CHECK_OP(facsOfVar.size(),>=,1,"");
                // set offset
                varToFacOffset_[vi] = offset;

                for(auto fi : facsOfVar){
                    const auto factor = model_[fi];
                    if(factor->arity()>1){
                        const auto vi0 = factor->vi(0);
                        const auto vi1 = factor->vi(1);
                        if(vi == vi0){
                            reverseMsgIndex_[fi][0] = nMsgOut;
                        }
                        else if(vi == vi1){
                            reverseMsgIndex_[fi][1] = nMsgOut;
                        }
                        else {
                            INFERNO_CHECK(false,"internal error");
                        }
                        ++nMsgOut;
                    }
                }
                offset += nMsgOut*2*nLabels_;
                //INFERNO_CHECK_OP(offset,<,msgStorage_.size(),"");
            }
            lastVarOffset_ = offset;
            for(const auto fi : model_.factorIds()){
                const auto factor = model_[fi];
                const auto arity = factor->arity();
                if(arity>1){
                    // set offset
                    facToVarOffset_[fi] = offset;
                    offset += arity*2*nLabels_;
                }
            }
            //INFERNO_CHECK_OP(offset,==,msgStorage_.size(),"hups");
        }

        ValueType * facToVarMsg(const Fi fi, const size_t mi, const uint8_t bi){
            //INFERNO_CHECK_OP(model_[fi]->arity(),==,2,"");
            const uint64_t offset = facToVarOffset_[fi];
            //INFERNO_CHECK_OP(offset,>=,lastVarOffset_,"");
            //INFERNO_CHECK_OP(offset,!=,std::numeric_limits<uint64_t>::max(),"");
            return msgStorage_.data() + offset + (2*nLabels_)*mi + bi*nLabels_;
        }

        ValueType * varToFacMsg(const Vi vi, const size_t mi, const uint8_t bi){
            const uint64_t offset = varToFacOffset_[vi];
            //INFERNO_CHECK_OP(offset,<,lastVarOffset_,"");
            //INFERNO_CHECK_OP(offset,!=,std::numeric_limits<uint64_t>::max(),"");
            return msgStorage_.data() + offset + (2*nLabels_)*mi + bi*nLabels_;
        }

        uint64_t factorsMi(const Fi fi, const uint32_t v)const{
            return reverseMsgIndex_[fi][v] ;
        }
        DiscreteLabel nLabels()const{
            return nLabels_;
        }

    private:
        const Model model_;
        const FactorsOfVariables<Model> factorsOfVariables_;
        FacToVarMsgOffset facToVarOffset_;
        VarToFacMsgOffset varToFacOffset_;
        ReverseMsgIndex reverseMsgIndex_;
        std::vector<ValueType> msgStorage_;
        DiscreteLabel nLabels_;
        uint64_t lastVarOffset_;
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
                options.getOption("damping", 0.95, damping_);
                options.getOption("nSteps", 1000, nSteps_);
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
            stopInference_(false){
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

            uint8_t bi = 0;
            // outer loop
            for(size_t i=0; i<options_.nSteps_; ++i){
                convergence_ = 0;
                // factor to variable message
                for(const auto fi : model_.factorIds()){
                    sendFacToVar(fi,bi);
                }

                // variable to factor message
                for(const auto vi : model_.variableIds()){
                    sendVarToFac(vi,bi);
                }
                //std::cout<<"convergence  "<<convergence_<<"\n";
                if(visitor!=NULL)
                    visitor->visit(this);
            }


            if(visitor!=NULL)
                visitor->end(this);
        }

        void sendFacToVar(const Fi fi, const uint8_t bi){

            const auto factor = model_[fi];
            if(factor->arity()>1){
                const Vi vis[2] = {factor->vi(0),factor->vi(1)};
                // get incoming messages
                const ValueType * varToFac[2] ={
                    msg_.varToFacMsg(vis[0], msg_.factorsMi(fi,0), bi),
                    msg_.varToFacMsg(vis[1], msg_.factorsMi(fi,1), bi)
                };   
                // get outcoming messages         
                ValueType * facToVar[2] ={
                    msg_.facToVarMsg(fi, 0, bi),
                    msg_.facToVarMsg(fi, 1, bi)
                };

                factor->valueTable()->facToVarMsg(varToFac, facToVar);

                /*

               

                // init outgoing messages
                for(auto i=0;i<2;++i)
                    std::fill(facToVar[i], facToVar[i]+nLabels_, std::numeric_limits<ValueType>::infinity());



                DiscreteLabel conf[2];
                for(conf[1]=0; conf[1]<nLabels_; ++conf[1])
                for(conf[0]=0; conf[0]<nLabels_; ++conf[0]){
                    const ValueType facVal = factor->eval2(conf[0], conf[1]);
                    //INFERNO_CHECK_NUMBER(facVal);
                    const ValueType vals[2] = {facVal + varToFac[1][conf[1]],facVal + varToFac[0][conf[0]]};
                    for(auto i=0;i<2;++i){
                        //INFERNO_CHECK_NUMBER(vals[i]);
                        facToVar[i][conf[i]] = std::min(facToVar[i][conf[i]], vals[i]);
                    }
                }
                */
                ///for(auto l=0; l<nLabels_;++l){
                //    INFERNO_CHECK_NUMBER(facToVar[0][l]);
                //    INFERNO_CHECK_NUMBER(facToVar[1][l]);
                //}
            }
        }
        void sendVarToFac(const Vi vi, const uint8_t bi){
            
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
                    const ValueType * m = msg_.facToVarMsg(fi, vi0 == vi ? 0 : 1, bi);
                    for(DiscreteLabel l=0; l<nLabels_; ++l){
                        sMsgBuffer_[l] += m[l];
                    }
                }
                else{
                    //INFERNO_CHECK(false,"");
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
                    const ValueType * ftv = msg_.facToVarMsg(fi, vi0 == vi ? 0 : 1, bi);
                    ValueType  * vtf = msg_.varToFacMsg(vi, msgIndex, bi);
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
        FactorsOfVariables<Model> factorsOfVariables_;
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
