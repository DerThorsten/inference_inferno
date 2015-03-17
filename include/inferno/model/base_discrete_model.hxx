#ifndef INFERNO_MODEL_BASE_MODEL_HXX
#define INFERNO_MODEL_BASE_MODEL_HXX

#include <initializer_list>
#include <iterator>
#include <limits>

#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/utilities/small_vector.hxx"
#include "inferno/model/model_policies.hxx"
#include "inferno/model/base_discrete_factor.hxx"



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
    for models variables ids
*/
template<class MODEL>
class ModelsVariableIds{
public:
    ModelsVariableIds(const MODEL & model) 
    :   model_(model){

    }//
    typedef typename MODEL::VariableIdIter const_iterator;
    const_iterator begin()const{
        return model_.variableIdsBegin();
    }
    const_iterator end()const{
        return model_.variableIdsEnd();
    }
private:
    const MODEL & model_;
};

/** \brief Proxy class to use range based loop
    for models factor ids
*/
template<class MODEL>
class ModelsFactorIds{
public:
    ModelsFactorIds(const MODEL & model) 
    :   model_(model){

    }//
    typedef typename MODEL::FactorIdIter const_iterator;
    const_iterator begin()const{
        return model_.factorIdsBegin();
    }
    const_iterator end()const{
        return model_.factorIdsEnd();
    }
private:
    const MODEL & model_;
};



/// \cond
template<class MODEL>
struct FactorIdToFactorProxy{
    typedef typename MODEL::FactorProxy result_type;

