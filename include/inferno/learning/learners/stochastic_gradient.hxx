/**     
*/
#ifndef INFERNO_LEARNING_LEARNERS_STOCHASTIC_GRADIENT_HXX
#define INFERNO_LEARNING_LEARNERS_STOCHASTIC_GRADIENT_HXX


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
        typedef typename Dataset::GtConf        GtConf;
        typedef typename Dataset::LossFunction  LossFunction;

        typedef inference::BaseDiscreteInferenceFactory<Model> InferenceFactoryBase;

        struct Options{
            Options(

            ){

            }

            uint64_t nPertubations_;
            uint64_t maxIterations_;
            double sigma_;
        };

        StochasticGradient(Dataset & dset, const Options & options)
        :   dataset_(dset),
            options_(options){
        }

        Dataset & dataset(){
            return dataset_;
        }

        template<class DATASET>
        void learn(InferenceFactoryBase & inferenceFactory){

            // get dataset
            auto & dset = dataset();

            WeightType weights = //....

            // multiple weight-vectors stacked as matrix
            WeightMatrix weightMatrix(weights,options_.nPertubations_);   
            
            // random gen
            boost::mt19937 rng; // I don't seed it on purpouse (it's not relevant)
            boost::normal_distribution<> nd(0.0, sigma_);
            boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > normalDist(rng, nd);

            // indices
            IndexVector< > indices(dset.size());


            for(size_t i=0; i<options_.maxIterations_; ++i){

                // iterate in random order
                indices.randomShuffle(rng);

                for(const auto trainingInstanceIndex : indices){

                    // unlock model
                    dset.unlock(trainingInstanceIndex);

                    // get model, gt, and loss-function
                    auto & model = dset.model(trainingInstanceIndex);
                    const auto & gt = dset.gt(trainingInstanceIndex);
                    auto & lossFunction = dset.lossFunction(trainingInstanceIndex);


                    // get perturbed weight matrix
                    weightMatrix.pertubate(weights, normalDist);


                    // argmin for perturbed model
                    for(const auto & perturbedWeightVector : weightMatrix){
                        // set perturbed weights
                        model.updateWeights(perturbedWeightVector);

                        // get argmin 

                    }


                    // accumulate gradients
                    WeightType gradient(weights.size());

                    for( ){

                    }

                    // take gradient step
                    WeightType weightAfterStep(weights);


                    // line-seach lambda
                    auto  evalGradientStep = [&] (const WeightType & stepSize){
                        weightAfterStep = stepSize*gradient;
                        dset.updateWeights(weightAfterStep);
                        return dset.eval(inferenceFactory)
                    }

                    // do the line search
                    utilities::line_search::BinarySearch lineSearch;
                    optStepSize = lineSearch(evalGradientStep,0.0001, 10.0);

                    // apply opt step size
                    weightAfterStep = stepSize*optStepSize;
                    dset.updateWeights(weightAfterStep);

                    // lock
                    dset.lock(trainingInstanceIndex);
                }




            }
        }
    private:
        Options options_;

    };

} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_LEARNING_LEARNERS_STOCHASTIC_GRADIENT_HXX */
