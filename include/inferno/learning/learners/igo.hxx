/**     
*/
#ifndef INFERNO_LEARNING_LEARNERS_IGO_HXX
#define INFERNO_LEARNING_LEARNERS_IGO_HXX


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
    class Igo{
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
                const uint64_t nElites = 10,
                const uint64_t maxIterations = 10000,
                const double   sigma = 1.0,
                const int      verbose =2,
                const int      seed = 0,
                const double   beta = 0.5
            )
            :   nPertubations_(nPertubations),
                nElites_(nElites),
                maxIterations_(maxIterations),
                sigma_(sigma),
                verbose_(verbose),
                seed_(seed),
                beta_(beta)
            {
            }

            uint64_t nPertubations_;
            uint64_t nElites_;
            uint64_t maxIterations_;
            double   sigma_;
            double   alpha_;
            int      verbose_;
            int seed_;
            double n_;
            double beta_;
        };

        Igo(Dataset & dset, const Options & options = Options())
        :   dataset_(dset),
            options_(options){
        }

        Dataset & dataset(){
            return dataset_;
        }

        void learn(InferenceFactoryBase * inferenceFactory, WeightVector & weightVector){

            WeightVector eliteMean(weightVector.size());
            vars_.resize(weightVector.size(), options_.sigma_);

            ValueType bestLoss = dataset_.averageLoss(inferenceFactory);

            // get dataset
            auto & dset = dataset();


            // multiple weight-vectors stacked as matrix
            WeightMatrix            weightMatrix(weightVector,options_.nPertubations_);   
            std::vector<LossType>   losses(options_.nPertubations_);
            utilities::IndexVector< > randIndices(options_.nPertubations_);

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

                indices.randomShuffle();
                // FIXME indices.randomShuffle(rng);

                for(const auto trainingInstanceIndex : indices){

                    dset.unlock(trainingInstanceIndex);

                    auto & model = dset.model(trainingInstanceIndex);
                    const auto & gt = dset.groundTruth(trainingInstanceIndex);
                    auto & lossFunction = dset.lossFunction(trainingInstanceIndex);

                    // get random weight vector
                    weightMatrix.gaussianWeights(weightVector, vars_, rng);
                    
                    auto  cc = 0;
                    for(auto & rWeightVector : weightMatrix){

                        model.updateWeights(rWeightVector);

                        auto inference = inferenceFactory->create(model);
                        ConfMap conf(model);
                        inference->infer();
                        inference->conf(conf); 
                        const auto l = lossFunction.eval(model, gt, conf);
                        //std::cout<<"l "<<l<<"\n";
                        losses[cc] = l;
                        ++cc;
                    }
                    randIndices.reset();
                    vigra::indexSort(losses.begin(), losses.end(), randIndices.begin(), std::less<ValueType>());

                    std::fill(eliteMean.begin(), eliteMean.end(), 0.0);
                    for(size_t e=0; e<options_.nElites_; ++e){
                        eliteMean += weightMatrix[randIndices[e]]; 
                    }
                    eliteMean /= options_.nElites_;
                    // lock
                    dset.lock(trainingInstanceIndex);
                }
                WeightVector tmp = weightVector;
                tmp *= (1.0-options_.beta_);
                eliteMean *= options_.beta_;
                tmp += eliteMean;
                dset.updateWeights(tmp);
                const ValueType newLoss = dataset_.averageLoss(inferenceFactory);
                if(newLoss < bestLoss){
                    bestLoss = newLoss;
                    weightVector = tmp;
                    dset.updateWeights(weightVector);
                }
                else{
                    dset.updateWeights(weightVector);
                }
                std::cout<<"best loss "<<bestLoss<<"\n";

            }
        }
    private:

        Dataset & dataset_;
        Options options_;
        
        std::vector<ValueType> means_;
        std::vector<ValueType> vars_;
    };

} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_LEARNING_LEARNERS_IGO_HXX */
