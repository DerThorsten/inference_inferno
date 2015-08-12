/** \file view_submodel.hxx 
    \brief  functionality for inferno::models::ViewSubmodel is implemented in this header.
*/
#ifndef INFERNO_MODEL_VIEW_SUBMODEL_HXX
#define INFERNO_MODEL_VIEW_SUBMODEL_HXX

#include <cstdint> 
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <boost/iterator/counting_iterator.hpp>

#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/model/discrete_factor_base.hxx"
#include "inferno/model/discrete_model_base.hxx"


namespace inferno{
namespace models{

// forward declaration
template<class BASE_MODEL, class BASE_MODEL_FACTORS_OF_VARIABLES>
class ViewSubmodel;

/** \namespace inferno::models::detail_view_submodel
    \brief detail namespace only for functionality of
    ViewSubmodel.
*/
namespace detail_view_submodel{

    /** \brief factor class of  inferno::models::ViewSubmodel
        
        This class should is only used by inferno::models::ViewSubmodel.
        Here we use a design with virtual functions.
        Therefore the factor class itself can vary on runtime.
        A factor can either be fully included or partially
        included.
        Fully included factors are implemented  inferno::models::detail_view_submodel::FullyIncludedFactor,
        and partially included factors in inferno::models::detail_view_submodel::PartiallyIncludedFactor
    */ 
    template<class VIEW_SUBMODEL>
    class FactorBase : public inferno::models::DiscreteFactorBase< FactorBase<VIEW_SUBMODEL> >{
    public:
        typedef typename VIEW_SUBMODEL::VariableDescriptor VariableDescriptor;
        virtual ~FactorBase(){}
        virtual VariableDescriptor variable(const size_t d) const = 0;
        virtual const inferno::value_tables::DiscreteValueTableBase * valueTable()const=0;
    };
    /** \brief factor class of  inferno::models::ViewSubmodel implementing a fully included factor

        This factor directly uses the value table of the factor this factor is viewing to.
    */
    template<class VIEW_SUBMODEL>
    class FullyIncludedFactor :  public FactorBase<VIEW_SUBMODEL>
    {
    private:
        typedef typename VIEW_SUBMODEL::VariableDescriptor VariableDescriptor;
        typedef typename VIEW_SUBMODEL::BaseModelFactorProxy BaseModelFactorProxy;
    public:
        FullyIncludedFactor(const VIEW_SUBMODEL & viewSubmodel, const Fi baseFi)
        :   viewSubmodel_(&viewSubmodel),
            baseModelFactor_(viewSubmodel.baseModel().factor(baseFi)){

        }
        uint32_t arity()const{
            return baseModelFactor_->arity();
        }
        virtual VariableDescriptor variable(const size_t d) const {
            return viewSubmodel_->fromBaseModelVariableDescriptor(baseModelFactor_->variable(d));
        }
        virtual const inferno::value_tables::DiscreteValueTableBase * valueTable()const{
            return baseModelFactor_->valueTable();
        }
    private:
        const VIEW_SUBMODEL * viewSubmodel_;
        const BaseModelFactorProxy baseModelFactor_;
    };


    /** \brief value table calls tailor-made  for inferno::models::detail_view_submodel::PartiallyIncludedFactor
        
        This value table is only used within inferno::models::ViewSubmodel.
    */
    template<class PFACTOR>
    class  PartialView : public inferno::value_tables::DiscreteValueTableBase{
    public:
        PartialView(const PFACTOR * factorPtr)
        :   inferno::value_tables::DiscreteValueTableBase(),
            pFactor_(factorPtr)
        {

        }
        void setFactorPtr(const PFACTOR * factorPtr){
            pFactor_ = factorPtr;
        }
        virtual ValueType eval(const LabelType *conf)const{
           return pFactor_->eval(conf);
        }
        virtual LabelType shape(const uint32_t d) const{
            return pFactor_->shape(d);
        }
        virtual uint32_t  arity()const{
            return pFactor_->arity();
        }
    private:
        const PFACTOR * pFactor_;
    };



