#define PY_ARRAY_UNIQUE_SYMBOL inferno_inference_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related (via inferno python)
#include "inferno/inferno_python.hxx"

// vigra numpy array converters
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>

// standart c++ headers 
#include <string>

// inferno related
#include "inferno/inferno.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/inference/base_discrete_inference.hxx"
#include "inferno/inference/visitors.hxx"

namespace inferno{
namespace inference{

    namespace bp = boost::python;

    template<class MODEL>
    typename DiscreteInferenceBase<MODEL>::Visitor * visitorFactory(
         const MODEL & model
    ){
        return  new typename DiscreteInferenceBase<MODEL>::Visitor ();
    }

    template<class MODEL>
    VerboseVisitor<MODEL> * verboseVisitorFactory(
         const MODEL & model,
         const uint64_t printNth,
         const bool singleLine
    ){
        return  new VerboseVisitor<MODEL>(printNth, singleLine);
    }

    template<class MODEL>
    void exportVisitorsT(const std::string modelName){

        {
            const std::string visitorClsName = std::string("VerboseVisitor") + modelName;
            typedef VerboseVisitor<MODEL> VisitorType;
            bp::class_<VisitorType>(visitorClsName.c_str(),bp::no_init)
                .def("visitor",&VisitorType::visitor,bp::return_internal_reference<>())
            ;

            // export visitor factory
            bp::def("verboseVisitor", & verboseVisitorFactory<MODEL>,
                    (
                        bp::arg("printNth") = 1,
                        bp::arg("singleLine") = true
                    ),
                    bp::return_value_policy<bp::manage_new_object>()
            );
        }

        {
            typedef DiscreteInferenceBase<MODEL> BaseInf;
            const std::string visitorClsName = std::string("Visitor") + modelName;
            typedef typename BaseInf::Visitor VisitorType;
            bp::class_<VisitorType>(visitorClsName.c_str(),bp::no_init)
            ;

            // export visitor factory
            bp::def("visitor", & visitorFactory<MODEL>,
                    bp::return_value_policy<bp::manage_new_object>()
            );
        }


    }


    void exportVisitors(){
        exportVisitorsT<inferno::models::GeneralDiscreteGraphicalModel>("GeneralDiscreteGraphicalModel");
    }

}
}


