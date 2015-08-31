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
                const double   m = 0.5,
                const int      verbose = 2,
                const int      nThreads = 0
            )
            :   maxIterations_(maxIterations),
                c_(c),
                n_(n),
                m_(m),
                verbose_(verbose),
                nThreads_(nThreads)
            {
            }
            uint64_t maxIterations_;
            double   c_;
            double   n_;
            double   m_;
            int      verbose_;
            int      nThreads_;
        };

        SubGradient(Dataset & dset, const Options & options = Options())
        :   dataset_(dset),
            options_(options),
            currentN_(options.n_){
        }

        Dataset & dataset(){
            return dataset_;
        }

        void learn(
            LossAugmentedInferenceFactoryBase * lossAugmentedInferenceFactory, 
            WeightVector & weightVector,
            InferenceFactoryBase * inferenceFactory = nullptr
        ){
            auto & dset = dataset();
            WeightVector accFeatureDiff(weightVector.size());
            WeightVector gradient(weightVector.size());
            WeightVector oldGradient(weightVector.size());

            for(size_t iter=0; iter<options_.maxIterations_; ++iter){

                //if(options_.verbose_ == 2){
                //    std::cout<<"Iteration "<<iter<<"\n";
                //
                //    std::cout<<"Weights : ";
                //    for(size_t wi=0; wi<weightVector.size(); ++wi){
                //        std::cout<<weightVector[wi]<<" ";
                //    }
                //    std::cout<<"\n";
                //}
                // reset accumulatedFeatures 
                std::fill(accFeatureDiff.begin(), accFeatureDiff.end(), 0.0);


                // iterate over all training instances
                LossType totalLoss = 0;
                for(size_t trainingInstanceIndex=0; trainingInstanceIndex<dataset_.size(); ++trainingInstanceIndex){
                    

                    // unlock model
                    dset.unlock(trainingInstanceIndex);

                    auto & model = dset.model(trainingInstanceIndex);
                    const auto & gt = dset.groundTruth(trainingInstanceIndex);
                    auto & lossFunction = dset.lossFunction(trainingInstanceIndex);



                        
                    
                    // get the loss augmented model
                    LossAugmentedModel lossAugmentedModel;
                    lossFunction.makeLossAugmentedModel(model, gt, lossAugmentedModel);


                    // compute argmin
                    auto lossAugmentedInference = lossAugmentedInferenceFactory->create(lossAugmentedModel);
                    

                    lossAugmentedInference->infer();

                    LossAugmentedConfMap lossAugmentedConf(lossAugmentedModel);
                    lossAugmentedInference->conf(lossAugmentedConf);

                    // map conf
                    ConfMap modelConf(model);
                    auto lVarIter = lossAugmentedModel.variableDescriptorsBegin();
                    for(const auto var : model.variableDescriptors()){
                        modelConf[var] = lossAugmentedConf[*lVarIter];
                        ++lVarIter;
                    }

                    const auto modelEval = model.eval(modelConf);
                    const auto lossModelEval = lossAugmentedModel.eval(lossAugmentedConf);
                    const auto localLoss = modelEval - lossModelEval;

                    //std::cout<<"model         "<<modelEval<<"\n";
                    //std::cout<<"lossModelEval "<<lossModelEval<<"\n";
                    //std::cout<<"loss  "<<localLoss<<"\n";
                    totalLoss += localLoss;


                    // accumulate
                    INFERNO_CHECK_OP(accFeatureDiff.size(), ==, weightVector.size(),"");
                    model.accumulateJointFeaturesDifference(accFeatureDiff, gt, modelConf);

                    // lock
                    dset.lock(trainingInstanceIndex);       
                }
                
                const auto normalizationFactor = options_.c_ / dset.size();
                accFeatureDiff *= normalizationFactor;

                gradient = weightVector;
                gradient += accFeatureDiff;

                // take gradient step
                takeGradientStep(inferenceFactory, weightVector, gradient, oldGradient, iter);
                

            }            
        }
    private:



        void takeGradientStep(
            InferenceFactoryBase * inferenceFactory,
            WeightVector & currentWeights,
            WeightVector & gradient,
            WeightVector & oldGradient,
            const uint64_t iteration
        ){
            const auto iter = (double(iteration+1));
            const double stepSize = options_.n_/iter;
            //const double stepSize = currentN_;



            WeightVector oGrad = oldGradient;
            WeightVector wBuffer = currentWeights;

            if(iter>0){
                oldGradient *= options_.m_;
                gradient *= stepSize;
                gradient += oldGradient;
            }
            else{
                gradient *= stepSize;
            }
            currentWeights -= gradient;
            dataset_.updateWeights(currentWeights);




            //currentN_*=facs[bestIndex];
                
            if( iteration%10 == 0 ){
                std::cout<<"iteration "<<iteration<<" av loss"<<dataset_.averageLoss(inferenceFactory)<<"\n";
            }
            else{
                std::cout<<"iteration "<<iteration<<" \n";
            }
        }

        Dataset & dataset_;
        Options options_;
        double currentN_;
    };

} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_LEARNING_LEARNERS_SUB_GRADIENT_HXX */
