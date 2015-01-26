#ifndef INFERNO_MODEL_BASE_MODEL_HXX
#define INFERNO_MODEL_BASE_MODEL_HXX

#include <initializer_list>

#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/utilities/small_vector.hxx"
#include "inferno/model/base_discrete_factor.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{



template<class MODEL>
class DiscreteGraphicalModelBase{
public:

    template<class T>
    double eval(std::initializer_list<T> conf)const{
        return model().eval(conf.begin());
    }

    template<class CONF_ITER>
    double eval(CONF_ITER conf)const{

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
private:
    const MODEL & model()const{
        return * static_cast<const MODEL *>(this);
    }
    MODEL & model(){
        return * static_cast<MODEL *>(this);
    }
};



}

#endif 
