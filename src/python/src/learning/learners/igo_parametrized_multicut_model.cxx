#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_learners_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related
#include "export_igo.hxx"



#include "inferno/learning/dataset/explicit_dataset.hxx"
#include "inferno/learning/loss_functions/loss_functions.hxx"
#include "inferno/python/model/parametrized_multicut_model.hxx"

namespace inferno{
namespace learning{
namespace learners{


    typedef models::PyParametrizedMulticutModel PModel;
    typedef loss_functions::VariationOfInformation<PModel> PLossFunction;
    typedef dataset::VectorDataset<PLossFunction>  PDataset;

    template void 
    exportIgo<PDataset>(const std::string & );
}
}
}


