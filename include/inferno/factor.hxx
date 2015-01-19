#ifndef INFERNOS_FACTOR_HXX
#define INFERNOS_FACTOR_HXX

#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <memory>

#include "inferno/inferno.hxx"
#include "inferno/mixed_label.hxx"


namespace inferno{





template<class T>
class Bounds{
public:
    Bounds(const T lowerBound = T(), const T upperBound = T());
    T lowerBound()const;
    T upperBound()const;

private:
    T lowerBound_;
    T upperBound_;
};

typedef Bounds<DiscreteLabel>   DiscreteLabelBounds;
typedef Bounds<ContinousLabel> ContinousLabelBounds;
typedef Bounds<MixedLabel>     MixedLabelBounds;


typedef std::initializer_list<DiscreteLabel>  DiscreteLabelInitList;
typedef std::initializer_list<ContinousLabel> ContinousLabelInitList;
typedef std::initializer_list<DiscreteLabel>  MixedLabelInitList;

class Factor{
public:
    virtual size_t arity() const = 0 ;
};



class DiscreteFactor : public Factor{
public:
    virtual DiscreteLabelBounds bounds(size_t d) const = 0;
    virtual FunctionValueType   eval(DiscreteLabelInitList conf) const = 0 ;
    virtual FunctionValueType   eval(const DiscreteLabel * conf) const = 0 ;

};

class ContinousFactor : public Factor{
public:
    virtual ContinousLabelBounds bounds(size_t d) const = 0;
    virtual FunctionValueType   eval(ContinousLabelInitList conf) const = 0 ;
    virtual FunctionValueType   eval(const ContinousLabel * conf) const = 0 ;
};

class MixedFactor : public Factor{
public:
    virtual MixedLabelBounds    bounds(size_t d) const =0;
    virtual FunctionValueType   eval(MixedLabelInitList conf) const = 0 ;
    virtual FunctionValueType   eval(const MixedLabel * conf) const = 0 ;
};



class DiscreteConstraint : public DiscreteFactor{
public:
    virtual bool feasible(DiscreteLabelInitList conf) const = 0 ;
    virtual bool feasible(const DiscreteLabel * conf) const = 0 ;
};




// a dummy factor testing the api
class TwoClassUnary : public DiscreteFactor{

public:
    TwoClassUnary(const FunctionValueType v0, const FunctionValueType v1);
    virtual size_t arity() const ;
    virtual DiscreteLabelBounds bounds(size_t d) const ;
    virtual FunctionValueType   eval(DiscreteLabelInitList conf) const ;
    virtual FunctionValueType   eval(const DiscreteLabel * conf) const ;
private:
    FunctionValueType v0_,v1_;
};


// a dummy factor testing the api
class TwoClassPottsBinary : public DiscreteFactor{

public:
    TwoClassPottsBinary(const FunctionValueType v);
    virtual size_t arity() const ;
    virtual DiscreteLabelBounds bounds(size_t d) const ;
    virtual FunctionValueType   eval(DiscreteLabelInitList conf) const ;
    virtual FunctionValueType   eval(const DiscreteLabel * conf) const ;
private:
    FunctionValueType v_;
};



}

#endif
