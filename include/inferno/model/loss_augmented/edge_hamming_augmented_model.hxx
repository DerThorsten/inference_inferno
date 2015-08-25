#ifndef INFERNO_MODEL_LOSS_AUGMENTED_EDGE_HAMMING_LOSS_AUGMENTED_MODEL
#define INFERNO_MODEL_LOSS_AUGMENTED_EDGE_HAMMING_LOSS_AUGMENTED_MODEL




// inferno
#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/value_tables/discrete_unary_value_table_base.hxx"
#include "inferno/model/discrete_factor_base.hxx"
#include "inferno/model/discrete_unary_base.hxx"
#include "inferno/model/structure_view_model_base.hxx"
#include "inferno/model/maps/model_maps.hxx"
//#include "inferno/learning/loss_functions/edge_hamming.hxx" 


namespace inferno{
namespace learning{
namespace loss_functions{

    template<class MODEL>
    class EdgeHamming;
}
}
}

namespace inferno{
namespace models{


    template<class LOSSLESS_MODEL>
    class EdgeHammingLossAugmentedModel :
    public StructureViewModelBase<
        EdgeHammingLossAugmentedModel<LOSSLESS_MODEL>,
        LOSSLESS_MODEL
    >
    {

        typedef EdgeHammingLossAugmentedModel<LOSSLESS_MODEL> Self;
        typedef DeadCodeUnary<Self> UnaryImpl;
        //typedef 
    public:
        typedef LOSSLESS_MODEL LosslessModel;
        typedef typename LosslessModel:: template VariableMap<DiscreteLabel> GtMap;
        typedef learning::loss_functions::EdgeHamming<LosslessModel> LossFunction;
        typedef typename LossFunction::FactorWeightMap LossFactorWeightMap;

        typedef UnaryImpl UnaryProxy;






        EdgeHammingLossAugmentedModel(
            LosslessModel & losslessModel,
            const GtMap & gt,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel
        )
        :
            losslessModel_(&losslessModel),
            gt_(gt),
            betas_(losslessModel_,0),
            useIgnoreLabel_(useIgnoreLabel),
            ignoreLabel_(ignoreLabel)
        {
            // CHECk THAT MODEL IS POTTS!!! (AND STAYS POTTS)
            const auto mxArity = losslessModel_->maxArity();
            INFERNO_CHECK_OP(mxArity,==,2,
                "Only second order multicut models are allowed");

            this->makeBetas();
        }

        void makeBetas()const{

            const auto & m = *losslessModel_;
            const auto & gt = gt_;

            for(auto fac : m.factorDescriptor()){   
                const auto factor = m[fac];
                ValueType beta;
                if(factor->isPotts(beta)){

                    const auto u = factor->variable(0);
                    const auto v = factor->variable(1);
                    const auto lu = gt[u];
                    const auto lv = gt[v];

                    if(useIgnoreLabel_ && (u==ignoreLabel_ || v==ignoreLabel_)){
                        betas_[fac] = beta;
                    }
                    else{
                        const auto lossWeight = edgeLossWeightMap_[fac];
                        if(lu == lv){
                            // nodes SHOULD be merged
                            // CONF   BETA    LOSS
                            // (a,a) = 0   -    0
                            // (a,b) = beta -  lossWeight
                            betas_[fac] = beta - lossWeight;
                        }
                        else{ //=> (lu != lv)
                            // nodes SHOULD NOT(!) be merged
                            // CONF   BETA    LOSS
                            // (a,a) = 0   -   lossWeight
                            // (a,b) = beta -  0
                            //
                            // => add 'lossWeight' to both confs
                            // 
                            // (a,a) = 0   -   0
                            // (a,b) = beta +  lossWeight
                            betas_[fac] = beta + lossWeight;
                        }
                    }
                }
                else{
                    throw RuntimeError("Only potts factors are supported");
                }
            }
        }

        LosslessModel & baseModel() {
            return *losslessModel_;
        }
        const LosslessModel & baseModel()const{
            return *losslessModel_;
        }
        const LosslessModel & constBaseModel()const{
            return *losslessModel_;
        }
    private:

        mutable typename LosslessModel:: template FactorMap<ValueType> betas_;
        const LossFactorWeightMap * edgeLossWeightMap_;

        LosslessModel * losslessModel_;
        const GtMap   * gt_;
        bool useIgnoreLabel_;
        DiscreteLabel ignoreLabel_;
    };
} // end namespace model
} // end namespace inferno

#endif /* INFERNO_MODEL_LOSS_AUGMENTED_EDGE_HAMMING_LOSS_AUGMENTED_MODEL */
