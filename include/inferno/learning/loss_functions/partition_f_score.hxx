#ifndef INFERNO_LEARNING_LOSS_FUNCTIONS_F_SCORE
#define INFERNO_LEARNING_LOSS_FUNCTIONS_F_SCORE


// boost
#include <boost/concept_check.hpp>


// vigra
#include <vigra/multi_array.hxx>

// inferno
#include "inferno/inferno.hxx"
#include "inferno/learning/loss_functions/loss_functions.hxx"


namespace inferno{
namespace learning{
namespace loss_functions{

    template<class MODEL>
    class PartitionFScore : public NonDecomposableLossFunctionImplBase<MODEL>{

    public:
        typedef NonDecomposableLossFunctionImplBase<MODEL> Base;
        typedef typedef typename Base::Model Model;
        typedef typedef typename Base::ConfMap ConfMap;
        typedef typedef typename Base::LossAugmentedModel LossAugmentedModel;

        // specific typedef for fscore
        typedef typename Model:: template FactorMap<double> FactorWeightMap;

        PartitionFScore()
        :   factorWeightMap_(),
            beta_(0.5),
            useIgnoreLabel_(false),
            ignoreLabel_(-1){
        }
        PartitionFScore(
            const Model & model,
            const double beta,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel
        )
        :   PartitionFScore(model, beta, useIgnoreLabel, 
                            ignoreLabel,FactorWeightMap(model,1)){
        }

        PartitionFScore(
            const Model & model,
            const double beta,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel,
            const FactorWeightMap & factorWeightMap
        )
        :   factorWeightMap_(model),
            beta_(beta){
            for(const auto fac : model.factorDescriptors())
                factorWeightMap_[fac] = factorWeightMap[fac];
        }

        virtual std::string name()const override{
            return std::move(std::string("PartitionFScore"));
        }
        
        virtual LossType eval(
            const Model & model, 
            const ConfMap & confGt, 
            const ConfMap & conf
        ) const override {
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

        virtual LossType maximumLoss()const override{
            return 1.0;
        }
    private:
        FactorWeightMap factorWeightMap_;
        double beta_;
        bool useIgnoreLabel_;
        DiscreteLabel ignoreLabel_;
    };



    


} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LOSS_FUNCTIONS_F_SCORE*/
