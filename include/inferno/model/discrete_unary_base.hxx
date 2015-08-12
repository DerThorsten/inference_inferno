#ifndef INFERNO_MODEL_BASE_FACTOR_HXX
#define INFERNO_MODEL_BASE_FACTOR_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/utilities/shape_walker.hxx"


namespace inferno{
namespace models{


class Lifetime{
    enum FactorProxy{
        TmpObj,
        PtrObj
    };
    enum ValueTableOwner{
        FactorIsOwner,
        ModelIsOwner
    };
};



/** \brief Base CRT Factor Base
*/
template<class FACTOR >
class DiscreteFactorBase{

private:



    struct ShapeFunctor{
        ShapeFunctor()
        : f_(NULL){}
        ShapeFunctor(const FACTOR * f)
        : f_(f){
        }
        DiscreteLabel operator()(const size_t d)const{
            return f_->shape(d);
        }
        const FACTOR * f_;
    };

    struct ConfRange{
        typedef ConfIterator< ShapeFunctor > const_iterator;
        ConfRange(const FACTOR * factor)
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

    ConfIterator< ShapeFunctor > confIter()const{
        ShapeFunctor shape(factor());
        return ConfIterator< ShapeFunctor >(shape, factor()->arity(), factor()->size());
    }

    ConfRange confs()const{
        return ConfRange(factor());
    }

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
    size_t arity()const{
        return factor()->valueTable()->arity();
    }
    uint64_t size()const{
        return factor()->valueTable()->size();
    }

    bool isPotts(ValueType & val)const{
        return factor()->valueTable()->isPotts(val);
    }
    bool isGeneralizedPotts()const{
        return factor()->valueTable()->isGeneralizedPotts();
    }

    void bufferShape(DiscreteLabel * buffer)const{
        factor()->valueTable()->bufferShape(buffer);
    }

    void bufferValueTable(ValueType * buffer)const{
        factor()->valueTable()->bufferValueTable(buffer);
    }

    void addToBuffer(ValueType * buffer)const{
        factor()->valueTable()->addToBuffer(buffer);
    }

    void addWeightedToBuffer(ValueType * buffer, const ValueType w)const{
        factor()->valueTable()->addWeightedToBuffer(buffer, w);
    }

    void facToVarMsg(const ValueType ** inMsgs, ValueType ** outMsgs)const{
        factor()->valueTable()->facToVarMsg(inMsgs,outMsgs);
    }

    uint64_t accumulateShape()const{
        return factor()->valueTable()->accumulateShape();
    }

    const FACTOR * operator ->()const{
        return static_cast<const FACTOR *>(this); 
    }

    template<class GM_CONF, class ITER_FACTOR_CONF>
    void getFactorConf(
        const GM_CONF & gmConf,
        const ITER_FACTOR_CONF factorConf     
    )const{
        const auto arity = factor()->arity();
        for(auto a=0; a<arity; ++a){
            factorConf[a] = gmConf[factor()->variable(a)];
        }
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
    FactorVariableIdsCout<FACTOR_PROXY> variableIds(const FACTOR_PROXY & factor){
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
            stream<<"x="<<factor->variable(d);
        }
        else
            stream<<"x="<<factor->variable(d)<<", ";
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
            stream<<"f(x"<<factor->variable(0)<<"="<<l0<<") = ";
            stream<<factor->eval(l0)<<"\n";
        }
    }
    else if(arity == 2 ){
        for(LabelType l1=0; l1<factor->shape(1); ++l1)
        for(LabelType l0=0; l0<factor->shape(0); ++l0){
            stream<<"f(x"<<factor->variable(0)<<"="<<l0<<" x"<<factor->variable(1)<<"="<<l1<<") = ";
            stream<<factor->eval(l0, l1)<<"\n";
        }
    }

    return stream;
}

} // end namespace inferno::models
} // inferno

#endif /* INFERNO_MODEL_BASE_FACTOR_HXX */
