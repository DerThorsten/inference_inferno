#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <memory>

#include "inferno/inferno.hxx"
#include "inferno/model.hxx"
#include "inferno/model_info.hxx"
#include "inferno/small_vector.hxx"

namespace inferno{



DiscreteFactorSharedPtr 
Model::getFactor(const int64_t fi, const DiscreteTag tag) const {
    return std::dynamic_pointer_cast<DiscreteFactor>(this->getFactor(fi));
}

ContinousFactorSharedPtr 
Model::getFactor(const int64_t fi, const ContinousTag tag) const {
    return std::dynamic_pointer_cast<ContinousFactor>(this->getFactor(fi));
}

MixedFactorSharedPtr 
Model::getFactor(const int64_t fi, const MixedTag tag) const {
    return std::dynamic_pointer_cast<MixedFactor>(this->getFactor(fi));
}



template<class LABEL_TYPE>
FactorValueType Model::evaluateSumT(const LABEL_TYPE * conf) const{
    SmallVector<LABEL_TYPE> fconf;
    const FactorsInfo facInfo = this->factorsInfo();
    const bool isDense = facInfo.isDense();

    FactorValueType totalValue = 0.0;

    for(int64_t fi=facInfo.minFactorId(); fi<= facInfo.maxFactorId(); ++fi){
        if(isDense || this->isFactorId(fi)){
            FactorSharedPtr factor = this->getFactor(fi);
            const size_t arity = factor->arity();
            if(arity>fconf.size())
                fconf.resize(arity);
            for(size_t d=0; d<arity; ++d){
                fconf[d] = conf[factor->vi(d)];
            }
            totalValue += factor->eval(fconf.data());
        }
    }
    return totalValue;
}

FactorValueType Model::evaluateSum(const MixedLabel * conf) const{
   return this->evaluateSumT(conf); 
}
FactorValueType Model::evaluateSum(const ContinousLabel * conf) const{
   return this->evaluateSumT(conf); 
}
FactorValueType Model::evaluateSum(const DiscreteLabel * conf) const{
   return this->evaluateSumT(conf); 
}





ExplicitDiscreteModel::ExplicitDiscreteModel(
    const size_t nVar,
    const DiscreteLabelBounds & bounds
)
:   DiscreteModel(),
    nVar_(nVar_),
    varBounds_(1, bounds),
    factors_(){

}

void 
ExplicitDiscreteModel::addFactor(
    DiscreteFactorSharedPtr factor
){
    factors_.push_back(factor);
}

FactorSharedPtr 
ExplicitDiscreteModel::getFactor(
    const int64_t fi
) const {
    return factors_[fi];
}

void 
ExplicitDiscreteModel::varBounds(
   const int64_t vi,
   DiscreteLabelBounds & bounds
) const {
   bounds = vi < varBounds_.size() ? 
       varBounds_[vi] : varBounds_[0];
}


VariablesInfo 
ExplicitDiscreteModel::variablesInfo()const{
    return VariablesInfo(0,nVar_-1,nVar_);
}

FactorsInfo 
ExplicitDiscreteModel::factorsInfo()const{
    return FactorsInfo(0,factors_.size()-1,factors_.size());
}

inline bool 
ExplicitDiscreteModel::isVariableId(const int64_t id) const{
    return id>=0 && id<nVar_;
}

inline bool 
ExplicitDiscreteModel::isFactorId(const int64_t id) const{
    return id>=0 && id<factors_.size();
}


}
