#ifndef INFERNO_LEARNING_LEARNING_HXX
#define INFERNO_LEARNING_LEARNING_HXX

// std
#include <cstdint>

#include "inferno/learning/weights.hxx"

namespace inferno{
namespace learning{


    enum class RegularizerType {  
        L1,
        L2,
        ConstL1,
        ConstL2
    };

    class Regularizer{

    public:
       Regularizer(
           const RegularizerType r = RegularizerType::L1,
           const ValueType   v       = 1.0
       )
       :   regularizer_(r),
           value_(v){
       }

        ValueType eval(const WeightVector & weightVector)const{
              
            if(regularizer_ == RegularizerType::L1){
                return value_ * weightVector.getNorm(1);
            }
            else if(regularizer_ == RegularizerType::L2){
                return value_ * weightVector.getNorm(2);
            }
            else if(regularizer_ == RegularizerType::ConstL1){
                return value_ * std::abs(1.0 - weightVector.getNorm(1));
            }
            else if(regularizer_ == RegularizerType::ConstL2){
                const auto d = 1.0 - weightVector.getNorm(2);
                return value_ * d * d;
            }
             
        } 
        RegularizerType regularizer()const{
            return regularizer_;
        }
        ValueType value()const{
            return value_;
        }
   private:
       RegularizerType regularizer_;
       ValueType value_;
    };


} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LEARNING_HXX*/
