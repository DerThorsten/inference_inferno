#ifndef INFERNO_INFERNO_HXX
#define INFERNO_INFERNO_HXX

#include <cstdint>






#include <stdexcept>
#include <sstream>

#include "opengm/config.hxx"
#include "opengm/utilities/metaprogramming.hxx"




// as runtime assertion but cefined even if NDEBUG

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

#define INFERNO_CHECK(expression,message) if(!(expression)) { \
   std::stringstream s; \
   s << message <<"\n";\
   s << "Inferno assertion " << #expression \
   << " failed in file " << __FILE__ \
   << ", line " << __LINE__ << std::endl; \
   throw std::runtime_error(s.str()); \
 }


/// runtime assertion
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
typedef int64_t DiscreteLabel;
typedef double  ContinousLabel;
typedef double  FunctionValueType;


struct RuntimeError
: public std::runtime_error
{
   typedef std::runtime_error base;
   RuntimeError(const std::string& message = std::string());
};

}

#endif
