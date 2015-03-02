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


// export helper
#include "exportInference.hxx"

// standart c++ headers (whatever you need (string and vector are just examples))
#include <string>
#include <vector>
#include <map>

// inferno relatex
#include "inferno/inferno.hxx"
#include "inferno/inferno_python.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/inference/base_discrete_inference.hxx"
#include "inferno/inference/icm.hxx"
#include "inferno/inference/mp.hxx"
namespace inferno{


    namespace bp = boost::python;




    template<class BASE_INF>
    vigra::NumpyAnyArray getConf(BASE_INF * inf, vigra::NumpyArray<1, DiscreteLabel> out){
        const auto & model = inf->model();
        vigra::TinyVector<vigra::MultiArrayIndex, 1> shape(model.maxVarId()+1);
        out.reshapeIfEmpty(shape);
        {
            ScopedGILRelease _gil;
            for(const auto vi : model.variableIds())
                out(vi) = inf->label(vi);
        }
        return out;

    }

    template<class INF,class MODEL>
    inference::InferenceOptions getDefaultOptions(const MODEL & model){
        inference::InferenceOptions  options;
        INF::defaultOptions(options);
        return options;
    }



    void setOpt(inference::InferenceOptions & options, const std::string & key, bp::object obj){
        std::cout<<"set option val of key "<<key<<"\n";

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
            bp::extract<std::string> e(obj);
            if(e.check()){
                options.set(key, e());
                return ;
            }
        }
        {
            bp::extract<inference::InferenceOptions> e(obj);
            if(e.check()){
                options.set(key, e());
                return ;
            }
        }
        throw RuntimeError("no converter found for object with key "+key);
    }

    template<class MODEL>
    void exportDiscreteInferenceBaseT(const std::string modelName){


        {

            typedef inference::InferenceOptions InfOpts;



            bp::class_<InfOpts>("InferenceOptions",bp::init<>())
                .def("__setitem__",&setOpt)
                .def("__str__",&InfOpts::asString)
            ;
        }
        const std::string baseClsName = std::string("DiscreteInferenceBase") + modelName;
        typedef inference::DiscreteInferenceBase<MODEL> BaseInf;
        bp::class_<export_helper::BaseInfWrap<MODEL>, boost::noncopyable>(baseClsName.c_str())
            // pure virtual functions
            .def("name", bp::pure_virtual( &BaseInf::name))
            .def("infer", bp::pure_virtual(&BaseInf::infer),(bp::arg("visitor")=NULL))
            .def("__conf", bp::pure_virtual(&BaseInf::conf))
            .def("conf", vigra::registerConverters(getConf<BaseInf>), 
                (bp::arg("out") = bp::object() )
            )
            .def("label", bp::pure_virtual(&BaseInf::label))
            .def("model", bp::pure_virtual(&BaseInf::model),bp::return_internal_reference<>())
            .def("stopInference", bp::pure_virtual(&BaseInf::stopInference))
        ;

        {
            // export icm
            typedef inference::Icm<MODEL> Inference;
            const std::string infClsName = std::string("Icm") + modelName;
            bp::class_< Inference,bp::bases<BaseInf> >(infClsName.c_str(),bp::no_init)
            ; 
            // export factory
            bp::def("icmOptions", &getDefaultOptions<Inference, MODEL>);
            bp::def("icm", & inference::inferenceFactory<Inference>,
                (
                    bp::arg("model"),
                    bp::arg("options") = inference::InferenceOptions()
                ),
                CustWardPost< 0,1 ,RetValPolNewObj>()  
            );
        }
        {
            // export icm
            typedef inference::MessagePassing<MODEL> Inference;
            const std::string infClsName = std::string("MessagePassing") + modelName;
            bp::class_< Inference,bp::bases<BaseInf> >(infClsName.c_str(),bp::no_init)
            ; 
            // export factory
            bp::def("messagePassingOptions", &getDefaultOptions<Inference, MODEL>);
            bp::def("messagePassing", & inference::inferenceFactory< Inference>,
                (
                    bp::arg("model"),
                    bp::arg("options") = inference::InferenceOptions()
                ),
                CustWardPost< 0,1 ,RetValPolNewObj>()  
            );
        }
    }


    void exportDiscreteInferenceBase(){
        exportDiscreteInferenceBaseT<GeneralDiscreteGraphicalModel>("GeneralDiscreteGraphicalModel");
    }

}



