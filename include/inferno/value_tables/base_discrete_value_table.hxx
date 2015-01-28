#ifndef INFERNO_VALUE_TABLES_BASE_DISCRETE_VALUE_TYPE
#define INFERNO_VALUE_TABLES_BASE_DISCRETE_VALUE_TYPE

#include <cstdint>
#include <vector>


#include "inferno/inferno.hxx"
#include "inferno/utilities/shape_walker.hxx"

namespace inferno{





template<class VECTOR>
bool allValuesEq(const VECTOR vector) {
    for(size_t i=0;i<vector.size();++i) {
        if(vector[0]!=vector[i]) {
            return false;
        }
    }
    return true;
}


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

    /** evaluate an n-ary value table for a given configuration
        
        \param conf : pointer to a sequence of LabelType 
            which must be as least as long as the the arity
            of this factor

        \returns : value at given conf
    */
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

    /** evaluate an unary value table for a given label

        \param l0 : label 

        \returns : value at given conf
    */
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

    /** \brief check if the value table encodes a generalized potts function.
        
        A generalized potts function is a 
        function where all configures which encode the same partitioning 
        have the same value.
        A unary function is never considered as a generalized potts function.

        \warning 
            This default implementation can only detect generalized potts
            functions up to an arity of 4.
            For any value table  with arity > 4, this function will return
            false.
    */
    virtual bool isGeneralizedPotts() const{
        const auto arity = this->arity();
        if(arity==2) {
            DiscreteLabel l[2] = {0, 1};
            DiscreteLabel s[2];
            this->bufferShape(s);

            const ValueType v0 = this->eval(l);
            l[1]=0;
            const ValueType v1 = this->eval(l);



            for(l[0]=0;l[0]<s[0];++l[0]) 
            for(l[1]=0;l[1]<s[1];++l[1]) {
                if((l[0]==l[1] && this->eval(l)!=v1) || ((l[0]!=l[1] &&   !fEq(this->eval(l), v0)   )) ) return false;
               
            }

            return true;
        }

        else if(arity==3) {
            DiscreteLabel l[3] = {0, 1, 2};
            DiscreteLabel s[3];
            this->bufferShape(s);

            const ValueType v000 = this->eval(l);
            l[2]=0; l[1]=1; l[0]=1;
            const ValueType v001 = this->eval(l);
            l[2]=1; l[1]=0; l[0]=1;
            const ValueType v010 = this->eval(l);
            l[2]=1; l[1]=1; l[0]=0;
            const ValueType v100 = this->eval(l); 
            l[2]=0; l[1]=0; l[0]=0;
            const ValueType v111 = this->eval(l);
            for(l[0]=0;l[0]<s[0];++l[0]) 
            for(l[1]=0;l[1]<s[1];++l[1]) 
            for(l[2]=0;l[2]<s[2];++l[2]) {
                if((l[1]!=l[2] && l[0]!=l[2]  && l[0]!=l[1] &&   !fEq(this->eval(l),v000)   ) ) return false;
                if((l[1]!=l[2] && l[0]!=l[2]  && l[0]==l[1] &&   !fEq(this->eval(l),v001)   ) ) return false;
                if((l[1]!=l[2] && l[0]==l[2]  && l[0]!=l[1] &&   !fEq(this->eval(l),v010)   ) ) return false;
                if((l[1]==l[2] && l[0]!=l[2]  && l[0]!=l[1] &&   !fEq(this->eval(l),v100)   ) ) return false;
                if((l[1]==l[2] && l[0]==l[2]  && l[0]==l[1] &&   !fEq(this->eval(l),v111)   ) ) return false;
            }
            return true;
        } 
        else if(arity==4) {
            DiscreteLabel l[] = {0, 1, 2, 3};
            DiscreteLabel s[4];
            this->bufferShape(s);

            const ValueType v000000 = this->eval(l);
            l[3]=2; l[2]=1; l[1]=0;l[0]=0;
            const ValueType v000001 = this->eval(l);
            l[3]=2; l[2]=0; l[1]=1;l[0]=0;
            const ValueType v000010 = this->eval(l);
            l[3]=2; l[2]=0; l[1]=0;l[0]=1;
            const ValueType v000100 = this->eval(l); 
            l[3]=1; l[2]=0; l[1]=0;l[0]=0; //3-1
            const ValueType v000111 = this->eval(l);
            l[3]=0; l[2]=1; l[1]=2; l[0]=0;
            const ValueType v001000 = this->eval(l);
            l[3]=0; l[2]=1; l[1]=1; l[0]=0;
            const ValueType v001100 = this->eval(l);
            l[3]=0; l[2]=1; l[1]=0; l[0]=0; //3-1
            const ValueType v011001 = this->eval(l);
            l[3]=0; l[2]=0; l[1]=0; l[0]=1; //3-1
            const ValueType v110100 = this->eval(l); 
            l[3]=0; l[2]=0; l[1]=0; l[0]=0;
            const ValueType v111111 = this->eval(l);
            l[3]=1; l[2]=1; l[1]=0; l[0]=0;
            const ValueType v100001 = this->eval(l); 
            l[3]=1; l[2]=0; l[1]=1; l[0]=0;
            const ValueType v010010 = this->eval(l); 
            l[3]=0; l[2]=0; l[1]=1; l[0]=2;
            const ValueType v100000 = this->eval(l); 
            l[3]=0; l[2]=1; l[1]=0; l[0]=2;
            const ValueType v010000 = this->eval(l);
            l[3]=0; l[2]=0; l[1]=1; l[0]=0; //3-1
            const ValueType v101010 = this->eval(l); 


            for(l[0]=0;l[0]<s[0];++l[0]) 
            for(l[1]=0;l[1]<s[1];++l[1]) 
            for(l[2]=0;l[2]<s[2];++l[2])  
            for(l[3]=0;l[3]<s[3];++l[3]) {
                if((l[2]!=l[3] && l[1]!=l[3] && l[0]!=l[3] && l[1]!=l[2] && l[0]!=l[2]  && l[0]!=l[1] &&  !fEq(this->eval(l),v000000)   ) ) {return false;}
                if((l[2]!=l[3] && l[1]!=l[3] && l[0]!=l[3] && l[1]!=l[2] && l[0]!=l[2]  && l[0]==l[1] &&  !fEq(this->eval(l),v000001)   ) ) {return false;}
                if((l[2]!=l[3] && l[1]!=l[3] && l[0]!=l[3] && l[1]!=l[2] && l[0]==l[2]  && l[0]!=l[1] &&  !fEq(this->eval(l),v000010)   ) ) {return false;}
                if((l[2]!=l[3] && l[1]!=l[3] && l[0]!=l[3] && l[1]==l[2] && l[0]!=l[2]  && l[0]!=l[1] &&  !fEq(this->eval(l),v000100)   ) ) {return false;}
                if((l[2]!=l[3] && l[1]!=l[3] && l[0]!=l[3] && l[1]==l[2] && l[0]==l[2]  && l[0]==l[1] &&  !fEq(this->eval(l),v000111)   ) ) {return false;}

                if((l[2]!=l[3] && l[1]!=l[3] && l[0]==l[3] && l[1]!=l[2] && l[0]!=l[2]  && l[0]!=l[1] &&  !fEq(this->eval(l),v001000)   ) ) {return false;}
                if((l[2]!=l[3] && l[1]!=l[3] && l[0]==l[3] && l[1]==l[2] && l[0]!=l[2]  && l[0]!=l[1] &&  !fEq(this->eval(l),v001100)   ) ) {return false;}

                if((l[2]!=l[3] && l[1]==l[3] && l[0]!=l[3] && l[1]!=l[2] && l[0]==l[2]  && l[0]!=l[1] &&  !fEq(this->eval(l),v010010)   ) ) {return false;}
                if((l[2]!=l[3] && l[1]==l[3] && l[0]!=l[3] && l[1]!=l[2] && l[0]!=l[2]  && l[0]!=l[1] &&  !fEq(this->eval(l),v010000)   ) ) {return false;}
                if((l[2]!=l[3] && l[1]==l[3] && l[0]==l[3] && l[1]!=l[2] && l[0]!=l[2]  && l[0]==l[1] &&  !fEq(this->eval(l),v011001)   ) ) {return false;}

                if((l[2]==l[3] && l[1]==l[3] && l[0]!=l[3] && l[1]==l[2] && l[0]!=l[2]  && l[0]!=l[1] &&  !fEq(this->eval(l),v110100)   ) ) {return false;}
                if((l[2]==l[3] && l[1]==l[3] && l[0]==l[3] && l[1]==l[2] && l[0]==l[2]  && l[0]==l[1] &&  !fEq(this->eval(l),v111111)   ) ) {return false;}

                if((l[2]==l[3] && l[1]!=l[3] && l[0]!=l[3] && l[1]!=l[2] && l[0]!=l[2]  && l[0]==l[1] &&  !fEq(this->eval(l),v100001)   ) ) {return false;}
                if((l[2]==l[3] && l[1]!=l[3] && l[0]!=l[3] && l[1]!=l[2] && l[0]!=l[2]  && l[0]!=l[1] &&  !fEq(this->eval(l),v100000)   ) ) {return false;}
                if((l[2]==l[3] && l[1]!=l[3] && l[0]==l[3] && l[1]!=l[2] && l[0]==l[2]  && l[0]!=l[1] &&  !fEq(this->eval(l),v101010)   ) ) {return false;}
            }
            return true;
        }
        else{
            return false;
        }
    }

