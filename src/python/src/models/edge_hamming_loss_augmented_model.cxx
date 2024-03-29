#define PY_ARRAY_UNIQUE_SYMBOL inferno_models_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/def_visitor.hpp>


// export helper
#include "exportModels.hxx"

// standart c++ headers (whatever you need (string and vector are just examples))
#include <string>
#include <vector>

// inferno relatex
#include "inferno/python/model/parametrized_multicut_model.hxx"
#include "inferno/python/model/general_discrete_model.hxx"

#include "inferno/inferno_python.hxx"
#include "inferno/learning/loss_functions/edge_hamming.hxx"



namespace inferno{
namespace models{

    namespace bp = boost::python;


    
    template<class BASE_MODEL>
    void exportEdgeHammingLossAugmentedModelT(){

        // the class
        typedef BASE_MODEL BaseModel;
        typedef learning::loss_functions::EdgeHamming<BaseModel> EdgeHammingLossFunction;
        typedef typename EdgeHammingLossFunction::LossAugmentedModel Model;
        const auto baseModelName = ModelName<BaseModel>::name();
        const auto modelName = std::string("EdgeHammingLossAugmented") + baseModelName;

        bp::class_<Model, boost::noncopyable>(modelName.c_str(),bp::init<>())
            .def(export_helper::ExportModelAPI<Model>(modelName.c_str()))
        ;
    }

    void exportEdgeHammingLossAugmentedModel(){
        exportEdgeHammingLossAugmentedModelT<PyGeneralDiscreteModel>();
        exportEdgeHammingLossAugmentedModelT<PyParametrizedMulticutModel>();
    }

} // end namespace inferno::models  
} // end namespace inferno


