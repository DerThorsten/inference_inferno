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




FactorsInfo::FactorsInfo(
    const int64_t  minFactorId,
    const int64_t  maxFactorId,
    const uint64_t numFactors
)
:   minFactorId_(minFactorId),
    maxFactorId_(maxFactorId),
    numFactors_(numFactors){
}

int64_t  
FactorsInfo::minFactorId()const{
    return minFactorId_;
}

int64_t  
FactorsInfo::maxFactorId()const{
    return maxFactorId_;
}
uint64_t
FactorsInfo::numFactors()const{
    return numFactors_;
}

bool 
FactorsInfo::isDense() const {
    return (maxFactorId_ - minFactorId_)+1 
        == numFactors_;
}






}
