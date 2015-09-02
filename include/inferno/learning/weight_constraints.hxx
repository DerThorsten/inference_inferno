#ifndef INFERNO_LEARNING_WEIGHT_CONSTRAINTS_HXX
#define INFERNO_LEARNING_WEIGHT_CONSTRAINTS_HXX

// std
#include <map>

// inferno
#include "inferno/learning/weights.hxx"
#include "inferno/utilities/linear_constraint.hxx"

namespace inferno{
namespace learning{



    class WeightConstraints{
    public:
        typedef utilities::LinearConstraint<uint64_t, ValueType, ValueType> Constraint;
        typedef std::pair<WeightType, WeightType> LowerUpperPair;
        typedef std::pair<uint64_t, double >      WeightIdCoeffPair;

        WeightConstraints(const uint64_t nWeights = 0)
        :   nWeights_(nWeights),
            constraints_(),
            weightBounds_(){
        }
        void addBound(
            const uint64_t weightId, 
            const WeightType lowerBound, 
            const WeightType upperBound
        ){
            const auto val = LowerUpperPair(lowerBound, upperBound);
            weightBounds_.insert(std::make_pair(weightId, val));
        }

        template<class WEIGHT_ID_ITER, class COEFF_ITER>
        void addConstraint(
            WEIGHT_ID_ITER weightIdsBegin,
            WEIGHT_ID_ITER weightIdsEnd,
            COEFF_ITER coeffsBegin,
            const WeightType lowerBound,
            const WeightType upperBound
        ){
            constraints_.emplace_back(weightIdsBegin, weightIdsEnd, 
                                      coeffsBegin, lowerBound, 
                                      upperBound);
        }
    private:
               
        uint64_t nWeights_;
        std::vector<Constraint> constraints_;
        std::map<uint64_t ,LowerUpperPair > weightBounds_;
        
    };

} // end namespace inferno::learning
} // end namespace inferno 

#endif /*INFERNO_LEARNING_WEIGHT_CONSTRAINTS_HXX*/
