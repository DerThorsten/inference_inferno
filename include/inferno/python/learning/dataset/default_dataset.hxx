#ifndef INFERNO_PYTHON_LEARNING_DATASET_DEFAULT_DATASET_HXX
#define INFERNO_PYTHON_LEARNING_DATASET_DEFAULT_DATASET_HXX

// boost
#include <boost/python/return_value_policy.hpp>
// inferno 
#include "inferno/utilities/owning_ptr_vector.hxx"
#include "inferno/learning/dataset/default_dataset.hxx"


namespace inferno{
namespace learning{
namespace dataset{


    namespace bp = boost::python;

    namespace detail_python{
        template<class LOSS_FUNCTION_BASE>
        struct DefaultDatasetHelper{

            typedef LOSS_FUNCTION_BASE LossFunctionBase;
            typedef typename LossFunctionBase::Model Model;
            typedef typename LossFunctionBase::ConfMap ConfMap;
            typedef DefaultDataset<
                utilities::OwningPtrVector<LossFunctionBase>,
                std::vector<Model>,
                std::vector<ConfMap>
            > Dataset;
        };
    }

    template<class LOSS_FUNCTION_BASE>
    using PyDefaultDataset = 
        typename detail_python::DefaultDatasetHelper<LOSS_FUNCTION_BASE>::Dataset;
        




} // end namespace inferno::learning::dataset
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_PYTHON_LEARNING_DATASET_DEFAULT_DATASET_HXX */
