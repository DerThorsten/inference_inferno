#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_learners_PyArray_API
//#define NO_IMPORT_ARRAY


// boost python related
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>

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

#include "inferno/model/general_discrete_model.hxx"
#include "inferno/model/parametrized_multicut_model.hxx"
#include "inferno/learning/dataset/explicit_dataset.hxx"
#include "inferno/learning/loss_functions/loss_functions.hxx"
#include "inferno/learning/loss_functions/edge_hamming.hxx"



namespace inferno{
namespace learning{
namespace learners{

    namespace bp = boost::python;

    template<class DATASET>
    void exportStochasticGradient(const std::string & );

    template<class DATASET>
    void exportSubGradient(const std::string & );
       
}
}
}




// export my module
BOOST_PYTHON_MODULE_INIT(learners) {

    namespace bp = boost::python;
    // Do not change next 4 lines

    import_array(); 
    vigra::import_vigranumpy();
    bp::numeric::array::set_module_and_type("numpy", "ndarray");
    bp::docstring_options docstringOptions(true,true,false);
    // No not change 4 line above

    namespace ll = inferno::learning::learners;

    /////////////////////////////////////////////////////
    // general discrete model
    /////////////////////////////////////////////////////
    {
        using namespace inferno;
        typedef models::GeneralDiscreteModel Model;

        {
            typedef learning::loss_functions::VariationOfInformation<Model> LossFunction;
            typedef learning::dataset::VectorDataset<LossFunction>  Dataset;
            ll::exportStochasticGradient<Dataset>(
                std::string("StochasticGradient_VariationOfInformation_GeneralDiscreteModel")
            );
        }
        {
            typedef learning::loss_functions::EdgeHamming<Model> LossFunction;
            typedef learning::dataset::VectorDataset<LossFunction>  Dataset;
            ll::exportSubGradient<Dataset>(
                std::string("SubGradient_EdgeHamming_GeneralDiscreteModel")
            );
        }
    }
    /////////////////////////////////////////////////////
    // parametrized multicut model
    /////////////////////////////////////////////////////
    {
        using namespace inferno;
        typedef models::ParametrizedMulticutModel<
            models::detail_python::EdgeArray, 
            models::detail_python::FeatureArray
        > Model;

        {
            typedef learning::loss_functions::VariationOfInformation<Model> LossFunction;
            typedef learning::dataset::VectorDataset<LossFunction>  Dataset;
            ll::exportStochasticGradient<Dataset>(
                std::string("StochasticGradient_VariationOfInformation_ParametrizedMulticutModel")
            );
        }
        {
            typedef learning::loss_functions::EdgeHamming<Model> LossFunction;
            typedef learning::dataset::VectorDataset<LossFunction>  Dataset;
            ll::exportSubGradient<Dataset>(
                std::string("SubGradient_EdgeHamming_ParametrizedMulticutModel")
            );
        }
    }

}
