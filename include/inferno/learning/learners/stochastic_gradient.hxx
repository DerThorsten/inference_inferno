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
                const int      verbose =2
            )
            :   nPertubations_(nPertubations),
                maxIterations_(maxIterations),
                sigma_(sigma),
                verbose_(verbose)
            {
            }

            uint64_t nPertubations_;
            uint64_t maxIterations_;
            double   sigma_;
            int      verbose_;
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

            // multiple weight-vectors stacked as matrix
            WeightMatrix            noiseMatrix(weightVector,options_.nPertubations_);
            WeightMatrix            weightMatrix(weightVector,options_.nPertubations_);   
            std::vector<LossType>   losses(options_.nPertubations_);

            // random gen
            boost::mt19937 rng; // I don't seed it on purpouse (it's not relevant)
            boost::normal_distribution<> nd(0.0, options_.sigma_);
            boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > normalDist(rng, nd);

            // indices
            utilities::IndexVector< > indices(dset.size());


            for(size_t i=0; i<options_.maxIterations_; ++i){
                std::cout<<"Iteration "<<i<<" "<<options_.maxIterations_<<"\n";

                std::cout<<"Weights : ";
                for(size_t wi=0; wi<weightVector.size(); ++wi){
                    std::cout<<weightVector[wi]<<" ";
                }
                std::cout<<"\n";

                // iterate in random order
                indices.randomShuffle();
                // FIXME indices.randomShuffle(rng);

                for(const auto trainingInstanceIndex : indices){

                    // unlock model
                    dset.unlock(trainingInstanceIndex);

                    // get model, gt, and loss-function
                    auto & model = dset.model(trainingInstanceIndex);
                    const auto & gt = dset.groundTruth(trainingInstanceIndex);
                    auto & lossFunction = dset.lossFunction(trainingInstanceIndex);


                    // pertubate (and remember noise matrix)
                    weightMatrix.pertubate(weightVector,noiseMatrix,normalDist);

                    // to remember arg mins
                    ConfMapVector confMapVector(options_.nPertubations_);
                    for(auto & cmap : confMapVector){
                        cmap.assign(model);
                    }


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
                    INFERNO_CHECK_OP(gradient.size(),>,0,"");
                    INFERNO_CHECK_OP(gradient.size(),==,weightVector.size(),"");
                    noiseMatrix.weightedSum(losses, gradient);
                    gradient /= options_.nPertubations_;
 
                    // reset the weights to the current weights
                    model.updateWeights(weightVector);
                    takeGradientStep(weightVector, gradient, i);


                    
                    dset.updateWeights(weightVector);
                    

                    // lock
                    dset.lock(trainingInstanceIndex);
                }

                std::cout<<"avergeLoss "<<dataset_.avergeLoss(inferenceFactory)<<"\n";


            }
        }
    private:



        void takeGradientStep(
            WeightVector & currentWeights,
            WeightVector & gradient,
            const uint64_t iteration
        ){
            double it(iteration+1);
            //WeightVector newWeights = currentWeights;

            const auto  effectiveStepSize = 1.0/(std::sqrt(it));
            WeightVector g=gradient;
            g*=effectiveStepSize;
            currentWeights -= g;
            currentWeights *= 1.0;
        }

        Dataset & dataset_;
        Options options_;
    };

} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_LEARNING_LEARNERS_STOCHASTIC_GRADIENT_HXX */
