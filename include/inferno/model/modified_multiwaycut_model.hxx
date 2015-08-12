/** \file modified_multiwaycut_model.hxx 
    \brief  functionality for inferno::ModifiedMultiwaycutModel is implemented in this header.
     NOT YET FINISHED!

    \warning Not yet finished
*/
/// \cond
#ifndef INFERNO_MODEL_MODIFIED_MULTIWAYCUT_MODEL_HXX
#define INFERNO_MODEL_MODIFIED_MULTIWAYCUT_MODEL_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/model/discrete_model_base.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{
namespace models{


class ModifiedMultiwaycutModel;


namespace detail_mmwc_model{

    class  HybridValueTable : public inferno::value_tables::DiscreteValueTableBase{
    public:
        HybridValueTable(const Vi nVar, const Vi nSemanticClasses,
                         const uint8_t arity, const ValueType * data
        )
        :   inferno::value_tables::DiscreteValueTableBase(),
            nVar_(nVar),
            nSemanticClasses_(nSemanticClasses),
            arity_(arity),
            data_(data)
        {
        } 
        ValueType eval(const LabelType *conf)const{
            if(arity_==1)
                return data_[conf[0]/nVar_];
            else{
                return conf[0] == conf[1] ? 0.0 : *data_;
            }
        }
        ValueType eval(const LabelType l0)const{
            return data_[l0/nVar_];
        }
        ValueType eval(const LabelType l0, const LabelType l1)const{
            return l0==l1 ? 0 : *data_;
        }
        LabelType shape(const uint32_t d) const{
            return nVar_*nSemanticClasses_;
        }
        uint32_t  arity()const{
            return arity_;
        }
        bool isGeneralizedPotts() const{
            return arity_ == 2 ? true : false;   
        }
        bool isPotts(ValueType & beta) const{
            if(arity_ == 2)
                beta = data_[0];
            return arity_ == 2 ? true : false;
        }
        void facToVarMsg(const ValueType ** inMsgs, ValueType ** outMsgs)const{
            if(arity_ == 2)
                inferno::value_tables::pottsFacToVarMsg(shape(0), data_[0], inMsgs, outMsgs);
            else
                throw RuntimeError("facToVarMsg should not be called with an arity of 1");
        }

    private:
        Vi                  nVar_;
        Vi                  nSemanticClasses_;
        uint8_t             arity_;
        const ValueType *   data_;
    };   
}



class ModifiedMultiwayCutFactor : public DiscreteFactorBase<ModifiedMultiwayCutFactor>{
public:
    ModifiedMultiwayCutFactor(const Vi nVar, const Vi nSemanticClasses,
                              const uint8_t arity, const ValueType * data,
                              const Vi u, const Vi v = Vi()
    )
    :   DiscreteFactorBase<ModifiedMultiwayCutFactor>(),
        vt_(nVar, nSemanticClasses, arity, data),
        u_(u),
        v_(v)
    {
    }

    const value_tables::DiscreteValueTableBase * valueTable()const{
       return &vt_;
    }   
    size_t arity()const{
        return vt_.arity();
    }
    LabelType shape(const size_t d)const{
        return vt_.shape(d);
    }
    Vi vi(const size_t d)const{
        return d==0 ? u_ : v_; 
    }
private:
    detail_mmwc_model::HybridValueTable vt_;
    Vi u_,v_;
};

/** \brief Modified multiway-cut graphical model
    
    \ingroup models
    \ingroup discrete_models
*/
class ModifiedMultiwaycutModel : 
public DiscreteGraphicalModelBase<ModifiedMultiwaycutModel>{
    
    friend class detail_mmwc_model::HybridValueTable;
public:

    typedef boost::counting_iterator<uint64_t> FactorIdIter;
    typedef boost::counting_iterator<Vi> VariableIdIter;

    /// \todo rename / re-factor this typedef
    typedef ModifiedMultiwayCutFactor FactorProxy;

    /// \todo rename / re-factor this typedef
    typedef const FactorProxy & FactorProxyRef;


    template<class T>
    class VariableMap : public std::vector<T>{
    public:
        VariableMap(const ModifiedMultiwaycutModel & m, const T & val)
        : std::vector<T>(m.nVariables(),val){
        }//
        VariableMap(const ModifiedMultiwaycutModel & m)
        : std::vector<T>(m.nVariables()){
        }
    };

    template<class T>
    class FactorMap : public std::vector<T>{
    public:
        FactorMap(const ModifiedMultiwaycutModel & m, const T & val)
        : std::vector<T>(m.nFactors(),val){
        }
        FactorMap(const ModifiedMultiwaycutModel & m)
        : std::vector<T>(m.nFactors()){
        }
    };


    ModifiedMultiwaycutModel(const Vi nVar, const Vi nSemanticClasses)
    :   nVar_(nVar),
        edges_(),
        beta_(),
        nSemanticClasses_(nSemanticClasses){

    }   
    void addPottsFactor(const Vi u, const Vi v, const ValueType beta){
        edges_.push_back(u);
        edges_.push_back(v);
        beta_.push_back(beta);
    }

    template<class SEMANTIC_COST_ITER>
    void addUnaryFactor(const Vi u, SEMANTIC_COST_ITER costBegin, SEMANTIC_COST_ITER costEnd){
        unariesVis_.push_back(u);
        for( ;costBegin != costEnd; ++costBegin){
            unaries_.push_back(*costBegin);
        }
    }

    template<class SEMANTIC_COST_TYPE>
    void addUnaryFactor(const Vi u, std::initializer_list<SEMANTIC_COST_TYPE> list){
        this->addUnaryFactor(u, list.begin(), list.end());
    }

    Vi nSemanticClasses()const{
        return nSemanticClasses_;
    }

    Fi nUnaryFactors()const{
        return unariesVis_.size();
    }

    Fi nPairwiseFactors()const{
        return beta_.size();
    }

    uint64_t maxArity()const{
        return 2;
    }

    FactorIdIter factorIdsBegin()const{
        return FactorIdIter(0);
    }

    FactorIdIter factorIdsEnd()const{
        return FactorIdIter(nUnaryFactors()+nPairwiseFactors());
    }

    VariableIdIter variableIdsBegin()const{
        return VariableIdIter(0);
    }

    VariableIdIter variableIdsEnd()const{
        return VariableIdIter(nVar_);
    }

    FactorProxy factor(const Fi factorId)const{
        if(factorId<nUnaryFactors()){
            const ValueType * data =  unaries_.data() + factorId*nSemanticClasses_;
            const Vi u = unariesVis_[factorId];
            return FactorProxy(nVar_, nSemanticClasses_, 1, data, u);
        }
        else{
            const Fi edgeIndex = factorId - nUnaryFactors();
            const ValueType * data = beta_.data() + edgeIndex;
            const Vi uu = edgeIndex*2;
            const Vi vv = uu+1;
            return FactorProxy(nVar_, nSemanticClasses_, 2, data, edges_[uu], edges_[vv]);
        }
    }

    LabelType nLabels(const Vi variabeId)const{
        return LabelType(nVar_*nSemanticClasses_); 
    }


private:
    uint64_t nVar_;
    uint64_t nSemanticClasses_;

    std::vector<Vi> edges_;
    std::vector<ValueType> beta_;

    std::vector<Vi>         unariesVis_;
    std::vector<ValueType>  unaries_;

};




}
}


#endif /* INFERNO_MODEL_MODIFIED_MULTIWAYCUT_MODEL_HXX */
/// \endcond
