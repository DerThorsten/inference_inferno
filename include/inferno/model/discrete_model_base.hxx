#ifndef INFERNO_MODEL_BASE_MODEL_HXX
#define INFERNO_MODEL_BASE_MODEL_HXX

#include <initializer_list>
#include <iterator>
#include <limits>

#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/utilities/small_vector.hxx"
#include "inferno/learning/weights.hxx"

#include "inferno/model/model_policies.hxx"
#include "inferno/model/discrete_factor_base.hxx"

#include "inferno/model/maps/map_iterators.hxx"


namespace inferno{
namespace models{

    

/// \cond
template<class ITER_TAG, bool IS_SORTED>
struct MinElement;

template<class ITER_TAG>
struct MinElement<ITER_TAG, true>{
    template<class ITER>
    static ITER minElement(ITER begin, ITER end){
        return  begin;
    }
};

template<class ITER_TAG>
struct MinElement<ITER_TAG, false>{
    template<class ITER>
    static ITER minElement(ITER begin, ITER end){
        return  std::min_element(begin, end);
    }
};


template<class ITER_TAG, bool IS_SORTED>
struct MaxElement{
    template<class ITER>
    static ITER maxElement(ITER begin, ITER end){
        return  std::max_element(begin, end);
    }
};


template< >
struct MaxElement<std::random_access_iterator_tag, true>{
    template<class ITER>
    static ITER maxElement(ITER begin, ITER end){
        --end;
        return end;
    }
};

template< >
struct MaxElement<std::bidirectional_iterator_tag, true>{
    template<class ITER>
    static ITER maxElement(ITER begin, ITER end){
        --end;
        return end;
    }
};
/// \endcond







/** \brief Proxy class to use range based loop
    for models variables descriptors
*/
template<class MODEL>
class ModelVariableDescriptors{
public:
    ModelVariableDescriptors(const MODEL & model) 
    :   model_(model){

    }//
    typedef typename MODEL::VariableDescriptorIter const_iterator;
    const_iterator begin()const{
        return model_.variableDescriptorsBegin();
    }
    const_iterator end()const{
        return model_.variableDescriptorsEnd();
    }
private:
    const MODEL & model_;
};

/** \brief Proxy class to use range based loop
    for models factor descriptors
*/
template<class MODEL>
class ModelsFactorDescriptors{
public:
    ModelsFactorDescriptors(const MODEL & model) 
    :   model_(model){

    }//
    typedef typename MODEL::FactorDescriptorIter const_iterator;
    const_iterator begin()const{
        return model_.factorDescriptorsBegin();
    }
    const_iterator end()const{
        return model_.factorDescriptorsEnd();
    }
private:
    const MODEL & model_;
};

/** \brief Proxy class to use range based loop
    for models unary descriptors
*/
template<class MODEL>
class ModelsUnaryDescriptors{
public:
    ModelsUnaryDescriptors(const MODEL & model) 
    :   model_(model){

    }//
    typedef typename MODEL::UnaryDescriptorIter const_iterator;
    const_iterator begin()const{
        return model_.unaryDescriptorsBegin();
    }
    const_iterator end()const{
        return model_.unaryDescriptorsEnd();
    }
private:
    const MODEL & model_;
};

/// \cond
template<class MODEL>
struct FactorDescriptorToId{
    typedef Fi result_type;

    FactorDescriptorToId(const MODEL & model)
    : model_(&model){

    }//
    result_type operator()(const typename MODEL::FactorDescriptor f)const{
        return model_->factorId(f);
    }
    const MODEL * model_;
};

template<class MODEL>
struct VariableDescriptorToId{
    typedef Vi result_type;

    VariableDescriptorToId(const MODEL & model)
    : model_(&model){

    }//
    result_type operator()(const typename MODEL::VariableDescriptor f)const{
        return model_->variableId(f);
    }
    const MODEL * model_;
};
/// \endcond



/// \cond
template<class MODEL>
struct FactorIdToFactorProxy{
    typedef typename MODEL::FactorProxy result_type;

