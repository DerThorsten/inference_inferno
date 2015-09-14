/** \file discrete_value_table_base.hxx 
    \brief inferno::value_tables::DiscreteValueTableBase
    is implemented in this header. 
*/
#ifndef INFERNO_CONSTRAINT_TABLES_CONSTRAINT_TABLE_BASE_HXX
#define INFERNO_CONSTRAINT_TABLES_CONSTRAINT_TABLE_BASE_HXX

#include <cstdint>
#include <vector>
#include <limits>

#include <boost/iterator/iterator_facade.hpp>
#include "inferno/inferno.hxx"
#include "inferno/utilities/utilities.hxx"
#include "inferno/utilities/shape_walker.hxx"



    

namespace inferno{
namespace constraint_tables{

    
    class DiscreteConstaintTableBase{
    public:


        virtual ArityType arity() const = 0;
        virtual DiscreteLabel shape(const ArityType  i) const = 0;
        virtual bool feasible(const DiscreteLabel *conf) const = 0;

    };



    
} // end namespace value_tables
} // end namespace inferno

#endif /* INFERNO_CONSTRAINT_TABLES_CONSTRAINT_TABLE_BASE_HXX */