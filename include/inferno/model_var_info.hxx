#ifndef INFERNO_MODEL_VAR_INFO_HXX
#define INFERNO_MODEL_VAR_INFO_HXX

#include <cstdint>

namespace inferno{


    class VariablesInfo{
    public:
        VariablesInfo(
            const int64_t  minVariableId=0,
            const int64_t  maxVariableId=0,
            const uint64_t numVariables=0
        );

        int64_t  minVariableId()const;
        int64_t  maxVariableId()const;
        uint64_t numVariables()const;
        bool isDense() const ;
    private:
        int64_t  minVariableId_;
        int64_t  maxVariableId_;
        uint64_t numVariables_;
    };
}

#endif
