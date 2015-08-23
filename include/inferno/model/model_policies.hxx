#ifndef INFERNO_MODEL_MODEL_POLICIES_HXX
#define INFERNO_MODEL_MODEL_POLICIES_HXX

// boost
#include <boost/logic/tribool.hpp>

#include "inferno/inferno.hxx"
#include "inferno/meta/meta.hxx"

namespace inferno{
namespace models{
namespace policies{

    // equally
    template<class MODEL>
    struct VariableIdsPolicy{
        static const bool HasDenseIds = true;
        static const bool HasSortedIds = true;
        static const bool AreIdsAndDescritporsEqual = true;
        static const bool AreIdsAndDescritporsEquallySorted = true;
    };

    template<class MODEL>
    struct FactorIdsPolicy{
        static const bool HasDenseIds = true;
        static const bool HasSortedIds = true;
        static const bool AreIdAndDescritporEqual = true;
        static const bool AreIdsAndDescritporsEquallySorted = true;
    };

    template<class MODEL>
    struct UnaryIdsPolicy{
        static const bool HasDenseIds = true;
        static const bool HasSortedIds = true;
        static const bool AreIdAndDescritporEqual = true;
        static const bool AreIdsAndDescritporsEquallySorted = true;

        // are they continous
        static const bool HasContinousUnaries = false;
    };


    template<class MODEL>
    struct FactorViPolicy{
        static const bool HasSortedVis = false;
    };

    template<class MODEL>
    struct ModelPolicy{

        // label space
        typedef meta::TriboolMaybeType  HasSimpleLabelSpaceType;
        typedef meta::UnknownNumberType MaxNumberOfLabelType;
        typedef meta::Int64Type<0>      MinNumberOfLabelType;

        // arity
        typedef meta::UnknownNumberType MaxArityType;

        // unaries ?
        typedef meta::TriboolMaybeType HasUnariesType;
        typedef meta::TriboolMaybeType HasSingleUnaryForEachVariableType;
        typedef meta::TriboolMaybeType HasContinousStoredUnariesType;


        // multicut
        typedef meta::TriboolMaybeType IsSecondOrderSubmodularModelType;
        typedef meta::TriboolMaybeType IsMulticutModelType;

  
    };



} // end namespace inferno::models::policies
} // end namespace inferno::models
} // end namespace infero

#endif /* INFERNO_MODEL_MODEL_POLICIES_HXX */
