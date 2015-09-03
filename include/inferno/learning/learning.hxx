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
            const ValueType   _c       = 1.0
        )
        :   regularizer_(r),
            c_(_c){
        }

        template<class WEIGHT_CONSTRAINTS>
        ValueType evalRegularizer(
            const WeightVector & weightVector,
            const WEIGHT_CONSTRAINTS & weightConstraints
        )const{  
            if(regularizer_ == RegularizerType::L1){
                return weightVector.getNorm(1,weightConstraints);
            }
            else if(regularizer_ == RegularizerType::L2){
                return weightVector.getNorm(2,weightConstraints);
            }
            else if(regularizer_ == RegularizerType::ConstL1){
                return std::abs(1.0 - weightVector.getNorm(1,weightConstraints) );
            }
            else if(regularizer_ == RegularizerType::ConstL2){
                const auto d = 1.0 - weightVector.getNorm(2, weightConstraints);
                return d * d;
            } 
        }
        template<class WEIGHT_CONSTRAINTS>
        ValueType eval(
            const WeightVector & weightVector, 
            const WEIGHT_CONSTRAINTS & weightConstraints,
            const LossType loss
        )const{   
            return c_ * loss + this->evalRegularizer(weightVector,weightConstraints);
        } 
        RegularizerType regularizer()const{
            return regularizer_;
        }
        ValueType c()const{
            return c_;
        }

        std::string prettyRegularizerString()const{
            if(regularizer_ == RegularizerType::L1){
                return std::move(std::string("‖ω‖¹"));
            }
            else if(regularizer_ == RegularizerType::L2){
                return std::move(std::string("‖ω‖²"));
            }
            else if(regularizer_ == RegularizerType::ConstL1){
                return std::move(std::string("|1-‖ω‖¹|"));
            }
            else if(regularizer_ == RegularizerType::ConstL2){
                return std::move(std::string("|1-‖ω‖²|²"));
            }
        }

        std::string prettyLossString()const{
            return std::move(std::string("Δ(y',y)"));
        }
        std::string prettyLossSumString()const{
            return std::move(std::string("C ⋅ Σ Δ(y',y)ₘ"));
        }

        std::string prettyObjectiveString()const{
            auto s = prettyRegularizerString() + std::string(" + ") +
                prettyLossSumString();
            return std::move(s);
        }

   private:
       RegularizerType regularizer_;
       ValueType c_;
    };


} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LEARNING_HXX*/
