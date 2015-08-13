/**     
*/
#ifndef INFERNO_LEARNING_LEARNERS_STOCHASTIC_GRADIENT_HXX
#define INFERNO_LEARNING_LEARNERS_STOCHASTIC_GRADIENT_HXX


namespace inferno{
namespace learning{
namespace learners{




    template<class MODEL, class LOSS_FUNCTION>
    class StochasticGradient{
    public:
        typedef MODEL Model;
        typedef inference::BaseDiscreteInferenceFactory<Model> InferenceFactoryBase;

        struct Options{
            Options(

            ){

            }

            uint64_t nPertubations_;
            uint64_t maxIterations_;
        };


        template<class DATASET>
        void learn(DATASET & dataset, InferenceFactoryBase * inferenceFactory, WeightVector & weights){


            WeightMatrix weightMatrix(weights,options_.nPertubations_);   
            
            for(size_t i=0; i<options_.maxIterations_; ++i){

                // iterate in random order
                
            }
        }
    private:
        Options options_;

    };

} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_LEARNING_LEARNERS_STOCHASTIC_GRADIENT_HXX */