    /** \brief factor class of  inferno::models::ViewSubmodel implementing a partially included factor

        This factor directly uses the value table of the factor this factor is viewing to.
        The variables of the base factor which are not included are fixed to
        their current label.
    */
    template<class VIEW_SUBMODEL>
    class PartiallyIncludedFactor :  public FactorBase<VIEW_SUBMODEL>
    {
        template<class BASE_MODEL, class BASE_MODEL_FACTORS_OF_VARIABLES>
        friend class inferno::models::ViewSubmodel;
    private:
        typedef typename VIEW_SUBMODEL::VariableDescriptor VariableDescriptor;
        typedef typename VIEW_SUBMODEL::BaseModelFactorProxy BaseModelFactorProxy;
    public:
        PartiallyIncludedFactor(
            const VIEW_SUBMODEL & viewSubmodel, 
            const Fi baseFi,
            const size_t newArity,
            const uint64_t indexMappingOffset,
            const uint64_t fixedVarPosOffset,
            const uint64_t confBufferOffset
        )
        :   viewSubmodel_(&viewSubmodel),
            baseModelFactor_(viewSubmodel.baseModel().factor(baseFi)),
            newArity_(newArity),
            indexMappingOffset_(indexMappingOffset),
            fixedVarPosOffset_(fixedVarPosOffset),
            confBufferOffset_(confBufferOffset),
            newVt_(this){
        }
        uint32_t arity()const{
            return newArity_;
        }
        ValueType eval(const DiscreteLabel * conf)const{
            DiscreteLabel * buffer = viewSubmodel_->confBuffer_.data()+ confBufferOffset_;
            // copy non fixed
            for(size_t d=0; d<newArity_; ++d){
                const size_t baseFacD = viewSubmodel_->indexMapping_[indexMappingOffset_ + d];
                buffer[baseFacD] = conf[d];
            }
            // copy fixed
            const auto baseArity = baseModelFactor_->arity();
            const auto nFixed = baseArity  - newArity_;
            for(size_t i=0; i<nFixed; ++i){
                const auto p = viewSubmodel_->fixedVarPos_[fixedVarPosOffset_+i];
                buffer[p] = viewSubmodel_->baseModelLabels_[baseModelFactor_->variable(p)];
            }
            return baseModelFactor_->eval(buffer);
        }
        virtual DiscreteLabel shape(const size_t d) const {
            const size_t baseFacD = viewSubmodel_->indexMapping_[indexMappingOffset_ + d];
            return baseModelFactor_->shape(baseFacD);
        }
        virtual VariableDescriptor variable(const size_t d) const {
            const size_t baseFacD = viewSubmodel_->indexMapping_[indexMappingOffset_ + d];
            return viewSubmodel_->fromBaseModelVariableDescriptor(baseModelFactor_->variable(baseFacD));
        }
        virtual const inferno::value_tables::DiscreteValueTableBase * valueTable()const{
            return &newVt_;
        }
        
    private:
        void finish(){
            newVt_.setFactorPtr(this);
        }
        const VIEW_SUBMODEL * viewSubmodel_;
        const BaseModelFactorProxy baseModelFactor_;
        const size_t  newArity_;
        const uint64_t indexMappingOffset_;
        const uint64_t fixedVarPosOffset_;
        const uint64_t confBufferOffset_;

        PartialView<PartiallyIncludedFactor<VIEW_SUBMODEL> > newVt_;
    };


    /// \cond
    template<class BASE_MODEL>
    class VariableMapping{

    public:
        typedef BASE_MODEL BaseModel;



        typedef typename BaseModel::VariableDescriptor BaseModelVariableDescriptor;
        typedef Vi VariableDescriptor;

        typedef boost::counting_iterator<VariableDescriptor> VariableDescriptorIter; 



        typedef boost::counting_iterator<Vi> VariableIdIter;

        VariableMapping(const BaseModel & baseModel)
        :   notInSubmodelId_(baseModel.maxVarId()+1),
            fromBaseVarDesc_(baseModel,notInSubmodelId_),
            toBaseVarDesc_(){

        }

        template<class BASE_VI_ITER>
        void setSubmodelVariables(
            BASE_VI_ITER baseViIter,
            BASE_VI_ITER baseViEnd
        ){
            toBaseVarDesc_.resize(0);
            // forward and backward mapping 
            // between vi and baseVi
            nVar_ = 0 ;
            for( ; baseViIter!=baseViEnd; ++baseViIter, ++nVar_){
                const Vi baseVi = *baseViIter;
                fromBaseVarDesc_[baseVi] = nVar_;
                toBaseVarDesc_.push_back(baseVi);
            }
        }