    FactorIdToFactorProxy(const MODEL & model)
    : model_(&model){

    }//
    result_type operator()(const uint64_t fId)const{
        return model_->operator[](fId);
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
    typedef typename MODEL::FactorIdIter FactorIdIter;
    typedef FactorIdToFactorProxy<MODEL> UnaryFunction;
    typedef boost::transform_iterator<UnaryFunction, FactorIdIter> const_iterator;

    const_iterator begin()const{
        return boost::make_transform_iterator(model_.factorIdsBegin(), UnaryFunction(model_));
    }
    const_iterator end()const{
        return boost::make_transform_iterator(model_.factorIdsEnd(), UnaryFunction(model_));
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
    typedef policies::VariableIdsPolicy<MODEL> VariableIdsPolicy;
    typedef policies::FactorIdsPolicy<MODEL> FactorIdsPolicy;


    typedef FactorIdToFactorProxy<MODEL> U; 



    /// \brief evaluate the energy of the model for a certain configuration
    template<class CONFIG>
    double eval(const CONFIG  &conf)const{

        std::cout<<"is DENSE "<<VariableIdsPolicy::HasDenseIds<<"\n";

        double sum = 0.0;
        const size_t maxArity = model().maxArity();
        std::vector<LabelType> confBuffer(maxArity);
        auto fiter = model().factorIdsBegin();
        auto fiterEnd = model().factorIdsEnd();
        for(;fiter!=fiterEnd; ++fiter){
            const auto factorId = *fiter;
            const auto factor = model()[factorId];
            
            // get the configuration of the factor
            factor->getFactorConf(conf, confBuffer.begin());

            sum += factor->eval(confBuffer.data());
        }
        return sum;
    }

    /** \brief get the maximum factor order of a model
     
        <b> Complexity:<b> O(|Factors|)

        <b> Derived models should overload this function if
        a better complexity can be achieved</b>


    */
    size_t maxArity()const{
        size_t maxArity = 0;
        auto fiter = model().factorIdsBegin();
        auto fiterEnd = model().factorIdsEnd();
        for(;fiter!=fiterEnd; ++fiter){
            auto factorId = *fiter;
            auto factor = model()[factorId];
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
        for(const auto vi : model().variableIds()){
            const DiscreteLabel lvi = model().nLabels(vi);
            minNumberOfLabels = std::min(lvi, minNumberOfLabels);
            maxNumberOfLabels = std::max(lvi, maxNumberOfLabels);
        }   
    }


    /// \brief number of variables in the graphical model
    uint64_t nVariables() const {
        return std::distance(model().variableIdsBegin(), model().variableIdsEnd());
    }

    /// \brief number of factors in the graphical model
    uint64_t nFactors() const {
        return std::distance(model().factorIdsBegin(), model().factorIdsEnd());
    }

    /** \brief get the minimal variable id in the model

        \warning 
        - if the graphical model has no variables,
            calling this function will have undefined behavior.
        - if VariableIdsPolicy::HasSortedIds is false, 
          this default implementation is O(N)

    */
    Vi minVarId() const{
        typedef typename std::iterator_traits<typename MODEL::VariableIdIter>::iterator_category  IterTag;
        return * MinElement<IterTag, VariableIdsPolicy::HasSortedIds>::minElement(
            model().variableIdsBegin(), model().variableIdsEnd()
        );
    }
    /** \brief get the maximal variable id in the model

        \warning 
        - if the graphical model has no variables,
            calling this function will have undefined behavior.
        - if VariableIdsPolicy::HasSortedIds is false, 
          this default implementation is O(N)
    */
    Vi maxVarId() const{
        typedef typename std::iterator_traits<typename MODEL::VariableIdIter>::iterator_category  IterTag;
        return * MaxElement<IterTag, VariableIdsPolicy::HasSortedIds>::maxElement(
            model().variableIdsBegin(), model().variableIdsEnd()
        );
    }


    /** \brief get the minimal factor id in the model

        \warning if the graphical model has no variables,
        calling this function will have undefined behavior.
    */
    Fi minFactorId() const{
        typedef typename std::iterator_traits<typename MODEL::FactorIdIter>::iterator_category  IterTag;
        return * MinElement<IterTag, FactorIdsPolicy::HasSortedIds >::minElement(
            model().factorIdsBegin(), model().factorIdsEnd()
        );
    }
    /** \brief get the maximal variable id in the model

        \warning if the graphical model has no factors,
        calling this function will have undefined behavior.
    */
    Fi maxFactorId() const{
        typedef typename std::iterator_traits<typename MODEL::FactorIdIter>::iterator_category  IterTag;
        return * MaxElement<IterTag, FactorIdsPolicy::HasSortedIds >::maxElement(
            model().factorIdsBegin(), model().factorIdsEnd()
        );
    }

    /** check if the variables are dense
    
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

    /// \brief the number of unary factors in
    /// the graphical model
    ///
    /// this function should be overloaded by 
    /// all models where this can be computed
    /// efficiently
    Fi nUnaryFactors()const{
        Fi nUnarys = 0;
        auto fiter = model().factorIdsBegin();
        auto fiterEnd = model().factorIdsEnd();
        for(;fiter!=fiterEnd; ++fiter){
            auto factorId = *fiter;
            auto factor = model()[factorId];
            if(factor->arity()==1){
                ++nUnarys;
            }
        }
        return nUnarys;
    }

    /// \brief check if the model has any unary at all
    bool hasUnaries() const {
        for(const auto factor : model().factors())
            if(factor->arity() == true)
                return true;
        return false;
    }

    /// \brief the number of second order factors in
    /// the graphical model
    ///
    /// this function should be overloaded by 
    /// all models where this can be computed
    /// efficiently
    Fi nPairwiseFactors()const{
        Fi nPairwise = 0;
        auto fiter = model().factorIdsBegin();
        auto fiterEnd = model().factorIdsEnd();
        for(;fiter!=fiterEnd; ++fiter){
            auto factorId = *fiter;
            auto factor = model()[factorId];
            if(factor->arity()==2){
                ++nPairwise;
            }
        }
        return nPairwise;
    }

    ModelsVariableIds<MODEL> variableIds() const{
        return ModelsVariableIds<MODEL>(model());
    }
    ModelsFactorIds<MODEL> factorIds() const{
        return ModelsFactorIds<MODEL>(model());
    }
    ModelsFactors<MODEL> factors() const{
        return ModelsFactors<MODEL>(model());
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
                                if(!factor->isPotts(ValueType())){
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;  
    }

private:
   

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
    DenseVariableIdsImpl(const MODEL & model){
    };

    Vi toSparse(const Vi denseVi)const{
        return denseVi;
    }

    Vi toDense(const Vi sparseId)const{
        return sparseId;
    }
};

template<class MODEL>
class DenseVariableIdsImpl<MODEL, false>
{
public:
    typedef MODEL Model;
    DenseVariableIdsImpl(const Model & model)
    :   denseToSparse_(model.nVariables()),
        sparseToDense_(model)
    {
        Vi denseVi = 0;
        for(Vi sparseVi : model.variableIds()){
            denseToSparse_[denseVi] = sparseVi;
            sparseToDense_[sparseVi] = denseVi;
            ++denseVi;
        }
    };

    Vi toSparse(const Vi denseVi)const{
        INFERNO_ASSERT_OP(denseVi, <, denseToSparse_.size());
        return denseToSparse_[denseVi];
    }

    Vi toDense(const Vi sparseId)const{
        return sparseToDense_[sparseId];
    }
private:
    std::vector<Vi> denseToSparse_;
    typename  Model:: template VariableMap<Vi> sparseToDense_;
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
