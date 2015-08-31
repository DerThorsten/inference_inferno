#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_learners_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related
#include "export_igo.hxx"


#include "inferno/model/general_discrete_model.hxx"
#include "inferno/learning/dataset/explicit_dataset.hxx"
#include "inferno/learning/loss_functions/loss_functions.hxx"

namespace inferno{
namespace learning{
namespace learners{


    typedef models::GeneralDiscreteModel Model;
    typedef loss_functions::VariationOfInformation<Model> LossFunction;
    typedef dataset::VectorDataset<LossFunction>  Dataset;

    template void 
    exportIgo<Dataset>(const std::string & );
}
}
}


