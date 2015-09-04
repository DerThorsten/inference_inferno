/** \file discrete_value_table_base.hxx 
    \brief inferno::value_tables::DiscreteValueTableBase
    is implemented in this header. 
*/
#ifndef INFERNO_VALUE_TABLES_CONSTRAIN_DISCRETE_VALUE_TYPE_BASE_HXX
#define INFERNO_VALUE_TABLES_CONSTRAIN_DISCRETE_VALUE_TYPE_BASE_HXX

#include <cstdint>
#include <vector>
#include <limits>

#include "inferno/inferno.hxx"
#include "inferno/utilities/linear_constraint.hxx"
#include "inferno/value_tables/value_table_utilities.hxx"
    
namespace inferno{

/** \namespace inferno::value_tables 
    \brief all (discrete) value tables are implemented 
    in this namespace.

    All values tables and related functionality
    is implemented within inferno::value_tables .
*/
namespace value_tables{


class ConstraintDiscreteValueTableBase : public DiscreteValueTableBase
{
public:
    using DiscreteValueTableBase::eval;
    
    ConstraintDiscreteValueTableBase()
    :   DiscreteValueTableBase(){
    }
    virtual ~ConstraintDiscreteValueTableBase(){
    }



    virtual bool isFeasible(const DiscreteLabel * conf) const = 0;


    virtual ValueType eval(const DiscreteLabel * conf)const{
        return isFeasible(conf) ? 0.0 : infVal();
    }

    virtual ValueType evalSoft(const DiscreteLabel * conf, const ValueType softInf)const{
        return isFeasible(conf) ? 0.0 : softInf;
    }
};




class DiscreteValueTableIndicatorVariable{
public:

private:
    std::vector<ArityType>      valueTableVars_;
    std::vector<DiscreteLabel>  valueTableLabels_;
};


using DiscreteValueTableIndicatorVariableConstraint = utilities::LinearConstraint<
    DiscreteValueTableIndicatorVariable, double, double  
>;


/// \brief Constraint suitable for (I)-LP Solvers
class LinearConstraintDiscreteValueTableBase : public ConstraintDiscreteValueTableBase
{
public:
    typedef DiscreteValueTableIndicatorVariableConstraint IndicatorVariableConstraint;
    typedef std::vector<IndicatorVariableConstraint>  IndicatorVariableConstraintVector;

    LinearConstraintDiscreteValueTableBase()
    :   ConstraintDiscreteValueTableBase(){
    }
    virtual ~LinearConstraintDiscreteValueTableBase(){
    }

    
    virtual void violatedConstraints(
        const DiscreteLabel * conf,
        IndicatorVariableConstraintVector & violatedConstraints
    ) const = 0;

    // with default impl
    virtual bool isFeasible(const DiscreteLabel * conf) const override{
        IndicatorVariableConstraintVector vc;
        this->violatedConstraints(conf, vc);
        return vc.empty();
    }
private:


};













} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_CONSTRAIN_DISCRETE_VALUE_TYPE_BASE_HXX*/
