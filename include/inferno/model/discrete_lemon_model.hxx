/** \file general_discrete_model.hxx 
    \brief  functionality for inferno::models::GeneralDiscreteModel is implemented in this header.
*/
#ifndef INFERNO_MODEL_DISCRETE_LEMON_MODEL_HXX
#define INFERNO_MODEL_DISCRETE_LEMON_MODEL_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/value_tables/discrete_unary_value_table_base.hxx"
#include "inferno/model/discrete_factor_base.hxx"
#include "inferno/model/discrete_unary_base.hxx"
#include "inferno/model/discrete_model_base.hxx"


namespace inferno{
namespace models{



template<class MODEL>
class DiscreteLemonModelFactor :
public DiscreteFactorBase< DiscreteLemonModelFactor<MODEL>, MODEL>
{

};

template<class MODEL>
class DiscreteLemonModelUnary :
public DiscreteUnaryBase<DiscreteLemonModelUnary<MODEL>, MODEL>
{
    
};



template<class GRAPH, class UNARY_VAR_MAP, class FACTOR_EDGE_MAP>
class DiscreteLemonModel : 
public DiscreteGraphicalModelBase<GeneralDiscreteModel>{

public:
    typedef typename GRAPH::Node VariableDescriptor;
    typedef typename GRAPH::Edge FactorDescriptor;
    typedef typename GRAPH::Node UnaryDescriptor;
};





} // end namespace inferno::models
} // inferno


#endif  /*INFERNO_MODEL_DISCRETE_LEMON_MODEL_HXX*/
