#ifndef  INFERNO_SRC_PYTHON_VALUE_TABLES_EXPORT_VALUE_TABLES_HXX
#define INFERNO_SRC_PYTHON_VALUE_TABLES_EXPORT_VALUE_TABLES_HXX

#include <boost/python/def_visitor.hpp> 

#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"

namespace inferno{
namespace export_helper{

    namespace bp = boost::python;
    struct BaseVtWrap : 
    public value_tables::DiscreteValueTableBase, 
    bp::wrapper<value_tables::DiscreteValueTableBase>
    {
        typedef value_tables::DiscreteValueTableBase BaseVt;

        // pure virtual functions = 0
        ValueType eval(const LabelType * conf)const{
            return this->get_override("eval")(conf);
        }
        LabelType shape(const uint32_t i) const {
            return this->get_override("shape")(i);
        }
        uint32_t arity() const {
            return this->get_override("arity")();
        }

        // virtual functions with default impl.
        uint64_t size()const{
            if (bp::override f = this->get_override("size"))
                return size(); // *note*
            return BaseVt::size();
        }
        uint64_t default_size()const{ 
            return this->BaseVt::size(); 
        }

    };

}
}

#endif /* INFERNO_SRC_PYTHON_MODELS_EXPORT_MODELS_HXX */
