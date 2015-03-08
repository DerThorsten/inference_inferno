#define PY_ARRAY_UNIQUE_SYMBOL inferno_models_PyArray_API
#define NO_IMPORT_ARRAY


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
#include "inferno/model/general_discrete_tl_model.hxx"
#include "inferno/model/implicit_potts_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"
#include "inferno/value_tables/explicit.hxx"

namespace inferno{
namespace models{

    namespace bp = boost::python;

    typedef TlModel<value_tables::PottsValueTable, value_tables::UnaryValueTable> GeneralDiscreteTlGraphicalModel;


    void exportGeneralDiscreteTlGraphicalModel(){

        bp::class_<GeneralDiscreteTlGraphicalModel>("GeneralDiscreteTlGraphicalModel",bp::init<Vi,DiscreteLabel>())
        ;
    }
}
}