    FactorIdToFactorProxy(const MODEL & model)
    : model_(&model){

    }//
    result_type operator()(const uint64_t fId)const{
        return model_->factor(fId);
    }
    const MODEL * model_;
};

template<class MODEL>
struct FactorDescriptorToFactorProxy{
    typedef typename MODEL::FactorProxy result_type;
    typedef typename MODEL::FactorDescriptor FactorDescriptor;
    FactorDescriptorToFactorProxy(const MODEL & model)
    : model_(&model){

    }//
    result_type operator()(const FactorDescriptor fdesc)const{
        return model_->factor(fdesc);
    }
    const MODEL * model_;
};

template<class MODEL>
struct UnaryDescriptorToUnaryProxy{
    typedef typename MODEL::UnaryProxy result_type;
    typedef typename MODEL::UnaryDescriptor UnaryDescriptor;
    UnaryDescriptorToUnaryProxy(const MODEL & model)
    : model_(&model){

    }//
    result_type operator()(const UnaryDescriptor udesc)const{
        return model_->unary(udesc);
    }
    const MODEL * model_;
};
/// \endcond

/** \brief Proxy class to use range based loop
    for models factor
*/
template<class MODEL>
class ModelsFactors{
public:
    ModelsFactors(const MODEL & model) 
    :   model_(model){

    }//
    typedef typename MODEL::FactorDescriptorIter FactorDescriptorIter;
    typedef FactorDescriptorToFactorProxy<MODEL> UnaryFunction;
    typedef boost::transform_iterator<UnaryFunction, FactorDescriptorIter> const_iterator;

    const_iterator begin()const{
        return boost::make_transform_iterator(model_.factorDescriptorsBegin(), UnaryFunction(model_));
    }
    const_iterator end()const{
        return boost::make_transform_iterator(model_.factorDescriptorsEnd(), UnaryFunction(model_));
    }
private:
    const MODEL & model_;
};

/** \brief Proxy class to use range based loop
    for models unaries
*/
template<class MODEL>
class ModelsUnaries{
public:
    ModelsUnaries(const MODEL & model) 
    :   model_(model){

    }//
    typedef typename MODEL::UnaryDescriptorIter UnaryDescriptorIter;
    typedef UnaryDescriptorToUnaryProxy<MODEL> UnaryFunction;
    typedef boost::transform_iterator<UnaryFunction, UnaryDescriptorIter> const_iterator;

    const_iterator begin()const{
        return boost::make_transform_iterator(model_.unaryDescriptorsBegin(), UnaryFunction(model_));
    }
    const_iterator end()const{
        return boost::make_transform_iterator(model_.unaryDescriptorsEnd(), UnaryFunction(model_));
    }
private:
    const MODEL & model_;
};


/** \brief Base class for any discrete model class
        implemented with the
        <a href="http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern"> CRT-pattern</a>.  

        To create a new discrete model, one has to derive
        from a this class, namely inferno::DiscreteGraphicalModelBase.
        Here we use the  
        <a href="http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern"> 
        curiously recurring template pattern (CRTP) </a>.
        Therefore any class deriving from inferno::DiscreteGraphicalModelBase 
        <b>must</b> implement a minimal API which consists of a few
        typedefs, static members, and member functions.
        In addition to the minimal API, a lot of additional member 
        function <b>can</b> be implemented.
        If these additional functions are not implemented, inferno::DiscreteGraphicalModelBase 
        will provide default implementations.
*/




template<class MODEL>
class DiscreteGraphicalModelBase{
public:


    // POLICIES
    typedef MODEL Model;
    typedef policies::VariableIdsPolicy<Model> VariableIdsPolicy;
    typedef policies::FactorIdsPolicy<Model> FactorIdsPolicy;

    /// \brief get the constant term of the model
    ValueType constTerm()const{
        return static_cast<ValueType>(0);
    }

