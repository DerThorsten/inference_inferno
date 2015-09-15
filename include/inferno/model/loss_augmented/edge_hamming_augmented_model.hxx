#ifndef INFERNO_MODEL_LOSS_AUGMENTED_EDGE_HAMMING_LOSS_AUGMENTED_MODEL
#define INFERNO_MODEL_LOSS_AUGMENTED_EDGE_HAMMING_LOSS_AUGMENTED_MODEL




// inferno
#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/value_tables/potts_value_table_base.hxx"
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
}  // end namespace inferno::learning::loss_functions
}  // end namespace inferno::learning


namespace value_tables{

    template<class MODEL>
    class  EdgeHammingLossAugmentedModelPottsValueTable : public PottsValueTableBase
    {
    private:
        typedef typename MODEL::BaseModel BaseModel;
        typedef typename BaseModel::FactorDescriptor BaseModelFactorDescriptor;
        typedef typename BaseModel::VariableDescriptor BaseModelVariableDescriptor;
        typedef typename BaseModel::FactorProxy  BaseModelFactorProxy;
    
        using PottsValueTableBase::eval;
    public:


        EdgeHammingLossAugmentedModelPottsValueTable(
            const ValueType val,                                        
            const BaseModel & baseModel,
            const BaseModelFactorDescriptor fac
        )
        :   PottsValueTableBase(),
            beta_(val),
            baseFactor_(baseModel.factor(fac)){

        }

        virtual ~EdgeHammingLossAugmentedModelPottsValueTable(){
        }
        virtual ValueType beta() const override {
            return beta_;
        }
        virtual DiscreteLabel shape(const uint32_t d)const override{
            return baseFactor_->shape(d);
        }

        BaseModelVariableDescriptor variable(const uint32_t d)const{
            return baseFactor_->variable(d);
        }
    private:
        ValueType beta_;
        BaseModelFactorProxy baseFactor_;
    };

}  // end namespace inferno::value_tables






namespace models{


    template<class MODEL>
    class EdgeHammingLossAugmentedModelFactor : 
    public  DiscreteFactorBase<
        EdgeHammingLossAugmentedModelFactor<MODEL>, 
        MODEL
    > 
    {
    private:
        typedef DiscreteFactorBase<
            EdgeHammingLossAugmentedModelFactor<MODEL>, 
            MODEL
        > Base;
        //using Base::eval;

        typedef MODEL Model;
        typedef typename MODEL::BaseModel BaseModel;
        typedef typename BaseModel::FactorDescriptor BaseModelFactorDescriptor;
        typedef typename BaseModel::VariableDescriptor BaseModelVariableDescriptor;
        typedef typename BaseModel::FactorProxy  BaseModelFactorProxy;
        typedef value_tables::EdgeHammingLossAugmentedModelPottsValueTable<Model> Vt;

    public:
        EdgeHammingLossAugmentedModelFactor(
            const ValueType val,                                        
            const BaseModel & baseModel,
            const BaseModelFactorDescriptor fac
        )
        :   vt_(val, baseModel, fac){
        }

        const value_tables::DiscreteValueTableBase * valueTable()const{
            return &vt_;
        }   
        DiscreteLabel shape(const size_t d)const{
            return vt_.shape(d);
        }

        BaseModelVariableDescriptor variable(const size_t d)const{
            return vt_.variable(d);
        }

    private:
        Vt vt_;

    };



