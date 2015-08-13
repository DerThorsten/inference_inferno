#ifndef INFERNO_LEARNING_LEARNERS_LEARNERS_HXX
#define INFERNO_LEARNING_LEARNERS_LEARNERS_HXX

// inferno
#include "inferno/inferno.hxx"

namespace inferno{
namespace learning{

/** \namespace learners
    
    - loss augmented:
        - Struct-max-margin
        - Sub-gradient
    - loss evaluating:
        - stochastic gradient
*/
namespace learners{

    /// \brief Tag to indicate that a learner does not support any Loss
    struct NoLossTag{
    };

    /// \brief Tag to indicate that a learner does support decomposable loss function
    struct DecoposableLossTag{
    };

    /// \brief Tag to indicate that a learner does support any loss function
    struct AnyLossTag{
    };




} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LEARNERS_LEARNERS_HXX*/
