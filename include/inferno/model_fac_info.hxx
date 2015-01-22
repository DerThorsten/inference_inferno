#ifndef INFERNO_MODEL_FAC_INFO_HXX
#define INFERNO_MODEL_FAC_INFO_HXX

#include <cstdint>

namespace inferno{

    class FactorsInfo{
    public:
        FactorsInfo(
            const int64_t  minFactorId=0,
            const int64_t  maxFactorId=0,
            const uint64_t numFactors=0
        );
        int64_t  minFactorId()const;
        int64_t  maxFactorId()const;
        uint64_t numFactors()const;
        bool isDense() const ;
    private:
        int64_t  minFactorId_;
        int64_t  maxFactorId_;
        uint64_t numFactors_;
    };

}

#endif
