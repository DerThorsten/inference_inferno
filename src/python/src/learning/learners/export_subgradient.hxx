//#ifndef INFERNO_SRC_PYTHON_SRC_LEARNING_LEARNERS_EXPORT_STOCHASTIC_GRADIENT_HXX
//#define INFERNO_SRC_PYTHON_SRC_LEARNING_LEARNERS_EXPORT_STOCHASTIC_GRADIENT_HXX

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
    LEARNER * subGradientFactory(
        typename LEARNER::Dataset & dataset,
        const uint64_t              maxIterations,
        const double                c,
        const double                n,
        const double                m,
        const int                   verbose 
    ){
        LEARNER * learner;
        {
            ScopedGILRelease allowThreads;
            const auto options = typename LEARNER::Options(
                maxIterations,
                c,
                n,
                m,
                verbose
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
    void exportSubGradient(const std::string & className){
        typedef DATASET Dataset;

        // the class to export
        typedef learning::learners::SubGradient<Dataset> Learner; 
        typedef typename Learner::Options Options;      

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
            &subGradientFactory<Learner>,
            (
                bp::arg("dataset"),
                bp::arg("maxIterations") = uint64_t(defaultOptions.maxIterations_) ,
                bp::arg("c") = double(defaultOptions.c_) ,
                bp::arg("n") = double(defaultOptions.n_) ,
                bp::arg("m") = double(defaultOptions.m_) ,
                bp::arg("verbose")= int(defaultOptions.verbose_)
            ),
            RetValPol< CustWardPost<0,1,NewObj> >()
        );

    }
}
}
}


//#endif /* INFERNO_SRC_PYTHON_SRC_LEARNING_LEARNERS_EXPORT_STOCHASTIC_GRADIENT_HXX*/
