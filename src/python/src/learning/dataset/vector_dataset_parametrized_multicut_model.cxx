#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_dataset_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related

// inferno relatex
#include "inferno/inferno.hxx"
#include "inferno/inferno_python.hxx"
#include "inferno/learning/loss_functions/loss_functions.hxx"
#include "inferno/learning/loss_functions/edge_hamming.hxx"
#include "inferno/model/parametrized_multicut_model.hxx"

// exporter
#include "export_vector_dataset.hxx"



namespace inferno{
namespace learning{
namespace dataset{

    namespace bp = boost::python;
    
    template void 
    exportVectorDataset<
        inferno::learning::loss_functions::VariationOfInformation<
            inferno::models::PyParametrizedMulticutModel
        >
    >(const std::string &);


    template void 
    exportVectorDataset<
        inferno::learning::loss_functions::EdgeHamming<
            inferno::models::PyParametrizedMulticutModel
        >
    >(const std::string &);
}
}
}


