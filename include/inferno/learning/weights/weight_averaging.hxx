#ifndef INFERNO_LEARNING_WEIGHTS_WEIGHT_AVERAGING
#define INFERNO_LEARNING_WEIGHTS_WEIGHT_AVERAGING



namespace inferno{
namespace learning{
namespace weights{


    class WeightAveraging{
    public:
        WeightAveraging(WeightVector & weights, int order=2)
        :   oldWeights_(),
            order_(order),
            iteration_(1)
        {
            if(order!=1 && order!=2 && order != -1){
                throw inferno::RuntimeError("order must be -1,1 or 2");
            }
            if(order_ != -1)
                oldWeights_ = weights;
        }
        WeightAveraging()
        :   oldWeights_(),
            order_(-1),
            iteration_(1)
        {

        }
        void setWeights(WeightVector & weights){
            if(order_ != -1)
                oldWeights_ = weights;
        }


        void operator()(
            const WeightVector & weights,
            WeightVector & averagedWeights
        ){
            const ValueType t = static_cast<ValueType>(iteration_);
            if(order_ == -1){

                // most probably this will be a
                // self assignment
                averagedWeights = weights;
                // old weights do not need to be updated in 
                // case of no averaging
                //oldWeights_ = weights;
            }
            else if(order_==1){
                const ValueType rho = 2.0 / (t + 2.0);
                for(size_t i=0; i<weights.size(); ++i){
                    averagedWeights[i] = oldWeights_[i]*(1.0 - rho) + weights[i]*rho;
                    oldWeights_[i] = weights[i];
                }
            }
            else if(order_ == 2){
                const ValueType rho = 6.0 * (t+1.0) / ( (t+2.0)*(2.0*t + 3.0) );
                for(size_t i=0; i<weights.size(); ++i){
                    averagedWeights[i] = oldWeights_[i]*(1.0 - rho) + weights[i]*rho;
                    oldWeights_[i] = weights[i];
                }
            }
            else{
                throw inferno::RuntimeError("internal error");
            }
            ++iteration_;
        }
    private:
        WeightVector  oldWeights_;
        int order_;
        uint64_t iteration_;
    };


}   // end namespace inferno::learning::weights
}   // end namespace inferno::learning
}   // end namespace inferno


#endif /*INFERNO_LEARNING_WEIGHTS_WEIGHT_AVERAGING*/
