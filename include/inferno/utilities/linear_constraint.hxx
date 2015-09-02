#ifndef INFERNO_UTILITIES_LINEAR_CONSTRAINT_HXX
#define INFERNO_UTILITIES_LINEAR_CONSTRAINT_HXX


namespace inferno{
namespace utilities{


template<
    class INDEX,
    class COEFFICIENT,
    class BOUND
>
class LinearConstraint{

public:
    typedef INDEX Index;
    typedef COEFFICIENT Coefficient;
    typedef BOUND Bound;
private:
    typedef std::pair<Index, Coefficient> IndexCoefficientPair;
public:
    
    LinearConstraint()
    :   indicesAndCoefficients_(),
        lowerBound_(-1.0*std::numeric_limits<BOUND>::infinity()),
        upperBound_(std::numeric_limits<BOUND>::infinity()){
    }
    
    template<
        class WEIGHT_INDEX_ITER, 
        class COEFFICIENT_INDEX_ITER
    >
    LinearConstraint(
        WEIGHT_INDEX_ITER weightIndicesBegin,
        WEIGHT_INDEX_ITER weightIndicesEnd,
        COEFFICIENT_INDEX_ITER coefficientsBegin,
        const ValueType lowerBound = -1.0*std::numeric_limits<BOUND>::infinity(),
        const ValueType upperBound = std::numeric_limits<BOUND>::infinity()
    )
    :   indicesAndCoefficients_(),
        lowerBound_(lowerBound),
        upperBound_(upperBound)
    {
        while(weightIndicesBegin != weightIndicesEnd){
            IndexCoefficientPair p(*weightIndicesBegin, *coefficientsBegin);
            indicesAndCoefficients_.push_back(p);
            ++weightIndicesBegin;
            ++coefficientsBegin;
        }
    }

 

private:


    std::vector<IndexCoefficientPair> indicesAndCoefficients_;
    Bound lowerBound_;
    Bound upperBound_;
};

} // end namespace inferno::utilities
} // end namespace inferno

#endif /*INFERNO_UTILITIES_LINEAR_CONSTRAINT_HXX*/
