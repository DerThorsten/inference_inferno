#ifndef INFERNO_MODEL_HXX
#define INFERNO_MODEL_HXX

#include "inferno/inferno.hxx"
#include "inferno/discrete_value_table.hxx"
#include "inferno/small_vector.hxx"
#include "inferno/factor.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{



template<class MODEL>
class DiscreteGraphicalModelBase{
public:

private:
    const MODEL * model()const{
        return static_cast<const MODEL *>(this);
    }
    MODEL * model(){
        return static_cast<MODEL *>(this);
    }
};


class DiscreteGraphicalModel : 
public DiscreteGraphicalModelBase<DiscreteGraphicalModel>{


private:
    typedef boost::counting_iterator<uint64_t> FactorIdIter;
    typedef boost::counting_iterator<Vi> VariableIdIter;
    typedef const DiscreteFactor * FactorProxy;

public:
    typedef const DiscreteFactor * ConstFactorPtr;
    //
    FactorIdIter factorIdsBegin()const{
        return FactorIdIter(0);
    }
    FactorIdIter factorIdsEnd()const{
        return FactorIdIter(factors_.size());
    }
    VariableIdIter variableIdsBegin()const{
        return VariableIdIter(0);
    }
    VariableIdIter variableIdsEnd()const{
        return VariableIdIter(nVar_);
    }

    FactorProxy operator[](const uint64_t factorId)const{
        return &factors_[factorId];
    }

    LabelType numberOfLabels(const uint64_t variabeId)const{
        return variabeId >= numberOfLabels_.size() ? numberOfLabels_[0] : numberOfLabels_[variabeId]; 
    }


    DiscreteGraphicalModel(const uint64_t nVar, const LabelType nLabes)
    :   nVar_(nVar),
        numberOfLabels_(1, nLabes),
        valueTables_(),
        factors_(){
    }
    const uint64_t addValueTable( DiscreteValueTable * vt){
        valueTables_.push_back(vt);
        return valueTables_.size()-1;
    }   
    template<class VI_ITER>
    const uint64_t addFactor(const uint64_t vti , VI_ITER viBegin, VI_ITER viEnd){
        factors_.push_back(DiscreteFactor(valueTables_[vti], viBegin, viEnd));
        return factors_.size()-1;
    }
    template<class VI_T>
    const uint64_t addFactor(const uint64_t vti , std::initializer_list<VI_T>  list){
        factors_.push_back(DiscreteFactor(valueTables_[vti], list));
        return factors_.size()-1;
    }
private:
    const uint64_t nVar_;
    std::vector<LabelType>              numberOfLabels_;
    std::vector<DiscreteValueTable * >  valueTables_;
    std::vector<DiscreteFactor>         factors_;
    

};



class ImplicitMulticutModel : 
public DiscreteGraphicalModelBase<ImplicitMulticutModel>{

public:

    typedef boost::counting_iterator<uint64_t> FactorIdIter;
    typedef boost::counting_iterator<Vi> VariableIdIter;
    typedef ImplicitMulticutModelFactor FactorProxy;


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
        const uint64_t uu = factorId/2;
        const uint64_t vv = uu+1;
        return FactorProxy(nVar_,edges_[uu],edges_[vv],beta_[uu]);
    }

    LabelType numberOfLabels(const uint64_t variabeId)const{
        return LabelType(nVar_); 
    }


private:
    uint64_t nVar_;
    std::vector<Vi> edges_;
    std::vector<ValueType> beta_;


};




}

#endif 
