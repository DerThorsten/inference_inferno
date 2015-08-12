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


// export helper
#include "exportValueTables.hxx"

// standart c++ headers (whatever you need (string and vector are just examples))
#include <string>
#include <vector>

// inferno relatex
#include "inferno/inferno.hxx"


#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/value_tables/potts.hxx"

namespace inferno{


    namespace bp = boost::python;





    void exportDiscreteValueTableBase(){

        typedef value_tables::DiscreteValueTableBase BaseVt;
        bp::class_<export_helper::BaseVtWrap, boost::noncopyable>("DiscreteValueTableBase")

            // pure virtual functions
            //.def("eval", bp::pure_virtual( &BaseVt::eval))
            .def("shape", bp::pure_virtual(&BaseVt::shape))
            .def("arity", bp::pure_virtual(&BaseVt::arity))
            
            // virtual functions with default implementation
            .def("size", &BaseVt::size, &export_helper::BaseVtWrap::default_size)
        ;


        // export potts
        typedef value_tables::PottsValueTable PottsVt;
        bp::class_< PottsVt,bp::bases<BaseVt> >("PottsValueTable",bp::init<DiscreteLabel,ValueType>())
        ;

    }
}



