#ifndef INFERNO_MODEL_UNARY_BASE_HXX
#define INFERNO_MODEL_UNARY_BASE_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/utilities/shape_walker.hxx"


namespace inferno{
namespace models{




/** \brief Base CRT Unary Base
*/
template<class UNARY >
class DiscreteUnaryBase{

public:
    const UNARY * operator ->()const{
        return static_cast<const UNARY *>(this); 
    }


    ValueType eval(const DiscreteLabel l)const{
        return unary()->unaryValueTable()->eval(l);
    }
private:
    const UNARY * unary()const{
        return static_cast<const UNARY *>(this);
    }
    UNARY * unary(){
        return static_cast<UNARY *>(this);
    }
};



} // end namespace inferno::models
} // inferno

#endif /* INFERNO_MODEL_UNARY_BASE_HXX */
