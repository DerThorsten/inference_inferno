/// \cond
#ifndef INFERNO_MODEL_MODIFIED_MULTIWAYCUT_MODEL_HXX
#define INFERNO_MODEL_MODIFIED_MULTIWAYCUT_MODEL_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/model/base_discrete_model.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{
namespace models{


/**
*/
class ModifiedMultiwayCutFactor : public DiscreteFactorBase<ModifiedMultiwayCutFactor>{
public:
    ModifiedMultiwayCutFactor(const Vi nLabels = Vi(), const Vi u = Vi(), const Vi v=Vi(), const ValueType beta=ValueType())
    :   u_(u),
        v_(v),
        pottsFunction_(nLabels, beta){
    }

    const value_tables::DiscreteValueTableBase * valueTable()const{
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
    value_tables::PottsValueTable pottsFunction_;
};

/** \brief Modified multiway-cut graphical model
    
    \ingroup models
    \ingroup discrete_models
*/
class ModifiedMultiwayCut : 
public DiscreteGraphicalModelBase<ModifiedMultiwayCut>{

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
        VariableMap(const ModifiedMultiwayCut & m, const T & val)
        : std::vector<T>(m.nVariables(),val){
        }//
        VariableMap(const ModifiedMultiwayCut & m)
        : std::vector<T>(m.nVariables()){
        }
    };

    template<class T>
    class FactorMap : public std::vector<T>{
    public:
        FactorMap(const ModifiedMultiwayCut & m, const T & val)
        : std::vector<T>(m.nFactors(),val){
        }
        FactorMap(const ModifiedMultiwayCut & m)
        : std::vector<T>(m.nFactors()){
        }
    };


    ModifiedMultiwayCut(const uint64_t nVar)
    :   nVar_(nVar),
        edges_(),
        beta_(){

    }   
    uint64_t addFactor(const Vi u, const Vi v, const ValueType beta){
        edges_.push_back(u);
        edges_.push_back(v);
        beta_.push_back(beta);
    }

    uint64_t maxArity()const{
        return 2;
    }

    FactorIdIter factorIdsBegin()const{
        return FactorIdIter(0);
    }
    FactorIdIter factorIdsEnd()const{
        return FactorIdIter(beta_.size());
    }
    VariableIdIter variableIdsBegin()const{
        return VariableIdIter(0);
    }
    VariableIdIter variableIdsEnd()const{
        return VariableIdIter(nVar_);
    }

    FactorProxy operator[](const uint64_t factorId)const{
        const uint64_t uu = factorId*2;
        const uint64_t vv = uu+1;
        return FactorProxy(nVar_,edges_[uu],edges_[vv],beta_[uu/2]);
    }

    LabelType nLabels(const uint64_t variabeId)const{
        return LabelType(nVar_); 
    }


private:
    uint64_t nVar_;
    std::vector<Vi> edges_;
    std::vector<ValueType> beta_;


};




}
}


#endif /* INFERNO_MODEL_MODIFIED_MULTIWAYCUT_MODEL_HXX */
/// \endcond
