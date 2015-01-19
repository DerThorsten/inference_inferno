#ifndef INFERNOS_MODEL_HXX
#define INFERNOS_MODEL_HXX

#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <memory>

#include "inferno/mixed_label.hxx"
#include "inferno/mixed_label_vec.hxx"
#include "inferno/variable_space.hxx"
#include "inferno/factor.hxx"
#include "inferno/function.hxx"


namespace inferno{





class ModelBase{
    

    // number of variables
    virtual int64_t minFactorId()const = 0;
    virtual int64_t maxFactorId()const = 0;
    virtual bool isFactorId(const int64_t var)const = 0;
    virtual int64_t nFactors()const = 0;



    // number of factors
    virtual int64_t minVariableId()const = 0;
    virtual int64_t maxVariableId()const = 0;
    virtual bool isVariableId(const int64_t var)const = 0;
    virtual int64_t nVariables()const = 0;


    virtual std::shared_ptr<FactorBase>  getFactor(const int64_t) = 0 ;


    virtual VarSpace  varSpace(const int64_t vi)const = 0 ;




};




class ExplicitModel : public ModelBase{
public:
    ExplicitModel(const int64_t nVar = 0, const VarSpace & space = VarSpace());
    void addFactor( std::shared_ptr<FactorBase> factor);

    virtual int64_t minFactorId()const;
    virtual int64_t maxFactorId()const;
    virtual int64_t nFactors()const;
    virtual bool isFactorId(const int64_t id)const;
    virtual int64_t minVariableId()const;
    virtual int64_t maxVariableId()const;
    virtual int64_t nVariables()const;
    virtual bool isVariableId(const int64_t id)const;
    virtual std::shared_ptr<FactorBase>  getFactor(const int64_t fi);
    virtual VarSpace  varSpace(const int64_t vi)const;
private:
    int64_t nVar_;
    std::vector< std::shared_ptr<FactorBase> > factors_;
    std::vector<VarSpace> space_;

};


}

#endif
