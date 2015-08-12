/** \file discrete_inference_base.hxx 
    \brief  Functionality which is shared between all inference algorithms
    is implemented in this header.

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_HMMWC_HXX
#define INFERNO_INFERENCE_HMMWC_HXX


#include "inferno/inferno.hxx"
#include "inferno/inference/discrete_inference_base.hxx"
#include "inferno/utilities/ufd.hxx"
#include "inferno/model/algorithms/connected_components.hxx"
    
#include "inferno/utilities/parallel/pool.hxx"

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
    private:
        typedef models::ConnectedComponents<Model> ConnectedComp;
        typedef typename ConnectedComp::Options CCOpt;
    public:
        
        struct Options{

        };



        Hmmwc(const Model & model, const Options & options = Options())
        :   BaseInf(),
            model_(model),
            connectedComp_(model,CCOpt{true}),
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
                        const auto val = fac->eval(label);
                        if(val<minVal){
                            minVal = val;
                            argMin = label;
                        }
                    }
                    std::cout<<argMin<<" ["<<minVal<<"]\n";
                    const auto vi = fac->variable(0);
                    conf_[vi] = argMin;
                    semanticConf_[vi] = argMin;
                    partitionConf_[vi] = argMin;
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

        void topDownSplitting(){
            std::cout<<"to-down-splitting\n";
            const Vi nCC = connectedComp_.run(partitionConf_) + 1;

            std::cout<<"vars "<<model_.nVariables()<<" nCC "<<nCC<<"\n\n";

            for(const auto anchor : connectedComp_.anchors()){
                std::cout<<" anchors "<<anchor<<"\n";
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
            this->topDownSplitting();


            if(visitor!=NULL)
                visitor->visit(this);    



            if(visitor!=NULL)
                visitor->end(this);
        }
        // get result
        virtual void conf(Conf & confMap ) {
            for(const auto varDesc : model_.variableDescriptors())
                confMap[varDesc] = conf_[varDesc];
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
        ConnectedComp connectedComp_;
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
