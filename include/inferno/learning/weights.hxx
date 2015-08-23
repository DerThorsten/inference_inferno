#ifndef INFERNO_LEARNING_WEIGHTS_HXX
#define INFERNO_LEARNING_WEIGHTS_HXX

// vigra
#include <vigra/multi_array.hxx>

// inferno
#include "inferno/inferno.hxx"

namespace inferno{
namespace learning{




    /** \brief Weights class holding weights for structured learning
    */
    class Weights : public vigra::MultiArray<1, WeightType>{
    private:
        typedef vigra::MultiArray<1, WeightType> BaseType;
    public:
        typedef uint32_t WeightDescriptor;
        Weights(const size_t nWeights = 0 , const WeightType initValue = WeightType())
        : BaseType(typename BaseType::difference_type(nWeights), initValue){

        }

    private:

    };

    typedef Weights WeightVector;
    /** \brief Weights class holding weights for structured learning
    */
    class WeightMatrix : public std::vector< WeightVector >{
    private:
        typedef std::vector< WeightVector > BaseType;
    public:
        typedef uint32_t WeightDescriptor;

        WeightMatrix(const WeightVector & weightVector, const size_t arrayLength = 0)
        : BaseType(arrayLength, weightVector){

        }

        template<class F>
        void pertubate(const WeightVector & source, F && functor){
            for(auto & wVec : *this){
                for(size_t wi=0; wi<wVec.size(); ++wi){
                    wVec[wi] = source[wi] + functor(); 
                }
            }
        }
    private:

    };

} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_WEIGHTS_HXX*/
