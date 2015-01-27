#ifndef INFERNO_VALUE_TABLES_BASE_DISCRETE_VALUE_TYPE
#define INFERNO_VALUE_TABLES_BASE_DISCRETE_VALUE_TYPE

#include <cstdint>
#include <vector>


#include "inferno/inferno.hxx"
#include "inferno/utilities/shape_walker.hxx"

namespace inferno{







/// \brief DiscreteValueTable abstract base Class
///
/// \ingroup DiscreteValueTable
class DiscreteValueTable{
private:

    struct ShapeFunctor{
        ShapeFunctor(){}
        ShapeFunctor(const DiscreteValueTable * f)
        : f_(f){
        }
        DiscreteLabel operator()(const size_t d)const{
            return f_->shape(d);
        }
        const DiscreteValueTable * f_;
    };

    
public:
    typedef LabelType L;

    // this should be enough as pure virtual interface

    /// evaluate the value table for a given configuration
    /// \param conf pointer to a sequence of LabelType 
    ///     which must be as least as long as the the arity
    ///     of this factor
    /// \returns value at given conf
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

    /** \brief 
            copy the shape into a buffer
    
        This is mainly to avoid virtual calls
        and get the complete shape with a single
        virtual call if this function is overloaded.
        
        \warning
            The buffer must be preallocated and 
            must hold at least DiscreteValueTable::arity
            values


        \warning
            This default implementation itself
            calls virtual functions multiple times
    */
    virtual void bufferShape(DiscreteLabel * buffer)const{
        const size_t arity = this->arity();
        for(size_t i=0; i<arity; ++i)
            buffer[i] = this->shape(i);
    }

    /** \brief 
            copy the complete value table in 
            a buffer
     
        This is mainly to avoid virtual calls
        and get the complete shape with a single
        virtual call if this function is overloaded.

        \warning
            The buffer must be preallocated and 
            must hold at least DiscreteValueTable::size
            values


        \warning
            This default implementation itself
            has multiple virtual calls
    */
    virtual void bufferValueTable(ValueType * buffer)const{
        const size_t arity = this->arity();
        if(arity == 0){
            buffer[0] = this->eval(0l);
        }
        else if(arity == 1){
            const DiscreteLabel s[1] = {this->shape(0)};
            for(DiscreteLabel l0=0; l0<s[0]; ++l0){
                buffer[l0] = this->eval(l0);
            }
        }
        else if(arity == 2){
            int64_t c=0;
            DiscreteLabel s[2];
            this->bufferShape(s);
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                buffer[c] = this->eval(l0, l1);
                ++c;
            }
        }
        else if(arity == 3){
            int64_t c=0;
            DiscreteLabel s[3];
            this->bufferShape(s);
            for(DiscreteLabel l2=0; l2 < s[2]; ++l2)
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                buffer[c] = this->eval(l0, l1, l2);
                ++c;
            }
        }
        else if(arity == 4){
            int64_t c=0;
            DiscreteLabel s[4];
            this->bufferShape(s);
            for(DiscreteLabel l3=0; l3 < s[3]; ++l3)
            for(DiscreteLabel l2=0; l2 < s[2]; ++l2)
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                buffer[c] = this->eval(l0, l1, l2, l3);
                ++c;
            }
        }
        else if(arity == 5){
            int64_t c=0;
            DiscreteLabel s[5];
            this->bufferShape(s);
            for(DiscreteLabel l4=0; l4 < s[4]; ++l4)
            for(DiscreteLabel l3=0; l3 < s[3]; ++l3)
            for(DiscreteLabel l2=0; l2 < s[2]; ++l2)
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                buffer[c] = this->eval(l0, l1, l2, l3, l4);
                ++c;
            }
        }
        else { // (arity >= 6)

            
            const int64_t nConf = this->size();
            ConfIterator<ShapeFunctor> confIter(ShapeFunctor(this), arity, nConf);
            ConfIterator<ShapeFunctor> confEnd = confIter.getEnd();
            int64_t c=0;
            for( ; confIter != confEnd; ++confIter){
                buffer[c] = this->eval(confIter->data());
                ++c;
            }

        }
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
        return 2;
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

#endif /*INFERNO_VALUE_TABLES_BASE_DISCRETE_VALUE_TYPE*/
