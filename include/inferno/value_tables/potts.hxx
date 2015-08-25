#ifndef INFERNO_VALUE_TABLES_POTTS_HXX
#define INFERNO_VALUE_TABLES_POTTS_HXX

#include <cstdint>
#include <vector>


#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/value_tables/potts_value_table_base.hxx"

namespace inferno{


namespace value_tables{



/** \brief discrete potts function with an arity of 2.
       

    \ingroup value_tables
    \ingroup discrete_value_tables
*/
class  PottsValueTable : public PottsValueTableBase{
public:
    using DiscreteValueTableBase::eval;
    PottsValueTable(const LabelType l,  const ValueType beta)
    :   PottsValueTableBase(),
        nl_(l),
        beta_(beta){
    }
    virtual LabelType shape(const uint32_t d) const override{
        return nl_;
    }

    virtual ValueType beta()const override{
        return beta_;
    }
private:
    LabelType nl_;
    ValueType beta_;
};


class  L1ValueTable : public DiscreteValueTableBase{
public:
    using DiscreteValueTableBase::eval;
    L1ValueTable(const LabelType l,  const ValueType beta)
    :   DiscreteValueTableBase(),
        nl_(l),
        beta_(beta){
    }
    ValueType eval(const LabelType l1, const LabelType l2)const{
        return beta_*std::abs(l1-l2);
    }
    ValueType eval(const LabelType *conf)const{
        return this->eval(conf[0], conf[1]);
    }
    LabelType shape(const uint32_t d) const{
        return nl_;
    }
    uint32_t  arity()const{
        return 2;
    }
    bool isGeneralizedPotts() const{
        return nl_ == 2 ? true : false;
    }
    bool isPotts(ValueType & beta) const{
        if(nl_==2){
            beta = beta_;
            return true;
        }
        return false;
    }

    void facToVarMsg(const ValueType ** inMsgs, ValueType ** outMsgs)const{
        //simple2OrderFacToVarMsg(this,nl_,inMsgs, outMsgs);
        l1FacToVarMsg(this, nl_, beta_, inMsgs, outMsgs);
    }
private:
    LabelType nl_;
    ValueType beta_;
};


class  TruncatedL1ValueTable : public DiscreteValueTableBase{
public:
    using DiscreteValueTableBase::eval;
    TruncatedL1ValueTable(const LabelType l,  const ValueType beta, const ValueType truncateAt)
    :   DiscreteValueTableBase(),
        nl_(l),
        beta_(beta),
        truncateAt_(truncateAt){
    }
    ValueType eval(const LabelType l1, const LabelType l2)const{
        const ValueType r =  beta_*std::abs(l1-l2);
        return r > truncateAt_ ? truncateAt_ : r;
    }
    ValueType eval(const LabelType *conf)const{
        return this->eval(conf[0], conf[1]);
    }
    LabelType shape(const uint32_t d) const{
        return nl_;
    }
    uint32_t  arity()const{
        return 2;
    }
    bool isGeneralizedPotts() const{
        return nl_ == 2 ? true : false;
    }
    bool isPotts(ValueType & beta) const{
        if(nl_==2){
            beta = beta_;
            return true;
        }
        return false;
    }

    void facToVarMsg(const ValueType ** inMsgs, ValueType ** outMsgs)const{
        //simple2OrderFacToVarMsg(this,nl_,inMsgs, outMsgs);
        truncatedL1FacToVarMsg(this, nl_, beta_,truncateAt_, inMsgs, outMsgs);
    }
private:
    LabelType nl_;
    ValueType beta_;
    ValueType truncateAt_;
};



} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_POTTS_HXX*/
