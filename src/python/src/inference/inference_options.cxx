#define PY_ARRAY_UNIQUE_SYMBOL inferno_inference_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related
//#include <boost/python.hpp>
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/def_visitor.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

// vigra numpy array converters
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>
#include <boost/python/exception_translator.hpp>


// standart c++ headers (whatever you need (string and vector are just examples))
#include <string>
#include <vector>
#include <map>

// inferno related
#include "inferno/inference/discrete_inference_base.hxx"




namespace inferno{
namespace inference{

    namespace bp = boost::python;




    void exportInferenceOptions(){
        typedef InferenceOptions InfOpts;
        bp::class_<InfOpts>("InferenceOptions",bp::init<>())
        ;
    }


} // end namespace inferno::inference
} // end namespace inferno



