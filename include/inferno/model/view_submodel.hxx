#ifndef INFERNO_MODEL_VIEW_SUBMODEL_HXX
#define INFERNO_MODEL_VIEW_SUBMODEL_HXX

#include <cstdint> 
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <boost/iterator/counting_iterator.hpp>

#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/model/base_discrete_factor.hxx"
#include "inferno/model/base_discrete_model.hxx"


namespace inferno{


namespace models{




namespace detail_view_submodel{

    template<class VIEW_SUBMODEL>
    class FactorBase : public inferno::DiscreteFactorBase< FactorBase<VIEW_SUBMODEL> >{
    public:
        virtual ~FactorBase(){}
        virtual Vi vi(const size_t d) const = 0;
        virtual const inferno::value_tables::DiscreteValueTableBase * valueTable()const=0;
    };

    template<class VIEW_SUBMODEL>
    class FullyIncludedFactor :  public FactorBase<VIEW_SUBMODEL>
    {
    private:
        typedef typename VIEW_SUBMODEL::BaseModelFactorProxy BaseModelFactorProxy;
    public:
        FullyIncludedFactor(const VIEW_SUBMODEL & viewSubmodel, const Fi baseFi)
        :   viewSubmodel_(&viewSubmodel),
            baseModelFactor_(viewSubmodel.baseModel()[baseFi]){

        }

        virtual Vi vi(const size_t d) const {
            return viewSubmodel_->baseViToVi(baseModelFactor_->vi(d));
        }
        virtual const inferno::value_tables::DiscreteValueTableBase * valueTable()const{
            return baseModelFactor_->valueTable();
        }
    private:
        const VIEW_SUBMODEL * viewSubmodel_;
        const BaseModelFactorProxy baseModelFactor_;
    };

    template<class BASE_MODEL>
    class ViMapping{

    public:
        typedef BASE_MODEL BaseModel;
        ViMapping(const BaseModel & baseModel)
        :   notInSubmodelId_(baseModel.maxVarId()+1),
            baseViToVi_(baseModel,notInSubmodelId_),
            viToBaseVi_(),
            nVar_(0){

        }

        template<class BASE_VI_ITER>
        void setSubmodelVariables(
            BASE_VI_ITER baseViIter,
            BASE_VI_ITER baseViEnd
        ){
            // forward and backward mapping 
            // between vi and baseVi
            nVar_ = 0 ;
            for( ; baseViIter!=baseViEnd; ++baseViIter, ++nVar_){
                const Vi baseVi = *baseViIter;
                baseViToVi_[baseVi] = nVar_;
                viToBaseVi_[nVar_] = baseVi;
            }
        }


        Vi nVariables()const{
            return nVar_;
        }
        Vi maxVarId()const{
            return nVar_-1;
        }
        Vi minVarId()const{
            return 0;
        }
        Vi baseViToVi(const Vi baseVi)const{
            return baseViToVi_[baseVi];
        }
        Vi viToBaseVi(const Vi vi)const{
            return viToBaseVi_[vi];
        }

        bool isInSubmodel(const Vi baseVi)const{
            // \todo buffer max id
            return baseViToVi_[baseVi] < nVar_ ;
        }

