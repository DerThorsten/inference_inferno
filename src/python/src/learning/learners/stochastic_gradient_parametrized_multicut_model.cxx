#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_learners_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related
#include "export_stochastic_gradient.hxx"


#include "inferno/model/parametrized_multicut_model.hxx"
#include "inferno/learning/dataset/explicit_dataset.hxx"
#include "inferno/learning/loss_functions/loss_functions.hxx"

namespace inferno{
namespace learning{
namespace learners{


    typedef models::ParametrizedMulticutModel<
            models::detail_python::EdgeArray, 
            models::detail_python::FeatureArray
    > PModel;
    typedef loss_functions::VariationOfInformation<PModel> PLossFunction;
    typedef dataset::VectorDataset<PLossFunction>  PDataset;

    template void 
    exportStochasticGradient<PDataset>(const std::string & );
}
}
}


