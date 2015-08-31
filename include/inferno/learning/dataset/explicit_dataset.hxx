#ifndef INFERNO_LEARNING_LEARNERS_EXPLICIT_DATASET_HXX
#define INFERNO_LEARNING_LEARNERS_EXPLICIT_DATASET_HXX

// std
#include <vector>

// boost
#include <boost/concept_check.hpp>
#include <boost/iterator/counting_iterator.hpp>

// inferno
#include "inferno/inferno.hxx"
#include "inferno/utilities/parallel/pool.hxx"
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
        TrainingInstance(Model & m, LossFunction & l, const GroundTruth & g)
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

        const GroundTruth & groundTruth() {
            return *gt_;
        }

    private:
        Model * model_;
        LossFunction * lossFunction_;
        const GroundTruth * gt_;
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

        LossType averageLoss(InferenceFactoryBase * inferenceFactory, size_t numThreads=0){
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

            /*
            std::vector<LossType> mLoss(dataset().size());
            boost::counting_iterator<size_t> begin(0);
            boost::counting_iterator<size_t> end(dataset().size());
            auto getLoss = [&,this] (size_t id, size_t i){
                dataset().unlock(i);

                auto & model =  dataset().model(i);
                auto & lossFunction =  dataset().lossFunction(i);
                const auto & groundTruth =  dataset().groundTruth(i);

                auto inf = inferenceFactory->create(model);
                inf->infer();

                C argMinConf(model);
                inf->conf(argMinConf);

                mLoss[i] = lossFunction.eval(model, groundTruth, argMinConf);

                dataset().lock(i);
            };
            //std::cout<<" nThreads "<<numThreads<<"\n";
            utilities::parallel_foreach( numThreads == 0 ? std::thread::hardware_concurrency() : numThreads,
                                         dataset().size(),begin,end,getLoss);
            
            return std::accumulate(mLoss.begin(), mLoss.end(),0.0);
            */
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
    class VectorDataset : 
        public DatasetBase< VectorDataset<LOSS_FUNCTION>,LOSS_FUNCTION> 
    {
    public:
        typedef LOSS_FUNCTION LossFunction;
        typedef typename LossFunction::Model Model;
        typedef typename Model:: template VariableMap<DiscreteLabel> GroundTruth;

        //static_assert(boost::DefaultConstructible<LOSS_FUNCTION>::value,"fofo");

        BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<Model>));
        BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<LossFunction>));
        BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<GroundTruth>));



        VectorDataset(
            std::vector<Model>         & models,
            std::vector<LossFunction>  & lossFunctions,
            const std::vector<GroundTruth>   & gts
        )
        :   models_(models),
            lossFunctions_(lossFunctions),
            gts_(gts)
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
        const GroundTruth & groundTruth(size_t i){
            return gts_[i];
        }

        std::vector<Model>         & models_;
        std::vector<LossFunction>  & lossFunctions_;
        const std::vector<GroundTruth>   & gts_;
    };


} // end namespace inferno::learning::learners
} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_LEARNERS_EXPLICIT_DATASET_HXX*/
