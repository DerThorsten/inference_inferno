#ifndef INFERNOS_MIXED_LABEL_VEC_HXX
#define INFERNOS_MIXED_LABEL_VEC_HXX
#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include "inferno/mixed_label.hxx"
namespace inferno{



class MixedLabelVec : std::vector< MixedLabel> {
    MixedLabelVec(int64_t size);
};



}

#endif
