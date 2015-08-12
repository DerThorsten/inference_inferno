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
        template<class VT>
        struct ShapeFunctor{
            ShapeFunctor()
            : f_(NULL){}
            ShapeFunctor(const VT * f)
            : f_(f){
            }
            DiscreteLabel operator()(const size_t d)const{
                return f_->shape(d);
            }
            const VT * f_;
        };

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



    template<class VT>
    inline void fallBackFacToVarMsg(
        const VT * vt,
        const ValueType ** inMsgs,
        ValueType ** outMsgs        
    ){
        const auto arity = vt->arity();
        if(arity == 1){
            throw RuntimeError("facToVarMsg must not be called on value tables with arity<2");
        }
        else if(arity == 2){
            // get shape
            DiscreteLabel s[2];
            vt->bufferShape(s);
            
            // initialize 
            for(auto a=0; a<2; ++a)
                std::fill(outMsgs[a], outMsgs[a]+s[a], std::numeric_limits<ValueType>::infinity());

            // minimize
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                const ValueType facVal = vt->eval(l0, l1);
                outMsgs[0][l0] = std::min(outMsgs[0][l0], facVal + inMsgs[1][l1]);
                outMsgs[1][l1] = std::min(outMsgs[1][l1], facVal + inMsgs[0][l0]);
            }
        }
        else if(arity == 3){
            // get shape
            DiscreteLabel s[3];
            vt->bufferShape(s);
            
            // initialize 
            for(auto a=0; a<3; ++a)
                std::fill(outMsgs[a], outMsgs[a]+s[a], std::numeric_limits<ValueType>::infinity());

            // minimize
            for(DiscreteLabel l2=0; l2 < s[2]; ++l2)
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                const ValueType facVal = vt->eval(l0, l1, l2);
                outMsgs[0][l0] = std::min(outMsgs[0][l0], facVal + inMsgs[1][l1] + inMsgs[2][l2]);
                outMsgs[1][l1] = std::min(outMsgs[1][l1], facVal + inMsgs[0][l0] + inMsgs[2][l2]);
                outMsgs[2][l2] = std::min(outMsgs[2][l2], facVal + inMsgs[0][l0] + inMsgs[1][l1]);
            }
        }
        else if(arity == 4){
            // get shape
            DiscreteLabel s[4];
            vt->bufferShape(s);
            
            // initialize 
            for(auto a=0; a<4; ++a)
                std::fill(outMsgs[a], outMsgs[a]+s[a], std::numeric_limits<ValueType>::infinity());

            // minimize
            for(DiscreteLabel l3=0; l3 < s[3]; ++l3)
            for(DiscreteLabel l2=0; l2 < s[2]; ++l2)
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                const ValueType facVal = vt->eval(l0, l1, l2, l3);
                outMsgs[0][l0] = std::min(outMsgs[0][l0], facVal + inMsgs[1][l1] + inMsgs[2][l2] + inMsgs[3][l3]);
                outMsgs[1][l1] = std::min(outMsgs[1][l1], facVal + inMsgs[0][l0] + inMsgs[2][l2] + inMsgs[3][l3]);
                outMsgs[2][l2] = std::min(outMsgs[2][l2], facVal + inMsgs[0][l0] + inMsgs[1][l1] + inMsgs[3][l3]);
                outMsgs[3][l3] = std::min(outMsgs[3][l3], facVal + inMsgs[0][l0] + inMsgs[1][l1] + inMsgs[2][l2]);
            }
        }
        else if(arity == 5){
            // get shape
            DiscreteLabel s[5];
            vt->bufferShape(s);
            
            // initialize 
            for(auto a=0; a<5; ++a)
                std::fill(outMsgs[a], outMsgs[a]+s[a], std::numeric_limits<ValueType>::infinity());

            // minimize
            for(DiscreteLabel l4=0; l4 < s[4]; ++l4)
            for(DiscreteLabel l3=0; l3 < s[3]; ++l3)
            for(DiscreteLabel l2=0; l2 < s[2]; ++l2)
            for(DiscreteLabel l1=0; l1 < s[1]; ++l1)
            for(DiscreteLabel l0=0; l0 < s[0]; ++l0){
                const ValueType facVal = vt->eval(l0, l1, l2, l3);
                outMsgs[0][l0] = std::min(outMsgs[0][l0], facVal + inMsgs[1][l1] + inMsgs[2][l2] + inMsgs[3][l3] + inMsgs[4][l4]);
                outMsgs[1][l1] = std::min(outMsgs[1][l1], facVal + inMsgs[0][l0] + inMsgs[2][l2] + inMsgs[3][l3] + inMsgs[4][l4]);
                outMsgs[2][l2] = std::min(outMsgs[2][l2], facVal + inMsgs[0][l0] + inMsgs[1][l1] + inMsgs[3][l3] + inMsgs[4][l4]);
                outMsgs[3][l3] = std::min(outMsgs[3][l3], facVal + inMsgs[0][l0] + inMsgs[1][l1] + inMsgs[2][l2] + inMsgs[4][l4]);
                outMsgs[4][l4] = std::min(outMsgs[4][l4], facVal + inMsgs[0][l0] + inMsgs[1][l1] + inMsgs[2][l2] + inMsgs[3][l3]);
            }
        }
        else { // (arity >= 6)
            const int64_t nConf = vt->size();
            typedef detail_value_tablle_utitlities::ShapeFunctor<VT> ShapeF;
            ConfIterator<ShapeF > confIter(ShapeF(vt), arity, nConf);
            ConfIterator<ShapeF > confEnd = confIter.getEnd();
            for( ; confIter != confEnd; ++confIter){
                const auto conf = confIter->data();
                // sum up incoming messages
                ValueType s = vt->eval(conf);
                for(uint32_t a=0; a<arity; ++a)
                    s += inMsgs[a][conf[a]];
                // fill outgoing messages
                for(uint32_t a=0; a<arity; ++a){
                    const auto confA = conf[a];
                    const auto currentVal = outMsgs[a][confA];
                    outMsgs[a][confA] = std::min(currentVal, s - inMsgs[a][confA]); 
                }
            }
        }
    }

    template<class VT>
    inline void simple2OrderFacToVarMsg(
        const VT * vt,
        const DiscreteLabel nl,
        const ValueType ** inMsgs,
        ValueType ** outMsgs        
    ){
        INFERNO_ASSERT_OP(vt->arity(),==,2);     
        // initialize 
        for(auto a=0; a<2; ++a)
            std::fill(outMsgs[a], outMsgs[a]+nl, std::numeric_limits<ValueType>::infinity());
        // minimize
        for(DiscreteLabel l1=0; l1 < nl; ++l1)
        for(DiscreteLabel l0=0; l0 < nl; ++l0){
            const ValueType facVal = vt->eval(l0, l1);
            outMsgs[0][l0] = std::min(outMsgs[0][l0], facVal + inMsgs[1][l1]);
            outMsgs[1][l1] = std::min(outMsgs[1][l1], facVal + inMsgs[0][l0]);
        }
    }
    template<class VT>
    inline void l1FacToVarMsg(
        const VT * vt,
        const DiscreteLabel nl,
        const ValueType beta,
        const ValueType ** inMsgs,
        ValueType ** outMsgs                 
    ){
        if(beta>=0){
            std::copy(inMsgs[1], inMsgs[1]+nl, outMsgs[0]);
            std::copy(inMsgs[0], inMsgs[0]+nl, outMsgs[1]);
            // "forward pass"
            for(DiscreteLabel l=1; l<nl; ++l){
                outMsgs[0][l] = std::min(outMsgs[0][l], outMsgs[0][l-1] + beta);
                outMsgs[1][l] = std::min(outMsgs[1][l], outMsgs[1][l-1] + beta);
            }
            // backward pass
            for(DiscreteLabel l = nl-2; l>=0; --l){
                outMsgs[0][l] = std::min(outMsgs[0][l], outMsgs[0][l+1] + beta);
                outMsgs[1][l] = std::min(outMsgs[1][l], outMsgs[1][l+1] + beta);
                if(l == 0)
                    break;
            }
        }
        else{
            simple2OrderFacToVarMsg(vt,nl, inMsgs, outMsgs);
        }
    }

    template<class VT>
    inline void truncatedL1FacToVarMsg(
        const VT * vt,
        const DiscreteLabel nl,
        const ValueType beta,
        const ValueType truncateAt,
        const ValueType ** inMsgs,
        ValueType ** outMsgs                 
    ){
        if(beta>=0){
            l1FacToVarMsg(vt, nl, beta, inMsgs, outMsgs);
            //if(truncateAt<beta*(nl)){
                const ValueType minIn0Trunc = *std::min_element(inMsgs[0],inMsgs[0]+nl)+truncateAt;
                const ValueType minIn1Trunc = *std::min_element(inMsgs[1],inMsgs[1]+nl)+truncateAt;

                for(DiscreteLabel l = 0; l<nl; ++l){
                    outMsgs[0][l] = std::min(outMsgs[0][l],minIn1Trunc);
                    outMsgs[1][l] = std::min(outMsgs[1][l],minIn0Trunc);
                }
            //}
        }
        else{
            simple2OrderFacToVarMsg(vt,nl, inMsgs, outMsgs);
        }
    }
}
}

#endif /* INFERNO_VALUE_TABLES_VALUE_TABLE_UTILITIES_HXX */