        VariableDescriptorIter variableDescriptorsBegin()const{
            return VariableDescriptorIter(0);
        }
        VariableDescriptorIter variableDescriptorsEnd()const{
            return VariableDescriptorIter(nVar_);
        }

        /// \deprecated
        VariableIdIter variableIdsBegin()const{
            return VariableIdIter(0);
        }
        
        /// \deprecated
        VariableIdIter variableIdsEnd()const{
            return VariableIdIter(nVar_);
        }
        Vi nVariables()const{
            return toBaseVarDesc_.size();
        }
        Vi maxVarId()const{
            return nVariables()-1;
        }
        Vi minVarId()const{
            return 0;
        }


        VariableDescriptor fromBaseModelVariableDescriptor(
            const BaseModelVariableDescriptor baseModelVariableDescriptor
        )const{
            return fromBaseVarDesc_[baseModelVariableDescriptor];
        }

        BaseModelVariableDescriptor toBaseModelVariableDescriptor(
            const VariableDescriptor variableDescriptor
        )const{
            return toBaseVarDesc_[variableDescriptor];
        }


        /// \deprecated
        Vi baseViToVi(const Vi baseVi)const{
            return fromBaseVarDesc_[baseVi];
        }

        /// \deprecated
        Vi viToBaseVi(const Vi vi)const{
            return toBaseVarDesc_[vi];
        }


        /// \brief check if a variable of the base model is part of the submodel
        bool inSubmodel(const BaseModelVariableDescriptor baseModelVariableDescriptor)const{
            return fromBaseVarDesc_[baseModelVariableDescriptor] < nVariables() ; 
        }

        /// \deprecated
        bool isInSubmodel(const Vi baseVi)const{
            // \todo buffer max id
            return fromBaseVarDesc_[baseVi] < nVariables() ;
        }

    private:
        typedef typename BASE_MODEL:: template VariableMap<VariableDescriptor> BaseModelViMap;
        Vi notInSubmodelId_;
        BaseModelViMap fromBaseVarDesc_;
        std::vector<BaseModelVariableDescriptor> toBaseVarDesc_;
        Vi nVar_;
    };
    /// \endcond
}









/** \brief view submodel for a few variables.
    
    \tparam BASE_MODEL base-model 
    \tparam BASE_MODEL_FACTORS_OF_VARIABLES factors-of-variables mapping
        w.r.t. the base-model.

    Efficiently generate a submodel w.r.t. a base-model.
    The use case is the following: 
    Most of the variables of the base-model should be fixed
    to a certain label. Only a small portion of the variables
    is not fixed. These non fixed variables are the
    variables of the ViewSubmodel.
    All modifications of the base-models value tables
    are reflected in the ViewSubmodel.

    \warning If the structure of the  base-model changes
    the ViewSubmodel becomes invalid.
    
    \ingroup models discrete_models
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

    typedef detail_view_submodel::VariableMapping<BASE_MODEL> VariableMapping;
    typedef detail_view_submodel::FactorBase<Self> FactorBase;
    typedef detail_view_submodel::FullyIncludedFactor<Self> FullyIncludedFactor;
    typedef detail_view_submodel::PartiallyIncludedFactor<Self> PartiallyIncludedFactor;

    friend class detail_view_submodel::PartiallyIncludedFactor<Self>;
    friend class detail_view_submodel::PartialView<Self>;
public:

    /** \brief container which can store an instance
        of T for any variable id.

        \warning changing the number of the variables in
        this model will invalidate any instance VariableMap.
    */
    template<class T>
    class VariableMap : public std::vector<T>{
    public:
        VariableMap(const ViewSubmodel & m, const T & val)
        : std::vector<T>(m.nVariables(),val){
        }//
        VariableMap(const ViewSubmodel & m)
        : std::vector<T>(m.nVariables()){
        }
    };
    /** \brief container which can store an instance
        of T for any factor id.

        \warning Adding additional factors or changing the number of the factors in
        this model will invalidate any instance FactorMap.
    */
    template<class T>
    class FactorMap : public std::vector<T>{
    public:
        FactorMap(const ViewSubmodel & m, const T & val)
        : std::vector<T>(m.nFactors(),val){
        }
        FactorMap(const ViewSubmodel & m)
        : std::vector<T>(m.nFactors()){
        }
    };


