#ifndef INFERNO_LEARNING_LEARNERS_EXPLICIT_DATASET_HXX
#define INFERNO_LEARNING_LEARNERS_EXPLICIT_DATASET_HXX

// std
#include <vector>

// boost
#include <boost/concept_check.hpp>

// inferno
#include "inferno/inferno.hxx"
#include "inferno/learning/weights.hxx"
#include "inferno/inference/base_discrete_inference_factory.hxx"

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
        TrainingInstance(Model & m, LossFunction & l, GroundTruth & g)
        :   model_(&m),
            lossFunction_(&l),
            gt_(&g){

        }

        Model & model() {
            return *model_;
        }

        LossFunction & lossFunction() {
            return *lossFunction_;
        }

        GroundTruth & groundTruth() {
            return *gt_;
        }

    private:
        Model * model_;
        LossFunction * lossFunction_;
        GroundTruth * gt_;
    };
    




    template<class DATASET, class LOSS_FUNCTION>
    class DatasetBase{

    public:
        //typedef typename LOSS_FUNCTION::Model MODEL;


        typedef TrainingInstance<LOSS_FUNCTION> value_type;

        typedef inference::BaseDiscreteInferenceFactory<typename LOSS_FUNCTION::Model> 
            InferenceFactoryBase;

        void sanityCheck()const{

        }
        void unlock(size_t i){

        }
        void lock(size_t i){

        }

        value_type operator[](const size_t i){
            auto & ds = dataset();
            auto & model = ds.model(i);
            auto & lossFunction = ds.lossFunction(i);
            auto & groundTruth = ds.groundTruth(i);
            return value_type(model, lossFunction, groundTruth);
        }

        void updateWeights(const WeightVector & weights){
            for(size_t i=0; i<dataset().size(); ++i){
                dataset().unlock(i);
                dataset().model(i).updateWeights(weights);
                dataset().lock(i);
            }
        }

        template<class CONF>
        LossType evalLoss(size_t i, CONF & conf)const{

        }

        LossType avergeLoss(InferenceFactoryBase * inferenceFactory){
            LossType lossSum = 0;
            typedef typename LOSS_FUNCTION::Model M;
            typedef typename M:: template VariableMap<DiscreteLabel> C;

            for(size_t i=0; i<dataset().size(); ++i){

                dataset().unlock(i);

                auto & model =  dataset().model(i);
                auto & lossFunction =  dataset().lossFunction(i);
                const auto & groundTruth =  dataset().groundTruth(i);

                auto inf = inferenceFactory->create(model);
                inf->infer();

                C argMinConf(model);
                inf->conf(argMinConf);

                lossSum += lossFunction.eval(model, groundTruth, argMinConf);

                dataset().lock(i);
            }
            return lossSum/dataset().size();
        }


        size_t size()const{
            return dataset().nModels();
        }


    private:
        const DATASET & dataset()const{
            return * static_cast<const DATASET *>(this);
        }
        DATASET & dataset(){
            return * static_cast<DATASET *>(this);
        }
    };




    template<class LOSS_FUNCTION>
    class VectorDataset : public DatasetBase< VectorDataset<LOSS_FUNCTION>,
                                              LOSS_FUNCTION> 
    {
    public:
        typedef LOSS_FUNCTION LossFunction;
        typedef typename LossFunction::Model Model;
        typedef typename Model:: template VariableMap<DiscreteLabel> GroundTruth;

        //static_assert(boost::DefaultConstructible<LOSS_FUNCTION>::value,"fofo");

        BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<Model>));
        BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<LossFunction>));
        BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<GroundTruth>));



        VectorDataset(const size_t nModels = 0)
        :   models_(nModels),
            lossFunctions_(nModels),
            gts_(nModels)
        {

        }

        void resize(const size_t i){
            models_.resize(i);
            lossFunctions_.resize(i);
            gts_.resize(i);
        }

        size_t nModels()const{
            return models_.size();
        }
        Model & model(size_t i){
            return models_[i];
        }
        LossFunction & lossFunction(size_t i){
            return lossFunctions_[i];
        }
        GroundTruth & groundTruth(size_t i){
            return gts_[i];
        }

        std::vector<Model>         models_;
        std::vector<LossFunction>  lossFunctions_;
        std::vector<GroundTruth>   gts_;
    };


} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LEARNERS_EXPLICIT_DATASET_HXX*/
