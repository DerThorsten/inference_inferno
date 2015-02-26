/** \file implicit_Potts_model.hxx 
    \brief  functionality for inferno::ImplicitPottsModelFactor is implemented in this header.
*/
#ifndef INFERNO_MODEL_IMPLICIT_POTTS_MODEL_HXX
#define INFERNO_MODEL_IMPLICIT_POTTS_MODEL_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/model/base_discrete_model.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{
namespace models{


namespace detail_implicit_potts_model{

    class  HybridFunction : public inferno::value_tables::DiscreteValueTableBase{
    public:
        HybridFunction(const DiscreteLabel nl,
                       const uint8_t arity, 
                       const ValueType * data)
        :   inferno::value_tables::DiscreteValueTableBase(),
            nl_(nl),
            arity_(arity),
            data_(data){
        } 
        ValueType eval(const LabelType *conf)const{
            if(arity_==1)
                return data_[conf[0]];
            else{
                return  conf[0] == conf[1] ? 0.0 : *data_;
            }
        }
        ValueType eval1(const LabelType l0)const{
            return data_[l0];
        }
        ValueType eval2(const LabelType l0, const LabelType l1)const{
            return l0==l1 ? 0 : *data_;
        }
        LabelType shape(const uint32_t d) const{
            return nl_;
        }
        uint32_t  arity()const{
            return arity_;
        }
        bool isGeneralizedPotts() const{
            return arity_ == 2 ? true : false;   
        }
        bool isPotts(ValueType & beta) const{
            beta = data_[0];
            return arity_ == 2 ? true : false;
        }
    private:
        const DiscreteLabel nl_;
        const uint8_t arity_;
        const ValueType * data_;
    };   
}


/** \brief Factor of ImplicitPottsModel

    This factor stores an instance of PottsValueTabel
    and is generated on the fly from ImplicitPottsModel.
    Therefore each time ImplicitPottsModel::operator[]
    is called, a new allocated instance of this class
    is returned.
*/
class ImplicitPottsModelFactor : public DiscreteFactorBase<ImplicitPottsModelFactor>{
public:
    ImplicitPottsModelFactor(const DiscreteLabel nl,
                             const uint8_t arity, 
                             const ValueType * data,
                             const Vi u,
                             const Vi v = Vi())
    :   u_(u),
        v_(v),
        hybridFunction_(nl, arity, data){
    }

    const value_tables::DiscreteValueTableBase * valueTable()const{
        return &hybridFunction_;
    }   
    size_t arity()const{
        return hybridFunction_.arity();
    }
    ValueType eval(const LabelType *conf)const{
        hybridFunction_.eval(conf);
    }
    ValueType eval1(const LabelType l0)const{
        hybridFunction_.eval1(l0);
    }
    ValueType eval2(const LabelType l0, const LabelType l1)const{
        hybridFunction_.eval2(l0, l1);
    }
    LabelType shape(const size_t d)const{
        return hybridFunction_.shape(d);
    }
    Vi vi(const size_t d)const{
        return d==0? u_ : v_;
    }
    bool isGeneralizedPotts() const{
        return hybridFunction_.isGeneralizedPotts(); 
    }
    bool isPotts(ValueType & beta) const{
        return hybridFunction_.isPotts(beta);
    }
private:
    Vi u_,v_;
    detail_implicit_potts_model::HybridFunction hybridFunction_;
};


/** \brief implicit Potts graphical model
    
    \ingroup models
    \ingroup discrete_models
    \ingroup Potts
*/
class ImplicitPottsModel : 
public DiscreteGraphicalModelBase<ImplicitPottsModel>{

public:

    const static bool SortedVariableIds = true;
    const static bool SortedFactorIds = true;

    typedef boost::counting_iterator<uint64_t> FactorIdIter;
    typedef boost::counting_iterator<Vi> VariableIdIter;

    /// \todo rename / re-factor this typedef
    typedef ImplicitPottsModelFactor FactorProxy;

    /// \todo rename / re-factor this typedef
    typedef const FactorProxy & FactorProxyRef;

    /** \brief container which can store an instance
        of T for any variable id.

        \warning changing the number of the variables in
        this model will invalidate any instance VariableMap.
    */
    template<class T>
    class VariableMap : public std::vector<T>{
    public:
        VariableMap(const ImplicitPottsModel & m, const T & val)
        : std::vector<T>(m.nVariables(),val){
        }//
        VariableMap(const ImplicitPottsModel & m)
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
        FactorMap(const ImplicitPottsModel & m, const T & val)
        : std::vector<T>(m.nFactors(),val){
        }
        FactorMap(const ImplicitPottsModel & m)
        : std::vector<T>(m.nFactors()){
        }
    };


    ImplicitPottsModel(const uint64_t nVar, const DiscreteLabel nl)
    :   nVar_(nVar),
        nl_(nl),
        edges_(),
        beta_(),
        unaries_(nVar*nl, 0.0){
    }   
    template<class VAL_ITER>
    uint64_t addUnaryFactor(const Vi u,VAL_ITER valBegin, VAL_ITER valEnd){
        std::copy(valBegin, valEnd, unaries_.begin() + u*nl_ );
        return u;
    }

    uint64_t addPottsFactor(const Vi u, const Vi v, const ValueType beta){
        edges_.push_back(u);
        edges_.push_back(v);
        beta_.push_back(beta);
        return nVar_ + beta_.size()-1;
    }

    template<class CONF>
    ValueType eval(const CONF & conf)const{
        ValueType val = 0.0;
        for(Vi vi=0; vi<nVar_; ++vi){
            //if(vi%10==0)
            //    std::cout<<"vi "<<vi<<"\n";
            val += unaries_[(vi*nl_) +conf[vi]];
        }
        for(size_t e=0; e<beta_.size(); ++e){
            const Vi uu = e*2;
            const Vi vv = uu+1;
            //if(e%10==0)
            //    std::cout<<"e "<<e<<"\n";
            if(conf[edges_[uu]] != conf[edges_[vv]]){
                val += beta_[e];
            }
        }
        //std::cout<<"RES "<<val<<"\n";
        return val;
    }

    uint64_t maxArity()const{
        return beta_.empty() ? 1 : 2 ;
    }

    FactorIdIter factorIdsBegin()const{
        return FactorIdIter(0);
    }
    FactorIdIter factorIdsEnd()const{
        return FactorIdIter(nVar_ + beta_.size());
    }
    VariableIdIter variableIdsBegin()const{
        return VariableIdIter(0);
    }
    VariableIdIter variableIdsEnd()const{
        return VariableIdIter(nVar_);
    }

    FactorProxy operator[](const Fi factorId)const{
        // unary
        if(factorId<nVar_){
            const size_t offset =  factorId*nl_;
            return FactorProxy(nl_, 1,unaries_.data() + offset, factorId);
        }
        // 2 order potts
        else{
            const size_t edgeIndex = factorId - nVar_;
            const Vi uu = edgeIndex*2;
            const Vi vv = uu+1;
            return FactorProxy(nl_, 2,beta_.data() + edgeIndex, edges_[uu],edges_[vv]);
        }
    }

    LabelType nLabels(const uint64_t variabeId)const{
        return nl_; 
    }


private:
    uint64_t nVar_;
    DiscreteLabel nl_;
    std::vector<Vi> edges_;
    std::vector<ValueType> beta_;
    std::vector<ValueType> unaries_;

};



} // end namespace models
} // end namespace inferno

#endif 
