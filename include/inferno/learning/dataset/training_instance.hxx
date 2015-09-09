#ifndef INFERNO_LEARNING_LEARNERS_TRAINING_INSTANCE_HXX
#define INFERNO_LEARNING_LEARNERS_TRAINING_INSTANCE_HXX


namespace inferno{
namespace learning{
namespace dataset{


    template<class LOSS_FUNCTION>
    class TrainingInstance{
    public:
        typedef LOSS_FUNCTION LossFunction;
        typedef typename LossFunction::Model Model;
        typedef typename Model:: template VariableMap<DiscreteLabel> GroundTruth;
        TrainingInstance(){
        }
        TrainingInstance(Model * m, LossFunction * l, const GroundTruth * g)
        :   model_(m),
            lossFunction_(l),
            gt_(g){

        }

        Model * model() {
            return model_;
        }

        LossFunction * lossFunction() {
            return lossFunction_;
        }

        const GroundTruth * groundTruth() {
            return gt_;
        }

    private:
        Model * model_;
        LossFunction * lossFunction_;
        const GroundTruth * gt_;
    };
    
} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LEARNERS_TRAINING_INSTANCE_HXX*/
