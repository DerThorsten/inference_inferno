#ifndef INFERNO_LEARNING_LEARNERS_DEFAULT_DATASET
#define INFERNO_LEARNING_LEARNERS_DEFAULT_DATASET

// boost
#include <boost/concept_check.hpp>
#include <boost/iterator/counting_iterator.hpp>

// inferno
#include "inferno/inferno.hxx"
#include "inferno/learning/dataset/dataset_base.hxx"

namespace inferno{
namespace learning{
namespace dataset{


    template<class LOSS_FUNCTION_BASE>
    class DefaultDataset{
    public:

        typedef LOSS_FUNCTION_BASE LossFunctionBase;
        typedef typename LossFunctionBase::Model Model;
        typedef typename LossFunctionBase::ConfMap ConfMap;

        DefaultDataset(
            std::vector<Model>  & models,
            const std::vector<LossFunctionBase * > lossFunctionPtrs,
            const std::vector<ConfMap> & gts,
            const WeightConstraints & weightConstraints,     
            const Regularizer regularizer
              
        )   :
            models_(models),
            lossFunctionPtrs_(lossFunctionPtrs),
            gts_(gts),
            weightConstraints_(weightConstraints),
            regularizer_(regularizer)
        {

        }


        Model * model(const uint64_t i) {
            return &models_[i];
        }
        const Model * model(const uint64_t i)const{
            return &models_[i];
        }
        const ConfMap * gt(const uint64_t i){
            return &models_[i];
        }
        const ConfMap * gt(const uint64_t i)const{
            return &models_[i];
        }

        const LossFunctionBase * lossFunction(const uint64_t i){
            return lossFunctionPtrs_[i];
        }
        const LossFunctionBase * lossFunction(const uint64_t i)const{
            return lossFunctionPtrs_[i];
        }

    private:
        std::vector<Model>  & models_;
        const std::vector<LossFunctionBase * > & lossFunctionPtrs_;
        const std::vector<ConfMap> & gts_;
        const WeightConstraints & weightConstraints_;
        Regularizer regularizer_;
    };


} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LEARNERS_DEFAULT_DATASET*/
