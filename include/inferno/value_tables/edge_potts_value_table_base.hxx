/** \file discrete_value_table_base.hxx 
    \brief inferno::value_tables::DiscreteValueTableBase
    is implemented in this header. 
*/
#ifndef INFERNO_VALUE_TABLES_EDGE_POTTS_VALUE_TABLE_BASE_HXX
#define INFERNO_VALUE_TABLES_EDGE_POTTS_VALUE_TABLE_BASE_HXX

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


class EdgePottsValueTableBase : public DiscreteValueTableBase
{
public:
    using DiscreteValueTableBase::eval;
    
    EdgePottsValueTableBase():DiscreteValueTableBase(){

    }

    virtual ~EdgePottsValueTableBase(){
    }

    // pure virtual (in addition to pure
    //  virtual from base
    virtual ValueType beta() const = 0;


    // overrides
    virtual ValueType eval(const LabelType *conf)const override{
        return (conf[0]==conf[1])  ==  (conf[2]==conf[3]) ? 0 : this->beta();
    }
    virtual ArityType arity()const override{
        return 4;
    }
    virtual ValueType eval(
        const LabelType l1, 
        const LabelType l2, 
        const LabelType l3, 
        const LabelType l4
    )const override{
        return (l1==l2)  ==  (l3==l4) ? 0 : this->beta();
    }

    virtual bool isGeneralizedPotts() const override{
        return true;
    }
    virtual bool isPotts(ValueType & beta) const override{
        return false;
    }
};

} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_EDGE_POTTS_VALUE_TABLE_BASE_HXX*/
