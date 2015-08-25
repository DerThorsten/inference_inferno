#ifndef INFERNO_LEARNING_LOSS_FUNCTIONS_LOSS_FUNCTIONS_HXX
#define INFERNO_LEARNING_LOSS_FUNCTIONS_LOSS_FUNCTIONS_HXX


// boost
#include <boost/concept_check.hpp>


// vigra
#include <vigra/multi_array.hxx>

// inferno
#include "inferno/inferno.hxx"
#include "inferno/utilities/arithmetic_map.hxx"


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


    struct NoLossAugmentedModel{

    };


    template<class MODEL, class VARIABLE_MULTIPLIER, class LABEL_CONFUSION_COST>
    class UnstructuredLoss{
    public:
        typedef MODEL Model;
        typedef typename Model:: template NodeMap<DiscreteLabel> ConfMap;
        typedef VARIABLE_MULTIPLIER VariableMultiplier; 
        typedef LABEL_CONFUSION_COST LabelConfusionCost;

        typedef MODEL LossAugmentedModel;
        template<class CONF_GT, class CONF>
        LossType eval(const MODEL & model, CONF_GT & confGt, CONF & conf)const{
            LossType totalLoss = 0.0;
            for(const auto var : model.variables()){
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



    template<class MODEL>
    class VariationOfInformation{
    public:
        typedef MODEL Model;
        typedef typename Model:: template VariableMap<double> VariableSizeMap; 
        
        BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<VariableSizeMap>));


        VariationOfInformation()
        : variableSizeMap_(){

        }    


        VariationOfInformation(const Model & model,
                               const VariableSizeMap & varSizeMap,
                               const bool useIgnoreLabel=false,
                               const DiscreteLabel ignoreLabel=DiscreteLabel())
        :   variableSizeMap_(model),
            useIgnoreLabel_(useIgnoreLabel),
            ignoreLabel_(ignoreLabel){
            std::copy(varSizeMap.begin(), varSizeMap.end(),variableSizeMap_.begin());
        }

        void assign(const Model & model,
                    const VariableSizeMap & varSizeMap,
                    const bool useIgnoreLabel=false,
                    const DiscreteLabel ignoreLabel=DiscreteLabel())
        {
            variableSizeMap_.assign(model);
            std::copy(varSizeMap.begin(), varSizeMap.end(),variableSizeMap_.begin());
            //variableSizeMap_ = &variableSizeMap;
            useIgnoreLabel_ = useIgnoreLabel;
            ignoreLabel_ = ignoreLabel;
        }

        VariableSizeMap & variableSizeMap(){
            return variableSizeMap_;
        }
        bool & useIgnoreLabel(){
            return useIgnoreLabel_;
        }
        DiscreteLabel & ingoreLabel(){
            return ignoreLabel_;
        }
        const VariableSizeMap & variableSizeMap()const{
            return variableSizeMap_;
        }
        const bool & useIgnoreLabel()const{
            return useIgnoreLabel_;
        }
        const DiscreteLabel & ingoreLabel()const{
            return ignoreLabel_;
        }


        typedef MODEL LossAugmentedModel;
        template<class CONF_GT, class CONF>
        LossType eval(const MODEL & model, CONF_GT & confGt, CONF & conf)const{

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
                    const auto  size =1.0;// variableSizeMap_[var];
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
        void makeLossAugmentedModel(LossAugmentedModel & lossAugmentedModel)const;

    private:
        VariableSizeMap variableSizeMap_;
        bool useIgnoreLabel_;
        DiscreteLabel ignoreLabel_;

    };



    /*

    template<
        class LABEL_ITER_A, 
        class LABEL_ITER_B,
        class SIZE_ITER,
        class IGNORE_LABEL
    >
    double
    variationOfInformation
    (
        LABEL_ITER_A beginLabelsA,
        LABEL_ITER_A endLabelsA,
        LABEL_ITER_B beginLabelsB,
        SIZE_ITER_A beginSizes,
        const bool useIgnoreLabel = false
        const IGNORE_LABEL ignoreLabel = IGNORE_LABEL()

    )
    {
        typedef typename std::iterator_traits<LABEL_ITER_A>::value_type LabelA;
        typedef typename std::iterator_traits<LABEL_ITER_B>::value_type LabelB;
        typedef std::LabelPair<LabelA, LabelB> LabelPair;
        typedef std::unordered_map<LabelPair, double> PMatrix;
        typedef std::unordered_map<LabelA, double> PVectorA;
        typedef std::unordered_map<LabelB, double> PVectorB;

        // count
        double N = 0.0;
        PMatrix pjk;
        PVectorA pj;
        PVectorB pk;

        for(; beginLabelsA != endLabelsA; ++beginLabelsA, ++beginLabelsB, ++beginSizes) {
            if(!useIgnoreLabel || (*beginLabelsA != ignoreLabel && *beginLabelsB !=ignoreLabel)) {
                const auto size = *beginSizes;
                pj[*beginLabelsA]+=size;
                pk[*beginLabelsB]+=size;
                ++pjk[LabelPair(*beginLabelsA, *beginLabelsB)]+=size;
                N+=size;
            }
        }
        


        // normalize
        for(auto it = pj.begin(); it != pj.end(); ++it) {
            it->second /= N;
        }
        for(auto it = pk.begin(); it != pk.end(); ++it) {
            it->second /= N;
        }
        for(auto it = pjk.begin(); it != pjk.end(); ++it) {
            it->second /= N;
        }

        // compute information
        double H0 = 0.0;
        for(auto it = pj.begin(); it != pj.end(); ++it) {
            H0 -= it->second * std::log(it->second);
        }
        double H1 = 0.0;
        for(auto it = pk.begin(); it != pk.end(); ++it) {
            H1 -= it->second * std::log(it->second);
        }
        double I = 0.0;
        for(auto it = pjk.begin(); it != pjk.end(); ++it) {
            const LabelA j = it->first.first;
            const LabelB k = it->first.second;
            const double pjk_here = it->second;
            const double pj_here = pj[j];
            const double pk_here = pk[k];
            I += pjk_here * std::log( pjk_here / (pj_here * pk_here) );
        }

        return H0 + H1 - 2.0 * I;
    }
    */


} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LOSS_FUNCTIONS_LOSS_FUNCTIONS_HXX*/
