#ifndef INFERNOS_MODEL_HXX
#define INFERNOS_MODEL_HXX

#include <cstdint>
#include <iostream>
#include <limits>
#include <initializer_list>
#include <vector>
#include <memory>

#include "inferno/factor.hxx"


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



    class Model{
    public:
        virtual FactorSharedPtr getFactor(const size_t fi) const = 0;
        
        virtual VariablesInfo variablesInfo()const = 0;
        virtual FactorsInfo factorsInfo()const = 0;

        virtual bool isVariableId(const int64_t id) const = 0;
        virtual bool isFactorId(const int64_t id) const = 0;




        virtual DiscreteFactorSharedPtr getFactor(const size_t fi, const DiscreteTag) const ;
        virtual ContinousFactorSharedPtr getFactor(const size_t fi, const ContinousTag) const ;
        virtual MixedFactorSharedPtr getFactor(const size_t fi, const MixedTag) const ;
        
    };



    class DiscreteModel : public Model{
    public:
        DiscreteModel()
        : Model(){

        }
        virtual void varBounds(const size_t vi, DiscreteLabelBounds & bounds)const = 0;
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

        virtual FactorSharedPtr getFactor(const size_t fi)const;
        virtual VariablesInfo variablesInfo()const;
        virtual FactorsInfo factorsInfo()const;
        virtual bool isVariableId(const int64_t id) const;
        virtual bool isFactorId(const int64_t id) const;

        virtual void varBounds(const size_t vi, DiscreteLabelBounds & bounds)const;

    private:
        size_t nVar_;
        std::vector< DiscreteLabelBounds > varBounds_;
        std::vector<DiscreteFactorSharedPtr> factors_;
    };

}

#endif
