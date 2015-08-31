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
#include "inferno/learning/loss_functions/edge_hamming.hxx"


// inferno python
#include "inferno/python/export_non_copyable_vector.hxx"

namespace inferno{
namespace learning{
namespace loss_functions{

    namespace bp = boost::python;


    template<class MODEL>
    EdgeHamming<MODEL> * edgeHammingFactory(
        const MODEL & model,
        const typename EdgeHamming<MODEL>::FactorWeightMap & factorWeights,
        const double rescale,
        const bool useIgnoreLabel,
        const DiscreteLabel ignoreLabel
    ){
        EdgeHamming<MODEL> * lossFunction;
        {
            ScopedGILRelease allowThreads;
            lossFunction = new EdgeHamming<MODEL>(model, factorWeights, rescale, 
                                                  useIgnoreLabel, ignoreLabel);
        }
        return lossFunction;
    }

    template<class MODEL>
    void edgeHammingAssign(
        EdgeHamming<MODEL> & lossFunction,
        const MODEL & model,
        const typename EdgeHamming<MODEL>::FactorWeightMap & factorWeights,
        const double rescale,
        const double underseg,
        const double overseg,
        const bool useIgnoreLabel,
        const DiscreteLabel ignoreLabel
    ){

        ScopedGILRelease allowThreads;
        lossFunction.assign(model, factorWeights,rescale,
                            underseg,overseg,
                            useIgnoreLabel, ignoreLabel);
    }

    template<class MODEL>
    typename EdgeHamming<MODEL>::FactorWeightMap & 
    edgeHammingGetFactorWeightMap(
        EdgeHamming<MODEL> & lossFunction
    ){
        return lossFunction.factorWeightMap();
    }


    template<class MODEL>
    void exportEdgeHamming(){
        typedef MODEL Model;
        typedef EdgeHamming<Model> LossFunction;

        const auto modelClsName = models::ModelName<Model>::name();
        const auto clsName = std::string("EdgeHamming") + modelClsName;


        // the class
        bp::class_<LossFunction,boost::noncopyable>(clsName.c_str(), bp::no_init)
            .def("assign",
                &edgeHammingAssign<Model>, 
                (
                    bp::arg("model"),
                    bp::arg("factorWeights"),
                    bp::arg("rescale") = 1.0,
                    bp::arg("underseg") = 1.0,
                    bp::arg("overseg") = 1.0,
                    bp::arg("useIgnoreLabel") = false,
                    bp::arg("ignoreLabel") = -1
                )
            )
            .def(
                "factorWeightMap",
                &edgeHammingGetFactorWeightMap<Model>,
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
        bp::def("_edgeHamming",
            &edgeHammingFactory<MODEL>,
            RetValPol< NewObj >()
        );
    }
}
}
}


//#endif /* INFERNO_SRC_PYTHON_SRC_LEARNING_LOSS_FUNCTIONS_EXPORT_VARIATION_OF_INFORMATION_HXX*/
