/** \file inferno.hxx 
    \brief Main header of inferno.
    Any project using inferno should 
    include this header.


    This header has some fundamental typedefs.
    Runtime error classes 
    and useful macros for runtime checks.
    In fact, almost any other header of inferno
    will include this one.
*/
#ifndef INFERNO_INFERNO_HXX
#define INFERNO_INFERNO_HXX


#include <boost/iterator/counting_iterator.hpp>

#include "inferno/utilities/utilities.hxx"
#include "inferno/utilities/marray_wrap.hxx"

#include <cstdint>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <limits>
#include <cmath>



/** \def INFERNO_CHECK_OP(a,op,b,message)
    \brief macro for runtime checks
    
    \warning The check is done 
        <B> even in Release mode </B> 
        (therefore if NDEBUG <B>is</B> defined)

    \param a : first argument (like a number )
    \param op : operator (== )
    \param b : second argument (like a number )
    \param message : error message (as "my error")

    <b>Usage:</b>
    \code
        int a = 1;
        INFERNO_CHECK_OP(a, ==, 1, "this should never fail")
        INFERNO_CHECK_OP(a, >=, 2, "this should fail")
    \endcode
*/
#define INFERNO_CHECK_OP(a,op,b,message) \
    if(!  static_cast<bool>( a op b )   ) { \
       std::stringstream s; \
       s << "Inferno Error: "<< message <<"\n";\
       s << "Inferno check :  " << #a <<#op <<#b<< "  failed:\n"; \
       s << #a " = "<<a<<"\n"; \
       s << #b " = "<<b<<"\n"; \
       s << "in file " << __FILE__ << ", line " << __LINE__ << "\n"; \
       throw std::runtime_error(s.str()); \
    }

/** \def INFERNO_CHECK(expression,message)
    \brief macro for runtime checks
    
    \warning The check is done 
        <B> even in Release mode </B> 
        (therefore if NDEBUG <B>is</B> defined)

    \param expression : expression which can evaluate to bool
    \param message : error message (as "my error")

    <b>Usage:</b>
    \code
        int a = 1;
        INFERNO_CHECK_OP(a==1, "this should never fail")
        INFERNO_CHECK_OP(a>=2, "this should fail")
    \endcode
*/
#define INFERNO_CHECK(expression,message) if(!(expression)) { \
   std::stringstream s; \
   s << message <<"\n";\
   s << "Inferno assertion " << #expression \
   << " failed in file " << __FILE__ \
   << ", line " << __LINE__ << std::endl; \
   throw std::runtime_error(s.str()); \
 }



#define INFERNO_CHECK_NUMBER(number) \
   { \
   std::stringstream s; \
   s << "Inferno assertion failed in file " << __FILE__ \
   << ", line " << __LINE__ << std::endl; \
    if(std::isnan(number))\
        throw std::runtime_error(s.str()+" number is nan"); \
    if(std::isinf(number))\
        throw std::runtime_error(s.str()+"number is inf");\
    }

#ifdef NDEBUG
    #ifdef INFERNO_DEBUG 
        #define INFERNO_DO_DEBUG
    #endif
#else
    #ifdef INFERNO_DEBUG 
        #define INFERNO_DO_DEBUG
    #endif
#endif


/** \def INFERNO_ASSERT_OP(a,op,b,message)
    \brief macro for runtime checks
    
    \warning The check is <B>only</B> done in
        in Debug mode (therefore if NDEBUG is <B>not</B> defined)

    \param a : first argument (like a number )
    \param op : operator (== )
    \param b : second argument (like a number )
    \param message : error message (as "my error")

    <b>Usage:</b>
    \code
        int a = 1;
        INFERNO_ASSERT_OP(a, ==, 1) // will not fail here
        INFERNO_ASSERT_OP(a, >=, 2) // will fail here
    \endcode
*/
#ifdef NDEBUG
   #ifndef INFERNO_DEBUG 
      #define INFERNO_ASSERT_OP(a,op,b) { }
   #else
      #define INFERNO_ASSERT_OP(a,op,b) \
      if(!  static_cast<bool>( a op b )   ) { \
         std::stringstream s; \
         s << "Inferno assertion :  " << #a <<#op <<#b<< "  failed:\n"; \
         s << #a " = "<<a<<"\n"; \
         s << #b " = "<<b<<"\n"; \
         s << "in file " << __FILE__ << ", line " << __LINE__ << "\n"; \
         throw std::runtime_error(s.str()); \
      }
   #endif
