/**     
*/
#ifndef INFERNO_LEARNING_LEARNERS_SUB_GRADIENT_HXX
#define INFERNO_LEARNING_LEARNERS_SUB_GRADIENT_HXX


#include "inferno/learning/learners/learners.hxx"
#include "inferno/learning/weights/weight_averaging.hxx"
#include "inferno/utilities/index_vector.hxx"
#include "inferno/utilities/line_search/line_search.hxx"
#include "inferno/inference/base_discrete_inference_factory.hxx"


#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>



    #include <stdio.h>
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>


namespace inferno{
namespace learning{
namespace learners{



    #if 0
    inline int kbhit(void)
    {
      struct termios oldt, newt;
      int ch;
      int oldf;
     
      tcgetattr(STDIN_FILENO, &oldt);
      newt = oldt;
      newt.c_lflag &= ~(ICANON | ECHO);
      tcsetattr(STDIN_FILENO, TCSANOW, &newt);
      oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
      fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
     
      ch = getchar();
     
      tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
      fcntl(STDIN_FILENO, F_SETFL, oldf);
     
      if(ch != EOF)
      {
        ungetc(ch, stdin);
        return 1;
      }
     
      return 0;
    }
    #else
    inline int kbhit(void){
        return 0;
    }
    #endif


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
                const double   n = 1.0,
                const double   eps = 1.0e-10,
                const double   m = 0.5,
                const int      verbose = 2,
                const int      nThreads = 0,
                const int      averagingOrder = -1,
                const int      showLossEvery = 0,
                const int      showRegularizerEvery = 0
            )
            :   maxIterations_(maxIterations),
                n_(n),
                eps_(eps),
                m_(m),
                verbose_(verbose),
                nThreads_(nThreads),
                averagingOrder_(averagingOrder),
                showLossEvery_(showLossEvery),
                showRegularizerEvery_(showRegularizerEvery)
            {
            }
            uint64_t maxIterations_;
            double   n_;
            double   eps_;
            double   m_;
            int      verbose_;
            int      nThreads_;
            int      averagingOrder_;
            uint64_t showLossEvery_;
            uint64_t showRegularizerEvery_;
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

            weights::WeightAveraging weightAveraging(weightVector, options_.averagingOrder_);

            auto & dset = dataset();
            WeightVector accFeatureDiff(weightVector.size());
            WeightVector gradient(weightVector.size());
            WeightVector oldGradient(weightVector.size());
            WeightVector oldWeights = weightVector;

            for(size_t iter=0; iter<options_.maxIterations_; ++iter){

                if(kbhit())
                    break;
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
                
                const auto normalizationFactor = dset.regularizer().c() / dset.size();
                accFeatureDiff *= normalizationFactor;
                gradient = weightVector;
                gradient += accFeatureDiff;

                // take gradient step
                auto eps = takeGradientStep(inferenceFactory, weightVector,oldWeights, gradient, oldGradient, iter);
                

                weightAveraging(weightVector,weightVector);
                dset.updateWeights(weightVector);

                const auto showRegN  = options_.showRegularizerEvery_;
                const auto showLossN = options_.showLossEvery_;

                const bool showLoss = showLossN !=0 && (iter + 1) % showLossN == 0;
                const bool showReg =  showLoss || (showRegN !=0 && (iter + 1) % showRegN == 0);
                


                if(showReg || showLoss){

                    const auto regVal = dset.evalRegularizer(weightVector);
                    std::cout<<"i = "<<iter<<"\n";
           
                    const auto rString = dset.regularizer().prettyRegularizerString();
                    std::cout<<"    "<<rString<<" = "<<regVal<<"\n";

                    if(showLoss){
                        const auto lossVal = dset.totalLoss(inferenceFactory);
                        const auto cLossVal = dset.regularizer().c()*lossVal;
                        const auto lString = dset.regularizer().prettyLossSumString();
                        std::cout<<"    "<<lString<<" = "<<dset.regularizer().c()*cLossVal<<"\n";

                        const auto prettyObjectiveString = dset.regularizer().prettyObjectiveString();
                        std::cout<<"    "<<prettyObjectiveString<<" = "<< regVal + cLossVal<<"\n";

                    }
                    std::cout<<"\n";
                }
                

                if(eps<options_.eps_){
                    break;
                }

            }            
        }
    private:



        double takeGradientStep(
            InferenceFactoryBase * inferenceFactory,
            WeightVector & currentWeights,
            WeightVector & oldWeights,
            WeightVector & gradient,
            WeightVector & oldGradient,
            const uint64_t iter
        ){
            const auto diter = (double(iter+1));
            const double stepSize = options_.n_/diter;
            //const double stepSize = currentN_;



            WeightVector wBuffer = currentWeights;

            if(iter>0){
                oldGradient *= options_.m_;
                gradient *= stepSize;
                gradient += oldGradient;
            }
            else{
                gradient *= stepSize;
            }
            oldGradient = gradient;
            currentWeights -= gradient;

            // fix bounded weights
            const auto & wConstraints = dataset_.weightConstraints();
            for(const auto kv : wConstraints.weightBounds()){
                const auto wi = kv.first;
                const auto lowerBound = kv.second.first;
                const auto upperBound = kv.second.second;
                if(currentWeights[wi] < lowerBound){
                   currentWeights[wi] = lowerBound; 
                }
                if(currentWeights[wi] > upperBound){
                   currentWeights[wi] = upperBound; 
                }
            }

            dataset_.updateWeights(currentWeights);


            // compute convergence
            double dSum = 0;
            for(size_t wi=0; wi<currentWeights.size(); ++wi){
                const auto d = currentWeights[wi] - oldWeights[wi];
                dSum += d*d;
                oldWeights[wi]  = currentWeights[wi];
            }

            //currentN_*=facs[bestIndex];
                
            
            return dSum;
        }

        Dataset & dataset_;
        Options options_;
        double currentN_;
    };

} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_LEARNING_LEARNERS_SUB_GRADIENT_HXX */
