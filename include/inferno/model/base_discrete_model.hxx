#ifndef INFERNO_MODEL_BASE_MODEL_HXX
#define INFERNO_MODEL_BASE_MODEL_HXX

#include <initializer_list>

#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/utilities/small_vector.hxx"
#include "inferno/model/base_discrete_factor.hxx"



namespace inferno{


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





template<class MODEL>
class DiscreteGraphicalModelBase{
public:
        
    typedef FactorIdToFactorProxy<MODEL> U; 






    template<class T>
    double eval(std::initializer_list<T> conf)const{
        return model().eval(conf.begin());
    }//




    template<class CONFIG>
    double eval(CONFIG conf)const{



        double sum = 0.0;

        const size_t maxArity = model().maxArity();
        SmallVector<LabelType> confBuffer(maxArity);

        auto fiter = model().factorIdsBegin();
        auto fiterEnd = model().factorIdsEnd();
        for(;fiter!=fiterEnd; ++fiter){
            const auto factorId = *fiter;
            auto factor = model()[factorId];
            
            // get the configuration of the factor
            factor->getFactorConf(conf, confBuffer.begin());

            sum += factor->eval(confBuffer.data());
        }
        return sum;
    }

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

    /// \brief number of variables in the graphical model
    size_t nVariables() const {
        return std::distance(model().variableIdsBegin(), model().variableIdsEnd());
    }

    /// \brief number of factors in the graphical model
    size_t nFactors() const {
        return std::distance(model().factorIdsBegin(), model().factorIdsEnd());
    }

    /** \brief get the minimal variable id in the model

        \warning if the graphical model has no variables,
        calling this function will have undefined behavior.
    */
    Vi minVarId() const{
        return *model().variableIdsBegin();
    }
    /** \brief get the maximal variable id in the model

        \warning if the graphical model has no variables,
        calling this function will have undefined behavior.
    */
    Vi maxVarId() const{
        return * (model().variableIdsBegin() + (model.numberOfVariables()-1));
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

    /// \brief the number of second order factors in
    /// the graphical model
    ///
    /// this function should be overloaded by 
    /// all models where this can be computed
    /// efficiently
    size_t nPairwiseFactors()const{
        size_t nPairwise = 0;
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
    
public:

private:

};


}

#endif 
