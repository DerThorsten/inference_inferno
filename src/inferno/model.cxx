#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <memory>

#include "inferno/inferno.hxx"
#include "inferno/model.hxx"
#include "inferno/small_vector.hxx"

namespace inferno{


inline VariablesInfo::VariablesInfo(
    const int64_t  minVariableId,
    const int64_t  maxVariableId,
    const uint64_t numVariables
)
:   minVariableId_(minVariableId),
    maxVariableId_(maxVariableId),
    numVariables_(numVariables){
}

inline int64_t  
VariablesInfo::minVariableId()const{
    return minVariableId_;
}

inline int64_t  
VariablesInfo::maxVariableId()const{
    return maxVariableId_;
}
inline uint64_t
VariablesInfo::numVariables()const{
    return numVariables_;
}

inline bool 
VariablesInfo::isDense() const {
    return (maxVariableId_ - minVariableId_)+1 
        == numVariables_;
}

inline FactorsInfo::FactorsInfo(
    const int64_t  minFactorId,
    const int64_t  maxFactorId,
    const uint64_t numFactors
)
:   minFactorId_(minFactorId),
    maxFactorId_(maxFactorId),
    numFactors_(numFactors){
}

inline int64_t  
FactorsInfo::minFactorId()const{
    return minFactorId_;
}

inline int64_t  
FactorsInfo::maxFactorId()const{
    return maxFactorId_;
}
inline uint64_t
FactorsInfo::numFactors()const{
    return numFactors_;
}

inline bool 
FactorsInfo::isDense() const {
    return (maxFactorId_ - minFactorId_)+1 
        == numFactors_;
}

FactorValueType 
DiscreteModel::evaluateSum(
    const DiscreteLabel * conf
) const{

    SmallVector<MixedLabel> fconf;
    const FactorsInfo facInfo = this->factorsInfo();
    const bool isDense = facInfo.isDense();

    FactorValueType totalValue = 0.0;

    for(int64_t fi=facInfo.minFactorId(); fi<= facInfo.maxFactorId(); ++fi){
        if(isDense || this->isFactorId(fi)){
            SharedFactorPtr factor = this->getFactor(fi);
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
    SharedDiscreteFactorPtr factor
){
    factors_.push_back(factor);
}

SharedFactorPtr 
ExplicitDiscreteModel::getFactor(
    const size_t fi
) const {
    return factors_[fi];
}

void 
ExplicitDiscreteModel::varBounds(
    const size_t vi,
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
