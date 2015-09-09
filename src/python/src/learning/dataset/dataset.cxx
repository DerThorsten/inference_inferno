#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_dataset_PyArray_API
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
#include "inferno/learning/dataset/explicit_dataset.hxx"
#include "inferno/learning/loss_functions/loss_functions.hxx"
#include "inferno/learning/loss_functions/edge_hamming.hxx"

#include "inferno/python/model/general_discrete_model.hxx"
#include "inferno/python/model/parametrized_multicut_model.hxx"

namespace inferno{
namespace learning{
namespace dataset{

    namespace bp = boost::python;

    template<class LOSS_FUNCTION>
    void exportVectorDataset(const std::string & lossFunctionClsName);
    

    void exportDefaultDatasetParametrizedMulticutModel();
}
}
}




// export my module
BOOST_PYTHON_MODULE_INIT(dataset) {

    namespace bp = boost::python;
    // Do not change next 4 lines

    import_array(); 
    vigra::import_vigranumpy();
    bp::numeric::array::set_module_and_type("numpy", "ndarray");
    bp::docstring_options docstringOptions(true,true,false);
    // No not change 4 line above


    
    namespace ds = inferno::learning::dataset;


    // NEW
    ds::exportDefaultDatasetParametrizedMulticutModel();



    // OLD




    // General discrete model
    inferno::learning::dataset::exportVectorDataset<
        inferno::learning::loss_functions::VariationOfInformation<
            inferno::models::PyGeneralDiscreteModel
        >
    >(std::string("VariationOfInformation"));

    inferno::learning::dataset::exportVectorDataset<
        inferno::learning::loss_functions::EdgeHamming<
            inferno::models::PyGeneralDiscreteModel
        >
    >(std::string("EdgeHamming"));



    // parametrized multicut model
    inferno::learning::dataset::exportVectorDataset<
        inferno::learning::loss_functions::VariationOfInformation<
            inferno::models::PyParametrizedMulticutModel
        >
    >(std::string("VariationOfInformation"));

    inferno::learning::dataset::exportVectorDataset<
        inferno::learning::loss_functions::EdgeHamming<
            inferno::models::PyParametrizedMulticutModel
        >
    >(std::string("EdgeHamming"));



}