    /// \brief evaluate the energy of the model for a certain configuration
    template<class CONFIG>
    double eval(const CONFIG  &conf)const{
        
        ValueType sum = 0.0;
        const ArityType maxArity = model().maxArity();
        std::vector<LabelType> confBuffer(maxArity);
        for(const auto factor : model().factors()){
            // get the configuration of the factor
            factor->getFactorConf(conf, confBuffer.begin());
            sum += factor->eval(confBuffer.data());
        }
        for(const auto unary: model().unaries())
            sum += unary->eval(conf[unary->variable()]);

        return sum + model().constTerm();
    }

    /** \brief get the maximum factor order of a model
     
        <b> Complexity:<b> O(|Factors|)

        <b> Derived models should overload this function if
        a better complexity can be achieved</b>


    */
    size_t maxArity()const{
        ArityType maxArity = 0;
        for(auto factor : model().factors()){
            maxArity = std::max(factor->arity(), maxArity);
        }
        return maxArity;
    }

    /** \brief check if all variables have the same number of labels

        <b> Complexity:<b> O(DiscreteGraphicalModelBase::minMaxNLabels) 

        <b> Derived models should overload this function if
        a better complexity can be achieved</b>
    */
    bool hasSimpleLabelSpace(DiscreteLabel & l)const{
        DiscreteLabel  minNumberOfLabels; 
        DiscreteLabel  maxNumberOfLabels;
        model().minMaxNLabels(minNumberOfLabels, maxNumberOfLabels);
        if(minNumberOfLabels == maxNumberOfLabels){
            l = minNumberOfLabels;
            return true;
        }
        return false;
    }


    /** \brief get the minimal and maximal number of labels a 
        variable can take.

        <b> Complexity:<b> O(|Variables|)

        <b> Derived models should overload this function if
        a better complexity can be achieved</b>

        This is not the min. and max. label a variable 
        can take, but the min. and max. <b>number of labels </b>
        which variables can take in the model.
        Therefore it is the min. / max. over
        DiscreteGraphicalModelBase::nLabels(vi) for all variables.

        \warning If the model has no variables, the behavior
        of this function is undefined
    */
    void minMaxNLabels(DiscreteLabel & minNumberOfLabels, DiscreteLabel & maxNumberOfLabels)const{
        minNumberOfLabels = std::numeric_limits<DiscreteLabel>::max();
        maxNumberOfLabels = 0;
        for(const auto var : model().variableDescriptors()){
            const DiscreteLabel lvi = model().nLabels(var);
            minNumberOfLabels = std::min(lvi, minNumberOfLabels);
            maxNumberOfLabels = std::max(lvi, maxNumberOfLabels);
        }   
    }


    /// \brief number of variables in the graphical model
    uint64_t nVariables() const {
        return std::distance(model().variableDescriptorsBegin(), model().variableDescriptorsEnd());
    }

    /// \brief number of factors in the graphical model
    uint64_t nFactors() const {
        return std::distance(model().factorDescriptorsBegin(), model().factorDescriptorsEnd());
    }

    /// \brief number of factors in the graphical model
    uint64_t nUnaries() const {
        return std::distance(model().unaryDescriptorsBegin(), model().unaryDescriptorsEnd());
    }



