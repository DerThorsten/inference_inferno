#ifndef INFERNO_MODEL_INFOq_HXX
#define INFERNO_MODEL_INFOq_HXX

#include <cstdint>

namespace inferno{


    class VariablesInfo{
    public:
        VariablesInfo(
            const int64_t  minVariableId=0,
            const int64_t  maxVariableId=0,
            const uint64_t numVariables=0
        )
        :   minVariableId_(minVariableId),
            maxVariableId_(maxVariableId),
            numVariables_(numVariables){
        }

        int64_t  minVariableId()const{return minVariableId_;}
        int64_t  maxVariableId()const{return maxVariableId_;}
        uint64_t numVariables()const {return numVariables_;}
        bool isDense() const {
            return (maxVariableId_ - minVariableId_) == numVariables_;
        }
    private:
        int64_t  minVariableId_;
        int64_t  maxVariableId_;
        uint64_t numVariables_;
    };

    class FactorsInfo{
    public:
        FactorsInfo(
            const int64_t  minFactorId=0,
            const int64_t  maxFactorId=0,
            const uint64_t numFactors=0
        )
        :   minFactorId_(minFactorId),
            maxFactorId_(maxFactorId),
            numFactors_(numFactors){
        }
        int64_t  minFactorId()const{return minFactorId_;}
        int64_t  maxFactorId()const{return maxFactorId_;}
        uint64_t numFactors()const {return numFactors_;}
        bool isDense() const {
            return (maxFactorId_ - minFactorId_) == numFactors_;
        }
    private:
        int64_t  minFactorId_;
        int64_t  maxFactorId_;
        uint64_t numFactors_;
    };

}

#endif
