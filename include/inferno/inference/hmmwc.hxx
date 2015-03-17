/** \file base_discrete_inference.hxx 
    \brief  Functionality which is shared between all inference algorithms
    is implemented in this header.

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_HMMWC_HXX
#define INFERNO_INFERENCE_HMMWC_HXX

#include "inferno/inferno.hxx"
#include "inferno/inference/base_discrete_inference.hxx"
#include "inferno/model/factors_of_variables.hxx"

namespace inferno{


namespace inference{

    template<class MODEL>
    class Hmmwc  : public DiscreteInferenceBase<MODEL> {
    public:
        typedef MODEL Model;
        typedef Hmmwc<MODEL> Self;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor Visitor;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;
        typedef typename MODEL:: template VariableMap<uint8_t> UIn8VarMap;

        
        struct Options{

        };



        Hmmwc(const Model & model, const Options & options = Options())
        :   BaseInf(),
            model_(model),
            conf_(model, 0),
            semanticConf_(model,0),
            partitionConf_(model,0),
            allowCuts_(),
            lowerBound_(infVal()*-1.0),
            stopInference_(false){

            INFERNO_CHECK_OP(model_.maxArity(),==, 2, "Hmmwc is only implemented for models with maxArity == 2");

            std::cout<<"Hmmwc constructor \n";
            model_.guessAllowCutsWithin(allowCuts_);
            startLabel_.resize(allowCuts_.size());

            DiscreteLabel s=0;
            std::cout<<" #"<<allowCuts_.size()<<" semantic classes\n";
            for(auto i=0; i<allowCuts_.size(); ++i){
                startLabel_[i] = s;
                s+= allowCuts_[i] ? model_.nVariables() : 1;
                std::cout<<" sc "<<i<<" cuts within? "<<allowCuts_[i]<<" (label range "<<startLabel_[i]<<" - "<<s<<")"<<"\n";
            }
        }

        void computeTrivialLowerBoundUpperBound(){
            lowerBound_ = 0.0;

            for(const auto fac : model_.factors()){
                const auto arity = fac->arity();
                if(arity == 1){
                    auto minVal = infVal();
                    DiscreteLabel argMin = 0;
                    for(const auto label : startLabel_){
                        const auto val = fac->eval1(label);
                        if(val<minVal){
                            minVal = val;
                            argMin = label;
                        }
                    }
                    conf_[fac->vi(0)] = argMin;
                    lowerBound_ += minVal;
                }
                else{
                    ValueType beta;
                    if(!fac->isPotts(beta)){
                        throw RuntimeError("second order factors isPotts must be true");
                    }
                    if(beta<0.0)
                        lowerBound_ +=beta;
                }
            }
        }


        // MUST HAVE INTERACE
        virtual std::string name() const {
            return "Hmmwc";
        }
        // inference
        virtual void infer( Visitor  * visitor  = NULL) {

            if(visitor!=NULL)
                visitor->begin(this);

            
            this->computeTrivialLowerBoundUpperBound();

            if(visitor!=NULL)
                visitor->visit(this);    



            if(visitor!=NULL)
                visitor->end(this);
        }
        // get result
        virtual void conf(Conf & confMap ) {
            for(const auto vi : model_.variableIds())
                confMap[vi] = conf_[vi];
        }
        virtual DiscreteLabel label(const Vi vi ) {
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
        //virtual void setConf(const Conf & conf){
        //    
        //}

        // get results optional interface
        //virtual ValueType upperBound(){
        //    
        //}
        //virtual ValueType energy(){
        //    
        //}
        virtual ValueType lowerBound(){
            return lowerBound_;
        }

    private:
        const Model & model_;
        Conf conf_;
        Conf semanticConf_;
        Conf partitionConf_;

        std::vector<bool> allowCuts_;
        std::vector<DiscreteLabel> startLabel_;
        ValueType lowerBound_;
        bool stopInference_;
    };
  
    

} // end namespace inferno::inference
} // end namespace inferno


#endif /* INFERNO_INFERENCE_HMMWC_HXX */