    template<class LOSSLESS_MODEL>
    class EdgeHammingLossAugmentedModel :
    public StructureViewModelBase<
        EdgeHammingLossAugmentedModel<LOSSLESS_MODEL>,
        LOSSLESS_MODEL
    >
    {

        typedef EdgeHammingLossAugmentedModel<LOSSLESS_MODEL> Self;
        typedef DeadCodeUnary<Self> UnaryImpl;
        typedef EdgeHammingLossAugmentedModelFactor<Self> FactorImpl;

        //typedef 
    public:
        typedef LOSSLESS_MODEL LosslessModel;
        typedef typename LosslessModel:: template VariableMap<DiscreteLabel> GtMap;
        //typedef learning::loss_functions::EdgeHamming<LosslessModel> LossFunction;
        typedef typename LosslessModel:: template FactorMap<double> LossFactorWeightMap;


        typedef EdgeHammingLossAugmentedModelFactor<Self> FactorType;
        typedef DeadCodeUnary<Self>  UnaryType;

        typedef UnaryImpl UnaryProxy;
        typedef FactorImpl FactorProxy;
        typedef typename LOSSLESS_MODEL::ConstraintProxy  ConstraintProxy;


        EdgeHammingLossAugmentedModel(
        )
        :
            betas_(),
            edgeLossWeightMap_(nullptr),
            losslessModel_(nullptr),
            gt_(nullptr),
            useIgnoreLabel_(false),
            ignoreLabel_(-1)
        {

        }

        EdgeHammingLossAugmentedModel(
            const LosslessModel & losslessModel,
            const LossFactorWeightMap & edgeLossWeightMap,
            const GtMap & gt,
            const double rescale,
            const double underseg,
            const double overseg,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel
        )
        :
            betas_(losslessModel_,0),
            edgeLossWeightMap_(&edgeLossWeightMap),
            losslessModel_(&losslessModel),
            gt_(gt),
            rescale_(rescale),
            underseg_(underseg),
            overseg_(overseg),
            useIgnoreLabel_(useIgnoreLabel),
            ignoreLabel_(ignoreLabel)
        {
            // CHECk THAT MODEL IS POTTS!!! (AND STAYS POTTS)
            const auto mxArity = losslessModel_->maxArity();
            INFERNO_CHECK_OP(mxArity,==,2,
                "Only second order multicut models are allowed");

            this->makeBetas();
        }

        void assign(
            const LosslessModel & losslessModel,
            const LossFactorWeightMap & edgeLossWeightMap,
            const GtMap & gt,
            const double rescale,
            const double underseg,
            const double overseg,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel
        ){
            losslessModel_ = &losslessModel;
            edgeLossWeightMap_ = &edgeLossWeightMap;
            gt_ = &gt;
            rescale_ = rescale;
            underseg_ = underseg;
            overseg_ = overseg;
            useIgnoreLabel_ = useIgnoreLabel;
            ignoreLabel_ = ignoreLabel;
            betas_.assign(*losslessModel_);
            this->makeBetas(); 
        }

        FactorProxy factor(const typename Self::FactorDescriptor fac)const{
            return FactorImpl(betas_[fac],this->baseModel(), fac);
        }

        UnaryProxy unary(const typename Self::UnaryDescriptor unary)const{
            return UnaryImpl();
        }

        ConstraintProxy constraint(const typename Self::ConstraintDescriptor c)const{
            return losslessModel_->constraint(c);
        }

        LosslessModel & baseModel() {
            return *losslessModel_;
        }
        const LosslessModel & baseModel()const{
            return *losslessModel_;
        }
    private:
        void makeBetas()const{


            const auto & m =  *losslessModel_;
            const auto & gt = *gt_;

            for(auto fac : m.factorDescriptors()){  
                const auto factor = m.factor(fac);
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
                        const auto lossWeight = (*edgeLossWeightMap_)[fac]*rescale_;

                        if(lu == lv){
                            // nodes SHOULD be merged
                            // CONF   BETA    LOSS
                            // (a,a) = 0   -    0
                            // (a,b) = beta -  lossWeight*overseg
                            betas_[fac] = beta - lossWeight*overseg_;
                        }
                        else{ //=> (lu != lv)
                            // nodes SHOULD NOT(!) be merged
                            // CONF   BETA    LOSS
                            // (a,a) = 0   -   lossWeight*underseg
                            // (a,b) = beta -  0
                            //
                            // => add 'lossWeight' to both confs
                            // 
                            // (a,a) = 0   -   0
                            // (a,b) = beta +  lossWeight*underseg
                            betas_[fac] = beta + lossWeight*underseg_;
                        }
                    }
                }
                else{
                    throw RuntimeError("Only potts factors are supported");
                }
            }
        }
        mutable typename LosslessModel:: template FactorMap<ValueType> betas_;
        const LossFactorWeightMap * edgeLossWeightMap_;

        const LosslessModel * losslessModel_;
        const GtMap   * gt_;
        double rescale_;
        double underseg_;
        double overseg_;
        bool useIgnoreLabel_;
        DiscreteLabel ignoreLabel_;
    };
} // end namespace model
} // end namespace inferno

#endif /* INFERNO_MODEL_LOSS_AUGMENTED_EDGE_HAMMING_LOSS_AUGMENTED_MODEL */