    /** \brief get the minimal variable id in the model

        \warning 
        - if the graphical model has no variables,
            calling this function will have undefined behavior.
        - if VariableIdsPolicy::HasSortedIds is false, 
          this default implementation is O(N)

    */
    Vi minVarId() const{
        typedef typename Model::VariableDescriptorIter Iter;
        typedef VariableDescriptorToId<Model> UnaryFunction;
        typedef boost::transform_iterator<UnaryFunction, Iter> IdIter;
        typedef typename std::iterator_traits<IdIter>::iterator_category  IterTag;
        const auto & m  = this->model();
        auto begin = boost::make_transform_iterator(m.variableDescriptorsBegin(), UnaryFunction(m));
        auto end   = boost::make_transform_iterator(m.variableDescriptorsEnd(), UnaryFunction(m));
        return * MinElement<IterTag, VariableIdsPolicy::HasSortedIds>::minElement(begin,end);
    }
    /** \brief get the maximal variable id in the model

        \warning 
        - if the graphical model has no variables,
            calling this function will have undefined behavior.
        - if VariableIdsPolicy::HasSortedIds is false, 
          this default implementation is O(N)
    */
    Vi maxVarId() const{
        typedef typename Model::VariableDescriptorIter Iter;
        typedef VariableDescriptorToId<Model> UnaryFunction;
        typedef boost::transform_iterator<UnaryFunction, Iter> IdIter;
        typedef typename std::iterator_traits<IdIter>::iterator_category  IterTag;
        const auto & m  = this->model();
        auto begin = boost::make_transform_iterator(m.variableDescriptorsBegin(), UnaryFunction(m));
        auto end   = boost::make_transform_iterator(m.variableDescriptorsEnd(), UnaryFunction(m));
        return * MaxElement<IterTag, VariableIdsPolicy::HasSortedIds>::maxElement(begin,end);
    }


    /** \brief get the minimal factor id in the model

        \warning if the graphical model has no variables,
        calling this function will have undefined behavior.
    */
    Fi minFactorId() const{
        typedef typename Model::FactorDescriptorIter Iter;
        typedef FactorDescriptorToId<Model> UnaryFunction;
        typedef boost::transform_iterator<UnaryFunction, Iter> IdIter;
        typedef typename std::iterator_traits<IdIter>::iterator_category  IterTag;
        const auto & m  = this->model();
        auto begin = boost::make_transform_iterator(m.factorDescriptorsBegin(), UnaryFunction(m));
        auto end   = boost::make_transform_iterator(m.factorDescriptorsEnd(), UnaryFunction(m));
        return * MinElement<IterTag, FactorIdsPolicy::HasSortedIds>::minElement(begin,end);
    }
    /** \brief get the maximal variable id in the model

        \warning if the graphical model has no factors,
        calling this function will have undefined behavior.
    */
    Fi maxFactorId() const{
        typedef typename Model::FactorDescriptorIter Iter;
        typedef FactorDescriptorToId<Model> UnaryFunction;
        typedef boost::transform_iterator<UnaryFunction, Iter> IdIter;
        typedef typename std::iterator_traits<IdIter>::iterator_category  IterTag;
        const auto & m  = this->model();
        auto begin = boost::make_transform_iterator(m.factorDescriptorsBegin(), UnaryFunction(m));
        auto end   = boost::make_transform_iterator(m.factorDescriptorsEnd(), UnaryFunction(m));
        return * MaxElement<IterTag, FactorIdsPolicy::HasSortedIds>::maxElement(begin,end);
    }

    /** check if the variables are dense
        
        \todo fixme!!!!

        dense means the variables can start at 
        any id, but from there on the ids must be
        consecutive without holes.

        \warning if the graphical model has no variables,
        calling this function will have undefined behavior.
    */
    bool denseVariableIds()const{
        if(model().nVariables()<=1)
            return true;
        else{
            const Vi minVarId = *model().variableIdsBegin();
            const Vi maxVarId =  model().variableIdsBegin()[nVariables()-1];
            return (maxVarId-minVarId)+1 == model().nVariables() ;
        }
    }



    /// \brief check if the model has any unary at all
    bool hasUnaries() const {
        const auto b = model().unaryDescriptorsBegin();
        const auto e = model().unaryDescriptorsEnd();
        return b != e;
    }

    /// \brief the number of second order factors in
    /// the graphical model
    ///
    /// this function should be overloaded by 
    /// all models where this can be computed
    /// efficiently
    Fi nPairwiseFactors()const{
        Fi nPairwise = 0;

        for(const auto factor : model().factors()){
            if(factor->arity()==2)
                ++nPairwise;
        }
        return nPairwise;
    }

