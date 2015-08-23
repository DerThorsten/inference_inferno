/** \file discrete_value_table_base.hxx 
    \brief inferno::value_tables::DiscreteValueTableBase
    is implemented in this header. 
*/
#ifndef INFERNO_VALUE_TABLES_DISCRETE_UNARY_VALUE_TABLE_BASE_HXX
#define INFERNO_VALUE_TABLES_DISCRETE_UNARY_VALUE_TABLE_BASE_HXX

#include <cstdint>
#include <vector>
#include <limits>

#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
    
namespace inferno{

/** \namespace inferno::value_tables 
    \brief all (discrete) value tables are implemented 
    in this namespace.

    All values tables and related functionality
    is implemented within inferno::value_tables .
*/
namespace value_tables{


class DiscreteUnaryValueTableBase :
    public DiscreteValueTableBase
{
public:
    using DiscreteValueTableBase::eval;
    using DiscreteValueTableBase::shape;

    DiscreteUnaryValueTableBase(){
    }

    virtual ~DiscreteUnaryValueTableBase() = default;
    virtual DiscreteLabel shape() const = 0;
    virtual ValueType eval(const DiscreteLabel l ) const = 0;

    virtual uint32_t arity() const final{
        return 1;
    }
    virtual DiscreteLabel shape(const uint32_t i) const final{
        return this->shape();
    }
    virtual ValueType eval(const DiscreteLabel * l) const final{
        this->eval(*l);
    }

};

///\cond
class  DeadCodeUnaryValueTable : public DiscreteUnaryValueTableBase{
public:
  
    
    DeadCodeUnaryValueTable()
    :   DiscreteUnaryValueTableBase(){
    }


    ValueType eval(const DiscreteLabel l1)const{
        return ValueType();
    }
    DiscreteLabel shape() const{
        return DiscreteLabel();
    }
private:
    const std::vector<ValueType> values_;
};
///\endcond

} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_DISCRETE_UNARY_VALUE_TABLE_BASE_HXX*/
