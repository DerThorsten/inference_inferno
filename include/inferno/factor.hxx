#ifndef INFERNO_FACTOR_HXX
#define INFERNO_FACTOR_HXX

#include "inferno/inferno.hxx"
#include "inferno/discrete_value_table.hxx"
#include "inferno/small_vector.hxx"
namespace inferno{


template<class FACTOR >
class DiscreteFactorBase{
public:

    ValueType eval(const LabelType * conf)const{
        return factor()->valueTable()->eval(conf);
    }
    ValueType eval(const LabelType l0)const{
        return factor()->valueTable()->eval(l0);
    }

    ValueType eval(const LabelType l0, 
                         const LabelType l1)const{
        return factor()->valueTable()->eval(l0, l1);
    }

    ValueType eval(const LabelType l0, 
                         const LabelType l1, 
                         const LabelType l2)const{
        return factor()->valueTable()->eval(l0, l1, l2);
    }

    ValueType eval(const LabelType l0, 
                         const LabelType l1, 
                         const LabelType l2, 
                         const LabelType l3)const{
        return factor()->valueTable()->eval(l0, l1, l2, l3);
    }

    ValueType eval(const LabelType l0, 
                         const LabelType l1, 
                         const LabelType l2, 
                         const LabelType l3,
                         const LabelType l4)const{
        return factor()->valueTable()->eval(l0, l1, l2, l3, l4);
    }

    uint64_t size()const{
        return factor()->valueTable()->size();
    }
    const FACTOR * operator ->()const{
        return static_cast<const FACTOR *>(this); 
    }
private:
    const FACTOR * factor()const{
        return static_cast<const FACTOR *>(this);
    }
    FACTOR * factor(){
        return static_cast<FACTOR *>(this);
    }
};




namespace io{

    template<class FACTOR_PROXY>
    struct FactorValueTableCout{
        FactorValueTableCout(const FACTOR_PROXY & factor)
        : factor_(factor){

        }
        const FACTOR_PROXY & factor_;
    };
    
    template<class FACTOR_PROXY>
    struct FactorVariableIdsCout{
        FactorVariableIdsCout(const FACTOR_PROXY & factor)
        : factor_(factor){

        }
        const FACTOR_PROXY & factor_;
    };

    template<class FACTOR_PROXY>
    FactorValueTableCout<FACTOR_PROXY> valueTable(const FACTOR_PROXY & factor){
        return FactorValueTableCout<FACTOR_PROXY>(factor);
    }


    template<class FACTOR_PROXY>
    FactorVariableIdsCout<FACTOR_PROXY> varibleIds(const FACTOR_PROXY & factor){
        return FactorVariableIdsCout<FACTOR_PROXY>(factor);
    }

}

template< class FACTOR_PROXY>
std::ostream& operator <<(std::ostream& stream, const io::FactorVariableIdsCout<FACTOR_PROXY> & factorCouter) {
    const FACTOR_PROXY & factor = factorCouter.factor_;
    const size_t arity = factor->arity();
    stream<<"f(";
    for(size_t d=0; d<arity; ++d){
        if(d==arity-1){
            stream<<"x="<<factor->vi(d);
        }
        else
            stream<<"x="<<factor->vi(d)<<", ";
    }
    stream<<")";
    return stream;
}

template< class FACTOR_PROXY>
std::ostream& operator <<(std::ostream& stream, const io::FactorValueTableCout<FACTOR_PROXY> & factorCouter) {
    const FACTOR_PROXY & factor = factorCouter.factor_;
    const size_t arity = factor->arity();
    if(arity == 0 ){

    }
    else if(arity == 1 ){
        for(LabelType l0=0; l0<factor->shape(0); ++l0){
            stream<<"f(x"<<factor->vi(0)<<"="<<l0<<") = ";
            stream<<factor->eval(l0)<<"\n";
        }
    }
    else if(arity == 2 ){
        for(LabelType l1=0; l1<factor->shape(1); ++l1)
        for(LabelType l0=0; l0<factor->shape(0); ++l0){
            stream<<"f(x"<<factor->vi(0)<<"="<<l0<<" x"<<factor->vi(1)<<"="<<l1<<") = ";
            stream<<factor->eval(l0, l1)<<"\n";
        }
    }

    return stream;
}


/*
    
*/

class DiscreteFactor : public DiscreteFactorBase<DiscreteFactor> {
public:

    template<class VI_T>
    DiscreteFactor(const DiscreteValueTable * vt,
                   std::initializer_list<VI_T> list)
    :   vis_(list),
        vt_(vt){

    }

    template<class VI_ITER>
    DiscreteFactor(const DiscreteValueTable * vt,
                   const VI_ITER viBegin, 
                   const VI_ITER viEnd)
    :   vis_(viBegin, viEnd),
        vt_(vt){

    }
    const DiscreteValueTable * valueTable()const{
        return vt_;
    }   
    size_t arity()const{
        return vis_.size();
    }
    LabelType shape(const size_t d)const{
        return vt_->shape(d);
    }
    Vi vi(const size_t d)const{
        return vis_[d];
    }


private:
    const std::vector<Vi> vis_;
    const DiscreteValueTable * vt_;

};



class ImplicitMulticutModelFactor : public DiscreteFactorBase<ImplicitMulticutModelFactor>{
public:
    ImplicitMulticutModelFactor(const Vi nLabels = Vi(), const Vi u = Vi(), const Vi v=Vi(), const ValueType beta=ValueType())
    :   u_(u),
        v_(v),
        pottsFunction_(nLabels, beta){
    }

    const DiscreteValueTable * valueTable()const{
        return &pottsFunction_;
    }   
    size_t arity()const{
        return 2;
    }
    LabelType shape(const size_t d)const{
        return pottsFunction_.shape(d);
    }
    Vi vi(const size_t d)const{
        return d==0? u_ : v_;
    }
private:
    Vi u_,v_;
    PottsValueTable pottsFunction_;
};





}

#endif 
