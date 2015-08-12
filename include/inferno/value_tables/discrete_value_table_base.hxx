/** \file discrete_value_table_base.hxx 
    \brief inferno::value_tables::DiscreteValueTableBase
    is implemented in this header. 
*/
#ifndef INFERNO_VALUE_TABLES_BASE_DISCRETE_VALUE_TYPE
#define INFERNO_VALUE_TABLES_BASE_DISCRETE_VALUE_TYPE

#include <cstdint>
#include <vector>
#include <limits>

#include "inferno/inferno.hxx"
#include "inferno/utilities/shape_walker.hxx"
#include "inferno/value_tables/value_table_utilities.hxx"
    
namespace inferno{

/** \namespace inferno::value_tables 
    \brief all (discrete) value tables are implemented 
    in this namespace.

    All values tables and related functionality
    is implemented within inferno::value_tables .
*/
namespace value_tables{





/// \cond
template<class VECTOR>
bool allValuesEq(const VECTOR vector) {
    for(size_t i=0;i<vector.size();++i) {
        if(vector[0]!=vector[i]) {
            return false;
        }
    }
    return true;
}
/// \endcond

/** \brief DiscreteValueTableBase abstract base Class

    Any value table within inferno must derive from 
    this class.
    The only pure virtual functions are:
        - eval
        - shape
        - arity

    All other virtual functions have 
    a default implementation.
    For maximum speed, derived classes
    should overwrite as many 
    virtual functions as possible.
    The reason why only 3 of the virtual 
    functions are pure virtual is 
    rapid prototyping.
 

 \ingroup DiscreteValueTableBase
*/
class DiscreteValueTableBase{
private:

    struct ShapeFunctor{
        ShapeFunctor()
        : f_(NULL){}
        ShapeFunctor(const DiscreteValueTableBase * f)
        : f_(f){
        }
        DiscreteLabel operator()(const size_t d)const{
            return f_->shape(d);
        }
        const DiscreteValueTableBase * f_;
    };
    
    struct ConfRange{
        typedef ConfIterator< ShapeFunctor > const_iterator;
        ConfRange(const DiscreteValueTableBase * factor)
        :   begin_(factor->confIter()),
            end_()
        {
            end_ = begin_.getEnd();
        }
        const_iterator begin()const{
            return begin_;
        }
        const_iterator end()const{
            return end_;
        }
        const_iterator begin_;
        const_iterator end_;
    };
    
public:
    typedef LabelType L;

    virtual ~DiscreteValueTableBase(){
    }

    /** \brief evaluate an n-ary value table for a given configuration
        
        \param conf : pointer to a sequence of LabelType 
            which must be as least as long as the the arity
            of this factor

        \returns : value at given conf
    */
    virtual ValueType eval(const LabelType *conf)const=0;
    /** \brief shape of the function at axis 'i'. 
        
        The qrity of a value table / 
        function has as many `axis` defines 
        how many axis exist.
        And for each axis, shape will return
        the number of labels / configuration
        for this particular axis.

        \param i index of the axis.
            i must b in 0<= i < arity.

    */
    virtual LabelType shape(const uint32_t i) const=0;
    /** \brief arity of the value table.
        
        Number of variables the function / value table depends on.
        A unary has an arity of one.
        A binary of two and so on.

        \warning constant functions have the arity
        zero and are allowed.
    */
    virtual uint32_t  arity()const=0;


    /** \brief number of configurations in a value table.
        
        For a unary function, the size is equivalent to
        shape(0). For a binary function shape(0)*shape(1).
        For an n-ary function it is shape(0)*shape(1)*...*shape(n-1).
    */
    virtual uint64_t size()const{
        const uint32_t arity = this->arity();
        uint64_t size = 1;
        for(uint32_t d=0; d<arity; ++d){
            size *= this->shape(d);
        }
        return size;
    }


