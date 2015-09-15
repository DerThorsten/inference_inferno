#ifndef INFERNO_MODEL_CONSTRAINT_BASE_HXX
#define INFERNO_MODEL_CONSTRAINT_BASE_HXX

#include <boost/iterator/iterator_facade.hpp>

#include "inferno/inferno.hxx"
#include "inferno/utilities/utilities.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/utilities/shape_walker.hxx"

namespace inferno{
namespace models{




/** \brief Base CRT Factor Base
 * 
 * 
 *  ALl constraints MUST implement:
 * 
 *  DiscreteConstraintTableBase * contraintTable() const;
 *  VariableDescriptor  variable(ArityType i) const;
 * 
 * 
*/
template<class CONSTRAINT, class MODEL >
class DiscreteConstraintBase{

public:
    ArityType arity() const {
        return costr()->constraintTable()->arity();
    }
    DiscreteLabel shape(const ArityType  i) const {
        return costr()->constraintTable()->shape(i);
    }
    bool feasible(const DiscreteLabel *conf) const {
        return costr()->constraintTable()->feasible(conf);
    }

    ValueType feasible(const DiscreteLabel l0)const{
        return costr()->constraintTable()->feasible(l0);
    }
    ValueType feasible(const DiscreteLabel l0, const DiscreteLabel l1)const{
        return costr()->constraintTable()->feasible(l0, l1);
    }
    ValueType feasible(const DiscreteLabel l0, const DiscreteLabel l1, 
                      const DiscreteLabel l2)const{
        return costr()->constraintTable()->feasible(l0, l1, l2);
    }
    ValueType feasible(const DiscreteLabel l0, const DiscreteLabel l1, 
                      const DiscreteLabel l2, const DiscreteLabel l3)const{
        return costr()->constraintTable()->feasible(l0, l1, l2, l3);
    }
    ValueType feasible(const DiscreteLabel l0, const DiscreteLabel l1, 
                       const DiscreteLabel l2, const DiscreteLabel l3, 
                       const DiscreteLabel l4)const{
        return costr()->constraintTable()->feasible(l0, l1, l2, l3, l4);
    }
private:
    const CONSTRAINT * costr()const{
        return static_cast<const CONSTRAINT *>(this);
    }
    CONSTRAINT * costr(){
        return static_cast<CONSTRAINT *>(this);
    }
};






} // end namespace inferno::models
} // inferno

#endif /* INFERNO_MODEL_CONSTRAINT_BASE_HXX */
