#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <memory>

#include "inferno/factor.hxx"
#include "inferno/small_vector.hxx"

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


TwoClassUnary::TwoClassUnary(
    const int64_t vi,
    const FunctionValueType v0, 
    const FunctionValueType v1
)
:   DiscreteFactor(),
    vi_(vi),
    v0_(v0),
    v1_(v1){
}

inline size_t TwoClassUnary::arity() const {
    return 1;
}

inline void TwoClassUnary::bounds(size_t d, MixedLabelBounds & bounds) const {
    bounds = MixedLabelBounds(0,1);
}

inline void TwoClassUnary::bounds(size_t d, ContinousLabelBounds & bounds) const {
    bounds = ContinousLabelBounds(0.0,1.0);
}

inline void TwoClassUnary::bounds(size_t d, DiscreteLabelBounds & bounds) const {
    bounds = DiscreteLabelBounds(0,1);
}

inline FunctionValueType   TwoClassUnary::eval(const MixedLabel * conf) const {
    return DiscreteLabel(*conf) == 0 ? v0_ : v1_;
}

inline FunctionValueType   TwoClassUnary::eval(const DiscreteLabel * conf) const {
    return *conf == 0 ? v0_ : v1_;
}

inline FunctionValueType   TwoClassUnary::eval(const ContinousLabel * conf) const {
    return DiscreteLabel(std::round(*conf)) == 0 ? v0_ : v1_;
}



inline int64_t 
TwoClassUnary::vi(const size_t d) const {
    return vi_;
}




TwoClassPottsBinary::TwoClassPottsBinary(
    const int64_t vi0,
    const int64_t vi1,
    const FunctionValueType v
)
:   DiscreteFactor(),
    vi0_(vi0),
    vi1_(vi1),
    v_(v){
}

inline size_t TwoClassPottsBinary::arity() const {
    return 1;
}

inline void TwoClassPottsBinary::bounds(size_t d, MixedLabelBounds & bounds) const {
    bounds = MixedLabelBounds(0,1);
}

inline void TwoClassPottsBinary::bounds(size_t d, ContinousLabelBounds & bounds) const {
    bounds = ContinousLabelBounds(0.0,1.0);
}

inline void TwoClassPottsBinary::bounds(size_t d, DiscreteLabelBounds & bounds) const {
    bounds = DiscreteLabelBounds(0,1);
}

inline FunctionValueType   TwoClassPottsBinary::eval(const MixedLabel * conf) const {
    return DiscreteLabel(conf[0]) != DiscreteLabel(conf[1]) ? 
            v_ : 0.0;
}
inline FunctionValueType   TwoClassPottsBinary::eval(const DiscreteLabel * conf) const {
    return conf[0]!=conf[1] ? v_ : 0.0;
}
inline FunctionValueType   TwoClassPottsBinary::eval(const ContinousLabel * conf) const {
    return DiscreteLabel(std::round(conf[0])) != DiscreteLabel(std::round(conf[1])) ? 
            v_ : 0.0;
}

inline int64_t 
TwoClassPottsBinary::vi(const size_t d) const {
    return d==0 ? vi0_ : vi1_;
}

} 

// INSTANCIATIONS OF TEMPLATED CLASSES
template class inferno::Bounds<inferno::DiscreteLabel>;
template class inferno::Bounds<inferno::ContinousLabel>;
template class inferno::Bounds<inferno::MixedLabel>;
