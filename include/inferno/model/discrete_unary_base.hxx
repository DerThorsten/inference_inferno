#ifndef INFERNO_MODEL_UNARY_BASE_HXX
#define INFERNO_MODEL_UNARY_BASE_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_unary_value_table_base.hxx"

namespace inferno{
namespace models{


/** \brief Base CRT Unary Base
*/
template<class UNARY, class MODEL>
class DiscreteUnaryBase : 
public DiscreteFactorBase< UNARY, MODEL > {


    typedef DiscreteFactorBase< UNARY, MODEL >  BaseType;

    //using BaseType::eval;

public:
    const UNARY * operator ->()const{
        return static_cast<const UNARY *>(this); 
    }


    ValueType eval(const DiscreteLabel l)const{
        return unary()->valueTable()->eval(l);
    }
    //ValueType eval(const DiscreteLabel * l)const{
    //    return unary()->valueTable()->eval(*l);
    //}

    DiscreteLabel shape(const size_t d)const{
        return unary()->shape();
    }

    //typedef typename UNARY::VariableDescriptor VariableDescriptor;

    typename MODEL::VariableDescriptor
    variable(const size_t d)const{
        return unary()->variable();
    }



private:
    const UNARY * unary()const{
        return static_cast<const UNARY *>(this);
    }
    UNARY * unary(){
        return static_cast<UNARY *>(this);
    }
};


/// \cond
template<class MODEL>
class DeadCodeUnary :
public DiscreteUnaryBase<DeadCodeUnary<MODEL>, MODEL> {
public:
    typedef DiscreteUnaryBase<DeadCodeUnary<MODEL>, MODEL > BaseType;
    typedef typename MODEL::VariableDescriptor VariableDescriptor;

    using BaseType::shape;
    using BaseType::variable;
    using BaseType::eval;


    DeadCodeUnary() : BaseType(){
    }
    const value_tables::DiscreteUnaryValueTableBase * valueTable()const{
        return vt_;
    }   

    LabelType shape( )const{
        return LabelType();
    }

    VariableDescriptor variable()const{
        return VariableDescriptor();
    }
private:
    value_tables::DeadCodeUnaryValueTable * vt_;
};
/// \endcond


} // end namespace inferno::models
} // inferno

#endif /* INFERNO_MODEL_UNARY_BASE_HXX */
