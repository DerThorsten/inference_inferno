#ifndef INFERNO_VALUE_TABLES_UNARY_HXX
#define INFERNO_VALUE_TABLES_UNARY_HXX

#include <cstdint>
#include <vector>


#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"

namespace inferno{


namespace value_tables{


/** \brief discrete unary function with an arity of 1.
       

    \ingroup value_tables
    \ingroup discrete_value_tables
*/
class  UnaryValueTable : public DiscreteValueTableBase{
public:
    UnaryValueTable(const LabelType l)
    :   DiscreteValueTableBase(),
        values_(l){
    }
    template<class ITER>
    UnaryValueTable(ITER valBegin, ITER valEnd)
    :   DiscreteValueTableBase(),
        values_(valBegin, valEnd){
    }
    template<class T>
    UnaryValueTable(std::initializer_list<T> values)
    :   DiscreteValueTableBase(),
        values_(values){
    }
    virtual ValueType eval(const LabelType * conf)const{
        return values_[*conf];
    }
    virtual ValueType eval(const LabelType l1)const{
        return values_[l1];
    }
    virtual LabelType shape(const uint32_t d) const{
        return values_.size();
    }
    virtual uint32_t  arity()const{
        return 1;
    }
private:
    std::vector<ValueType> values_;
};


} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_UNARY_HXX*/
