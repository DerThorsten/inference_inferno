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
#include "inferno/inference/base_discrete_inference.hxx"




namespace inferno{
namespace inference{

    namespace bp = boost::python;


    void setOpt(InferenceOptions & options, const std::string & key, bp::object obj){
        {
            bp::extract<int64_t> e(obj);
            if(e.check()){
                options.set(key, e());
                return ;
            }
        }
        {
            bp::extract<bool> e(obj);
            if(e.check()){
                options.set(key, e());
                return ;
            }
        }
        {
            bp::extract<double> e(obj);
            if(e.check()){
                options.set(key, e());
                return ;
            }
        }
        {
            bp::extract<float> e(obj);
            if(e.check()){
                options.set(key, e());
                return ;
            }
        }
        
        {
            bp::extract<std::string> e(obj);
            if(e.check()){
                options.set(key, e());
                return ;
            }
        }
        {
            bp::extract<InferenceOptions> e(obj);
            if(e.check()){
                options.set(key, e());
                return ;
            }
        }
        throw RuntimeError("no converter found for object with key "+key);
    }


    void exportInferenceOptions(){
            typedef InferenceOptions InfOpts;
            bp::class_<InfOpts>("InferenceOptions",bp::init<>())
                .def("__setitem__",&setOpt)
                .def("__str__",&InfOpts::asString)
            ;
    }


} // end namespace inferno::inference
} // end namespace inferno



