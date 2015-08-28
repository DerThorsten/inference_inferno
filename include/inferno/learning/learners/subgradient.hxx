/**     
*/
#ifndef INFERNO_LEARNING_LEARNERS_SUB_GRADIENT_HXX
#define INFERNO_LEARNING_LEARNERS_SUB_GRADIENT_HXX


#include "inferno/learning/learners/learners.hxx"
#include "inferno/utilities/index_vector.hxx"
#include "inferno/utilities/line_search/line_search.hxx"
#include "inferno/inference/base_discrete_inference_factory.hxx"


#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

namespace inferno{
namespace learning{
namespace learners{




    template<class DATASET>
    class SubGradient{
    public:
        typedef DATASET                                                             Dataset;
        typedef typename Dataset::Model                                             Model;
        typedef typename Dataset::GroundTruth                                       GroundTruth;
        typedef typename Dataset::LossFunction                                      LossFunction;
        typedef typename Model:: template VariableMap<DiscreteLabel>                ConfMap;
        typedef inference::BaseDiscreteInferenceFactory<Model>                      InferenceFactoryBase;
        typedef typename LossFunction::LossAugmentedModel                           LossAugmentedModel;
        typedef inference::BaseDiscreteInferenceFactory<LossAugmentedModel>         LossAugmentedInferenceFactoryBase;
        typedef typename LossAugmentedModel:: template VariableMap<DiscreteLabel>   LossAugmentedConfMap;
        struct Options{
            Options(
                const uint64_t maxIterations = 10000,
                const double   c = 1.0,
                const double   n = 1.0,
                const int      verbose = 2
            )
            :   maxIterations_(maxIterations),
                c_(c),
                n_(n),
                verbose_(verbose)
            {
            }
            uint64_t maxIterations_;
            double   c_;
            double   n_;
            int      verbose_;
        };

        SubGradient(Dataset & dset, const Options & options = Options())
        :   dataset_(dset),
            options_(options){
        }

        Dataset & dataset(){
            return dataset_;
        }

        void learn(
            LossAugmentedInferenceFactoryBase * lossAugmentedInferenceFactory, 
            WeightVector & weightVector
        ){
            auto & dset = dataset();
            WeightVector accFeatureDiff(weightVector.size());
            WeightVector gradient(weightVector.size());
            for(size_t iter=0; iter<options_.maxIterations_; ++iter){

                // reset accumulatedFeatures 
                accFeatureDiff = WeightType(0.0);

                // iterate over all training instances
                for(size_t trainingInstanceIndex=0; trainingInstanceIndex<dataset_.size(); ++trainingInstanceIndex){

                    // unlock model
                    dset.unlock(trainingInstanceIndex);

                    auto & model = dset.model(trainingInstanceIndex);
                    const auto & gt = dset.groundTruth(trainingInstanceIndex);
                    auto & lossFunction = dset.lossFunction(trainingInstanceIndex);

                    // get the loss augmented model
                    LossAugmentedModel lossAugmentedModel;
                    lossFunction.makeLossAugmentedModel(model, lossAugmentedModel);

                    // compute argmin
                    auto lossAugmentedInference = lossAugmentedInferenceFactory->create(lossAugmentedModel);
                    LossAugmentedConfMap lossAugmentedConf(lossAugmentedModel);
                    lossAugmentedInference->conf(lossAugmentedConf);

                    // accumulate
                    model.accumulateJointFeaturesDifference(accFeatureDiff, gt, lossAugmentedConf);

                    // lock
                    dset.lock(trainingInstanceIndex);       
                }
                // normalize accFeatureDiff
                const auto normalizationFactor = options_.c_ / dset.size();
                accFeatureDiff *= normalizationFactor;

                gradient = weightVector;
                gradient += accFeatureDiff;

                // take gradient step
                takeGradientStep(weightVector, gradient, iter);
            }            
        }
    private:



        void takeGradientStep(
            WeightVector & currentWeights,
            WeightVector & gradient,
            const uint64_t iteration
        ){
            const auto iter = double(iteration+1);
            gradient *= options_.n_/(iter);
            currentWeights -= gradient;
        }

        Dataset & dataset_;
        Options options_;
    };

} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_LEARNING_LEARNERS_SUB_GRADIENT_HXX */
