#ifndef INFERNOS_VARIABLE_SPACE_HXX
#define INFERNOS_VARIABLE_SPACE_HXX
#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include "inferno/mixed_label.hxx"
#include "inferno/mixed_label_vec.hxx"
namespace inferno{

typedef int64_t DiscreteLabel;
typedef double  ContinousLabel;
typedef double  FunctionValueType;



class VarSpace{
public:
    VarSpace(const MixedLabel upperBound = MixedLabel(), const MixedLabel lowerBound = MixedLabel());
    MixedLabel upperBound()const;
    MixedLabel lowerBound()const;

private:
    MixedLabel upperBound_;
    MixedLabel lowerBound_;
};










}

#endif
