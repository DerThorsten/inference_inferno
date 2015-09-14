/** \file LpInference.hxx 
    \brief  Implementation of inferno::inference::LpInference  
*/
#ifndef INFERNO_INFERENCE_LP_INFERENCE_HXX
#define INFERNO_INFERENCE_LP_INFERENCE_HXX

#include <unordered_map>

#include "inferno/model/discrete_model_base.hxx"
#include "inferno/inference/discrete_inference_base.hxx"


namespace inferno{
namespace inference{

    

    class LpSolver{

    };








    template<class MODEL>
    class LpInference  : public DiscreteInferenceBase<MODEL> {
    public:
        typedef MODEL Model;
        typedef LpInference<MODEL> Self;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor Visitor;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;
        typedef kolmogorov::LpInference::LpInference<ValueType> QpboSolver;

        struct Options{
            Options(      

            )
            {
            }
            
        };



        LpInference(const Model & model, const Options & options = Options() )
        :   BaseInf(),
            model_(model),
            options_(options),
            stopInference_(false),
            bound_(-1.0*infVal()),
            value_(infVal())
        {


            // set up the LP
            // 
            //
            //

            uint64_t nLpVars = 0 ; 

            // model without constraints
            if(!model_->hasConstraints()){

            }   
            // model with constraints
            else{

            }


          
        }

        ~LpInference(){
            delete qpbo_;
        }

        // MUST HAVE INTERACE
        virtual std::string name() const {
            return "LpInference";
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
        // get results optional interface
        virtual ValueType lowerBound(){
            return bound_;
        }
        virtual ValueType upperBound(){
            return value_;
        }
        virtual ValueType energy(){
            return value_;
        }

    private:
        const Model & model_;
        Options       options_;
        bool          stopInference_;
        ValueType     bound_;
        ValueType     value_;

        LpSolver * lpSolver_;
    };


} // end namespace inference
} // end namespace inferno

#endif /* INFERNO_INFERENCE_LP_INFERENCE_HXX */
