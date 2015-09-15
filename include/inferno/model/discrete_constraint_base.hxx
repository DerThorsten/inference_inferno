#ifndef INFERNO_MODEL_CONSTRAINT_BASE_HXX
#define INFERNO_MODEL_CONSTRAINT_BASE_HXX

#include <boost/iterator/iterator_facade.hpp>

#include "inferno/inferno.hxx"
#include "inferno/utilities/utilities.hxx"
#include "inferno/constraint_tables/discrete_constraint_table_base.hxx"
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

    const CONSTRAINT * operator ->()const{
        return static_cast<const CONSTRAINT *>(this); 
    }

private:
    const CONSTRAINT * costr()const{
        return static_cast<const CONSTRAINT *>(this);
    }
    CONSTRAINT * costr(){
        return static_cast<CONSTRAINT *>(this);
    }
};



template<class MODEL>
class DeadCodeConstraint :
public DiscreteConstraintBase< DeadCodeConstraint<MODEL>,   MODEL>
{
public:
    typedef typename MODEL::VariableDescriptor VariableDescriptor;
    const constraint_tables::DiscreteConstraintTableBase * constraintTable() const{\
        return base_;
    }
    VariableDescriptor  variable(ArityType i) const{
        return VariableDescriptor();
    }

private:
    constraint_tables::DiscreteConstraintTableBase * base_;
};







} // end namespace inferno::models
} // inferno

#endif /* INFERNO_MODEL_CONSTRAINT_BASE_HXX */
