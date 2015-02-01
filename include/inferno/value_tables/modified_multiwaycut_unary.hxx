#ifndef INFERNO_VALUE_TABLES_UNARY_HXX
#define INFERNO_VALUE_TAB_MODIFIED_MULTIWATCUTL_MODIFIED_MULTIWATCUTES_UNARY_HXX

#include <cstdint>
#include <vector>


#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"

namespace inferno{


namespace value_tables{

typedef std::vector<bool> AllowCutsWithin;


struct ModifiedMultiwayCutSetup{
public:
    const bool allowCutsWithin(const DiscreteLabel semnaticLabel)const{
        return allowCutsWithin_[semnaticLabel] == 1;
    }
    DiscreteLabel getSemanticLabel(const DiscreteLabel label)const{

    }
private:
    AllowCutsWithin allowCutsWithin_;
    uint64_t multiplier_;
    uint64_t nLabelsUnary_;
};



/** \brief discrete unary function with an arity of 1.
       

    \ingroup value_tables
    \ingroup discrete_value_tables
*/
template<unsigned int N_SEMANTIC_CLASSES>
class  ModifiedMultiwayCutUnary : public DiscreteValueTableBase{
public:
    ModifiedMultiwayCutUnary(const LabelType l)
    :   DiscreteValueTableBase(),
        values_(l){
    }
    template<class ITER>
    ModifiedMultiwayCutUnary(ITER valBegin, ITER valEnd)
    :   DiscreteValueTableBase(),
        values_(valBegin, valEnd){
    }
    template<class T>
    ModifiedMultiwayCutUnary(std::initializer_list<T> values)
    :   DiscreteValueTableBase(),
        values_(values){
    }
    virtual ValueType eval(const LabelType *conf)const{
        return values_[mmcwSetup_->getSemanticLabel(conf[0])];
    }
    virtual ValueType eval(const LabelType l1)const{
        return values_[mmcwSetup_->getSemanticLabel(l1)];
    }
    virtual LabelType shape(const uint32_t d) const{
        return mmcwSetup_->nLabelsUnary_;
    }
    virtual uint32_t  arity()const{
        return 1;
    }
private:
    std::array<ValueType, N_SEMANTIC_CLASSES> values_;
    ModifiedMultiwayCutSetup * mmcwSetup_;
};


} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_MODIFIED_MULTIWATCUT_UNARY_HXX*/
