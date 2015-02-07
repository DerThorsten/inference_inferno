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
        uint32_t arity()const{
            return baseModelFactor_->arity();
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




    template<class VIEW_SUBMODEL>
    class PartiallyIncludedFactor :  public FactorBase<VIEW_SUBMODEL>
    {
    private:
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
            baseModelFactor_(viewSubmodel.baseModel()[baseFi]),
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
                buffer[p] = viewSubmodel_->baseModelLabels_[baseModelFactor_->vi(p)];
            }
            return baseModelFactor_->eval(buffer);
        }
        virtual Vi shape(const size_t d) const {
            const size_t baseFacD = viewSubmodel_->indexMapping_[indexMappingOffset_ + d];
            return baseModelFactor_->shape(baseFacD);
        }
        virtual Vi vi(const size_t d) const {
            const size_t baseFacD = viewSubmodel_->indexMapping_[indexMappingOffset_ + d];
            return viewSubmodel_->baseViToVi(baseModelFactor_->vi(baseFacD));
        }
        virtual const inferno::value_tables::DiscreteValueTableBase * valueTable()const{
            return &newVt_;
        }
        void finish(){
            newVt_.setFactorPtr(this);
        }
    private:
        const VIEW_SUBMODEL * viewSubmodel_;
        const BaseModelFactorProxy baseModelFactor_;
        const size_t  newArity_;
        const uint64_t indexMappingOffset_;
        const uint64_t fixedVarPosOffset_;
        const uint64_t confBufferOffset_;

        PartialView<PartiallyIncludedFactor<VIEW_SUBMODEL> > newVt_;
    };



    template<class BASE_MODEL>
    class ViMapping{

    public:
        typedef boost::counting_iterator<Vi> VariableIdIter;

        typedef BASE_MODEL BaseModel;
        ViMapping(const BaseModel & baseModel)
        :   notInSubmodelId_(baseModel.maxVarId()+1),
            baseViToVi_(baseModel,notInSubmodelId_),
            viToBaseVi_(){

        }

        template<class BASE_VI_ITER>
        void setSubmodelVariables(
            BASE_VI_ITER baseViIter,
            BASE_VI_ITER baseViEnd
        ){
            viToBaseVi_.resize(0);
            // forward and backward mapping 
            // between vi and baseVi
            nVar_ = 0 ;
            for( ; baseViIter!=baseViEnd; ++baseViIter, ++nVar_){
                const Vi baseVi = *baseViIter;
                baseViToVi_[baseVi] = nVar_;
                viToBaseVi_.push_back(baseVi);
            }
        }

        VariableIdIter variableIdsBegin()const{
            return VariableIdIter(0);
        }

        VariableIdIter variableIdsEnd()const{
            return VariableIdIter(nVar_);
        }
        Vi nVariables()const{
            return viToBaseVi_.size();
        }
        Vi maxVarId()const{
            return nVariables()-1;
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
            return baseViToVi_[baseVi] < nVariables() ;
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

    typedef detail_view_submodel::ViMapping<BASE_MODEL> ViMapping;
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

    typedef typename ViMapping::VariableIdIter VariableIdIter;
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
        baseModelLabels_(baseModel),
        viMapping_(baseModel),
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

        viMapping_.setSubmodelVariables(baseViIter, baseViEnd);

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
                    const auto factor = baseModel_[baseFi];
                    const auto arity = factor->arity();
                    // two cases can happen
                    // -factor is partial included 
                    //   (not all vis of factors are included in this model)
                    // -factor is total included
                    //   (all vis are in included  )
                    auto nNotFixed = 0;
                    auto nFixed  = 0;
                    for(size_t a=0; a<arity; ++a){
                        const auto baseVi = factor->vi(a);
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

                        std::cout<<"add partial with nNotFixed"<<nNotFixed<<"\n";

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

    VariableIdIter variableIdsBegin()const{
        return viMapping_.variableIdsBegin();
    }

    VariableIdIter variableIdsEnd()const{
        return viMapping_.variableIdsEnd();
    }

    uint64_t nVariables()const{
        return viMapping_.nVariables();
    }
    Vi minVarId()const{
        return viMapping_.minVarId();
    }
    Vi maxVarId()const{
        return viMapping_.maxVarId();
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

    /** \brief get variable index of this
        from a variable index of the base model

        \param baseVi : variable index w.r.t. the base model

        \return : variable index w.r.t. this model

        \warning baseVi  must be included in the submodel,
        otherwise the result is undefined
    */
    Vi baseViToVi(const Vi baseVi)const{
        return viMapping_.baseViToVi(baseVi);
    }

    /** \brief get variable index of the
        base model from a variable index w.r.t this model

        \param vi : variable index w.r.t. the this model

        \return : variable index w.r.t. base model
    */
    Vi viToBaseVi(const Vi vi)const{
        return viMapping_.viToBaseVi(vi);
    }

    bool isInSubmodel(const Vi baseVi)const{
        return viMapping_.isInSubmodel(baseVi);
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

    ValueType constTerm()const{
        ValueType constT = 0.0;
        SmallVector<DiscreteLabel> conf(maxArity_);
        for(const auto baseFi : baseModel_.factorIds()){
            if(baseFiIncluding_[baseFi] == NotIncluded){
                const auto baseFactor = baseModel_[baseFi];
                baseFactor->getFactorConf(baseModelLabels_, conf.begin());
                constT += baseFactor->eval(conf.data());
            }
        }
        return constT;
    }

private:
    const BASE_MODEL & baseModel_;
    const BASE_MODEL_FACTORS_OF_VARIABLES & baseModelFacOfVars_;
    BaseModelLabelMap baseModelLabels_;
    ViMapping viMapping_;
    
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
