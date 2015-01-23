#ifndef DISCRETE_VALUE_TYPE

#include <cstdint>

typedef int64_t LabelType;
typedef int64_t ValueType;

class DiscreteValueTable{
    typedef LabelType L;

    // this should be enough as pure virtual interface
    virtual ValueType eval(const LabelType *conf)const=0;
    virtual LabelType shape(const uint32_t) const=0;
    virtual uint32_t  arity()const=0;


    // with default impl
    virtual uint32_t size()const{
        const uint32_t arity = this->arity();
        uint32_t size = 1;
        for(uint32_t d=0; d<arity; ++d){
            size *= this->shape(d);
        }
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
    virtual ValueType eval(const LabelType *conf)const{
        return conf[0] != conf[1] ? 0.0 : val_;
    }
    virtual ValueType eval(const LabelType l1, const LabelType l2)const{
        return l1!=l2 ? 0 : val_;
    }
    virtual LabelType shape(const uint32_t d) const{
        return nl_;
    }
    virtual uint32_t  arity()const{
        return nl_*nl_;
    }
private:
    LabelType nl_;
    ValueType val_;
};


class GraphicalModel{

    public:

    private:

        std::vector<DiscreteValueTable *>
};




#endif