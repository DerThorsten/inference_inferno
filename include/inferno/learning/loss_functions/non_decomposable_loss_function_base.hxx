#ifndef INFERNO_LEARNING_LOSS_FUNCTIONS_F_SCORE
#define INFERNO_LEARNING_LOSS_FUNCTIONS_F_SCORE

// boost
#include <boost/concept_check.hpp>

// inferno
#include "inferno/inferno.hxx"
#include "inferno/learning/loss_functions/non_decomposable_loss_function_base.hxx"

namespace inferno{
namespace learning{
namespace loss_functions{


    /// \brief  bass loss function class
    template<class MODEL>
    class LossFunctionBase{

    public:
        typedef MODEL Model;
        typedef typename Model:: template VariableMap<DiscreteLabel> ConfMap;

        virtual ~LossFunctionBase(){
        }

        // pure virtual interface
        virtual std::string name() const = 0;
        virtual LossType eval(
            const Model & model, 
            const ConfMap & confGt, 
            const ConfMap & conf
        ) const = 0 ;

        // with default implementation
        virtual LossType maximumLoss()const {
            return infVal();
        }

    private:
        
    };




    /// \base class for non decomposable loss functions
    template<class MODEL>
    class NonDecomposableLossFunctionBase : 
        public LossFunctionBase<MODEL>
    {

    public:
        typedef MODEL Model;
        typedef typename Model:: template VariableMap<DiscreteLabel> ConfMap;
        typedef NoLossAugmentedModel LossAugmentedModel;

        virtual ~NonDecomposableLossFunctionImplBase(){
        }


    private:
        
    };

    
    /// \base class for decomposable loss functions
    template<class MODEL, class LOSS_AUGMENTED_MODEL>
    class DecomposableLossFunctionBase : 
        public LossFunctionBase<MODEL>
    {

    public:
        typedef MODEL Model;
        typedef typename Model:: template VariableMap<DiscreteLabel> ConfMap;
        typedef LOSS_AUGMENTED_MODEL LossAugmentedModel;

        virtual ~DecomposableLossFunctionBase(){
        }

        virtual void makeLossAugmentedModel(
            const Model & model, 
            const ConfMap & gt, 
            LossAugmentedModel & lossAugmentedModel
        ) const = 0;


    private:
        
    };







} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LOSS_FUNCTIONS_NON_DECOMPOSABLE_LOSS_FUNCTION_BASE*/
