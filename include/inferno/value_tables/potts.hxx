#ifndef INFERNO_VALUE_TABLES_POTTS_HXX
#define INFERNO_VALUE_TABLES_POTTS_HXX

#include <cstdint>
#include <vector>


#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"

namespace inferno{


namespace value_tables{



/** \brief discrete potts function with an arity of 2.
       

    \ingroup value_tables
    \ingroup discrete_value_tables
*/
class  PottsValueTable : public DiscreteValueTableBase{
public:
    PottsValueTable(const LabelType l,  const ValueType beta)
    :   DiscreteValueTableBase(),
        nl_(l),
        beta_(beta){
    }
    ValueType eval(const LabelType *conf)const{
        return conf[0] == conf[1] ? 0.0 : beta_;
    }
    ValueType eval2(const LabelType l1, const LabelType l2)const{
        return l1==l2 ? 0 : beta_;
    }
    LabelType shape(const uint32_t d) const{
        return nl_;
    }
    uint32_t  arity()const{
        return 2;
    }
    bool isGeneralizedPotts() const{
        return true;
    }
    bool isPotts(ValueType & beta) const{
        beta = beta_;
        return true;
    }

    void facToVarMsg(const ValueType ** inMsgs, ValueType ** outMsgs)const{
        pottsFacToVarMsg(nl_, beta_, inMsgs, outMsgs);
    }
private:
    LabelType nl_;
    ValueType beta_;
};


} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_POTTS_HXX*/
