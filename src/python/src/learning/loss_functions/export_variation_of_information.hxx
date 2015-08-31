//#ifndef INFERNO_SRC_PYTHON_SRC_LEARNING_LOSS_FUNCTIONS_EXPORT_VARIATION_OF_INFORMATION_HXX
//#define INFERNO_SRC_PYTHON_SRC_LEARNING_LOSS_FUNCTIONS_EXPORT_VARIATION_OF_INFORMATION_HXX
// boost python related
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/def_visitor.hpp>


// vigra numpy array converters
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>
#include <boost/python/exception_translator.hpp>

// standart c++ headers (whatever you need (string and vector are just examples))
#include <string>
#include <vector>

// inferno relatex
#include "inferno/inferno.hxx"
#include "inferno/inferno_python.hxx"
#include "inferno/learning/loss_functions/loss_functions.hxx"


// inferno python
#include "inferno/python/export_non_copyable_vector.hxx"


namespace inferno{
namespace learning{
namespace loss_functions{

    namespace bp = boost::python;


    template<class MODEL>
    VariationOfInformation<MODEL> * variationOfInformationFactory(
        const MODEL & model,
        const typename VariationOfInformation<MODEL>::VariableSizeMap & sizeMap,
        const bool useIgnoreLabel,
        const DiscreteLabel ignoreLabel
    ){
        VariationOfInformation<MODEL> * lossFunction;
        {
            ScopedGILRelease allowThreads;
            lossFunction = new VariationOfInformation<MODEL>(model, sizeMap, useIgnoreLabel, ignoreLabel);
        }
        return lossFunction;
    }

    template<class MODEL>
    void variationOfInformationAssign(
        VariationOfInformation<MODEL> & lossFunction,
        const MODEL & model,
        const typename VariationOfInformation<MODEL>::VariableSizeMap & sizeMap,
        const bool useIgnoreLabel,
        const DiscreteLabel ignoreLabel
    ){

        ScopedGILRelease allowThreads;
        lossFunction.assign(model, sizeMap, useIgnoreLabel, ignoreLabel);
    }

    template<class MODEL>
    typename VariationOfInformation<MODEL>::VariableSizeMap & 
    variationOfInformationGetSizeMap(
        VariationOfInformation<MODEL> & lossFunction
    ){
        return lossFunction.variableSizeMap();
    }


    template<class MODEL>
    void exportVariationOfInformation(){
        typedef MODEL Model;
        typedef VariationOfInformation<Model> LossFunction;

        const auto modelClsName = models::ModelName<Model>::name();
        const auto clsName = std::string("VariationOfInformation") + modelClsName;


        // the class
        bp::class_<LossFunction,boost::noncopyable>(clsName.c_str(), bp::no_init)
            .def("assign",
                &variationOfInformationAssign<Model>, 
                (
                    bp::arg("model"),
                    bp::arg("sizeMap"),
                    bp::arg("useIgnoreLabel") = false,
                    bp::arg("ignoreLabel") = -1
                )
            )
            .def(
                "variableSizeMap",
                &variationOfInformationGetSizeMap<Model>,
                bp::return_internal_reference<>()
            )
        ;

        // the class vector
        const std::string vectorClsName = clsName + std::string("Vector");
        typedef std::vector<LossFunction> LossFunctionVector;

        bp::class_<LossFunctionVector,boost::noncopyable >(vectorClsName.c_str(), bp::init<>())
            .def(bp::init<const size_t >())
            .def(python::NonCopyableVectorVisitor<LossFunctionVector>())
        ;


        // the factory
        // the factory function
        bp::def("_variationOfInformation",
            &variationOfInformationFactory<MODEL>,
            RetValPol< NewObj >()
        );
    }
}
}
}


//#endif /* INFERNO_SRC_PYTHON_SRC_LEARNING_LOSS_FUNCTIONS_EXPORT_VARIATION_OF_INFORMATION_HXX*/
