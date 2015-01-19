#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <memory>

#include "inferno/model.hxx"


namespace inferno{




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

}
