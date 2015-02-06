#ifndef INFERNO_MODEL_VIEW_SUBMODEL_HXX
#define INFERNO_MODEL_VIEW_SUBMODEL_HXX


#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <boost/iterator/counting_iterator.hpp>

#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/model/base_discrete_model.hxx"
#include "inferno/model/base_discrete_model.hxx"

namespace inferno{
namespace models{
/** \brief view submodel for a few variables.
    
    This class is designed for the following use case.
    Generate different submodels (different vi)
    from  BASE_MODEL (one submodel at the time).

*/
template<class BASE_MODEL>
class ViewSubmodel :
public DiscreteGraphicalModelBase< ViewSubmodel<BASE_MODEL> > {

public:
    const static bool SortedVariableIds = true;
    const static bool SortedFactorIds = true;
    typedef boost::counting_iterator<Vi> VariableIdIter;

    // typedefs of adapter API

    /** \var typedef BaseModel 
        \brief BaseModel this adapter is viewing on
    */
    typedef BASE_MODEL BaseModel;
    typedef typename BaseModel:: template VariableMap<Vi> BaseModelViMap;
    typedef typename BaseModel:: template VariableMap<DiscreteLabel> BaseModelLabelMap;

    ViewSubmodel(const BASE_MODEL & baseModel)
    :   baseModel_(baseModel),
        baseViToVi_(baseModel),
        viToBaseVi_(baseModel.nVariables()),
        baseModelLabels_(baseModel),
        nVar_(0){

    }


    // adapter API

    /** \brief set which variables from the base model 
        are in the submodel.

        \todo check that baseViIter is sorted
    */
    template<class BASE_VI_ITER>
    void setSubmodelVariables(
        BASE_VI_ITER baseViIter,
        BASE_VI_ITER baseViEnd
    ){
        nVar_ = 0 ;
        for( ; baseViIter!=baseViEnd; ++baseViIter, ++nVar_){
            const Vi baseVi = *baseViIter;
            baseViToVi_[baseVi] = nVar_;
            viToBaseVi_[nVar_] = baseVi;
        }
    }

    VariableIdIter variableIdsBegin()const{
        return VariableIdIter(0);
    }

    VariableIdIter variableIdsEnd()const{
        return VariableIdIter(nVar_);
    }

    LabelType nLabels(const Vi variabeId)const{
        return baseModel_.nLabels(this->viToBaseVi(variabeId));
    }


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
        return viToBaseVi_[vi];
    }


private:
    const BASE_MODEL & baseModel_;
    BaseModelViMap baseViToVi_;
    std::vector<Vi> viToBaseVi_;
    BaseModelLabelMap baseModelLabels_;
    Vi nVar_;
};



} // end namespace model
} // end namespace inferno


#endif /* INFERNO_MODEL_VIEW_SUBMODEL_HXX */
