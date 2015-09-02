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
#include "inferno/inferno.hxx"
#include "inferno/learning/weight_constraints.hxx"



namespace inferno{
namespace learning{

    namespace bp = boost::python;


    void addWeightConstraint(
        WeightConstraints & weightConstraints,
        const vigra::NumpyArray<1, uint64_t> & weightIds,
        const vigra::NumpyArray<1, ValueType> & coefficients,
        const ValueType lowerBound,
        const ValueType upperBound
    ){
        weightConstraints.addConstraint(
            weightIds.begin(), weightIds.end(),
            coefficients.begin(),
            lowerBound, upperBound
        );
    }

    void exportWeightConstraints(){

        bp::class_<WeightConstraints>("WeightConstraints",
            bp::init<const uint64_t>(
                (
                    bp::arg("nWeights") = 0 
                )
            )
        )
        .def("addConstraint",
            vigra::registerConverters(&addWeightConstraint),
            (
                bp::arg("weightIds"),
                bp::arg("coefficients"),
                bp::arg("lowerBound") = -1.0*infVal(),
                bp::arg("upperBound") = infVal()
            )
        )
        .def("addBound",&WeightConstraints::addBound,
            (
                bp::arg("weightId"),
                bp::arg("lowerBound") = -1.0*infVal(),
                bp::arg("upperBound") = infVal()
            )
        )   
        ;
    
    }
}
}


