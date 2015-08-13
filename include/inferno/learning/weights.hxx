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
        Weights(const size_t nWeights = 0 , WeightType initValue = WeightType())
        : BaseType(typename BaseType::difference_type(nWeights), initValue){

        }

        void pertubate(){

        }
    private:

    };

    typedef Weights WeightVector;
    /** \brief Weights class holding weights for structured learning
    */
    class WeightMatrix : public vigra::MultiArray<2, WeightType>{
    private:
        typedef vigra::MultiArray<2, WeightType> BaseType;
    public:
        typedef uint32_t WeightDescriptor;

        WeightMatrix(const WeightVector & weightVector, const size_t arrayLength = 0 , WeightType initValue = WeightType())
        : BaseType(typename BaseType::difference_type(weightVector.size(), arrayLength), initValue){

        }


        WeightMatrix(const size_t nWeights = 0, const size_t arrayLength = 0 , WeightType initValue = WeightType())
        : BaseType(typename BaseType::difference_type(nWeights, arrayLength), initValue){

        }

    private:

    };

} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_WEIGHTS_HXX*/
