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

#include "inferno/learning/learners/igo.hxx"


namespace inferno{
namespace learning{
namespace learners{

    namespace bp = boost::python;

    template<class LEARNER>
    LEARNER * igoFactory(
        typename LEARNER::Dataset & dataset,
        const uint64_t              nPertubations,
        const uint64_t              nElites,
        const uint64_t              maxIterations,
        const double                sigma,
        const int                   verbose,
        const int                   seed,
        const double                beta
    ){
        LEARNER * learner;
        {
            ScopedGILRelease allowThreads;
            const auto options = typename LEARNER::Options(
                nPertubations,
                nElites,
                maxIterations,
                sigma,
                verbose,
                seed,
                beta
            );
            learner = new LEARNER(dataset, options);
        }
        return learner;
    }

    template<class LEARNER>
    void igoLearn(
        LEARNER & learner,
        typename LEARNER::InferenceFactoryBase * inferenceFactory,
        WeightVector & weightVector
    ){
        
        {
            ScopedGILRelease allowThreads;
            learner.learn(inferenceFactory, weightVector);
        }
    }


    template<class DATASET>
    void exportIgo(const std::string & className){
        typedef DATASET Dataset;
        typedef typename Dataset::Model Model;
        typedef typename Dataset::LossFunction LossFunction;
        typedef typename Dataset::GroundTruth GroundTruth;

        // the class to export
        typedef learning::learners::Igo<Dataset> Learner; 
        typedef typename Learner::Options Options;      

        // the class
        bp::class_<Learner,boost::noncopyable>(className.c_str(),bp::no_init)
            .def("learn",
                &igoLearn<Learner>,
                (
                    bp::arg("inferenceFactory"),
                    bp::arg("weightVector")
                )
            )
        ;

        const Options defaultOptions;
        // the factory function
        bp::def("igo",
            &igoFactory<Learner>,
            (
                bp::arg("dataset"),
                bp::arg("nPertubations") = uint64_t(defaultOptions.nPertubations_) ,
                bp::arg("nElites") = double(defaultOptions.nElites_) ,
                bp::arg("maxIterations") = uint64_t(defaultOptions.maxIterations_) ,
                bp::arg("sigma") = double(defaultOptions.sigma_) ,
                bp::arg("verbose")= int(defaultOptions.verbose_) ,
                bp::arg("seed")= int(defaultOptions.seed_),
                bp::arg("beta")= int(defaultOptions.beta_)
            ),
            RetValPol< CustWardPost<0,1,NewObj> >()
        );

    }
}
}
}


//#endif /* INFERNO_SRC_PYTHON_SRC_LEARNING_LEARNERS_EXPORT_STOCHASTIC_GRADIENT_HXX*/
