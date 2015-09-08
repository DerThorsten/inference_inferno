#ifndef INFERNO_LEARNING_LOSS_FUNCTIONS_RAND_INDEX_HXX
#define INFERNO_LEARNING_LOSS_FUNCTIONS_RAND_INDEX_HXX


// boost
#include <boost/concept_check.hpp>

// vigra
#include <vigra/multi_array.hxx>

// inferno
#include "inferno/inferno.hxx"
#include "inferno/learning/loss_functions/loss_functions.hxx"
#include "inferno/learning/loss_functions/loss_function_base.hxx"

namespace inferno{
namespace learning{
namespace loss_functions{

    template<class MODEL>
    class RandIndex : public NonDecomposableLossFunctionBase<MODEL>{

    public:
        typedef NonDecomposableLossFunctionBase<MODEL> Base;
        typedef typename Base::Model Model;
        typedef typename Base::ConfMap ConfMap;
        typedef typename Base::LossAugmentedModel LossAugmentedModel;

        // specific typedef for randindex
        typedef typename Model:: template VariableMap<double> VariableSizeMap; 

        RandIndex()
        :   Base(),
            variableSizeMap_(),
            useIgnoreLabel_(false),
            ignoreLabel_(-1){
        }
        RandIndex(
            const Model & model,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel
        )
        :   RandIndex(model, useIgnoreLabel, 
                            ignoreLabel,VariableSizeMap(model,1)){
        }

        RandIndex(
            const Model & model,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel,
            const VariableSizeMap & variableSizeMap
        )
        :   variableSizeMap_(model),
            useIgnoreLabel_(useIgnoreLabel),
            ignoreLabel_(ignoreLabel)
            {
            for(const auto var : model.variableDescriptors())
                variableSizeMap_[var] = variableSizeMap[var];
        }

        virtual std::string name()const override{
            return std::move(std::string("RandIndex"));
        }
        
        virtual LossType eval(
            const Model & model, 
            const ConfMap & confGt, 
            const ConfMap & conf
        ) const override {
            typedef std::pair<DiscreteLabel, DiscreteLabel>       LabelPair;
            typedef ArithmeticOrderedMap<LabelPair, double>     LMatrix;
            typedef ArithmeticOrderedMap<DiscreteLabel, double> LVector;

            LVector vConfGt;
            LVector vConf;
            LMatrix mat;
            double accSize = 0;
            for(auto var : model.variableDescriptors()){

                // get labels
                const auto lConfGt = confGt[var];
                const auto lConf   = conf[var];

                // if not ignore label
                if(!ignoreLabel_ || (lConfGt!=ignoreLabel_ && lConf!=ignoreLabel_)){

                    // "size" of node
                    const auto  size = variableSizeMap_[var];
                    vConfGt[lConfGt] += size;
                    vConf[lConf]     += size;
                    mat[LabelPair(lConfGt,lConf)] += size;
                    accSize += size;
                }
            }
        

            double a = 0.0;
            double b = accSize * accSize;
            for(const auto kv : mat){
                const auto val = kv.second;
                a += val * (val - 1.0);
                b += val * val;
            }
            for(const auto kv : vConf){
                const auto val = kv.second;
                b -= val * val;
            }
            for(const auto kv : vConfGt){
                const auto val = kv.second;
                b -= val * val;
            }
            const auto matchingPairs = ((a + b) / 2.0);
            const auto ri = matchingPairs*2.0/(accSize*(accSize-1.0));
            return 1.0 - ri;

        }

    private:
        VariableSizeMap variableSizeMap_;
        bool useIgnoreLabel_;
        DiscreteLabel ignoreLabel_;
    };



    


} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LOSS_FUNCTIONS_RAND_INDEX_HXX*/
