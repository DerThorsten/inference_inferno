#ifndef INFERNO_LEARNING_LOSS_FUNCTIONS_LOSS_FUNCTIONS_HXX
#define INFERNO_LEARNING_LOSS_FUNCTIONS_LOSS_FUNCTIONS_HXX

// standard
#include <ctypes>

// vigra
#include <vigra/multi_array.hxx>

// inferno
#include "inferno/inferno.hxx"

namespace inferno{
namespace learning{

/** \namespace loss_functions

    - (node-weighted) unstructured 
        - hamming,
        - L1,
        - L2 
        - arbitrary label confusion costs
    - partition 
        - weighted edge hamming
    - structured
        - variation of information
        - rand index
*/
namespace loss_functions{




    template<class MODEL, class VARIABLE_MULTIPLIER, class LABEL_CONFUSION_COST>
    class UnstructuredLoss{
    public:
        typedef MODEL Model;
        typedef typename Model:: template NodeMap<DiscreteLabel> ConfMap;
        typedef VARIABLE_MULTIPLIER VariableMultiplier; 
        typedef LABEL_CONFUSION_COST LabelConfusionCost;

        typedef MODEL LossAugmentedModel;
        template<class CONF_GT, class CONF>
        LossType eval(CONF_GT & confGt, CONF & conf)const{
            LossType totalLoss = 0.0;
            for(const var : model_.variables()){
                const auto lConfGt = confGt[var];
                const auto lConf   = conf[var];
                if(lConfGt != lConf){
                    const auto l =  * labelConfusionMultiplier_(lConfGt,lConf);
                    const auto varMult =  variableMultiplier_[var];
                    totalLoss += l*varMult;
                }
            }
            return totalLoss;
        }
        void makeLossAugmentedModel(LossAugmentedModel & lossAugmentedModel)const;

    private:
        VariableMultiplier variableMultiplier_;
        LabelConfusionCost labelConfusionMultiplier_;

    };


} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LOSS_FUNCTIONS_LOSS_FUNCTIONS_HXX*/
