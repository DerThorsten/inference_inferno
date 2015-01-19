#include "inferno/small_vector.hxx"
#include "inferno/mixed_label.hxx"


// INSTANCIATIONS OF TEMPLATED CLASSES
template class inferno::SmallVector<inferno::DiscreteLabel,  inferno::USUAL_MAX_FACTOR_ORDER>;
template class inferno::SmallVector<inferno::ContinousLabel, inferno::USUAL_MAX_FACTOR_ORDER>;
template class inferno::SmallVector<inferno::MixedLabel,     inferno::USUAL_MAX_FACTOR_ORDER>;