#else
   #define INFERNO_ASSERT_OP(a,op,b) \
   if(!  static_cast<bool>( a op b )   ) { \
      std::stringstream s; \
      s << "Inferno assertion :  " << #a <<#op <<#b<< "  failed:\n"; \
      s << #a " = "<<a<<"\n"; \
      s << #b " = "<<b<<"\n"; \
      s << "in file " << __FILE__ << ", line " << __LINE__ << "\n"; \
      throw std::runtime_error(s.str()); \
   }
#endif

/** \def INFERNO_ASSERT(expression,message)
    \brief macro for runtime checks
    
    \warning The check is <B>only</B> done in
        in Debug mode (therefore if NDEBUG is <B>not</B> defined)

    \param expression : expression which can evaluate to bool

    <b>Usage:</b>
    \code
        int a = 1;
        INFERNO_ASSERT(a == 1) // will not fail here 
        INFERNO_ASSERT(a >= 2) // will fail here
    \endcode
*/
#ifdef NDEBUG
   #ifndef INFERNO_DEBUG
      #define INFERNO_ASSERT(expression) {}
   #else
      #define INFERNO_ASSERT(expression) if(!(expression)) { \
         std::stringstream s; \
         s << "Inferno assertion " << #expression \
         << " failed in file " << __FILE__ \
         << ", line " << __LINE__ << std::endl; \
         throw std::runtime_error(s.str()); \
      }
   #endif
#else
      #define INFERNO_ASSERT(expression) if(!(expression)) { \
         std::stringstream s; \
         s << "Inferno assertion " << #expression \
         << " failed in file " << __FILE__ \
         << ", line " << __LINE__ << std::endl; \
         throw std::runtime_error(s.str()); \
      }
#endif











namespace inferno{




const static int USUAL_MAX_FACTOR_ORDER = 10;


template<class T>
inline utilities::ConstIteratorRange<  boost::counting_iterator<T> >
range(const T size){
    typedef  boost::counting_iterator<T>  Iter;
    return utilities::ConstIteratorRange<  Iter >( Iter(0),Iter(size)); 
}


template<class T>
inline bool fEq(const T a,const T b) {
    return std::fabs(a - b) < std::numeric_limits<T>::epsilon();
}

/** \var typedef LabelType 
    \brief LabelType encodes a single discrete label
    
    LabelType is a fundamental (signed) type but
    labels currently start at zero.
    An signed type has been selected,
    since negative labels could encode
    a ``invalid'' label.
    This type is also used to encode
    the number of labels for variables.
    The number of labels of a single variable is currently 
    always the maximum label + 1, the minimum label
    is always 0.
*/
typedef int64_t LabelType;

typedef uint32_t ArityType;
typedef int64_t Vi;
typedef int64_t Fi;
typedef int64_t Ci;
typedef int64_t Vti;
typedef int64_t WeightIndex;

typedef LabelType DiscreteLabel;
typedef double    ContinousLabel;
typedef double    FunctionValueType;
typedef double    FactorValueType;
typedef double    ValueType;
typedef double    WeightType;
typedef double    JointFeaturesValueType;
typedef double    WeightGradientType;
typedef double    LossType;
typedef double    LpValueType;

typedef std::vector<DiscreteLabel > DiscreteLabelVec;
typedef std::vector<ContinousLabel > ContinousLabelVec;


typedef andres::View<ValueType> ValueMarrayView;
typedef andres::Marray<ValueType> ValueMarray;

/// \brief The semi ring on which inference is performed
enum SemiRing{
    MinSum = 0,
    MaxProd = 1,
    SumProd = 2
};



/// \return infinity for infernos value type
inline ValueType infVal(){
    return std::numeric_limits<ValueType>::infinity();
}

template<class T>
inline bool floatEqual(T x, T y)
{
  const T epsilon =  2*std::numeric_limits<T>::epsilon();
  return std::abs(x - y) <= epsilon;
  // see Knuth section 4.2.2 pages 217-218
}

template<class T>
inline bool isFloatZero(T x){
    return floatEqual(x, T(0));
}


/**  \brief Default inferno exception
*/
class RuntimeError
: public std::runtime_error
{
public:
    typedef std::runtime_error base;

    RuntimeError(const std::string& message)
    :   base(std::string("Inferno error: ") + message) {

    }
};

/**  \brief Exception for not yet implemented / future functionality
*/
class NotImplementedException
: public std::exception {

public:

    /// Construct with given error message:
    NotImplementedException(const std::string & message = std::string())
    :   errorMessage_()
    {
        errorMessage_ = std::string("Inferno Not Implemented Exception:") + message;
    }

    /// Provided for compatibility with std::exception.
    const char * what() const noexcept
    {
    return errorMessage_.c_str();
    }

private:

    std::string errorMessage_;
};



}

#endif