    typedef typename BASE_MODEL::FactorProxy BaseModelFactorProxy;

    typedef const FactorBase * FactorProxy;
    const static bool SortedVariableIds = true;
    const static bool SortedFactorIds = true;


    typedef typename VariableMapping::VariableDescriptorIter VariableDescriptorIter;
    typedef boost::counting_iterator<Fi> FactorDescriptorIter;


    /// \deprecated
    typedef typename VariableMapping::VariableIdIter VariableIdIter;

    /// \ deprecated
    typedef boost::counting_iterator<Fi> FactorIdIter;

    typedef Fi FactorDescriptor;

    typedef typename  VariableMapping::BaseModelVariableDescriptor BaseModelVariableDescriptor;
    typedef typename  VariableMapping::VariableDescriptor VariableDescriptor;



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
        baseModelLabels_(baseModel),
        variableMapping_(baseModel),
        baseFiIncluding_(baseModel, NotIncluded),
        includedFactors_(baseModel.nFactors(),0),
        includedFactorsStorageIndex_(baseModel.nFactors(),0),
        nFac_(0),
        maxArity_(baseModel.maxArity()){

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

        variableMapping_.setSubmodelVariables(baseViIter, baseViEnd);

        #ifdef INFERNO_DO_DEBUG
            const auto dist = std::distance(baseViIter, baseViEnd);
            INFERNO_ASSERT_OP(dist,==,nVariables());
        #endif

        SmallVector<size_t>        notFixedPos(maxArity_);
        SmallVector<size_t>        fixedPos(maxArity_);


        for(Vi vi=0; vi< this->nVariables(); ++vi){
            const Vi baseVi = viToBaseVi(vi);

            // iterate over all factors for baseVi
            for(const auto  baseFi : baseModelFacOfVars_[baseVi]){
                if(baseFiIncluding_[baseFi]==NotIncluded){
                    INFERNO_ASSERT_OP(baseFi,<,baseModel_.maxFactorId()+1);
                    const auto factor = baseModel_.factor(baseFi);
                    const auto arity = factor->arity();
                    // two cases can happen
                    // -factor is partial included 
                    //   (not all vis of factors are included in this model)
                    // -factor is total included
                    //   (all vis are in included  )
                    auto nNotFixed = 0;
                    auto nFixed  = 0;
                    for(size_t a=0; a<arity; ++a){
                        const auto baseVi = factor->variable(a);
                        if(this->isInSubmodel(baseVi)){
                           notFixedPos[nNotFixed] =a;
                           ++nNotFixed;
                        }
                        else{
                            fixedPos[nFixed] = a;
                            ++nFixed;
                        }
                    }
                    INFERNO_ASSERT_OP(nFixed+nNotFixed,==,arity);
                    INFERNO_ASSERT_OP(nFixed,<,arity);
                    INFERNO_ASSERT_OP(nNotFixed,<=,arity);
                    INFERNO_ASSERT_OP(nNotFixed,<=,maxArity_);
                    INFERNO_ASSERT_OP(nNotFixed,>,0);
                    // Factor is fully included
                    if(nNotFixed == arity){
                       baseFiIncluding_[baseFi] = FullyIncluded;
                       includedFactorsStorageIndex_[nFac_] = fullyIncFactors_.size();
                       fullyIncFactors_.push_back(FullyIncludedFactor(*this, baseFi));
                    }
                    // factor is partially included
                    else{
                        baseFiIncluding_[baseFi] = PartialIncluded;

                        // A BIT OF COMPLICATED SETUP
                        const auto indexMappingOffset = indexMapping_.size();
                        const auto fixedVarPosOffset = fixedVarPos_.size();
                        const auto confBufferOffset = confBuffer_.size();
                        
                        for(auto d=0; d<nNotFixed; ++d){
                            indexMapping_.push_back(notFixedPos[d]);
                        }

                        for(auto i=0; i<nFixed; ++i){
                            fixedVarPos_.push_back(fixedPos[i]);
                        }

                        // make possible new allocs
                        for(auto a=0; a<arity; ++a){
                            confBuffer_.push_back(DiscreteLabel());
                        }

                        includedFactorsStorageIndex_[nFac_] = partiallyIncFactors_.size();
                        partiallyIncFactors_.push_back(PartiallyIncludedFactor(*this, baseFi, nNotFixed,
                                                    indexMappingOffset, fixedVarPosOffset, confBufferOffset)
                        );

                    }
                    includedFactors_[nFac_] = baseFi;
                    ++nFac_;
                }
            }
        }

        for(auto & pFac : partiallyIncFactors_)
            pFac.finish();
    }


