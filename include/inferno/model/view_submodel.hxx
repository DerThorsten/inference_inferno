#ifndef INFERNO_MODEL_SPARSE_VIEW_SUBMODEL_HXX
#define INFERNO_MODEL_SPARSE_VIEW_SUBMODEL_HXX


#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/model/base_discrete_model.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{
namespace model{

template<class BASE_MODEL>
class ViewSubmodel :
public DiscreteGraphicalModelBase< ViewSubmodel<BASE_MODEL> > {

public:
    // typedefs of adapter API

    /** \var typedef BaseModel 
        \brief BaseModel this adapter is viewing on
    */
    typedef BASE_MODEL BaseModel;

    // adapter API

    /** \brief get variable index of this
        from a variable index of the base model

        \param baseVi : variable index w.r.t. the base model

        \return : variable index w.r.t. this model

        \warning baseVi  must be included in the submodel,
        otherwise the result is undefined
    */
    Vi baseViToVi(const Vi baseVi)const{
        return baseViToVi_[baseVi];
    }

    /** \brief get variable index of the
        base model from a variable index w.r.t this model

        \param vi : variable index w.r.t. the this model

        \return : variable index w.r.t. base model
    */
    Vi viToBaseVi(const Vi vi)const{
        return viToBaseVi_[vi]
    }


private:
    std::vector<Vi> baseViToVi_;
    std::vector<Vi> viToBaseVi_;

    const BASE_MODEL & baseModel_;
    Vi nVar_;
};



} // end namespace model
} // end namespace inferno


#endif /* INFERNO_MODEL_SPARSE_VIEW_SUBMODEL_HXX */
