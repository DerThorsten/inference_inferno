#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_learners_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related
#include "inferno/learning/loss_functions/loss_function_base.hxx"
#include "inferno/learning/loss_functions/partition_hamming.hxx"
#include "inferno/python/model/parametrized_multicut_model.hxx"
#include "inferno/python/learning/dataset/default_dataset.hxx"
#include "inferno/python/learning/learners/export_stochastic_gradient.hxx"
#include "inferno/python/learning/learners/export_subgradient.hxx"

namespace inferno{
namespace learning{
namespace learners{


    void exportLearnersParametrizedMulticutModel(){

        typedef models::PyParametrizedMulticutModel Model;
        const auto modelName = models::ModelName<Model>::name();

        
        {   // learners for non decomposable datasets
            typedef loss_functions::NonDecomposableLossFunctionBase<Model> LossBase;
            typedef dataset::PyDefaultDataset<LossBase> Dataset;
            const auto lossBaseName = std::string("NonDecomposableLossFunctionBase");

            {   // stochastic gradient
                const auto solverName = std::string("StochasticGradient") + modelName + lossBaseName;
                ExportStochasticGradient<Dataset>::exportLearner(solverName);
            }
        }

        {   // learners for partition hamming
            typedef typename loss_functions::PartitionHamming<Model>::Base LossBase;
            typedef dataset::PyDefaultDataset<LossBase> Dataset;
            const auto lossBaseName = std::string("PartitionHamming") + std::string("DecomposableLossFunctionBase");
            {
                // subgradient
                const auto solverName = std::string("Subgradient") + modelName + lossBaseName;
                ExportSubgradient<Dataset>::exportLearner(solverName);
            }
        }
    }

}
}
}


