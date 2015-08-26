#ifndef INFERNO_MODEL_BASE_FACTOR_HXX
#define INFERNO_MODEL_BASE_FACTOR_HXX

#include <boost/iterator/iterator_facade.hpp>

#include "inferno/inferno.hxx"
#include "inferno/utilities/utilities.hxx"
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
template<class FACTOR, class MODEL >
class DiscreteFactorBase{

private:
    typedef DiscreteFactorBase<FACTOR,MODEL> Self;
    typedef typename MODEL::VariableDescriptor VariableDescriptor;

    class VariableDescriptorIter : 
    public  boost::iterator_facade<Self, const VariableDescriptor,
                                   boost::random_access_traversal_tag> 
    {
        friend class boost::iterator_core_access;
    public:
        VariableDescriptorIter(const FACTOR * factor, uint32_t fvi)
        :   factor_(factor),
            fvi_(fvi),

            arity_(factor->arity()){
            if(fvi_>=0 && fvi_<arity_)
                var_ = factor_->variable(fvi_);
        }
        void increment(){
            ++fvi_;
            if(fvi_>=0 && fvi_<arity_)
                var_ = factor_->variable(fvi_);
        }
        void decrement(){
            --fvi_;
            if(fvi_>=0 && fvi_<arity_)
                var_ = factor_->variable(fvi_);
        }
        void advance(const size_t n){
            fvi_+=n;
            if(fvi_>=0 && fvi_<arity_)
                var_ = factor_->variable(fvi_);
        }
        bool equal(const VariableDescriptorIter & other) const{
            return this->fvi_ == other.fvi_;
        }

        const VariableDescriptor & dereference() const { 
            return var_; 
        }

    private:
        const FACTOR * factor_;
        int64_t fvi_;
        VariableDescriptor var_;
        const ArityType arity_;
    };






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

    typedef utilities::ConstIteratorRange<VariableDescriptorIter> VariableDescriptorIterRange;
public:

    ConfIterator< ShapeFunctor > confIter()const{
        ShapeFunctor shape(factor());
        return ConfIterator< ShapeFunctor >(shape, factor()->arity(), factor()->size());
    }


    VariableDescriptorIter variableDescriptorsBegin()const{
        return VariableDescriptorIter(factor(),0);
    }
    VariableDescriptorIter variableDescriptorsEnd()const{
        return VariableDescriptorIter(factor(),this->factor()->arity());
    }

    VariableDescriptorIterRange variables()const{
        return VariableDescriptorIterRange(this->variableDescriptorsBegin(),
                                           this->variableDescriptorsEnd());
    }

    ConfRange confs()const{
        return ConfRange(factor());
    }

    ValueType eval(const LabelType * conf)const{
        return factor()->valueTable()->eval(conf);
    }

    ValueType eval(const std::vector<DiscreteLabel> & conf)const{
        return factor()->valueTable()->eval(conf.data());
    }

    ValueType eval(const SmallVector<DiscreteLabel> & conf)const{
        return factor()->valueTable()->eval(conf.data());
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
    ArityType arity()const{
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
        ITER_FACTOR_CONF factorConf     
    )const{
        const auto arity = factor()->arity();
        for(auto a=0; a<arity; ++a){
            factorConf[a] = gmConf[factor()->variable(a)];
        }
    }

    void updateWeights(const learning::Weights & weights)const{ 
        factor()->valueTable()->updateWeights(weights);
    }
    uint64_t nWeights()const{
        return factor()->valueTable()->nWeights();
    }
    int64_t weightIndex(const size_t i)const{
        return factor()->valueTable()->nWeights();
    }

    WeightType weightGradient(const size_t factorsWeightIndex, const DiscreteLabel * conf)const{
        return factor()->valueTable()->weightGradient(factorsWeightIndex, conf);
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
    //if(arity == 0 ){
    //}
    //else if(arity == 1 ){
    //    for(LabelType l0=0; l0<factor->shape(0); ++l0){
    //        stream<<"f(x"<<factor->variable(0)<<"="<<l0<<") = ";
    //        stream<<factor->eval(l0)<<"\n";
    //    }
    //}
    //else if(arity == 2 ){
    //    for(LabelType l1=0; l1<factor->shape(1); ++l1)
    //    for(LabelType l0=0; l0<factor->shape(0); ++l0){
    //        stream<<"f(x"<<factor->variable(0)<<"="<<l0<<" x"<<factor->variable(1)<<"="<<l1<<") = ";
    //        stream<<factor->eval(l0, l1)<<"\n";
    //    }
    //}

    return stream;
}

} // end namespace inferno::models
} // inferno

#endif /* INFERNO_MODEL_BASE_FACTOR_HXX */
