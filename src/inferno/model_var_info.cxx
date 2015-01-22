#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <memory>


#include "inferno/model_var_info.hxx"


namespace inferno{




VariablesInfo::VariablesInfo(
    const int64_t  minVariableId,
    const int64_t  maxVariableId,
    const uint64_t numVariables
)
:   minVariableId_(minVariableId),
    maxVariableId_(maxVariableId),
    numVariables_(numVariables){
}

int64_t  
VariablesInfo::minVariableId()const{
    return minVariableId_;
}

int64_t  
VariablesInfo::maxVariableId()const{
    return maxVariableId_;
}
uint64_t
VariablesInfo::numVariables()const{
    return numVariables_;
}

bool 
VariablesInfo::isDense() const {
    return (maxVariableId_ - minVariableId_)+1 
        == numVariables_;
}



}
