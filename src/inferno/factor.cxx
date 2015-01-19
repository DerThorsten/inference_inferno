#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <memory>

#include "inferno/factor.hxx"

namespace inferno{



template<class T>
Bounds<T>::Bounds(const T lowerBound , const T upperBound){

}

template<class T>
inline T Bounds<T>::upperBound()const{
    return upperBound_;
}

template<class T>
inline T Bounds<T>::lowerBound()const{
    return lowerBound_;
}



TwoClassUnary::TwoClassUnary(const FunctionValueType v0, const FunctionValueType v1)
:   v0_(v0),
    v1_(v1){
}

inline size_t TwoClassUnary::arity() const {
    return 1;
}

inline DiscreteLabelBounds TwoClassUnary::bounds(size_t d) const {
    return DiscreteLabelBounds(0,1);
}

inline FunctionValueType   TwoClassUnary::eval(DiscreteLabelInitList conf) const {
    return *conf.begin() == 0 ? v0_ : v1_;
}

inline FunctionValueType   TwoClassUnary::eval(const DiscreteLabel * conf) const {
    return *conf == 0 ? v0_ : v1_;
}



TwoClassPottsBinary::TwoClassPottsBinary(const FunctionValueType v)
:   v_(v){
}

inline size_t TwoClassPottsBinary::arity() const {
    return 1;
}

inline DiscreteLabelBounds TwoClassPottsBinary::bounds(size_t d) const {
    return DiscreteLabelBounds(0,1);
}

inline FunctionValueType   TwoClassPottsBinary::eval(DiscreteLabelInitList conf) const {
    return conf.begin()[0]!=conf.begin()[1] ? v_ : 0.0;
}

inline FunctionValueType   TwoClassPottsBinary::eval(const DiscreteLabel * conf) const {
    return conf[0]!=conf[1] ? v_ : 0.0;
}


} 

// INSTANCIATIONS OF TEMPLATED CLASSES
template class inferno::Bounds<inferno::DiscreteLabel>;
template class inferno::Bounds<inferno::ContinousLabel>;
template class inferno::Bounds<inferno::MixedLabel>;
