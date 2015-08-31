#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_learners_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related
#include "export_subgradient.hxx"


#include "inferno/model/general_discrete_model.hxx"
#include "inferno/learning/dataset/explicit_dataset.hxx"
#include "inferno/learning/loss_functions/edge_hamming.hxx"

namespace inferno{
namespace learning{
namespace learners{

    //////////////////////////////////////////////////////
    // General Discrete Model
    //
    //
    //////////////////////////////////////////////////////
    typedef models::GeneralDiscreteModel Model;

    typedef loss_functions::EdgeHamming<Model> EdgeHammingLoss;
    typedef dataset::VectorDataset<EdgeHammingLoss>  EdgeHammingDataset;

    template void 
    exportSubGradient<EdgeHammingDataset>(const std::string & );
}
}
}

