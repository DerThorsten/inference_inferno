#define PY_ARRAY_UNIQUE_SYMBOL inferno_models_PyArray_API
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


// export helper
#include "exportModels.hxx"

// standart c++ headers (whatever you need (string and vector are just examples))
#include <string>
#include <vector>

// inferno relatex
#include "inferno/inferno_python.hxx"
#include "inferno/model/parametrized_multicut_model.hxx"
#include "inferno/learning/weights.hxx"

namespace inferno{
namespace models{

    namespace bp = boost::python;


    PyParametrizedMulticutModel * 
    parametrizedMulticutModelFactory(
        const uint64_t nVar,
        const vigra::NumpyArray<1, vigra::TinyVector<uint64_t,2> > & edges,
        const vigra::NumpyArray<2, ValueType> features,
        const learning::WeightVector & weights 
    ){
        PyParametrizedMulticutModel * model;
        {
            ScopedGILRelease allowThreads;
            model = new PyParametrizedMulticutModel(nVar, edges, features, weights);
        }
        return model;
    }

    void
    parametrizedMulticutModelAssign(
        PyParametrizedMulticutModel & model,
        const uint64_t nVar,
        const vigra::NumpyArray<1, vigra::TinyVector<uint64_t,2> > & edges,
        const vigra::NumpyArray<2, ValueType> features,
        const learning::WeightVector & weights 
    ){
        ScopedGILRelease allowThreads;
        model.assign(nVar, edges, features, weights);
    }




    void exportParametrizedMulticutModel(){

        // the class
        typedef PyParametrizedMulticutModel Model;
        const auto modelName = ModelName<Model>::name();
        bp::class_<Model, boost::noncopyable>(modelName.c_str(),bp::init<>())
            .def(export_helper::ExportModelAPI<Model>(modelName.c_str()))
            .def("_assign",vigra::registerConverters(&parametrizedMulticutModelAssign), 
                RetValPol< CustWardEnd<1,5> >()
            )
        ;

        // the factory function
        bp::def("_parametrizedMulticutModel",
            vigra::registerConverters(&parametrizedMulticutModelFactory),
            RetValPol< CustWardPost<0,4,NewObj> >()
        );

    }
} // end namespace inferno::models  
} // end namespace inferno


