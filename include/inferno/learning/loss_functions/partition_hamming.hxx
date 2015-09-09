#ifndef INFERNO_LEARNING_LOSS_FUNCTIONS_PARTITION_HAMMING_HXX
#define INFERNO_LEARNING_LOSS_FUNCTIONS_PARTITION_HAMMING_HXX


// boost
#include <boost/concept_check.hpp>


// vigra
#include <vigra/multi_array.hxx>

// inferno
#include "inferno/inferno.hxx"
#include "inferno/utilities/arithmetic_map.hxx"
#include "inferno/model/loss_augmented/edge_hamming_augmented_model.hxx"
#include "inferno/learning/loss_functions/loss_function_base.hxx"

namespace inferno{
namespace learning{
namespace loss_functions{


 


    template<class MODEL>
    class PartitionHamming :
        public DecomposableLossFunctionBase<
            MODEL, 
            models::EdgeHammingLossAugmentedModel<MODEL> 
        >
    {
    public:

        typedef MODEL Model;
        typedef DecomposableLossFunctionBase<
            MODEL,
            models::EdgeHammingLossAugmentedModel<MODEL> 
        > Base;
        typedef typename Model:: template VariableMap<DiscreteLabel> ConfMap;
        typedef typename Model:: template FactorMap<double> FactorWeightMap; 
        typedef models::EdgeHammingLossAugmentedModel<Model>  LossAugmentedModel;
        
        BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<FactorWeightMap>));



        PartitionHamming(const Model & model,
                         const double rescale,
                         const double underseg,
                         const double overseg,
                         const bool useIgnoreLabel,
                         const DiscreteLabel ignoreLabel)
        :   PartitionHamming(model, rescale, underseg, overseg, 
                            useIgnoreLabel, ignoreLabel,
                            FactorWeightMap(model, 1))
        {
        }

        PartitionHamming(const Model & model,
                         const double rescale,
                         const double underseg,
                         const double overseg,
                         const bool useIgnoreLabel,
                         const DiscreteLabel ignoreLabel,
                         const FactorWeightMap & factorWeightMap)
        :   factorWeightMap_(model),
            rescale_(rescale),
            underseg_(underseg),
            overseg_(overseg),
            useIgnoreLabel_(useIgnoreLabel),
            ignoreLabel_(ignoreLabel){
            std::copy(factorWeightMap.begin(), factorWeightMap.end(),factorWeightMap_.begin());
        }

        #if 0
        void assign(const Model & model,
                    const FactorWeightMap & factorWeightMap,
                    const double rescale,
                    const double underseg,
                    const double overseg,
                    const bool useIgnoreLabel=false,
                    const DiscreteLabel ignoreLabel=DiscreteLabel())
        {
            factorWeightMap_.assign(model);
            std::copy(factorWeightMap.begin(), factorWeightMap.end(),factorWeightMap_.begin());
            rescale_ = rescale;
            underseg_ = underseg;
            overseg_ = overseg;
            useIgnoreLabel_ = useIgnoreLabel;
            ignoreLabel_ = ignoreLabel;
        }
        #endif

        FactorWeightMap & factorWeightMap(){
            return factorWeightMap_;
        }
        bool & useIgnoreLabel(){
            return useIgnoreLabel_;
        }
        DiscreteLabel & ingoreLabel(){
            return ignoreLabel_;
        }
        const FactorWeightMap & factorWeightMap()const{
            return factorWeightMap_;
        }
        const bool & useIgnoreLabel()const{
            return useIgnoreLabel_;
        }
        const DiscreteLabel & ingoreLabel()const{
            return ignoreLabel_;
        }

        virtual std::string name()const override{
            return "PartitionHamming";
        }
        virtual LossType eval(const Model & model, const ConfMap & confGt, const ConfMap & conf)const override{
            LossType totalLoss = 0;
            for(const auto fac : model.factorDescriptors()){

                const auto factor = model.factor(fac);
                const auto weight = factorWeightMap_[fac];
                const auto u = factor->variable(0);
                const auto v = factor->variable(1);

                const auto luGt = confGt[u];
                const auto lvGt = confGt[v];
                const auto lu   = conf[u];
                const auto lv   = conf[v];

                if(!useIgnoreLabel_ || (luGt!=ignoreLabel_ && lvGt!=ignoreLabel_)){
                    if( (luGt==lvGt) && (lu!=lv) ){
                        totalLoss += weight*overseg_;
                    }
                    else if( (luGt!=lvGt) && (lu==lv) ){
                        totalLoss += weight*underseg_;
                    }
                }
            }
            return totalLoss*rescale_;
        }

        virtual void makeLossAugmentedModel(
            const Model & model,
            const ConfMap & gt,
            LossAugmentedModel & lossAugmentedModel
        )const override{
            lossAugmentedModel.assign(model, factorWeightMap_, 
                                      gt,rescale_,underseg_,
                                      overseg_,
                                      useIgnoreLabel_, 
                                      ignoreLabel_);
        }

    private:
        FactorWeightMap factorWeightMap_;
        double rescale_;
        double underseg_;
        double overseg_;
        bool useIgnoreLabel_;
        DiscreteLabel ignoreLabel_;

    };



   


} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LOSS_FUNCTIONS_PARTITION_HAMMING_HXX*/
