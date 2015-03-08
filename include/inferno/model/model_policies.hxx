#ifndef INFERNO_MODEL_MODEL_POLICIES_HXX
#define INFERNO_MODEL_MODEL_POLICIES_HXX


#include "inferno/inferno.hxx"


namespace inferno{
namespace models{
namespace policies{

    template<class MODEL>
    struct VariableIdsPolicy{
        static const bool HasDenseIds = true;
        static const bool HasSortedIds = true;
    };

    template<class MODEL>
    struct FactorIdsPolicy{
        static const bool HasDenseIds = true;
        static const bool HasSortedIds = true;
    };


    template<class MODEL>
    struct FactorViPolicy{
        static const bool HasSortedVis = false;
    };



} // end namespace inferno::models::policies
} // end namespace inferno::models
} // end namespace infero

#endif /* INFERNO_MODEL_MODEL_POLICIES_HXX */
