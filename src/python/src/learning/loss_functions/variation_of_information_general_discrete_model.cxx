#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_loss_functions_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related
#include "export_variation_of_information.hxx"
#include "inferno/python/model/general_discrete_model.hxx"

namespace inferno{
namespace learning{
namespace loss_functions{

    template void 
    exportVariationOfInformation<inferno::models::PyGeneralDiscreteModel>();
}
}
}


