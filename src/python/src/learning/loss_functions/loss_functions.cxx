#define PY_ARRAY_UNIQUE_SYMBOL inferno_learning_loss_functions_PyArray_API
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





namespace inferno{
namespace learning{
namespace loss_functions{

    namespace bp = boost::python;

    template<class MODEL>
    void exportVariationOfInformationT();
       
}
}
}




// export my module
BOOST_PYTHON_MODULE_INIT(loss_functions) {

    namespace bp = boost::python;
    // Do not change next 4 lines

    import_array(); 
    vigra::import_vigranumpy();
    bp::numeric::array::set_module_and_type("numpy", "ndarray");
    bp::docstring_options docstringOptions(true,true,false);
    // No not change 4 line above

    namespace lf = inferno::learning::loss_functions;

    lf::exportVariationOfInformationT<inferno::models::PyGeneralDiscreteModel>();
    lf::exportVariationOfInformationT<inferno::models::PyParametrizedMulticutModel>();

}
