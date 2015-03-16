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
            stopInference_(false){
        }


        // MUST HAVE INTERACE
        virtual std::string name() const {
            return "Hmmwc";
        }
        // inference
        virtual void infer( Visitor  * visitor  = NULL) {

           if(visitor!=NULL)
                visitor->begin(this);

         
                



            if(visitor!=NULL)
                visitor->end(this);
        }
        // get result
        virtual void conf(Conf & confMap ) {
            
        }
        virtual DiscreteLabel label(const Vi vi ) {
            
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
            
        }
        virtual ValueType energy(){
            
        }

    private:
        const Model & model_;
        bool stopInference_;
    };
  
    

} // end namespace inferno::inference
} // end namespace inferno


#endif /* INFERNO_INFERENCE_HMMWC_HXX */
