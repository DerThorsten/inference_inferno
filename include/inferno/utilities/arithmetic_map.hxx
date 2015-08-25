#pragma once
#ifndef INFERNO_ARITHMETIC_MAP_HXX
#define INFERNO_ARITHMETIC_MAP_HXX

#include <map>
#include <unordered_map>

#include "inferno/inferno.hxx"

namespace inferno {

    /// \brief enriches std::map / std::unordered map with unary arithmetic (+=, -=, *=, /=)
    /// 
    /// 
    template<class MAP>
    class ArithmeticMap : public MAP {
    public:
        ArithmeticMap()
        : MAP(){
            
        }

        
    };



    template<class MAP>
    ArithmeticMap<MAP> & operator *= (ArithmeticMap<MAP> & a, const typename MAP::mapped_type & value){
        for(auto & kv : a)
            kv.second *= value;
        return a;
    }
    template<class MAP>
    ArithmeticMap<MAP> & operator /= (ArithmeticMap<MAP> & a, const typename MAP::mapped_type & value){
        for(auto & kv : a)
            kv.second /= value;
        return a;
    }
    template<class MAP>
    ArithmeticMap<MAP> & operator += (ArithmeticMap<MAP> & a, const typename MAP::mapped_type & value){
        for(auto & kv : a)
            kv.second += value;
        return a;
    }
    template<class MAP>
    ArithmeticMap<MAP> & operator -= (ArithmeticMap<MAP> & a, const typename MAP::mapped_type & value){
        for(auto & kv : a)
            kv.second -= value;
        return a;
    }


    template<class KEY, class VALUE>
    using ArithmeticUnorderedMap = 
        ArithmeticMap<std::unordered_map<KEY,VALUE> >;

    template<class KEY, class VALUE>
    using ArithmeticOrderedMap = 
        ArithmeticMap<std::map<KEY,VALUE> >;



} // namespace inferno

#endif // INFERNO_ARITHMETIC_MAP_HXX
