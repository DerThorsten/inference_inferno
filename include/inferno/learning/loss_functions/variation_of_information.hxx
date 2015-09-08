#ifndef INFERNO_LEARNING_LOSS_FUNCTIONS_VARIATION_OF_INFORMATION_HXX
#define INFERNO_LEARNING_LOSS_FUNCTIONS_VARIATION_OF_INFORMATION_HXX


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
    class VariationOfInformation2 : public NonDecomposableLossFunctionBase<MODEL>{

    public:
        typedef NonDecomposableLossFunctionBase<MODEL> Base;
        typedef typename Base::Model Model;
        typedef typename Base::ConfMap ConfMap;
        typedef typename Base::LossAugmentedModel LossAugmentedModel;

        // specific typedef for variation of information
        typedef typename Model:: template VariableMap<double> VariableSizeMap; 

        VariationOfInformation2()
        :   Base(),
            variableSizeMap_(),
            useIgnoreLabel_(false),
            ignoreLabel_(-1){
        }
        VariationOfInformation2(
            const Model & model,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel
        )
        :   VariationOfInformation2(model, useIgnoreLabel, 
                            ignoreLabel,VariableSizeMap(model,1)){
        }

        VariationOfInformation2(
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
            return std::move(std::string("VariationOfInformation2"));
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
        

            // normalize
            vConfGt/=accSize;
            vConf/=accSize;
            mat/=accSize;

            // entropy
            auto computeEntropy = [&](const LVector & lvec){
                double h = 0.0;
                for(const auto & kv: lvec)
                    h -= kv.second * std::log(kv.second);
                return h;
            };
            double hConfGt = computeEntropy(vConfGt);
            double hConf   = computeEntropy(vConf);

            double I = 0.0;
            for(const auto & kv : mat){
                const auto lConfGt = kv.first.first;
                const auto lConf   = kv.first.second;
                const auto valVConfGt = vConfGt[lConfGt];
                const auto valVConf   = vConf[lConf];
                const auto valMat     = kv.second;
                I += valMat * std::log(valMat / (valVConfGt * valVConf));
            }
            return hConfGt + hConf - 2.0 * I;

        }

    private:
        VariableSizeMap variableSizeMap_;
        bool useIgnoreLabel_;
        DiscreteLabel ignoreLabel_;
    };



    


} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LOSS_FUNCTIONS_VARIATION_OF_INFORMATION_HXX*/
