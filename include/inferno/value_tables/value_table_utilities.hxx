/** \file value_table_utilities.hxx 
    \brief function related to value tables 
    are implemented here.

*/
#ifndef INFERNO_VALUE_TABLES_VALUE_TABLE_UTILITIES_HXX
#define INFERNO_VALUE_TABLES_VALUE_TABLE_UTILITIES_HXX

#include <cstdint>
#include <limits>
#include <algorithm>

#include "inferno/inferno.hxx"

namespace inferno{
namespace value_tables{

    /// \cond
    namespace detail_value_tablle_utitlities{
    inline std::pair<DiscreteLabel, DiscreteLabel> arg2min(const ValueType * begin, const ValueType * end){
        std::pair<DiscreteLabel, DiscreteLabel> res;
        ValueType min0=std::numeric_limits<ValueType>::infinity();
        ValueType min1=min0;
        const auto dist = std::distance(begin, end);
        for(auto i=0; i<dist; ++i){
            const ValueType val = begin[i];
            if(val<min1){
                if(val<min0){

                    min1 = min0;
                    res.second = res.first;
                    min0 = val;
                    res.first = i;
                    continue;
                }   
                else{
                    min1 = val;
                    res.second = i;
                }
            }
        }
        return res;
    }
    }
    /// \endcond
    
    inline void pottsFacToVarMsg(
        const DiscreteLabel nl,
        const ValueType beta,
        const ValueType ** inMsgs,
        ValueType ** outMsgs                 
    ){
        if(beta>=0){
            const ValueType minIn0Beta = *std::min_element(inMsgs[0],inMsgs[0]+nl)+beta;
            const ValueType minIn1Beta = *std::min_element(inMsgs[1],inMsgs[1]+nl)+beta;
            for(DiscreteLabel l=0; l<nl; ++l){
                outMsgs[0][l] = std::min(inMsgs[1][l], minIn1Beta);
                outMsgs[1][l] = std::min(inMsgs[0][l], minIn0Beta);
            }
        }
        else{
            DiscreteLabel aMin0,aMin1, aSMin0, aSMin1;
            std::tie(aMin0, aSMin0) = detail_value_tablle_utitlities::arg2min(inMsgs[0],inMsgs[0]+nl);
            std::tie(aMin1, aSMin1) = detail_value_tablle_utitlities::arg2min(inMsgs[1],inMsgs[1]+nl);
            const ValueType min0 = inMsgs[0][aMin0];
            const ValueType min1 = inMsgs[1][aMin1];
            const ValueType min0Beta = min0 + beta;
            const ValueType min1Beta = min1 + beta;
            const ValueType smin0Beta = std::min(inMsgs[0][aSMin0] + beta, min0);
            const ValueType smin1Beta = std::min(inMsgs[1][aSMin1] + beta, min1);
            for(DiscreteLabel l=0; l<nl; ++l){
                outMsgs[0][l] = aMin1 != l ?  min1Beta  : smin1Beta;
                outMsgs[1][l] = aMin0 != l ?  min0Beta  : smin0Beta;
            }
        }
    }

}
}

#endif /* INFERNO_VALUE_TABLES_VALUE_TABLE_UTILITIES_HXX */
