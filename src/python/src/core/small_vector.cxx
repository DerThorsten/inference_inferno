#define PY_ARRAY_UNIQUE_SYMBOL inferno_core_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

// vigra numpy array converters
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>
#include <boost/python/exception_translator.hpp>

// standart c++ headers (whatever you need (string and vector are just examples))
#include <string>
#include <vector>

// inferno relatex
#include "inferno/inferno.hxx"
#include "inferno/mixed_label.hxx"
#include "inferno/small_vector.hxx"





namespace inferno{
   namespace bp = boost::python;

    template<class T>
    void exportSmallVectorT(const std::string & clsName){

        typedef SmallVector<T> Vec;
        bp::class_< Vec >(clsName.c_str())
            .def(bp::vector_indexing_suite<Vec>())
        ;

    }
    
    void exportSmallVector(){


        //exportSmallVectorT<DiscreteLabel>("SmallVecDiscreteLabel");
        //exportSmallVectorT<ContinousLabel>("SmallVecContinousLabel");
        //exportSmallVectorT<MixedLabel>("SmallVecMixedLabel");
    }
}



