#ifndef INFERNO_LEARNING_WEIGHTS_HXX
#define INFERNO_LEARNING_WEIGHTS_HXX

// standard
#include <ctypes>

// vigra
#include <vigra/multi_array.hxx>

// inferno
#include "inferno/inferno.hxx"

namespace inferno{
namespace learning{

    /** \brief Weight class holding weights for structured learning
    */
    class Weights : public vigra::MultiArray<1, WeightType>{
    public:
        typedef uint32_t WeightDescriptor;
        Weights(const size_t nWeights, WeightType initValue = WeightType())
        : BaseType(nWeights, initValue){

        }
    private:

    };

} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_WEIGHTS_HXX*/