    /** \brief evaluate an unary 1-ary value table for a given label
            (or 0-ary value table)

        \param l0 : label of only variable of the value table.
            If the function has an arity of zero, l0 must
            be zero, otherwise the result will be undefined.

        \warning a function with an arity different to one or zero.
            will have undefined behavior if this
            function is called

        \returns : value at given configuration
    */
    virtual ValueType eval(const L l0)const{
        return this->eval(&l0);
    }
    /** \brief evaluate an binary / 2-ary value table for a given label

        \param l0 : label of the first variable
        \param l1 : label of the second variable

        \warning a function with an arity different to two
            will have undefined behavior if this
            function is called

        \returns : value at given configuration
    */
    virtual ValueType eval(const L l0, const L l1)const{
        L conf[] = {l0, l1};
        return this->eval(conf);
    }
    /** \brief evaluate an ternary / 3-ary value table for a given label

        \param l0 : label of the first variable
        \param l1 : label of the second variable
        \param l2 : label of the third variable
        \param l3 : label of the fourth variable

        \warning a function with an arity different to 3
            will have undefined behavior if this
            function is called

        \returns : value at given configuration
    */
    virtual ValueType eval(const L l0, const L l1, const L l2)const{
        L conf[] = {l0, l1, l2};
        return this->eval(conf);
    }
    /** \brief evaluate an 4-ary value table for a given label

        \param l0 : label of the first variable
        \param l1 : label of the second variable
        \param l2 : label of the third variable
        \param l3 : label of the fourth variable

        \warning a function with an arity different to four
            will have undefined behavior if this
            function is called

        \returns : value at given configuration
    */
    virtual ValueType eval(const L l0, const L l1, const L l2, const L l3)const{
        L conf[] = {l0, l1, l2, l3};
        return this->eval(conf);
    }
    /** \brief evaluate an 5-ary value table for a given label

        \param l0 : label of the first variable
        \param l1 : label of the second variable
        \param l2 : label of the third variable
        \param l3 : label of the fourth variable
        \param l4 : label of the fifth variable

        \warning a function with an arity different to five
            will have undefined behavior if this
            function is called

        \returns : value at given configuration
    */
    virtual ValueType eval(const L l0, const L l1, const L l2, const L l3, const L l4)const{
        L conf[] = {l0, l1, l2, l3, l4};
        return this->eval(conf);
    }
    /** \brief check if the value table encodes a generalized potts function.
        
        A generalized potts function is a 
        function where all configurations which encode the same partitioning 
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
        
        \param[out] beta : if the function is a potts 
            function beta will be :  beta * (x_0!=x_1).
            To be precise, beta is f(0,0) - f(1,0) in 
            the second order case for a potts function.
            If the function is not potts function
            the value beta is not changed

        A potts function must have an arity >=2.
        All configurations where all labels are the 
        same must have the same value, 
        and all configuration where not all labels
        are the same must have the same value.
        Therefore a potts function can have only
        two distinct values.

    */
    virtual bool isPotts(ValueType & beta) const{
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
            beta = vAB - vAA;
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
            beta = vAB - vAA;
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
            beta = vAB - vAA;
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
            beta = vAB - vAA;
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
            beta = vAB - vAA;
            return true;
        }
    }

    ConfIterator< ShapeFunctor > confIter()const{
        ShapeFunctor shape(this);
        return ConfIterator< ShapeFunctor >(shape, this->arity(), this->size());
    }
    ConfRange confs()const{
        return ConfRange(this);
    }


