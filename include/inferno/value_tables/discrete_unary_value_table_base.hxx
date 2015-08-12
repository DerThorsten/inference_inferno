/** \file discrete_value_table_base.hxx 
    \brief inferno::value_tables::DiscreteValueTableBase
    is implemented in this header. 
*/
#ifndef INFERNO_VALUE_TABLES_CONSTRAIN_DISCRETE_VALUE_TYPE_BASE_HXX
#define INFERNO_VALUE_TABLES_CONSTRAIN_DISCRETE_VALUE_TYPE_BASE_HXX

#include <cstdint>
#include <vector>
#include <limits>

#include "inferno/inferno.hxx"
#include "inferno/utilities/shape_walker.hxx"
#include "inferno/value_tables/value_table_utilities.hxx"
    
namespace inferno{

/** \namespace inferno::value_tables 
    \brief all (discrete) value tables are implemented 
    in this namespace.

    All values tables and related functionality
    is implemented within inferno::value_tables .
*/
namespace value_tables{


class DiscreteUnaryValueTableBase
{
public:
    DiscreteUnaryValueTableBase():DiscreteValueTableBase(){
    }
    virtual ~DiscreteUnaryValueTableBase() = default;
    virtual size() const = 0;
    virtual eval(const DiscreteLabel l ) = 0;
};

} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_CONSTRAIN_DISCRETE_VALUE_TYPE_BASE_HXX*/
