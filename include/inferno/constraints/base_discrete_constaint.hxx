#ifndef INFERNO_CONSTRAINTS_BASE_DISCRETE_CONSTRAINT_HXX
#define INFERNO_CONSTRAINTS_BASE_DISCRETE_CONSTRAINT_HXX

#include <vector>

namespace inferno{


    class IndicatorVariable{
    public:
        IndicatorVariable(const Vi modelVar=0, DiscreteLabel varLabel=0)
        :   modelVar_(modelVar),
            varLabel_(varLabel){
        }
    private:
        Vi              modelVar_;
        DiscreteLabel   varLabel_;
    };


    class IndicatorVariableVector
    :   public std::vector<IndicatorVariable> {
        IndicatorVariable(const size_t i=0):
        std::vector<IndicatorVariable>(i){

        }
    };


    /** \brief Base Class for any Constraint for Discrete Models


    */
    class DiscreteConstraintBase{
    };


    /** \brief Explicit Linear Constraint
        on tuples of indicator variables.

        In this context, <B>explicit</B>  means all constraint
        can be enumerated at modeling time.

    */
    class ExplicitIndicatorVarLinearConstraint : DiscreteConstraintBase{

    };


    /** \brief Explicit Linear Constraint
        on tuples of indicator variables.

        In this context, <B>lazy</B> means that it is 
        impossible / impractical to enumerate all 
        constraints at modeling time.

    */
    class LazyIndicatorVarLinearConstraint : DiscreteConstraintBase{

    };


}

#endif /*INFERNO_CONSTRAINTS_BASE_DISCRETE_CONSTRAINT_HXX*/