    ValueType argmin(DiscreteLabel * conf)const{
        const size_t arity = this->arity();
        ValueType minVal = std::numeric_limits<ValueType>::infinity();
        if(arity == 1){
            const DiscreteLabel s[1] = {this->shape(0)};
            for(DiscreteLabel l0=0; l0<s[0]; ++l0){
                const ValueType v = this->eval(l0);
                if(v<minVal){
                    conf[0] = l0;
                    minVal = v;
                }
            }
        }
        else if(arity == 2){
            DiscreteLabel s[2];
            this->bufferShape(s);
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                const ValueType v = this->eval(l0, l1);
                if(v<minVal){
                    conf[0] = l0;
                    conf[1] = l1;
                    minVal = v;
                }
            }
        }
        else if(arity == 3){
            DiscreteLabel s[3];
            this->bufferShape(s);
            for(DiscreteLabel l2=0; l2 < s[2]; ++l2)
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                const ValueType v = this->eval(l0, l1, l2);
                if(v<minVal){
                    conf[0] = l0;
                    conf[1] = l1;
                    conf[2] = l2;
                    minVal = v;
                }
            }
        }
        else if(arity == 4){
            DiscreteLabel s[4];
            this->bufferShape(s);
            for(DiscreteLabel l3=0; l3 < s[3]; ++l3)
            for(DiscreteLabel l2=0; l2 < s[2]; ++l2)
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                const ValueType v = this->eval(l0, l1, l2, l3);
                if(v<minVal){
                    conf[0] = l0;
                    conf[1] = l1;
                    conf[2] = l2;
                    conf[3] = l3;
                    minVal = v;
                }
            }
        }
        else if(arity == 5){
            DiscreteLabel s[5];
            this->bufferShape(s);
            for(DiscreteLabel l4=0; l4 < s[4]; ++l4)
            for(DiscreteLabel l3=0; l3 < s[3]; ++l3)
            for(DiscreteLabel l2=0; l2 < s[2]; ++l2)
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                const ValueType v = this->eval(l0, l1, l2, l3, l4);
                if(v<minVal){
                    conf[0] = l0;
                    conf[1] = l1;
                    conf[2] = l2;
                    conf[3] = l3;
                    conf[4] = l4;
                    minVal = v;
                }
            }
        }
        else { // (arity >= 6)
            const int64_t nConf = this->size();
            ConfIterator<ShapeFunctor> confIter(ShapeFunctor(this), arity, nConf);
            ConfIterator<ShapeFunctor> confEnd = confIter.getEnd();
            for( ; confIter != confEnd; ++confIter){
                const ValueType v = this->eval(confIter->data());
                if(v<minVal){
                    for(auto a=0; a<arity; ++a)
                        conf[a] = confIter->data()[a];
                    minVal = v;
                }
            }
        }
        return minVal;
    }


    /** \brief 
            copy the shape into a buffer
    
        This is mainly to avoid virtual calls
        and get the complete shape with a single
        virtual call if this function is overloaded.
        
        \warning
            The buffer must be preallocated and 
            must hold at least DiscreteValueTableBase::arity
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

    /** \brief sum up the shape

        This function is useful when
        calculating the required 
        space for all factor to 
        variable messages for a certain factor
    */
    virtual uint64_t accumulateShape()const{
        const size_t arity = this->arity();
        uint64_t res = 0;
        for(size_t i=0; i<arity; ++i)
            res += this->shape(i);
        return res;
    }


    /** \brief 
            copy the complete value table in 
            a buffer.

        This function is similar to bufferValueTable,
        but instead of copying the values
        to the buffer, the value for each label of this
        value table is added to the value of the buffer.


        This is mainly to avoid virtual calls
        and get the complete value table with a single
        virtual call if this function is overloaded.

        \warning
            The buffer must be preallocated and 
            must hold at least DiscreteValueTableBase::size
            values


        \warning
            This default implementation itself
            has multiple virtual calls
    */
    virtual void bufferValueTable(ValueType * buffer)const{
        const size_t arity = this->arity();
        if(arity == 1){
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

    /** \brief 
            Add the complete value table in 
            a buffer
     
        This is mainly to avoid virtual calls
        and get the complete accumulation with a single
        virtual call if this function is overloaded.

        \warning
            The buffer must be preallocated and 
            must hold at least DiscreteValueTableBase::size
            values

        \warning
            This default implementation itself
            has multiple virtual calls
    */
    virtual void addToBuffer(ValueType * buffer)const{
        const size_t arity = this->arity();
        if(arity == 1){
            const DiscreteLabel s[1] = {this->shape(0)};
            for(DiscreteLabel l0=0; l0<s[0]; ++l0){
                buffer[l0] += this->eval(l0);
            }
        }
        else if(arity == 2){
            int64_t c=0;
            DiscreteLabel s[2];
            this->bufferShape(s);
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                buffer[c] += this->eval(l0, l1);
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
                buffer[c] += this->eval(l0, l1, l2);
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
                buffer[c] += this->eval(l0, l1, l2, l3);
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
                buffer[c] += this->eval(l0, l1, l2, l3, l4);
                ++c;
            }
        }
        else { // (arity >= 6)
            const int64_t nConf = this->size();
            ConfIterator<ShapeFunctor> confIter(ShapeFunctor(this), arity, nConf);
            ConfIterator<ShapeFunctor> confEnd = confIter.getEnd();
            int64_t c=0;
            for( ; confIter != confEnd; ++confIter){
                buffer[c] += this->eval(confIter->data());
                ++c;
            }
        }
    }

    /** \brief 
            Add the complete value table in 
            a buffer
     
        This is mainly to avoid virtual calls
        and get the complete accumulation with a single
        virtual call if this function is overloaded.

        \warning
            The buffer must be preallocated and 
            must hold at least DiscreteValueTableBase::size
            values

        \warning
            This default implementation itself
            has multiple virtual calls
    */

    virtual void addWeightedToBuffer(ValueType * buffer, const ValueType w)const{
        const size_t arity = this->arity();
        if(arity == 1){
            const DiscreteLabel s[1] = {this->shape(0)};
            for(DiscreteLabel l0=0; l0<s[0]; ++l0){
                buffer[l0] += w * this->eval(l0);
            }
        }
        else if(arity == 2){
            int64_t c=0;
            DiscreteLabel s[2];
            this->bufferShape(s);
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                buffer[c] += w * this->eval(l0, l1);
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
                buffer[c] += w * this->eval(l0, l1, l2);
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
                buffer[c] += w * this->eval(l0, l1, l2, l3);
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
                buffer[c] += w * this->eval(l0, l1, l2, l3, l4);
                ++c;
            }
        }
        else { // (arity >= 6)
            const int64_t nConf = this->size();
            ConfIterator<ShapeFunctor> confIter(ShapeFunctor(this), arity, nConf);
            ConfIterator<ShapeFunctor> confEnd = confIter.getEnd();
            int64_t c=0;
            for( ; confIter != confEnd; ++confIter){
                buffer[c] += w * this->eval(confIter->data());
                ++c;
            }
        }
    }


    virtual void facToVarMsg(const ValueType ** inMsgs, ValueType ** outMsgs)const{
        fallBackFacToVarMsg(this,inMsgs,outMsgs);
    }






};



} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_BASE_DISCRETE_VALUE_TYPE*/