    VariableIdIter variableDescriptorsBegin()const{
        return variableMapping_.variableDescriptorsBegin();
    }

    VariableIdIter variableDescriptorsEnd()const{
        return variableMapping_.variableDescriptorsEnd();
    }



    /** \brief begin iterator to the variable indices
        of the model
    */
    VariableIdIter variableIdsBegin()const{
        return variableMapping_.variableIdsBegin();
    }

    /** \brief begin iterator to the variable indices
        of the model
    */
    VariableIdIter variableIdsEnd()const{
        return variableMapping_.variableIdsEnd();
    }

    /** \brief number of variables in the model

        <b>Complexity:</b> O(1)

    */
    uint64_t nVariables()const{
        return variableMapping_.nVariables();
    }
    /** \brief get the minimal variable id in the model 
        
        <b>Complexity:</b> O(1)

        \warning 
        - if the graphical model has no variables,
            calling this function will have undefined behavior.

    */
    Vi minVarId()const{
        return variableMapping_.minVarId();
    }
    /** \brief get the maximal variable id in the model

        <b>Complexity:</b> O(1)

        \warning 
        - if the graphical model has no variables,
            calling this function will have undefined behavior.
    */
    Vi maxVarId()const{
        return variableMapping_.maxVarId();
    }


    FactorDescriptorIter factorDescriptorsBegin()const{
        return FactorDescriptorIter(0);
    }
    /** \brief end iterator of the factor indices
        of the model
    */
    FactorDescriptorIter factorDescriptorsEnd()const{
        return FactorDescriptorIter(nFac_);
    }


    /** \brief begin iterator to the factor indices
        of the model
    */
    FactorIdIter factorIdsBegin()const{
        return FactorIdIter(0);
    }
    /** \brief end iterator of the factor indices
        of the model
    */
    FactorIdIter factorIdsEnd()const{
        return FactorIdIter(nFac_);
    }
    /** \brief get the number of labels for a certain
        variable of the model
        
        \param vi : variable id of the variable 
         for which the number of labels is returned.

    */
    LabelType nLabels(const Vi vi)const{
        return baseModel_.nLabels(this->viToBaseVi(vi));
    } 
    /** \brief get the factor for a certain factor index.

        \param fi : factor index
    */
    FactorProxy factor(const Fi fi) const {
        const Fi baseFi = includedFactors_[fi];
        FactorIncluding facIncType = baseFiIncluding_[baseFi];
        INFERNO_ASSERT(facIncType!=NotIncluded);
        if(facIncType==FullyIncluded){
            INFERNO_ASSERT_OP(includedFactorsStorageIndex_[fi],<,fullyIncFactors_.size());
            return & fullyIncFactors_[includedFactorsStorageIndex_[fi]];
        }
        else{
            INFERNO_ASSERT_OP(includedFactorsStorageIndex_[fi],<,partiallyIncFactors_.size());
            return & partiallyIncFactors_[includedFactorsStorageIndex_[fi]];
        }
    }







    VariableDescriptor fromBaseModelVariableDescriptor(
        const BaseModelVariableDescriptor baseModelVariableDescriptor
    )const{
        return variableMapping_.fromBaseModelVariableDescriptor(baseModelVariableDescriptor);
    }

    BaseModelVariableDescriptor toBaseModelVariableDescriptor(
        const VariableDescriptor variableDescriptor
    )const{
        return variableMapping_.toBaseModelVariableDescriptor(variableDescriptor);
    }

