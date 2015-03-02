/** \file value_table_utilities.hxx 
    \brief function related to value tables 
    are implemented here.

*/
#ifndef INFERNO_VALUE_TABLES_VALUE_TABLE_UTILITIES_HXX
#define INFERNO_VALUE_TABLES_VALUE_TABLE_UTILITIES_HXX

#include <cstdint>
#include <vector>
#include <limits>

#include "inferno/inferno.hxx"

namespace inferno{
namespace value_tables{


    
    
    inline void pottsFacToVarMsg(
        const DiscreteLabel nl,
        const ValueType beta,
        const ValueType ** inMsgs,
        ValueType ** outMsgs                 
    ){
        if(beta>=0){
            const ValueType minIn0 = *std::min_element(inMsgs[0],inMsgs[0]+nl);
            const ValueType minIn1 = *std::min_element(inMsgs[1],inMsgs[1]+nl);
            for(DiscreteLabel l=0; l<nl; ++l){
                outMsgs[0][l] = std::min(inMsgs[1][l], minIn1 + beta);
                outMsgs[1][l] = std::min(inMsgs[0][l], minIn0 + beta);
            }
        }
        else{
            const DiscreteLabel argMin0 = std::distance(inMsgs[0], std::min_element(inMsgs[0],inMsgs[0]+nl));
            const DiscreteLabel argMin1 = std::distance(inMsgs[1], std::min_element(inMsgs[1],inMsgs[1]+nl));
            const ValueType minIn0 = inMsgs[0][argMin0];
            const ValueType minIn1 = inMsgs[1][argMin1];
            for(DiscreteLabel l=0; l<nl; ++l){
                outMsgs[0][l] = argMin1 == l ? (minIn0 + beta )    : 
                outMsgs[1][l] = 
            }
        }
    }

}
}

#endif /* INFERNO_VALUE_TABLES_VALUE_TABLE_UTILITIES_HXX */
