#define PY_ARRAY_UNIQUE_SYMBOL inferno_inference_PyArray_API
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


// export helper
#include "exportInference.hxx"

// standart c++ headers (whatever you need (string and vector are just examples))
#include <string>
#include <vector>

// inferno relatex
#include "inferno/inferno.hxx"


#include "inferno/inferno.hxx"
#include "inferno/inference/base_discrete_inference.hxx"
#include "inferno/model/general_discrete_model.hxx"

namespace inferno{


    namespace bp = boost::python;




    template<class MODEL>
    void exportDiscreteInferenceBaseT(const std::string modelName){

        const std::string clsName = std::string("DiscreteInferenceBase") + modelName;
        typedef inference::DiscreteInferenceBase<MODEL> BaseInf;
        bp::class_<export_helper::BaseInfWrap<MODEL>, boost::noncopyable>(clsName.c_str())
            // pure virtual functions
            .def("name", bp::pure_virtual( &BaseInf::name))
            .def("infer", bp::pure_virtual(&BaseInf::infer))
            .def("conf", bp::pure_virtual(&BaseInf::conf))
            .def("model", bp::pure_virtual(&BaseInf::model),bp::return_internal_reference<>())
            .def("stopInference", bp::pure_virtual(&BaseInf::stopInference))
        ;
    }


    void exportDiscreteInferenceBase(){
        exportDiscreteInferenceBaseT<GeneralDiscreteGraphicalModel>("GeneralDiscreteGraphicalModel");
    }

}



