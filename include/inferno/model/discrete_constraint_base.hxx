#ifndef INFERNO_MODEL_CONSTRAINT_BASE_HXX
#define INFERNO_MODEL_CONSTRAINT_BASE_HXX

#include <boost/iterator/iterator_facade.hpp>

#include "inferno/inferno.hxx"
#include "inferno/utilities/utilities.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/utilities/shape_walker.hxx"

namespace inferno{
namespace models{




/** \brief Base CRT Factor Base
 * 
 * 
 *  ALl constraints MUST implement:
 * 
 *  DiscreteConstaintTableBase * contraintTable() const;
 *  VariableDescriptor  variable(ArityType i) const;
 * 
 * 
*/
template<class CONSTRAINT, class MODEL >
class DiscreteConstraintBase{

private:




   
public:

private:
    const CONSTRAINT * factor()const{
        return static_cast<const CONSTRAINT *>(this);
    }
    CONSTRAINT * factor(){
        return static_cast<CONSTRAINT *>(this);
    }
};






} // end namespace inferno::models
} // inferno

#endif /* INFERNO_MODEL_CONSTRAINT_BASE_HXX */
