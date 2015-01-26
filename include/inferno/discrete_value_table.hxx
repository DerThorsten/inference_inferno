#ifndef INFERNO_DISCRETE_VALUE_TYPE
#define INFERNO_DISCRETE_VALUE_TYPE

#include <cstdint>
#include <vector>


#include "inferno/inferno.hxx"

namespace inferno{


class DiscreteValueTable{
public:
    typedef LabelType L;

    // this should be enough as pure virtual interface
    virtual ValueType eval(const LabelType *conf)const=0;
    virtual LabelType shape(const uint32_t) const=0;
    virtual uint32_t  arity()const=0;


    // with default impl
    virtual uint64_t size()const{
        const uint32_t arity = this->arity();
        uint64_t size = 1;
        for(uint32_t d=0; d<arity; ++d){
            size *= this->shape(d);
        }
        return size;
    }

    virtual ValueType eval(const L l0)const{
        return this->eval(&l0);
    }
    virtual ValueType eval(const L l0, const L l1)const{
        L conf[] = {l0, l1};
        return this->eval(conf);
    }
    virtual ValueType eval(const L l0, const L l1, const L l2)const{
        L conf[] = {l0, l1, l2};
        return this->eval(conf);
    }
    virtual ValueType eval(const L l0, const L l1, const L l2, const L l3)const{
        L conf[] = {l0, l1, l2, l3};
        return this->eval(conf);
    }
    virtual ValueType eval(const L l0, const L l1, const L l2, const L l3, const L l4)const{
        L conf[] = {l0, l1, l2, l3, l4};
        return this->eval(conf);
    }
};


class  PottsValueTable : public DiscreteValueTable{
public:
    PottsValueTable(const LabelType l,  const ValueType beta)
    :   DiscreteValueTable(),
        nl_(l),
        beta_(beta){
    }
    virtual ValueType eval(const LabelType *conf)const{
        return conf[0] == conf[1] ? 0.0 : beta_;
    }
    virtual ValueType eval(const LabelType l1, const LabelType l2)const{
        return l1==l2 ? 0 : beta_;
    }
    virtual LabelType shape(const uint32_t d) const{
        return nl_;
    }
    virtual uint32_t  arity()const{
        return nl_*nl_;
    }
private:
    LabelType nl_;
    ValueType beta_;
};



class  UnaryValueTable : public DiscreteValueTable{
public:
    UnaryValueTable(const LabelType l)
    :   DiscreteValueTable(),
        values_(l){
    }
    template<class ITER>
    UnaryValueTable(ITER valBegin, ITER valEnd)
    :   DiscreteValueTable(),
        values_(valBegin, valEnd){
    }
    template<class T>
    UnaryValueTable(std::initializer_list<T> values)
    :   DiscreteValueTable(),
        values_(values){
    }
    virtual ValueType eval(const LabelType *conf)const{
        return values_[conf[0]];
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


}

#endif
