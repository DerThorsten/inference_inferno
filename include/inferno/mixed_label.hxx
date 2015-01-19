#ifndef INFERNOS_MIXED_LABEL_HXX
#define INFERNOS_MIXED_LABEL_HXX
#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include "inferno/inferno.hxx"

namespace inferno{



struct MixedLabel{
private:

public:

    MixedLabel();
    MixedLabel(const DiscreteLabel l);
    MixedLabel(const ContinousLabel l);
    template<class T>
    MixedLabel(const T l)
    :   isDiscrete_(std::numeric_limits<T>::is_integer),
        data_(){
        if(std::numeric_limits<T>::is_integer){
            data_ = static_cast<DiscreteLabel>(l);
        }
        else{
            const ContinousLabel ll = static_cast<ContinousLabel>(l);
            data_ = *reinterpret_cast<const DiscreteLabel *>(&ll);
        }
    }

    operator DiscreteLabel() const ;
    operator ContinousLabel() const ;
    DiscreteLabel d()const;
    ContinousLabel c()const;

    template<class T>
    operator T()const{
        return std::numeric_limits<T>::is_integer ? 
            static_cast<T>(DiscreteLabel(*this)) : 
            static_cast<T>(ContinousLabel(*this)) ;
    }

    bool isDiscrete() const;
private:
    ContinousLabel reinterpret()const;

    bool isDiscrete_;
    int64_t data_;
};
typedef std::vector<MixedLabel> MixedLabelVec;

MixedLabel operator "" _cl(const long double val);
MixedLabel operator "" _cl(const unsigned long long val);
MixedLabel operator "" _dl(const long double val);

MixedLabel operator "" _dl(const unsigned long long val);

inline std::ostream& operator<<(std::ostream& os, const MixedLabel & dt);


}

#endif
