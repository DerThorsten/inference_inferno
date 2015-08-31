/**     
*/
#ifndef INFERNO_LEARNING_LEARNERS_STOCHASTIC_GRADIENT_HXX
#define INFERNO_LEARNING_LEARNERS_STOCHASTIC_GRADIENT_HXX


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
    class StochasticGradient{
    public:
        typedef DATASET Dataset;
        typedef typename Dataset::Model         Model;
        typedef typename Dataset::GroundTruth   GroundTruth;
        typedef typename Dataset::LossFunction  LossFunction;
        typedef typename Model:: template VariableMap<DiscreteLabel> ConfMap;
        typedef std::vector<ConfMap> ConfMapVector;
        typedef inference::BaseDiscreteInferenceFactory<Model> InferenceFactoryBase;

        struct Options{
            Options(
                const uint64_t nPertubations = 100,
                const uint64_t maxIterations = 10000,
                const double   sigma = 1.0,
                const double   alpha  = 1.0,
                const int      verbose =2,
                const int      seed = 0,
                const double   n = 1.0
            )
            :   nPertubations_(nPertubations),
                maxIterations_(maxIterations),
                sigma_(sigma),
                alpha_(alpha),
                verbose_(verbose),
                seed_(seed),
                n_(n)
            {
            }

            uint64_t nPertubations_;
            uint64_t maxIterations_;
            double   sigma_;
            double   alpha_;
            int      verbose_;
            int seed_;
            double n_;
        };

        StochasticGradient(Dataset & dset, const Options & options = Options())
        :   dataset_(dset),
            options_(options){
        }

        Dataset & dataset(){
            return dataset_;
        }

        void learn(InferenceFactoryBase * inferenceFactory, WeightVector & weightVector){

            // get dataset
            auto & dset = dataset();
            //auto & weights = weightVector;

            bestLoss_  = dataset_.averageLoss(inferenceFactory);

            // multiple weight-vectors stacked as matrix
            WeightMatrix            noiseMatrix(weightVector,options_.nPertubations_);
            WeightMatrix            weightMatrix(weightVector,options_.nPertubations_);   
            std::vector<LossType>   losses(options_.nPertubations_);

            // random gen
            boost::mt19937 rng(options_.seed_); // I don't seed it on purpouse (it's not relevant)
            boost::normal_distribution<> nd(0.0, options_.sigma_);
            boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > normalDist(rng, nd);

            for(int ii=0; ii<10; ++ii){
                std::cout<<normalDist()<<"  ";
            }
            std::cout<<" \n";

            // indices
            utilities::IndexVector< > indices(dset.size());


            for(size_t i=0; i<options_.maxIterations_; ++i){
                //std::cout<<"Iteration "<<i<<" "<<options_.maxIterations_<<"\n";

                //std::cout<<"Weights : ";
                //for(size_t wi=0; wi<weightVector.size(); ++wi){
                //    std::cout<<weightVector[wi]<<" ";
                //}
                //std::cout<<"\n";

                // iterate in random order
                indices.randomShuffle();
                // FIXME indices.randomShuffle(rng);

                for(const auto trainingInstanceIndex : indices){

                    //std::cout<<"model "<<trainingInstanceIndex<<"\n";

                    // unlock model
                    dset.unlock(trainingInstanceIndex);

                    //std::cout<<"fetch "<<trainingInstanceIndex<<"\n";
                    // get model, gt, and loss-function
                    auto & model = dset.model(trainingInstanceIndex);
                    const auto & gt = dset.groundTruth(trainingInstanceIndex);
                    auto & lossFunction = dset.lossFunction(trainingInstanceIndex);

                    //std::cout<<"pertube \n";
                    // pertubate (and remember noise matrix)
                    weightMatrix.pertubate(weightVector,noiseMatrix,normalDist);

                    // to remember arg mins
                    //std::cout<<"conf assign \n";
                    ConfMapVector confMapVector(options_.nPertubations_);
                    for(auto & cmap : confMapVector){
                        cmap.assign(model);
                    }

                    // std::cout<<"solve loop \n";
                    // argmin for perturbed model
                    auto cc=0;
                    for(const auto & perturbedWeightVector : weightMatrix){
                        // set perturbed weights
                        model.updateWeights(perturbedWeightVector);

                        // get argmin 
                        auto inference = inferenceFactory->create(model);
                        inference->infer();
                        inference->conf(confMapVector[cc]); 


                
                        const auto l = lossFunction.eval(model, gt, confMapVector[cc]);
                        losses[cc] = l;
                        ++cc;
                    }

                    WeightVector gradient(weightVector.size(),0);
                    noiseMatrix.weightedSum(losses, gradient);
                    gradient /= options_.nPertubations_;
    
                    //for(size_t gg=0; gg<10; ++gg){
                    //    std::cout<<gradient[gg]<<"   ";
                    //}
                    //std::cout<<"\n";
                    // reset the weights to the current weights
                    model.updateWeights(weightVector);

                    takeGradientStep(inferenceFactory, weightVector, gradient, i);


                    
                    dset.updateWeights(weightVector);
                    

                    // lock
                    dset.lock(trainingInstanceIndex);
                }

                //std::cout<<"avergeLoss "<<dataset_.averageLoss(inferenceFactory)<<"\n";


            }
        }
    private:



        void takeGradientStep(
            InferenceFactoryBase * inferenceFactory,
            WeightVector & currentWeights,
            WeightVector & gradient,
            const uint64_t iteration
        ){
            double it(iteration+1);
            //WeightVector newWeights = currentWeights;

      
            const auto  effectiveStepSize = options_.n_/(it);

            auto takeStep = [&,this] (const double stepSize, bool undo = true, bool eval = true){

                WeightVector buffer = currentWeights;
                WeightVector g=gradient;
                g*=stepSize;
                currentWeights -= g;
                currentWeights *= options_.alpha_;
                dataset_.updateWeights(currentWeights);
                LossType loss = 0;
                if(eval)
                    loss = dataset_.averageLoss(inferenceFactory);
                if(undo)
                    currentWeights = buffer;
                return loss;
            };

            //std::cout<<"take step"<<takeStep(effectiveStepSize,false,true)<<"\n";


            std::vector<double> fracs({1.0, 0.1, 0.01,0.003, 2.0});
            std::vector<double> lossVal(fracs.size());

            
            bool improvment = false;

            int bestIndex = 0;
            double bestVal = infVal();

            double currentLoss;

            for(size_t i=0; i<fracs.size(); ++i){
                const double ss = effectiveStepSize*fracs[i];
                const double ll = takeStep(ss);
                lossVal[i] = ll;

                if(ll<bestLoss_){
                    //std::cout<<"improved via frac  "<<fracs[i]<<"     loss "<<ll<<"\n";
                    bestLoss_ = ll;
                    currentLoss = bestLoss_;
                    takeStep(ss, false, false);
                    improvment = true;
                    bestVal = ll;
                    bestIndex = i;
                    break;
                }
                if(ll<bestVal){
                    bestVal = ll;
                    bestIndex = i;
                }
            }
            if(!improvment){
                currentLoss  = lossVal[bestIndex];
                //std::cout<<"best via frac  "<<fracs[bestIndex]<<" loss "<<lossVal[bestIndex]<<"\n";
                const double ss = effectiveStepSize*fracs[bestIndex];
                takeStep(ss, false, false);
            }
            std::cout<<"iter "<<iteration<<" current average loss "<<currentLoss<<" frac "<<fracs[bestIndex]<<"\n";
        }

        Dataset & dataset_;
        Options options_;

        LossType bestLoss_;
    };

} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_LEARNING_LEARNERS_STOCHASTIC_GRADIENT_HXX */
