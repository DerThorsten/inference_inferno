#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <memory>

#include "inferno/model.hxx"


namespace inferno{











    ExplicitModel::ExplicitModel(const int64_t nVar , const VarSpace & space )
    : nVar_(nVar),
      space_(1,space){

    }

    void ExplicitModel::addFactor( std::shared_ptr<FactorBase> factor){
        factors_.push_back(factor);
    }

    int64_t ExplicitModel::minFactorId()const {
        return factors_.size()==0? -1: factors_.size()-1;
    }
    int64_t ExplicitModel::maxFactorId()const {
        return factors_.size()==0? -1: factors_.size()-1;
    }
    int64_t ExplicitModel::nFactors()const {
        return factors_.size();
    }
    bool ExplicitModel::isFactorId(const int64_t id)const {
        return id>=0 && id<factors_.size();
    }

    int64_t ExplicitModel::minVariableId()const {
        return nVar_>=0? 0: -1;
    }
    int64_t ExplicitModel::maxVariableId()const {
        return nVar_>=0? nVar_-1: -1;
    }
    int64_t ExplicitModel::nVariables()const {
        return nVar_;
    }
    bool ExplicitModel::isVariableId(const int64_t id)const {
        return id>=0 && id< nVar_;
    }

    std::shared_ptr<FactorBase>  ExplicitModel::getFactor(const int64_t fi) {

    }
    VarSpace  ExplicitModel::varSpace(const int64_t vi)const {
        return vi>space_.size() ? space_[0] : space_[vi];
    }

}
