/** \file base_discrete_inference.hxx 
    \brief  Functionality which is shared between all inference algorithms
    is implemented in this header.

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_ICM_HXX
#define INFERNO_INFERENCE_ICM_HXX

#include "inferno/inferno.hxx"
#include "inferno/utilities/delegate.hxx"
#include "inferno/inference/base_discrete_inference.hxx"
#include "inferno/inference/utilities/movemaker.hxx"
#include "inferno/model/factors_of_variables.hxx"
namespace inferno{


namespace inference{

    template<class MODEL>
    class Icm  : public DiscreteInferenceBase<MODEL> {
    public:
        typedef MODEL Model;
        typedef Icm<MODEL> Self;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor Visitor;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;
        typedef typename MODEL:: template VariableMap<uint8_t> UIn8VarMap;
        typedef Movemaker<Model> MovemakerType;

        static void defaultOptions(InferenceOptions & options){

        }

        struct Options{

        };

        Icm(const Model & model, const Options & options = Options())
        :   BaseInf(),
            model_(model),
            movemaker_(model),
            variablesNeighbours_(model),
            isLocalOpt_(model),
            stopInference_(false){
        }


        // MUST HAVE INTERACE
        virtual std::string name() const {
            return "Icm";
        }
        // inference
        virtual void infer( Visitor  * visitor  = NULL) {

            const auto factorOfVariables  = movemaker_.factorsOfVariabes();

            for(const auto vi : model_.variableIds())
                isLocalOpt_[vi] = false;

            if(visitor!=NULL)
                visitor->begin(this);

            bool changes = true;
            while(changes){
                changes = false;
                for(const auto vi : model_.variableIds()){
                    if(stopInference_){
                        changes = false;
                        break;
                    }
                    if(!isLocalOpt_[vi]){
                        const auto currentLabel = movemaker_.label(vi);
                        movemaker_.moveOptimally(&vi, &vi+1);
                        const auto newLabel = movemaker_.label(vi);
                        isLocalOpt_[vi] = true;
                        if(newLabel != currentLabel){
                            if(visitor!=NULL)
                                visitor->visit(this);
                            changes = true;
                            for(const auto ovi : variablesNeighbours_[vi])
                                isLocalOpt_[ovi] = false;
                        }
                    }
                }
            }

            if(visitor!=NULL)
                visitor->end(this);
        }
        // get result
        virtual void conf(Conf & confMap ) {
            movemaker_.conf(confMap);
        }
        virtual DiscreteLabel label(const Vi vi ) {
            return movemaker_.label(vi);
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
            movemaker_.initialize(conf);
        }

        // get results optional interface
        virtual ValueType upperBound(){
            return movemaker_.energy();
        }
        virtual ValueType energy(){
            return movemaker_.energy();
        }

    private:
        const Model & model_;
        MovemakerType movemaker_;
        inferno::models::VariablesNeighbours<Model> variablesNeighbours_;
        UIn8VarMap isLocalOpt_;
        bool stopInference_;
    };
  
    

} // end namespace inferno::inference
} // end namespace inferno


#endif /* INFERNO_INFERENCE_ICM_HXX */
