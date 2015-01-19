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

    struct VarInfo{


        
        int64_t minVarId_;
        int64_t maxVarId_;
        uint64_t numVar_;
    };


    class Model{
    public:
        virtual SharedFactorPtr getFactor(const size_t fi) const = 0;
    };



    class DiscreteModel : public Model{
    public:
        virtual void varBounds(const size_t vi, DiscreteLabelBounds & bounds)const = 0;
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
        void addFactor(SharedDiscreteFactorPtr factor);
        virtual SharedFactorPtr getFactor(const size_t fi)const;
        virtual void varBounds(const size_t vi, DiscreteLabelBounds & bounds)const;
    private:
        size_t nVar_;
        std::vector< DiscreteLabelBounds > varBounds_;
        std::vector<SharedDiscreteFactorPtr> factors_;
    };

}

#endif