    ModelVariableDescriptors<MODEL> variableDescriptors() const{
        return ModelVariableDescriptors<MODEL>(model());
    }
    ModelsFactorDescriptors<MODEL> factorDescriptors() const{
        return ModelsFactorDescriptors<MODEL>(model());
    }
    ModelsUnaryDescriptors<MODEL> unaryDescriptors() const{
        return ModelsUnaryDescriptors<MODEL>(model());
    }
    ModelsFactors<MODEL> factors() const{
        return ModelsFactors<MODEL>(model());
    }
    ModelsUnaries<MODEL> unaries() const{
        return ModelsUnaries<MODEL>(model());
    }

    /// at least for one semantic class allow cuts within must 
    /// be true.
    /// At least one unary must be present
    bool isSecondOrderModifiedMultiwaycutModel(std::vector<bool> & allowCutsInSemanticClass) const{
        // model must have an arity of 2
        if( model().maxArity() == 2){
            DiscreteLabel nLabels;
            // first simple check
            // - do all variables have the same number of labels
            if(model().hasSimpleLabelSpace(nLabels)){
                const Vi nVar = model().nVariables();

                // only if nLabels >= nVar we can have a 
                // valid mmwc model
                if(nLabels>nVar){
                    // check if we have unaries at all
                    if(model().hasUnaries()){

                        allowCutsInSemanticClass.clear();

                        // get the number of semantic classes
                        // where cuts within is allowed
                        const auto nSemanticClassesWithCuts= nLabels / nVar;
                        const auto nSemanticClassesWithoutCuts = nLabels - nSemanticClassesWithCuts;

                        allowCutsInSemanticClass.resize(0);
                        allowCutsInSemanticClass.reserve(nSemanticClassesWithCuts+nSemanticClassesWithoutCuts);

                        for(auto i=0; i<nSemanticClassesWithoutCuts; ++i)
                            allowCutsInSemanticClass.push_back(false);
                        for(auto i=0; i<nSemanticClassesWithCuts; ++i)
                            allowCutsInSemanticClass.push_back(true);

                        // check the actual factors
                        for(const auto factor : model().factors()){
                            const auto arity = factor.arity();
                            if(arity==1){

                            }
                            else{ // (arity =2 )
                                if(!factor->isPotts(ValueType()))
                                    return false;
                            }
                        }
                    }
                }
            }
        }
        return false;  
    }


    void guessAllowCutsWithin(std::vector<bool> & allowCutsInSemanticClass) const{
        DiscreteLabel nLabels;
        if(model().hasSimpleLabelSpace(nLabels)){
            allowCutsInSemanticClass.clear();
            // get the number of semantic classes
            // where cuts within is allowed
            const auto nSemanticClassesWithCuts= nLabels / model().nVariables();
            const auto nSemanticClassesWithoutCuts = nLabels - nSemanticClassesWithCuts*model().nVariables();

            allowCutsInSemanticClass.resize(0);
            allowCutsInSemanticClass.reserve(nSemanticClassesWithCuts + nSemanticClassesWithoutCuts);

            for(auto i=0; i<nSemanticClassesWithoutCuts; ++i)
                allowCutsInSemanticClass.push_back(false);
            for(auto i=0; i<nSemanticClassesWithCuts; ++i)
                allowCutsInSemanticClass.push_back(true);
        }
        else{
            allowCutsInSemanticClass.resize(0);
        }
    }


