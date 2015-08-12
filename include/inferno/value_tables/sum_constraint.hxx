#ifndef INFERNO_VALUE_TABLES_SUM_CONSTRAINT_HXX
#define INFERNO_VALUE_TABLES_SUM_CONSTRAINT_HXX

#include <cstdint>
#include <vector>
#include <algorithm>

#include "inferno/inferno.hxx"
#include "inferno/value_tables/constraint_discrete_value_table_base.hxx"

namespace inferno{


namespace value_tables{



/** \brief discrete potts function with an arity of 2.
       

    \ingroup value_tables
    \ingroup discrete_value_tables
*/
class  SumConstraint : public ConstraintDiscreteValueTableBase{
public:
    using ConstraintDiscreteValueTableBase::eval;
    template<class T>
    SumConstraint(
        std::initializer_list<T> shape,
        const ValueType lowerBound,
        const ValueType upperBound
    )
    :   SumConstraint(shape.begin(),shape.end(), lowerBound, upperBound){
    }


    template<class SHAPE_ITERATOR>
    SumConstraint(
        SHAPE_ITERATOR shapeBegin, 
        SHAPE_ITERATOR shapeEnd,
        const ValueType lowerBound,
        const ValueType upperBound
    )
    :   ConstraintDiscreteValueTableBase(),
        shape_(shapeBegin, shapeEnd),
        lowerBound_(lowerBound),
        upperBound_(upperBound){
    }


    
    LabelType shape(const uint32_t d) const{
        return shape_[d];
    }
    

    uint32_t  arity()const{
        return shape_.size();
    }

    bool isFeasible(const DiscreteLabel * conf) const {
        const DiscreteLabel init = 0;
        const auto sum  = std::accumulate(conf, conf + arity(), init);
        return sum >= lowerBound_ && sum <= upperBound_;
    }


private:
    std::vector<DiscreteLabel> shape_;
    ValueType lowerBound_;
    ValueType upperBound_;
};




} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_SUM_CONSTRAINT_HXX*/
