#ifndef  INFERNO_SRC_PYTHON_INFERENCE_EXPORT_INFERENCE_HXX
#define INFERNO_SRC_PYTHON_INFERENCE_EXPORT_INFERENCE_HXX

#include <boost/python/def_visitor.hpp> 

#include "inferno/inferno.hxx"
#include "inferno/inference/base_discrete_inference.hxx"

namespace inferno{
namespace export_helper{

    namespace bp = boost::python;

    template<class MODEL>
    struct BaseInfWrap : 
        public inference::DiscreteInferenceBase<MODEL>, 
        bp::wrapper<inference::DiscreteInferenceBase<MODEL> >
    {
        typedef MODEL Model;
        typedef typename MODEL :: template VariableMap<DiscreteLabel> Conf;
        typedef inference::DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor  VisitorType;


        std::string name()const{
            return this->get_override("name")();
        }
        // inference
        void infer( VisitorType  * visitor  = NULL) {
            this->get_override("infer")(visitor);
        }
        // get result
        void conf(Conf & conf ){
            this->get_override("__conf")(conf);
        }

        DiscreteLabel label(const Vi vi ) {
           return this->get_override("label")(vi);
        }

        // get model
        const Model & model() const{
            return this->get_override("model")();
        }
        // stop inference (via visitor)
        void stopInference(){
            this->get_override("stopInference")();
        }

        



    };

}
}

#endif /* INFERNO_SRC_PYTHON_INFERENCE_EXPORT_INFERENCE_HXX */
