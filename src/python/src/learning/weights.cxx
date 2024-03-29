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
#include "inferno/learning/weights.hxx"



namespace inferno{
namespace learning{

    namespace bp = boost::python;

    uint64_t wSize(const WeightVector & wv){
        return wv.size();
    }

    WeightType getWeight(const WeightVector & wv, const uint64_t id){
        return wv[id];
    }

    void setWeight(WeightVector & wv, const uint64_t id, const WeightType val){
        wv[id] = val;
    }

    void exportWeights(){
        bp::class_<WeightVector>("WeightVector",
                                 bp::init<const size_t, const WeightType>()
        )
        .def("__len__",&wSize)
        .def("__getitem__",&getWeight)
        .def("__setitem__",&setWeight)
        ;
    }
}
}


