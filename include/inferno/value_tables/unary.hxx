#ifndef INFERNO_VALUE_TABLES_UNARY_HXX
#define INFERNO_VALUE_TABLES_UNARY_HXX

#include <cstdint>
#include <vector>


#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/utilities/small_vector.hxx"
#include "inferno/utilities/marray_wrap.hxx"
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
        values_(size_t(l)){
    }
    template<class ITER>
    UnaryValueTable(ITER valBegin, ITER valEnd)
    :   DiscreteValueTableBase(),
        values_(valBegin, valEnd){
    }
    template<class T>
    UnaryValueTable(std::initializer_list<T> values)
    :   DiscreteValueTableBase(),
        values_(values.begin(), values.end()){
    }
    ValueType eval(const LabelType * conf)const{
        return values_[*conf];
    }
    ValueType eval1(const LabelType l1)const{
        return values_[l1];
    }
    LabelType shape(const uint32_t d) const{
        return values_.size();
    }
    uint32_t  arity()const{
        return 1;
    }
private:
    const std::vector<ValueType> values_;
};



/** \brief discrete unary function with an arity of 1.
       

    \ingroup value_tables
    \ingroup discrete_value_tables
*/
class  UnaryViewValueTable : public DiscreteValueTableBase{
public:

    UnaryViewValueTable(const DiscreteLabel nLabels, const ValueType * data)
    :   DiscreteValueTableBase(),
        nLabels_(nLabels),
        data_(data){
    }
    ValueType eval(const LabelType * conf)const{
        return 0.0; //data_[*conf];
    }
    ValueType eval1(const LabelType l1)const{
        return 0.0; //data_[l1];
    }
    LabelType shape(const uint32_t d) const{
        return nLabels_;
    }
    uint32_t  arity()const{
        return 1;
    }
private:
    const DiscreteLabel nLabels_;
    const ValueType * data_;
};



} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_UNARY_HXX*/
