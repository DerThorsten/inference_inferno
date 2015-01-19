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
    template<class U>
    Bounds(const Bounds<U> & other)
    : lowerBound_(other.lowerBound()),
      upperBound_(other.upperBound()){

    }
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
typedef std::initializer_list<MixedLabel>  MixedLabelInitList;

class Factor{
public:

    virtual void bounds(size_t d, MixedLabelBounds & bounds) const =0;
    virtual FunctionValueType   eval(MixedLabelInitList conf) const = 0 ;
    virtual FunctionValueType   eval(const MixedLabel * conf) const = 0 ;

    virtual size_t arity() const = 0 ;
};



class DiscreteFactor : public Factor{
public:
    // default impl for factor api
    virtual void bounds(size_t d, MixedLabelBounds & bounds) const  ;
    virtual FunctionValueType   eval(MixedLabelInitList conf) const ;
    virtual FunctionValueType   eval(const MixedLabel * conf) const ;


    // extra api for discrete factor
    virtual void bounds(size_t d, DiscreteLabelBounds & bounds) const =0;
    virtual FunctionValueType   eval(DiscreteLabelInitList conf) const = 0 ;
    virtual FunctionValueType   eval(const DiscreteLabel * conf) const = 0 ;

};

class ContinousFactor : public Factor{
public:

    // default impl for factor api
    virtual void bounds(size_t d, MixedLabelBounds & bounds) const  ;
    virtual FunctionValueType   eval(MixedLabelInitList conf) const ;
    virtual FunctionValueType   eval(const MixedLabel * conf) const ;

    virtual void bounds(size_t d, ContinousLabelBounds & bounds) const =0;
    virtual FunctionValueType   eval(ContinousLabelInitList conf) const = 0 ;
    virtual FunctionValueType   eval(const ContinousLabel * conf) const = 0 ;
};

class MixedFactor : public Factor{
public:
    // no extra api
};

typedef std::shared_ptr<Factor>             SharedFactorPtr;
typedef std::shared_ptr<DiscreteFactor>     SharedDiscreteFactorPtr;
typedef std::shared_ptr<ContinousFactor>    SharedContinousFactorPtr;
typedef std::shared_ptr<MixedFactor>        SharedMixedFactorPtr;



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
    virtual void bounds(size_t d , DiscreteLabelBounds & b) const ;
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
    virtual void bounds(size_t d , DiscreteLabelBounds & b) const ;
    virtual FunctionValueType   eval(DiscreteLabelInitList conf) const ;
    virtual FunctionValueType   eval(const DiscreteLabel * conf) const ;
private:
    FunctionValueType v_;
};



}

#endif
