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


    template<
        class LOSS_FUNCTION_BASE_VECTOR,
        class MODEL_VECTOR,
        class GT_VECTOR
    >
    class DefaultDataset
    : public
    NewDatasetBase<
        DefaultDataset<
            LOSS_FUNCTION_BASE_VECTOR,
            MODEL_VECTOR,
            GT_VECTOR
        >,
        typename std::remove_pointer< typename LOSS_FUNCTION_BASE_VECTOR::value_type>::type
    >


    {
    public:


        typedef typename std::remove_pointer< typename LOSS_FUNCTION_BASE_VECTOR::value_type>::type LossFunctionBase;
        typedef LossFunctionBase LossFunction;
        typedef typename LossFunctionBase::Model Model;
        typedef typename LossFunctionBase::ConfMap ConfMap;
        typedef ConfMap GroundTruth;

        DefaultDataset(
            MODEL_VECTOR & models,
            const LOSS_FUNCTION_BASE_VECTOR & lossFunctionPtrs,
            const GT_VECTOR & gts,
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

        uint64_t nModels()const{
            return models_.size();
        }

        uint64_t size()const{
            return models_.size();
        }

        Model * model(const uint64_t i) {
            return &models_[i];
        }
        const Model * model(const uint64_t i)const{
            return &models_[i];
        }
        const ConfMap * groundTruth(const uint64_t i){
            return &gts_[i];
        }
        const ConfMap * groundTruth(const uint64_t i)const{
            return &gts_[i];
        }

        const LossFunctionBase * lossFunction(const uint64_t i){
            return lossFunctionPtrs_[i];
        }
        const LossFunctionBase * lossFunction(const uint64_t i)const{
            return lossFunctionPtrs_[i];
        }

        const WeightConstraints & weightConstraints()const{
            return weightConstraints_;
        }

        const Regularizer &  regularizer()const{
            return regularizer_;
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
