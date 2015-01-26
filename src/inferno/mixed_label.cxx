#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <cmath>

#include "inferno/inferno.hxx"
#include "inferno/utilities/mixed_label.hxx"

namespace inferno{






MixedLabel::MixedLabel()
:   isDiscrete_(true),
    data_(0){
}


MixedLabel::MixedLabel(const DiscreteLabel l)
:   isDiscrete_(true),
    data_(l){
}


MixedLabel::MixedLabel(const ContinousLabel l)
:   isDiscrete_(false),
    data_(*reinterpret_cast<const DiscreteLabel *>(&l)){
}


MixedLabel::operator DiscreteLabel() const { 
    return isDiscrete_ ? 
        data_ : static_cast< DiscreteLabel >(std::round(reinterpret())) ;

}
MixedLabel::operator ContinousLabel() const { 
    return isDiscrete_ ? 
        static_cast< ContinousLabel >(data_) : reinterpret(); 
}

DiscreteLabel MixedLabel::d()const{
    return DiscreteLabel(*this);
}
ContinousLabel MixedLabel::c()const{
    return ContinousLabel(*this);
}


bool MixedLabel::isDiscrete() const{
    return isDiscrete_;
}

ContinousLabel MixedLabel::reinterpret()const {
    return (*reinterpret_cast<const ContinousLabel *>(&data_));
}


MixedLabel operator "" _cl(const long double val){
    return MixedLabel(val);
}

MixedLabel operator "" _cl(const unsigned long long val){
    return MixedLabel(val);
}

MixedLabel operator "" _dl(const long double val){
    return MixedLabel(val);
}

MixedLabel operator "" _dl(const unsigned long long val){
    return MixedLabel(val);
}





inline std::ostream& operator<<(std::ostream& os, const MixedLabel & dt)
{
    if(dt.isDiscrete())
        os << DiscreteLabel(dt)<<"d";
    else
        os << ContinousLabel(dt)<<"c";
    return os;
}



}
