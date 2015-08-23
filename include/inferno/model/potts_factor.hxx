#ifndef INFERNO_MODEL_POTTS_FACTOR_HXX
#define INFERNO_MODEL_POTTS_FACTOR_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/model/discrete_factor_base.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{
namespace models{



    template<class MODEL, class POTTS_VT>
    class PottsFactor :
    public DiscreteFactorBase<PottsFactor<MODEL, POTTS_VT>, MODEL, POTTS_VT> {
    public:
        typedef typename MODEL::VariableDescriptor VariableDescriptor;

        const PottsFactor

        const value_tables::DiscreteValueTableBase * valueTable()const{
            return vt_;
        }   
        ArityType arity()const{
            return 2;
        }
        DiscreteLabel shape(const size_t d)const{
            return vt_->shape(d);
        }

        VariableDescriptor variable(const size_t d)const{
            return vars_[d];
        }
    private:
        PottsValueTable vt_;
        VariableDescriptor[2]  vars_;
    };


} // end namespace models
} // end namespace inferno




#endif /*INFERNO_MODEL_POTTS_FACTOR_HXX*/
