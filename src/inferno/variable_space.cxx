#ifndef INFERNOS_LABEL_TYPE_CXX
#define INFERNOS_LABEL_TYPE_CXX
#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include "inferno/variable_space.hxx"
#include "inferno/mixed_label.hxx"
#include "inferno/mixed_label_vec.hxx"
namespace inferno{




VarSpace::VarSpace(const MixedLabel upperBound, const MixedLabel lowerBound )
: upperBound_(upperBound),
  lowerBound_(lowerBound){

}

MixedLabel VarSpace::upperBound()const{
    return upperBound_;
}
MixedLabel VarSpace::lowerBound()const{
    return lowerBound_;
}



}

#endif
