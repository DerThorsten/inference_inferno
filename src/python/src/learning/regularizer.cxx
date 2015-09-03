#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_PyArray_API
#define NO_IMPORT_ARRAY


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
#include "inferno/learning/learning.hxx"



namespace inferno{
namespace learning{

    namespace bp = boost::python;

    void exportRegularizer(){

        bp::enum_<RegularizerType>("RegularizerType")
            .value("L1",        RegularizerType::L1)
            .value("L2",        RegularizerType::L2)
            .value("ConstL1",   RegularizerType::ConstL1)
            .value("ConstL2",   RegularizerType::ConstL2)
        ;
    

        bp::class_<Regularizer>("Regularizer",
            bp::init<const RegularizerType, const ValueType>(
                (
                    bp::arg("regularizerType") = RegularizerType::L2,
                    bp::arg("c") = 1.0
                ) 
            )
        )

        ;
    }
}
}


