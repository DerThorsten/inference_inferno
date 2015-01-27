#ifndef INFERNO_MODEL_GENERAL_DISCRETE_MODEL_HXX
#define INFERNO_MODEL_GENERAL_DISCRETE_MODEL_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/model/base_discrete_model.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{


class GeneralDiscreteGraphicalModelFactor : public DiscreteFactorBase<GeneralDiscreteGraphicalModelFactor> {
public:

    template<class VI_T>
    GeneralDiscreteGraphicalModelFactor(const DiscreteValueTable * vt,
                   std::initializer_list<VI_T> list)
    :   vis_(list),
        vt_(vt){

    }

    template<class VI_ITER>
    GeneralDiscreteGraphicalModelFactor(const DiscreteValueTable * vt,
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




class GeneralDiscreteGraphicalModel : 
public DiscreteGraphicalModelBase<GeneralDiscreteGraphicalModel>{


public:
    typedef boost::counting_iterator<uint64_t> FactorIdIter;
    typedef boost::counting_iterator<Vi> VariableIdIter;
    typedef const GeneralDiscreteGraphicalModelFactor * FactorProxy;
    typedef FactorProxy FactorProxyRef;
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


    GeneralDiscreteGraphicalModel(const uint64_t nVar, const LabelType nLabes)
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
        factors_.push_back(GeneralDiscreteGraphicalModelFactor(valueTables_[vti], viBegin, viEnd));
        return factors_.size()-1;
    }
    template<class VI_T>
    const uint64_t addFactor(const uint64_t vti , std::initializer_list<VI_T>  list){
        factors_.push_back(GeneralDiscreteGraphicalModelFactor(valueTables_[vti], list));
        return factors_.size()-1;
    }
private:
    const uint64_t nVar_;
    std::vector<LabelType>              numberOfLabels_;
    std::vector<DiscreteValueTable * >  valueTables_;
    std::vector<GeneralDiscreteGraphicalModelFactor>         factors_;
    

};





}

#endif 
