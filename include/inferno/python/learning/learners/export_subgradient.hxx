#ifndef INFERNO_PYTHON_LEARNING_LEARNERS_EXPORT_SUBGRADIENT_HXX
#define INFERNO_PYTHON_LEARNING_LEARNERS_EXPORT_SUBGRADIENT_HXX


// boost python related
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/def_visitor.hpp>


// vigra numpy array converters
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>
#include <boost/python/exception_translator.hpp>

// standart c++ headers (whatever you need (string and vector are just examples))
#include <string>
#include <vector>

// inferno relatex
#include "inferno/inferno.hxx"
#include "inferno/inferno_python.hxx"

#include "inferno/learning/learners/subgradient.hxx"


namespace inferno{
namespace learning{
namespace learners{

    namespace bp = boost::python;

    template<class LEARNER>
    LEARNER * subGradientFactory2(
        typename LEARNER::Dataset & dataset,
        const uint64_t              maxIterations,
        const double                n,
        const double                eps,
        const double                m,
        const int                   verbose,
        const int                   nThreads,
        const int                   averagingOrder,
        const int                   showLossEvery,
        const int                   showRegularizerEvery
    ){
        LEARNER * learner;
        {
            ScopedGILRelease allowThreads;
            const auto options = typename LEARNER::Options(
                maxIterations,
                n,
                eps,
                m,
                verbose,
                nThreads,
                averagingOrder,
                showLossEvery,
                showRegularizerEvery
            );
            learner = new LEARNER(dataset, options);
        }
        return learner;
    }

    template<class LEARNER>
    void subGradientLearn(
        LEARNER & learner,
        typename LEARNER::LossAugmentedInferenceFactoryBase * inferenceFactory,
        WeightVector & weightVector
    ){
        
        {
            ScopedGILRelease allowThreads;
            learner.learn(inferenceFactory, weightVector);
        }
    }

    template<class LEARNER>
    void subGradientLearn2(
        LEARNER & learner,
        typename LEARNER::LossAugmentedInferenceFactoryBase * lossInfFactory,
        WeightVector & weightVector,
        typename LEARNER::InferenceFactoryBase * inferenceFactory
    ){
        
        {
            ScopedGILRelease allowThreads;
            learner.learn(lossInfFactory, weightVector, inferenceFactory);
        }
    }

    template<class DATASET>
    struct ExportSubgradient{
        typedef DATASET Dataset;
        typedef typename Dataset::Model Model;
        typedef typename Dataset::LossFunction LossFunction;
        typedef typename Dataset::GroundTruth GroundTruth;

        // the class to export
        typedef learning::learners::SubGradient<Dataset> Learner; 
        typedef typename Learner::Options Options;      


        static void exportLearner(const std::string & className){
            // the class
            bp::class_<Learner,boost::noncopyable>(className.c_str(),bp::no_init)
                .def("learn",
                    &subGradientLearn<Learner>,
                    (
                        bp::arg("lossAugmentedInferenceFactory"),
                        bp::arg("weightVector")
                    )
                )
                .def("learn",
                    &subGradientLearn2<Learner>,
                    (
                        bp::arg("lossAugmentedInferenceFactory"),
                        bp::arg("weightVector"),
                        bp::arg("inferenceFactory")
                    )
                )
            ;

            const Options defaultOptions;
            // the factory function
            bp::def("subGradient",
                &subGradientFactory2<Learner>,
                (
                    bp::arg("dataset"),
                    bp::arg("maxIterations") = uint64_t(defaultOptions.maxIterations_) ,
                    bp::arg("n") = double(defaultOptions.n_) ,
                    bp::arg("eps") = double(defaultOptions.eps_) ,
                    bp::arg("m") = double(defaultOptions.m_) ,
                    bp::arg("verbose")= int(defaultOptions.verbose_),
                    bp::arg("nThreads") =int(defaultOptions.nThreads_),
                    bp::arg("averagingOrder") =int(defaultOptions.averagingOrder_),
                    bp::arg("showLossEvery") =int(defaultOptions.showLossEvery_),
                    bp::arg("showRegularizerEvery") =int(defaultOptions.showRegularizerEvery_)
                ),
                RetValPol< CustWardPost<0,1,NewObj> >()
            );
        }

    };
}
}
}


#endif /* INFERNO_PYTHON_LEARNING_LEARNERS_EXPORT_SUBGRADIENT_HXX*/
