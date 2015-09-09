#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_dataset_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related

// inferno relatex
#include "inferno/inferno.hxx"
#include "inferno/learning/loss_functions/loss_function_base.hxx"

#include "inferno/inferno_python.hxx"
#include "inferno/python/model/parametrized_multicut_model.hxx"
#include "inferno/learning/loss_functions/partition_hamming.hxx"
#include "inferno/python/learning/dataset/export_default_dataset.hxx"




namespace inferno{
namespace learning{
namespace dataset{

    namespace bp = boost::python;
    
    void exportDefaultDatasetParametrizedMulticutModel(){

        typedef models::PyParametrizedMulticutModel Model;
        const auto modelName = models::ModelName<Model>::name();

        {   // dataset for NON decomposable datasets
            typedef loss_functions::NonDecomposableLossFunctionBase<Model> LossBase;
            const auto lossBaseName = std::string("NonDecomposableLossFunctionBase");
            const auto dsName = std::string("DefaultDataset") + modelName + lossBaseName;
                    
            ExportDefaultDataset<LossBase>::exportDataset(dsName);
        } 
        
        {   // dataset for decomposable partition hamming
            typedef loss_functions::DecomposableLossFunctionBase<
                Model, 
                typename loss_functions::PartitionHamming<Model>::LossAugmentedModel
            > LossBase;
            const auto lossBaseName = std::string("PartitionHamming") + std::string("DecomposableLossFunctionBase");
            const auto dsName = std::string("DefaultDataset") + modelName + lossBaseName;
            ExportDefaultDataset<LossBase>::exportDataset(dsName);
        }
    }
}
}
}


