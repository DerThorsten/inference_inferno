#ifndef INFERNO_MODEL_HXX
#define INFERNO_MODEL_HXX

#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <memory>

#include "inferno/factor.hxx"
#include "inferno/model_var_info.hxx"
#include "inferno/model_fac_info.hxx"

namespace inferno{


    class Model{
    public:
        virtual FactorSharedPtr          getFactor(const int64_t fi) const = 0;
        virtual DiscreteFactorSharedPtr  getFactor(const int64_t fi, const DiscreteTag) const ;
        virtual ContinousFactorSharedPtr getFactor(const int64_t fi, const ContinousTag) const ;
        virtual MixedFactorSharedPtr     getFactor(const int64_t fi, const MixedTag) const ;

        virtual VariablesInfo variablesInfo()const = 0;
        virtual FactorsInfo factorsInfo()const = 0;

        virtual bool isVariableId(const int64_t id) const = 0;
        virtual bool isFactorId(const int64_t id) const = 0;





        
    };



    class DiscreteModel : public Model{
    public:
        DiscreteModel()
        : Model(){

        } //
        virtual void varBounds(const int64_t, DiscreteLabelBounds & bounds)const = 0;
        virtual FactorValueType evaluateSum(const DiscreteLabel * conf) const;
    };

    class ContinousModel : public Model{
    public:
       
    };

    class MixedModel : public Model{
    public:
       
    };

    typedef std::shared_ptr<Model> SharedModelPtr;
    typedef std::shared_ptr<MixedModel> SharedMixedModelPtr;
    typedef std::shared_ptr<ContinousModel> SharedContinousModelPtr;
    typedef std::shared_ptr<DiscreteModel> SharedDiscreteModelPtr;


    class ExplicitDiscreteModel : public DiscreteModel{
    public:
        ExplicitDiscreteModel(const size_t nVar = 0, const DiscreteLabelBounds & bounds = DiscreteLabelBounds());
        void addFactor(DiscreteFactorSharedPtr factor);

        virtual FactorSharedPtr getFactor(const int64_t fi)const;
        virtual VariablesInfo variablesInfo()const;
        virtual FactorsInfo factorsInfo()const;
        virtual bool isVariableId(const int64_t id) const;
        virtual bool isFactorId(const int64_t id) const;

        virtual void varBounds(const int64_t, DiscreteLabelBounds & bounds)const;

    private:
        size_t nVar_;
        std::vector< DiscreteLabelBounds > varBounds_;
        std::vector<DiscreteFactorSharedPtr> factors_;
    };

}

#endif
