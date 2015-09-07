#ifndef INFERNO_PYTHON_LEARNING_DATASET_EXPORT_DEFAULT_DATASET_HXX
#define INFERNO_PYTHON_LEARNING_DATASET_EXPORT_DEFAULT_DATASET_HXX

// boost
#include <boost/python/return_value_policy.hpp>
// inferno 
#include "inferno/learning/dataset/default_dataset.hxx"


namespace inferno{
namespace learning{
namespace dataset{


    namespace bp = boost::python;


    template<class LOSS_FUNCTION_BASE>
    struct ExportDefaultDataset{

        typedef LOSS_FUNCTION_BASE LossFunctionBase;
        typedef typename LossFunctionBase::Model Model;
        typedef typename LossFunctionBase::ConfMap ConfMap;
        typedef DefaultDataset<LossFunctionBase> Dataset;

        static void exportDefaultDataset(
            const std::string & datasetName 
        ){
            // the class
            bp::class_<Dataset,boost::noncopyable>(datasetName.c_str(),bp::no_init)
            ;

            // the factory
            bp::def("defaultDataset",
                &vectorDatasetFactory<LossFunction>,
                (
                    bp::arg("models"),
                    bp::arg("lossFunctions"),
                    bp::arg("groundTruth"),
                    bp::arg("weightConstraints"),
                    bp::arg("regularizer") = Regularizer()
                ),
                RetValPol< 
                    CustWardPost<0,1,
                        CustWardPost<0,2,
                            CustWardPost<0,3,
                                CustWardPost<0,4,
                                    NewObj
                                >
                            >
                        >
                    > 
                >()
            );
        }

        Dataset * factor(
            std::vector<Model>  & models,
            const std::vector<LossFunctionBase * > lossFunctionPtrs,
            const std::vector<ConfMap> & gts,
            const WeightConstraints & weightConstraints,
            const Regularizer & regularizer   
        ){
            return new Dataset(models, lossFunctionPtrs, gts, 
                               weightConstraints, regularizer);
        }
    };





} // end namespace inferno::learning::dataset
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_PYTHON_LEARNING_DATASET_EXPORT_DEFAULT_DATASET_HXX */
