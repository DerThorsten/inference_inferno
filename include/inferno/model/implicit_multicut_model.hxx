#ifndef INFERNO_MODEL_IMPLICIT_MULTICUT_MODEL_HXX
#define INFERNO_MODEL_IMPLICIT_MULTICUT_MODEL_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/model/base_discrete_model.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{






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



class ImplicitMulticutModel : 
public DiscreteGraphicalModelBase<ImplicitMulticutModel>{

public:

    typedef boost::counting_iterator<uint64_t> FactorIdIter;
    typedef boost::counting_iterator<Vi> VariableIdIter;
    typedef ImplicitMulticutModelFactor FactorProxy;
    typedef const FactorProxy & FactorProxyRef;

    ImplicitMulticutModel(const uint64_t nVar)
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

#endif 