    /** \brief Check if value table encodes a potts function.

        A potts function must have an arity >=2.
        All configurations where all labels are the 
        same must have the same value, 
        and all configuration where not all labels
        are the same must have the same value.
        Therefore a potts function can have only
        two distinct values.
    */
    virtual bool isPotts() const{
        const auto arity = this->arity();
        if(arity == 1){
            return false;
        }
        else if(arity == 2){
            DiscreteLabel s[2];
            this->bufferShape(s);
            const auto vAA = this->eval(0l,0l);
            const auto vAB = this->eval(0l,1l);
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                const auto val = this->eval(l0, l1);
                if(l0==l1){
                    if(!fEq(val,vAA))
                        return false;
                }
                else if(!fEq(val,vAB)){
                        return false;
                }
            }
            return true;
        }
        else if(arity == 3){
            DiscreteLabel s[3];
            this->bufferShape(s);
            const auto vAA = this->eval(0l,0l,0l);
            const auto vAB = this->eval(0l,0l,1l);

            for(DiscreteLabel l2=0; l2 < s[2]; ++l2)
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                const auto val = this->eval(l0, l1, l2);
                if(l0==l1 && l0==l2){
                    if(!fEq(val,vAA))
                        return false;
                }
                else if(!fEq(val,vAB)){
                        return false;
                }
            }
            return true;
        }
        else if(arity == 4){
            DiscreteLabel s[4];
            this->bufferShape(s);
            const auto vAA = this->eval(0l,0l,0l,0l);
            const auto vAB = this->eval(0l,0l,0l,1l);

            for(DiscreteLabel l3=0; l3 < s[3]; ++l3)
            for(DiscreteLabel l2=0; l2 < s[2]; ++l2)
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                const auto val = this->eval(l0, l1, l2, l3);
                if(l0==l1 && l0==l2 && l0==l3){
                    if(!fEq(val,vAA))
                        return false;
                }
                else if(!fEq(val,vAB)){
                        return false;
                }
            }
            return true;
        }
        else if(arity == 5){
            DiscreteLabel s[5];
            this->bufferShape(s);
            const auto vAA = this->eval(0l,0l,0l,0l,0l);
            const auto vAB = this->eval(0l,0l,0l,0l,1l);

            for(DiscreteLabel l4=0; l4 < s[4]; ++l4)
            for(DiscreteLabel l3=0; l3 < s[3]; ++l3)
            for(DiscreteLabel l2=0; l2 < s[2]; ++l2)
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                const auto val = this->eval(l0, l1, l2, l3, l4);
                if(l0==l1 && l0==l2 && l0==l3 && l0==l4){
                    if(!fEq(val,vAA))
                        return false;
                }
                else if(!fEq(val,vAB)){
                        return false;
                }
            }
            return true;
        }
        else{
            ConfIterator<ShapeFunctor> confIter(ShapeFunctor(this), arity, this->size());
            ConfIterator<ShapeFunctor> confEnd = confIter.getEnd();

            const ValueType vAA=this->eval(confIter->data());
            ++confIter;
            const ValueType vAB=this->eval(confIter->data());
            ++confIter;
            for( ; confIter != confEnd; ++confIter){
                const auto val = this->eval(confIter->data());
                if(allValuesEq(*confIter)){
                   if(!fEq(val,vAA))
                        return false;
                }
                else if(!fEq(val,vAB)){
                        return false;
                }
            }
            return true;
        }
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
    virtual bool isGeneralizedPotts() const{
        return true;
    }
    virtual bool isPotts() const{
        return true;
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
