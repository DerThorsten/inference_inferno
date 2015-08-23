#ifndef INFERNO_INFERENCE_UTILITIES_MOVEMAKER_HXX
#define INFERNO_INFERENCE_UTILITIES_MOVEMAKER_HXX

#include <algorithm>
#include <vector>


#include "inferno/inferno.hxx"
#include "inferno/model/factors_of_variables.hxx"

namespace inferno {
namespace inference {
namespace proposal_generators {




    template<class MODEL>
    struct ProposalGenerator{
        
        // binary
        struct AlphExpansion{};
        struct AlphaBetaSwap{};

        // multilabel
        struct LabelRange{};

        // multicut problems
        struct RandomizedHierarchicalClustering{};
        struct RandomizedWatershed{};
    };




} // end namespace inferno::inference::proposal_generators
} // end namespace inferno::inference
} // end inference namespace inferno
