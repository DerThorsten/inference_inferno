#ifndef INFERNO_VALUE_TABLES_UNARY_UNARY_HXX
#define INFERNO_VALUE_TABLES_UNARY_UNARY_HXX

#include <cstdint>
#include <vector>


#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_unary_value_table_base.hxx"
#include "inferno/utilities/small_vector.hxx"
#include "inferno/utilities/marray_wrap.hxx"
namespace inferno{


namespace value_tables{


/** \brief discrete unary function with an arity of 1.
       

    \ingroup value_tables
    \ingroup discrete_value_tables
*/
class  UnaryValueTable : public DiscreteUnaryValueTableBase{
public:
  
    
    UnaryValueTable(const LabelType l)
    :   DiscreteUnaryValueTableBase(),
        values_(size_t(l)){
    }
    template<class ITER>
    UnaryValueTable(ITER valBegin, ITER valEnd)
    :   DiscreteUnaryValueTableBase(),
        values_(valBegin, valEnd){
    }
    template<class T>
    UnaryValueTable(std::initializer_list<T> values)
    :   DiscreteUnaryValueTableBase(),
        values_(values.begin(), values.end()){
    }

    ValueType eval(const DiscreteLabel l1)const{
        return values_[l1];
    }
    DiscreteLabel shape() const{
        return values_.size();
    }
private:
    const std::vector<ValueType> values_;
};



/** \brief discrete unary function with an arity of 1.
       

    \ingroup value_tables
    \ingroup discrete_value_tables
*/
class  UnaryViewValueTable : public DiscreteUnaryValueTableBase{
public:

    UnaryViewValueTable(const DiscreteLabel nLabels, const ValueType * data)
    :   DiscreteUnaryValueTableBase(),
        nLabels_(nLabels),
        data_(data){
    }

    virtual ValueType eval(const LabelType l1)const {
        return 0.0; //data_[l1];
    }
    DiscreteLabel shape() const{
        return nLabels_;
    }
private:
    const DiscreteLabel nLabels_;
    const ValueType * data_;
};



} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_UNARY_UNARY_HXX*/
