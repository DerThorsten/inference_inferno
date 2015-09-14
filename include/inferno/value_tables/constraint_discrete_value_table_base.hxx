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

    virtual bool isConstraint() const final {
        return true;
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

    /**
     * @brief      find violated constraints
     *   
     *
     * @param[in]  conf                        configuration to evaluate
     * @param[in]  maximumViolatedConstraints  for how many violated constraints
     *  should be search for.
     *   -1 means all violated constraints should be added (if tractable), 0 means it's up 
     *   to the value table to decide how many should be added, any other number 
     *   sets the maximum number of added constraints to that particular number.
     *   
     * @param[out] violatedConstraints      stores the violated constraints
     * 
     */
    virtual void violatedConstraints(
        const DiscreteLabel * conf,
        const int64_t maximumViolatedConstraints,
        IndicatorVariableConstraintVector & violatedConstraints
    ) const = 0;

    // with default impl
    virtual bool isFeasible(const DiscreteLabel * conf) const override{
        IndicatorVariableConstraintVector vc;
        this->violatedConstraints(conf, 0, vc);
        return vc.empty();
    }
private:
};



template<class MODEL>
class GlobalLinearConstraintDiscreteValueTableBase :
    public LinearConstraintDiscreteValueTableBase
{
public:
    using LinearConstraintDiscreteValueTableBase::violatedConstraints;
    typedef typename MODEL:: template VariableMap<DiscreteLabel> ConfMap;


    virtual void violatedConstraints(
        ConfMap & confMap,
        const int64_t maximumViolatedConstraints,
        IndicatorVariableConstraintVector & violatedConstraints
    )const = 0;
    
private:

};












} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_CONSTRAIN_DISCRETE_VALUE_TYPE_BASE_HXX*/
