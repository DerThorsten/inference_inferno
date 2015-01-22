#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <memory>


//#include "inferno/model_info.hxx"


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






}