    private:
        typedef typename BASE_MODEL:: template VariableMap<Vi> BaseModelViMap;
        Vi notInSubmodelId_;
        BaseModelViMap baseViToVi_;
        std::vector<Vi> viToBaseVi_;
        Vi nVar_;
    };

}









/** \brief view submodel for a few variables.
    
    This class is designed for the following use case.
    Generate different submodels (different vi)
    from  BASE_MODEL (one submodel at the time).

*/
template<class BASE_MODEL, class BASE_MODEL_FACTORS_OF_VARIABLES>
class ViewSubmodel :
    public DiscreteGraphicalModelBase< ViewSubmodel<BASE_MODEL, BASE_MODEL_FACTORS_OF_VARIABLES> > {
private: 
    typedef ViewSubmodel<BASE_MODEL, BASE_MODEL_FACTORS_OF_VARIABLES> Self;
    enum FactorIncluding{
        NotIncluded = 0,
        PartialIncluded = 1,
        FullyIncluded = 2
    };


    typedef detail_view_submodel::FactorBase<Self> FactorBase;
    typedef detail_view_submodel::FullyIncludedFactor<Self> FullyIncludedFactor;

public:
    typedef typename BASE_MODEL::FactorProxy BaseModelFactorProxy;

    typedef const FactorBase * FactorProxy;
    const static bool SortedVariableIds = true;
    const static bool SortedFactorIds = true;

    typedef boost::counting_iterator<Vi> VariableIdIter;
    typedef boost::counting_iterator<Fi> FactorIdIter;
    // typedefs of adapter API

    /** \var typedef BaseModel 
        \brief BaseModel this adapter is viewing on
    */
    typedef BASE_MODEL BaseModel;
    typedef typename BaseModel:: template VariableMap<Vi> BaseModelViMap;
    typedef typename BaseModel:: template VariableMap<DiscreteLabel> BaseModelLabelMap;


    typedef typename BaseModel:: template FactorMap<FactorIncluding> IncludedFactorMap;

    ViewSubmodel(const BASE_MODEL & baseModel, const BASE_MODEL_FACTORS_OF_VARIABLES & baseModelFactorsOfVariables)
    :   baseModel_(baseModel),
        baseModelFacOfVars_(baseModelFactorsOfVariables),
        baseViToVi_(baseModel, baseModel.maxVarId()+1),
        viToBaseVi_(baseModel.nVariables()),
        baseModelLabels_(baseModel),
        baseFiIncluding_(baseModel, NotIncluded),
        includedFactors_(baseModel.nFactors(),0),
        includedFactorsStorageIndex_(baseModel.nFactors(),0),
        nVar_(0),
        nFac_(0){

    }


    // adapter API

    /** \brief set which variables from the base model 
        are in the submodel.

    */
    template<class T>
    void setSubmodelVariables(std::initializer_list<T> viList){
        return setSubmodelVariables(viList.begin(), viList.end());
    }

    /** \brief set which variables from the base model 
        are in the submodel.

        \todo check that baseViIter is sorted
    */
    template<class BASE_VI_ITER>
    void setSubmodelVariables(
        BASE_VI_ITER baseViIter,
        BASE_VI_ITER baseViEnd
    ){
        // forward and backward mapping 
        // between vi and baseVi
        nVar_ = 0 ;
        for( ; baseViIter!=baseViEnd; ++baseViIter){
            const Vi baseVi = *baseViIter;
            baseViToVi_[baseVi] = nVar_;
            viToBaseVi_[nVar_] = baseVi;
            ++nVar_;
        }

        for(Vi vi=0; vi<nVar_; ++vi){
            const Vi baseVi = viToBaseVi(vi);

            // iterate over all factors for baseVi
            for(const auto  baseFi : baseModelFacOfVars_[baseVi]){
                if(baseFiIncluding_[baseFi]==NotIncluded){
                    INFERNO_ASSERT_OP(baseFi,<,baseModel_.maxFactorId()+1);
                    const auto factor = baseModel_[baseFi];
                    const auto arity = factor->arity();
                    // two cases can happen
                    // -factor is partial included 
                    //   (not all vis of factors are included in this model)
                    // -factor is total included
                    //   (all vis are in included  )
                   bool fullyIncluded = true;
                   for(size_t a=0; a<arity; ++a){
                       const auto baseVi = factor->vi(a);
                       if(!isInSubmodel(baseVi)){
                           fullyIncluded = false;
                           break;
                       }
                   }
                   if(fullyIncluded){
                       baseFiIncluding_[baseFi] = FullyIncluded;

                   }
                   else{
                       //baseFiIncluding_[baseFi] = PartialIncluded;
                   }
                }
                includedFactors_[nFac_] = baseFi;
                ++nFac_;
            }
        }
    }

    VariableIdIter variableIdsBegin()const{
        return VariableIdIter(0);
    }

    VariableIdIter variableIdsEnd()const{
        return VariableIdIter(nVar_);
    }
    FactorIdIter factorIdsBegin()const{
        return FactorIdIter(0);
    }

    FactorIdIter factorIdsEnd()const{
        return FactorIdIter(nFac_);
    }
    LabelType nLabels(const Vi variabeId)const{
        return baseModel_.nLabels(this->viToBaseVi(variabeId));
    }

    FactorProxy operator[](const Fi fi) const {
        
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

    bool isInSubmodel(const Vi baseVi)const{
        // \todo buffer max id
        return baseViToVi_[baseVi] < nVar_ ;
    }


    FactorIncluding baseFiIncluding(const Fi baseFi)const{
        return includedFactors_[baseFi];
    }



    const BaseModel & baseModel()const{
        return baseModel_;
    }


    /** \brief get the baseModel label map
    */
    BaseModelLabelMap & baseModelLabelMap(){
        return  baseModelLabels_;
    }

private:
    const BASE_MODEL & baseModel_;
    const BASE_MODEL_FACTORS_OF_VARIABLES & baseModelFacOfVars_;
    BaseModelViMap baseViToVi_;
    std::vector<Vi> viToBaseVi_;
    BaseModelLabelMap baseModelLabels_;
    IncludedFactorMap baseFiIncluding_;
    std::vector<Fi> includedFactors_;
    std::vector<Fi> includedFactorsStorageIndex_;
    Vi nVar_;
    Fi nFac_;

    

    std::vector<FullyIncludedFactor> fullyIncludedBuffer_;
};



} // end namespace model
} // end namespace inferno


#endif /* INFERNO_MODEL_VIEW_SUBMODEL_HXX */
