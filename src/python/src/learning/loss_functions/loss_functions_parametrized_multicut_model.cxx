#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_loss_functions_PyArray_API
#define NO_IMPORT_ARRAY


// inferno
#include "inferno/python/model/parametrized_multicut_model.hxx"
#include "inferno/inferno_python.hxx"
#include "inferno/python/learning/loss_functions/export_loss_functions.hxx"
#include "inferno/python/learning/loss_functions/export_partition_f_score.hxx"

namespace inferno{
namespace learning{
namespace loss_functions{

    void exportLossFunctionsParametrizedMulticutModel(){


        typedef models::PyParametrizedMulticutModel Model;

        auto modelName = models::ModelName<Model>::name();

        // first we export the base class for
        // NON - decomposable loss functions
        // since the base-class depends only on 'Model'
        exportNonDecomposableLossFunctionBase<Model>(modelName);


        // partition - fscore
        ExportPartitonFScore<Model>::exportLossFunction(modelName);

    }
}
}
}


