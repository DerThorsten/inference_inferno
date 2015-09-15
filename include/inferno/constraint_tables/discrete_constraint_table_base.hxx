/** \file discrete_value_table_base.hxx 
    \brief inferno::value_tables::DiscreteValueTableBase
    is implemented in this header. 
*/
#ifndef INFERNO_CONSTRAINT_TABLES_CONSTRAINT_TABLE_BASE_HXX
#define INFERNO_CONSTRAINT_TABLES_CONSTRAINT_TABLE_BASE_HXX

#include <cstdint>
#include <vector>
#include <limits>

#include <boost/iterator/iterator_facade.hpp>
#include "inferno/inferno.hxx"
#include "inferno/utilities/utilities.hxx"
#include "inferno/utilities/shape_walker.hxx"



    

namespace inferno{
namespace constraint_tables{

    
    class DiscreteConstraintTableBase{
    public:


        virtual ArityType arity() const = 0;
        virtual DiscreteLabel shape(const ArityType  i) const = 0;
        virtual bool feasible(const DiscreteLabel *conf) const = 0;



        virtual ValueType feasible(const DiscreteLabel l0)const{
            return this->feasible(&l0);
        }
        virtual ValueType feasible(const DiscreteLabel l0, const DiscreteLabel l1)const{
            DiscreteLabel conf[] = {l0, l1};
            return this->feasible(conf);
        }
        virtual ValueType feasible(const DiscreteLabel l0, const DiscreteLabel l1, 
                                   const DiscreteLabel l2)const{
            DiscreteLabel conf[] = {l0, l1, l2};
            return this->feasible(conf);
        }
        virtual ValueType feasible(const DiscreteLabel l0, const DiscreteLabel l1, 
                                   const DiscreteLabel l2, const DiscreteLabel l3)const{
            DiscreteLabel conf[] = {l0, l1, l2, l3};
            return this->feasible(conf);
        }
        virtual ValueType feasible(const DiscreteLabel l0, const DiscreteLabel l1, 
                                   const DiscreteLabel l2, const DiscreteLabel l3, 
                                   const DiscreteLabel l4)const{
            DiscreteLabel conf[] = {l0, l1, l2, l3, l4};
            return this->feasible(conf);
        }


        /**
         * @brief      lower bound of number of feasible solutions  
         *
         * @return     lower bound
         */
        uint64_t nFeasibleLowerBound() const{
            return 1;
        }

        /**
         * @brief      upper bound of number of feasible solutions
         *
         * @return     upper bound
         */
        uint64_t nFeasibleUpperBound() const{
            return this->size();
        }


        void repair(DiscreteLabel * infeasibleSolution) const {
            // default gives *any* feasible conf
        }


        // void repairRandomized() const {    
        // }
    };



    class VariableNodeLinearConstraintBase
    : public DiscreteConstraintTableBase
    {
    public:
        using DiscreteConstraintTableBase::feasible;

        typedef std::pair<ArityType, DiscreteLabel> IndicatorVariable;
        typedef LinearConstraint<IndicatorVariable, LpValueType, LpValueType> LinearConstraint;
        typedef std::vector<LinearConstraint> LinearConstraints;


        virtual void initalConstraints(LinearConstraints & constraints) const = 0;
        virtual void violatedConstraints(const Discreteabel * conf, LinearConstraints & constraints) const = 0;
        virtual void isLazy() const = 0;
        virtual bool feasible(const DiscreteLabel *conf) const override {
            LinearConstraints constraints;
            this->violatedConstraints(constraints);
            return constraints.size() == 0;
        }
    };



    
} // end namespace value_tables
} // end namespace inferno

#endif /* INFERNO_CONSTRAINT_TABLES_CONSTRAINT_TABLE_BASE_HXX */