    bool isSecondOrderMulticutModel()const{
        const auto m = model();
        if(!m.hasUnaries()){
            DiscreteLabel nl;
            if(m.hasSimpleLabelSpace(nl)){
                if(nl==m.nVariables()){
                    if(m.maxArity()==2){
                        for(auto factor : m.factors()){
                            ValueType beta;
                            if(!factor->isPotts(beta))
                                return false;
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    }
    bool isMulticutModel()const{
        const auto m = model();
        if(!m.hasUnaries()){
            DiscreteLabel nl;
            if(m.hasSimpleLabelSpace(nl)){
                if(nl==m.nVariables()){
                    for(auto factor : m.factors()){
                        if(!factor->isGeneralizedPotts())
                            return false;
                    }
                    return true;
                }
            }
        }
        return false;
    }
    
        
    void updateWeights(const learning::WeightVector & weights)const{ 
        for(const auto factor : model().factors())
            factor->updateWeights(weights);
        for(const auto unary: model().unaries())
            unary->updateWeights(weights);
    }

    template<class CONF>
    void accumulateJointFeatures(
        learning::WeightVector & accumulatedFeatures,
        CONF & conf
    )const{
        

        for(const auto unary : model().unaries()){
            const DiscreteLabel label = conf[unary->variable()];
            const auto vt = unary->valueTable();
            for(auto wi :vt->weightIndices())
                accumulatedFeatures[wi.global()] += vt->weightGradient(wi.local(),&label);
        }

        std::vector<DiscreteLabel> factorConf(this->maxArity());
        for(const auto factor : model().factors()){
            factor->getFactorConf(conf, factorConf.begin());
            const auto vt = factor->valueTable();
            for(auto wi :vt->weightIndices())
                accumulatedFeatures[wi.global()] += vt->weightGradient(wi.local(),factorConf.data());
        }

    }

    template<class MAP>
    typename MapIterTypeHelper<MODEL,MAP>::ConstVariableMapIterRange
    variableMapIter(const MAP & map)  const{
        return inferno::models::variableMapIter(  this->constModel(),map);
    }


private:
   
    const MODEL & constModel()const{
        return * static_cast<const MODEL *>(this);
    }
    const MODEL & model()const{
        return * static_cast<const MODEL *>(this);
    }
    MODEL & model(){
        return * static_cast<MODEL *>(this);
    }
};


template<class MODEL>
class FactorsOfVariable{
};




template<class MODEL, bool HAS_DENSE_IDS>
class DenseVariableIdsImpl;

template<class MODEL>
class DenseVariableIdsImpl<MODEL, true>
{
public:
    typedef typename MODEL::VariableDescriptor VariableDescriptor;
    DenseVariableIdsImpl(const MODEL & model)
    : model_(model){
    };

    VariableDescriptor toDescriptor(const Vi denseVi)const{
        return model_.variableDescritor(denseVi);
    }

    Vi toDenseId(const VariableDescriptor var)const{
        return model_.variableId(var);
    }
private:
    const MODEL & model_;
};

template<class MODEL>
class DenseVariableIdsImpl<MODEL, false>
{
public:
    typedef typename MODEL::VariableDescriptor VariableDescriptor;
    typedef MODEL Model;
    DenseVariableIdsImpl(const Model & model)
    :   denseViToDescriptor_(model.nVariables()),
        descriptorToDenseVi_(model)
    {
        Vi denseVi = 0;
        for(const auto var : model.variableDescriptors()){
            const auto vi = model.variableId(var);
            denseViToDescriptor_[denseVi] = var;
            descriptorToDenseVi_[var] = denseVi;
            ++denseVi;
        }
    };

    VariableDescriptor toDescriptor(const Vi denseVi)const{
        INFERNO_ASSERT_OP(denseVi, <, denseViToDescriptor_.size());
        return denseViToDescriptor_[denseVi];
    }

    Vi toDenseId(const VariableDescriptor var)const{
        return descriptorToDenseVi_[var];
    }
private:
    std::vector<VariableDescriptor> denseViToDescriptor_;
    typename  Model:: template VariableMap<Vi> descriptorToDenseVi_;
};



template<class MODEL>
class DenseVariableIds : public DenseVariableIdsImpl<MODEL, MODEL::VariableIdsPolicy::HasDenseIds>
{
public:
    typedef MODEL Model;
    DenseVariableIds(const Model & model)
    : DenseVariableIdsImpl<Model, Model::VariableIdsPolicy::HasDenseIds>(model){        
    }
};






} // end namespace inferno::models
} // end namespace inferno

#endif 
