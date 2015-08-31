#ifndef INFERNO_LEARNING_LOSS_FUNCTIONS_EDGE_HAMMING_HXX
#define INFERNO_LEARNING_LOSS_FUNCTIONS_EDGE_HAMMING_HXX


// boost
#include <boost/concept_check.hpp>


// vigra
#include <vigra/multi_array.hxx>

// inferno
#include "inferno/inferno.hxx"
#include "inferno/utilities/arithmetic_map.hxx"
#include "inferno/model/loss_augmented/edge_hamming_augmented_model.hxx"

namespace inferno{
namespace learning{
namespace loss_functions{


 


    template<class MODEL>
    class EdgeHamming{
    public:

        typedef MODEL Model;
        typedef typename Model:: template VariableMap<DiscreteLabel> ConfMap;
        typedef typename Model:: template FactorMap<double> FactorWeightMap; 

        typedef models::EdgeHammingLossAugmentedModel<Model>  LossAugmentedModel;
        
        BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<FactorWeightMap>));


        EdgeHamming()
        : edgeLossWeightMap_(){

        }    


        EdgeHamming(const Model & model,
                    const FactorWeightMap & varSizeMap,
                    const double rescale,
                    const double underseg,
                    const double overseg,
                    const bool useIgnoreLabel=false,
                    const DiscreteLabel ignoreLabel=DiscreteLabel())
        :   edgeLossWeightMap_(model),
            rescale_(rescale),
            underseg_(underseg),
            overseg_(overseg),
            useIgnoreLabel_(useIgnoreLabel),
            ignoreLabel_(ignoreLabel){
            std::copy(varSizeMap.begin(), varSizeMap.end(),edgeLossWeightMap_.begin());
        }

        void assign(const Model & model,
                    const FactorWeightMap & varSizeMap,
                    const double rescale,
                    const double underseg,
                    const double overseg,
                    const bool useIgnoreLabel=false,
                    const DiscreteLabel ignoreLabel=DiscreteLabel())
        {
            edgeLossWeightMap_.assign(model);
            std::copy(varSizeMap.begin(), varSizeMap.end(),edgeLossWeightMap_.begin());
            rescale_ = rescale;
            underseg_ = underseg;
            overseg_ = overseg;
            useIgnoreLabel_ = useIgnoreLabel;
            ignoreLabel_ = ignoreLabel;
        }

        FactorWeightMap & factorWeightMap(){
            return edgeLossWeightMap_;
        }
        bool & useIgnoreLabel(){
            return useIgnoreLabel_;
        }
        DiscreteLabel & ingoreLabel(){
            return ignoreLabel_;
        }
        const FactorWeightMap & factorWeightMap()const{
            return edgeLossWeightMap_;
        }
        const bool & useIgnoreLabel()const{
            return useIgnoreLabel_;
        }
        const DiscreteLabel & ingoreLabel()const{
            return ignoreLabel_;
        }


        template<class CONF_GT, class CONF>
        LossType eval(const MODEL & model, CONF_GT & confGt, CONF & conf)const{
            LossType totalLoss = 0;
            for(const auto fac : model.factorDescriptors()){

                const auto factor = model.factor(fac);
                const auto weight = edgeLossWeightMap_[fac];
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
        void makeLossAugmentedModel(
            Model & model,
            const ConfMap & gt,
            LossAugmentedModel & lossAugmentedModel
        )const{
            lossAugmentedModel.assign(model, edgeLossWeightMap_, 
                                      gt,rescale_,underseg_,
                                      overseg_,
                                      useIgnoreLabel_, 
                                      ignoreLabel_);
        }

    private:
        FactorWeightMap edgeLossWeightMap_;
        double rescale_;
        double underseg_;
        double overseg_;
        bool useIgnoreLabel_;
        DiscreteLabel ignoreLabel_;

    };



   


} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LOSS_FUNCTIONS_EDGE_HAMMING_HXX*/
