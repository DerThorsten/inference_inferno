/** \file base_discrete_inference.hxx 
    \brief  Functionality which is shared between all inference algorithms
    is implemented in this header.

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_FACTORY_HXX
#define INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_FACTORY_HXX

#include <iostream>
#include <iomanip>
#include <map>

#include <boost/any.hpp> 

#include "inferno/inferno.hxx"
#include "inferno/inference/base_discrete_inference.hxx"
    
namespace inferno{

/** \namespace inferno::inference
    \brief subnamespace of inferno
    where all inference related
    functionality is implemented.

    All inference algorithms and related
    functionality is implemented 
    within inferno::inference.
*/
namespace inference{

    typedef inferno::utilities::Concurrency Concurrency;

    template<class MODEL>
    class DiscreteInferenceBase;


    template<class MODEL>
    class BaseDiscreteInferenceFactory{
    public:
        typedef MODEL Model;
        typedef DiscreteInferenceBase<Model> BaseInference;
        virtual ~BaseDiscreteInferenceFactory(){}
        virtual std::shared_ptr<BaseInference> create(const Model & model) = 0;
    };


    template<class INFERENCE>
    class DiscreteInferenceFactory :
    public BaseDiscreteInferenceFactory<typename INFERENCE::Model>{
    public:
        typedef DiscreteInferenceBase<Model> BaseInference;
        typedef INFERENCE Inference;
        typedef Inference::Options Options;

        DiscreteInferenceFactory(const Options & options = Options())
        : options_(options){

        }
        virtual std::shared_ptr<BaseInference> create(const Model & model){
            return std::make_shared<BaseInference>(new Inference(model, options));
        }
    private:
        const Options & options_;
    };
    

}
}


#endif /* INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_FACTORY_HXX */
