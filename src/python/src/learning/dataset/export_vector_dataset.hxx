//#ifndef INFERNO_SRC_PYTHON_SRC_LEARNING_DATASET_EXPORT_VECTOR_DATASET_HXX
//#define INFERNO_SRC_PYTHON_SRC_LEARNING_DATASET_EXPORT_VECTOR_DATASET_HXX

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
#include "inferno/inferno_python.hxx"
#include "inferno/learning/dataset/explicit_dataset.hxx"
#include "inferno/learning/loss_functions/loss_functions.hxx"

namespace inferno{
namespace learning{
namespace dataset{

    namespace bp = boost::python;

    template<class LOSS_FUNCTION>
    VectorDataset<LOSS_FUNCTION> * vectorDatasetFactory(
        std::vector< typename LOSS_FUNCTION::Model>     & models,
        std::vector< LOSS_FUNCTION>                     & lossFunctions,
        const std::vector< typename LOSS_FUNCTION::ConfMap>   & gts

    ){
        return new VectorDataset<LOSS_FUNCTION>(models, lossFunctions, gts);
    }


    template<class LOSS_FUNCTION>
    void exportVectorDataset(const std::string & lossFunctionClsName){

        
        typedef LOSS_FUNCTION LossFunction;
        typedef typename LossFunction::Model Model;

        const auto modelClsName = models::ModelName<Model>::name();
        const auto clsName = std::string("VectorDataset") + modelClsName + lossFunctionClsName;

        typedef typename Model:: template VariableMap<DiscreteLabel> ConfMap;
        typedef VectorDataset<LossFunction> Dataset;

        // the class
        bp::class_<Dataset,boost::noncopyable>(clsName.c_str(), bp::no_init)
            .def("model",       &Dataset::model,        bp::return_internal_reference<>())
            .def("lossFunction",&Dataset::lossFunction, bp::return_internal_reference<>())
            .def("groundTruth", &Dataset::groundTruth,  bp::return_internal_reference<>())
            // todo unlock gil
            .def("averageLoss", &Dataset::averageLoss)
            .def("__len__",&Dataset::size)
            .def("updateWeights",&Dataset::updateWeights)
        ;

        // CustWardPost<0,1,NewObj>
        // the factory
        // the factory function
        bp::def("vectorDataset",
            &vectorDatasetFactory<LossFunction>,
            RetValPol< 
                CustWardPost<0,1,
                    CustWardPost<0,2,
                        CustWardPost<0,3,
                            NewObj
                        >
                    >
                > 
            >()
        );
    }

}
}
}

//#endif /*INFERNO_SRC_PYTHON_SRC_LEARNING_DATASET_EXPORT_VECTOR_DATASET_HXX*/
