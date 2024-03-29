#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_PyArray_API
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
#include "inferno/learning/learning.hxx"

namespace inferno{
namespace learning{

    namespace bp = boost::python;

    void exportWeights();
    void exportRegularizer();
    void exportWeightConstraints();

}
}




// export my module
BOOST_PYTHON_MODULE_INIT(learning) {

    namespace bp = boost::python;
    // Do not change next 4 lines

    import_array(); 
    vigra::import_vigranumpy();
    bp::numeric::array::set_module_and_type("numpy", "ndarray");
    bp::docstring_options docstringOptions(true,true,false);
    // No not change 4 line above


    inferno::learning::exportWeights();
    inferno::learning::exportRegularizer();
    inferno::learning::exportWeightConstraints();
}