    bool inSubmodel(const BaseModelVariableDescriptor baseModelVariableDescriptor)const{
        return variableMapping_.inSubmodel();
    }



    /** \brief get variable index of this
        from a variable index of the base model

        \param baseVi : variable index w.r.t. the base model

        \return : variable index w.r.t. this model

        \warning baseVi  must be included in the submodel,
        otherwise the result is undefined
    */
    Vi baseViToVi(const Vi baseVi)const{
        return variableMapping_.baseViToVi(baseVi);
    }

    /** \brief get variable index of the
        base model from a variable index w.r.t this model

        \param vi : variable index w.r.t. the this model

        \return : variable index w.r.t. base model
    */
    Vi viToBaseVi(const Vi vi)const{
        return variableMapping_.viToBaseVi(vi);
    }

    /** \brief query if a base-model variable
        is in the submodel

        \param baseVi : variable index w.r.t. the base-model
    */
    bool isInSubmodel(const Vi baseVi)const{
        return variableMapping_.isInSubmodel(baseVi);
    }

    /** \brief query if a factor of the base-model
        is not included, partially included or fully included.

        \param baseFi : factor index w.r.t. the base-model
    */
    FactorIncluding baseFiIncluding(const Fi baseFi)const{
        return includedFactors_[baseFi];
    }


    /** \brief get const reference to the base-model
    */
    const BaseModel & baseModel()const{
        return baseModel_;
    }


    /** \brief get the baseModel label map
    */
    BaseModelLabelMap & baseModelLabelMap(){
        return  baseModelLabels_;
    }

    /** \brief compute const term. 

        The constant term is the energy of 
        all factors which are not included
        in the sub-model (not even partially included).
        If the base-model labels change (baseModelLabelMap)
        the constant term will change.
        \warning The const term is recomputed on every call.

    */
    ValueType constTerm()const{
        ValueType constT = 0.0;
        SmallVector<DiscreteLabel> conf(maxArity_);
        for(const auto baseFi : baseModel_.factorDescriptors()){
            if(baseFiIncluding_[baseFi] == NotIncluded){
                const auto baseFactor = baseModel_.factor(baseFi);
                baseFactor->getFactorConf(baseModelLabels_, conf.begin());
                constT += baseFactor->eval(conf.data());
            }
        }
        return constT;
    }


    /// \brief convert factor descriptor into factor id
    ///
    /// For this type of graphical model, factor ids and descriptors
    /// are equivalent
    Fi factorId(const FactorDescriptor factorDescriptor)const{
        return factorDescriptor;
    }

    /// \brief convert variable descriptor into variable id
    ///
    /// For this type of graphical model, variable ids and descriptors
    /// are equivalent
    Vi variableId(const VariableDescriptor variableDescriptor)const{
        return variableDescriptor;
    }

    /// \brief convert factor id into factor descriptor
    ///
    /// For this type of graphical model, factor ids and descriptors
    /// are equivalent
    FactorDescriptor factorDescriptor(const Fi fi)const{
        return fi;
    }

    /// \brief convert factor id into factor descriptor
    ///
    /// For this type of graphical model, factor ids and descriptors
    /// are equivalent
    VariableDescriptor variableDescriptor(const Vi vi)const{
        return vi;
    }



private:
    const BASE_MODEL & baseModel_;
    const BASE_MODEL_FACTORS_OF_VARIABLES & baseModelFacOfVars_;
    BaseModelLabelMap baseModelLabels_;
    VariableMapping variableMapping_;
    
    IncludedFactorMap baseFiIncluding_;
    std::vector<Fi> includedFactors_;
    std::vector<Fi> includedFactorsStorageIndex_;



    std::vector<size_t>        indexMapping_;
    std::vector<size_t>        fixedVarPos_;
    mutable std::vector<DiscreteLabel> confBuffer_;

    Fi nFac_;
    size_t maxArity_;
    

    std::vector<FullyIncludedFactor> fullyIncFactors_;
    std::vector<PartiallyIncludedFactor> partiallyIncFactors_;
};



} // end namespace model
} // end namespace inferno


#endif /* INFERNO_MODEL_VIEW_SUBMODEL_HXX */
