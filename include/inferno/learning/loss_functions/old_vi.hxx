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


namespace loss_functions{


    template<class MODEL>
    class VariationOfInformation{
    public:
        typedef MODEL Model;
        typedef typename Model:: template VariableMap<double> VariableSizeMap; 
        typedef typename Model:: template FactorMap<double> FactorWeightMap;
        typedef NoLossAugmentedModel LossAugmentedModel;
        typedef typename Model:: template VariableMap<DiscreteLabel> ConfMap;

        BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<VariableSizeMap>));


        VariationOfInformation()
        :   variableSizeMap_(),
            factorWeightMap_(),
            beta_(0.5),
            useIgnoreLabel_(false),
            ignoreLabel_(-1)
        {

        }    


        VariationOfInformation(const Model & model,
                               const VariableSizeMap & varSizeMap,
                               const bool useIgnoreLabel=false,
                               const DiscreteLabel ignoreLabel=DiscreteLabel())
        :   variableSizeMap_(model),
            factorWeightMap_(model, 1.0),
            beta_(0.5),
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
            factorWeightMap_.assign(model, 1.0);
            std::copy(varSizeMap.begin(), varSizeMap.end(),variableSizeMap_.begin());
            //variableSizeMap_ = &variableSizeMap;
            beta_ = 0.5;
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



        template<class CONF_GT, class CONF>
        LossType eval(const MODEL & model, CONF_GT & confGt, CONF & conf)const{

            return fScore(model, confGt, conf);
            //return voiOrRi(model, confGt, conf);
        }



        void makeLossAugmentedModel(LossAugmentedModel & lossAugmentedModel)const;

    private:


        LossType voiOrRi(
            const Model & model, 
            const ConfMap & confGt, 
            const ConfMap & conf
        )const{

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
        
            if(false){
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
            else{
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
        }

        LossType fScore(
            const Model & model, 
            const ConfMap & confGt, 
            const ConfMap & conf
        )const{

            const auto sB = beta_*beta_;
            double truePositive = 0;
            double trueNegative = 0;
            double falsePositive = 0;
            double falseNegative = 0;
            for(const auto fac : model.factorDescriptors()){
                const auto factor = model.factor(fac);



                if(factor->arity() == 2){
                    const auto u = factor->variable(0);
                    const auto v = factor->variable(1);
                    const auto fWeight = factorWeightMap_[fac];
                    const auto gtU = confGt[u];
                    const auto gtV = confGt[v];
                    const auto cU = conf[u];
                    const auto cV = conf[v];
                    //std::cout<<"f weight "<<fWeight<<"\n";
                    if(!useIgnoreLabel_ || (gtU != ignoreLabel_ && gtV != ignoreLabel_)){
                        if(gtU == gtV){
                            if(cU == cV)
                                trueNegative += fWeight;
                            else
                                falsePositive += fWeight;
                        }
                        else{
                            if(cU == cV)
                                falseNegative += fWeight;
                            else
                                truePositive += fWeight;
                        }
                    }
                }
            }
            //std::cout<<"tp "<<truePositive<<"\n";
            //std::cout<<"tn "<<trueNegative<<"\n";
            //std::cout<<"fp "<<falsePositive<<"\n";
            //std::cout<<"fn "<<falseNegative<<"\n";

            auto score =  (1.0 + sB) * truePositive  /
            ( (1.0+sB)*truePositive + sB*falseNegative + falsePositive);
            return 1.0 - score;
        }


        VariableSizeMap variableSizeMap_;  // for fi and ri
        FactorWeightMap factorWeightMap_;
        ValueType       beta_;
        bool useIgnoreLabel_;
        DiscreteLabel ignoreLabel_;

    };



    


} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LOSS_FUNCTIONS_LOSS_FUNCTIONS_HXX*/
