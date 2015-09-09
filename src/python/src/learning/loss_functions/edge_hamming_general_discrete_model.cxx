#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_loss_functions_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related
#include "export_edge_hamming.hxx"
#include "inferno/python/model/general_discrete_model.hxx"

namespace inferno{
namespace learning{
namespace loss_functions{

    template void 
    exportEdgeHamming<inferno::models::PyGeneralDiscreteModel>();
}
}
}


