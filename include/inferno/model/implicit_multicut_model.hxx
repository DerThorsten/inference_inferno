/** \file implicit_multicut_model.hxx 
    \brief  functionality for inferno::ImplicitMulticutModelFactor is implemented in this header.
*/
#ifndef INFERNO_MODEL_IMPLICIT_MULTICUT_MODEL_HXX
#define INFERNO_MODEL_IMPLICIT_MULTICUT_MODEL_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/model/discrete_model_base.hxx"
#include <boost/iterator/counting_iterator.hpp>


namespace inferno{
namespace models{




/** \brief Factor of ImplicitMulticutModel

    This factor stores an instance of PottsValueTabel
    and is generated on the fly from ImplicitMulticutModel.
    Therefore each time ImplicitMulticutModel::operator[]
    is called, a new allocated instance of this class
    is returned.

    \todo move this to the namespace inferno::models
*/
class ImplicitMulticutModelFactor : public DiscreteFactorBase<ImplicitMulticutModelFactor>{
public:
    typedef Vi VariableDescriptor;
    ImplicitMulticutModelFactor(const Vi nLabels = Vi(), const Vi u = Vi(), const Vi v=Vi(), const ValueType beta=ValueType())
    :   u_(u),
        v_(v),
        pottsFunction_(nLabels, beta){
    }

    const value_tables::DiscreteValueTableBase * valueTable()const{
        return &pottsFunction_;
    }   
    size_t arity()const{
        return 2;
    }
    LabelType shape(const size_t d)const{
        return pottsFunction_.shape(d);
    }

    VariableDescriptor variable(const size_t d)const{
        return d==0? u_ : v_; 
    }

private:
    Vi u_,v_;
    value_tables::PottsValueTable pottsFunction_;
};


/** \brief implicit multicut graphical model
    
    \ingroup models
    \ingroup discrete_models
    \ingroup multicut
*/
class ImplicitMulticutModel : 
public DiscreteGraphicalModelBase<ImplicitMulticutModel>{

public:

    typedef uint64_t FactorDescriptor;
    typedef Vi       VariableDescriptor;

    typedef boost::counting_iterator<Fi> FactorDescriptorIter;
    typedef boost::counting_iterator<Vi>       VariableDescriptorIter;
    

    /// \deprecated
    typedef boost::counting_iterator<Fi> FactorIdIter;
    /// \deprecated
    typedef boost::counting_iterator<Vi> VariableIdIter;

    /// \todo rename / re-factor this typedef
    typedef ImplicitMulticutModelFactor FactorProxy;

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
        VariableMap(const ImplicitMulticutModel & m, const T & val)
        : std::vector<T>(m.nVariables(),val){
        }//
        VariableMap(const ImplicitMulticutModel & m)
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
        FactorMap(const ImplicitMulticutModel & m, const T & val)
        : std::vector<T>(m.nFactors(),val){
        }
        FactorMap(const ImplicitMulticutModel & m)
        : std::vector<T>(m.nFactors()){
        }
    };


    ImplicitMulticutModel(const uint64_t nVar)
    :   nVar_(nVar),
        edges_(),
        beta_(){

    }   
    uint64_t addFactor(const Vi u, const Vi v, const ValueType beta){
        edges_.push_back(u);
        edges_.push_back(v);
        beta_.push_back(beta);
        return beta_.size()-1;
    }

    uint64_t maxArity()const{
        return 2;
    }


    FactorDescriptorIter factorDescriptorsBegin()const{
        return FactorDescriptorIter(0);
    }
    FactorDescriptorIter factorDescriptorsEnd()const{
        return FactorDescriptorIter(beta_.size());
    }
    VariableDescriptorIter variableDescriptorsBegin()const{
        return VariableDescriptorIter(0);
    }
    VariableDescriptorIter variableDescriptorsEnd()const{
        return VariableDescriptorIter(nVar_);
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


    /// \deprecated
    FactorIdIter factorIdsBegin()const{
        return FactorIdIter(0);
    }
    /// \deprecated
    FactorIdIter factorIdsEnd()const{
        return FactorIdIter(beta_.size());
    }
    /// \deprecated
    VariableIdIter variableIdsBegin()const{
        return VariableIdIter(0);
    }
    /// \deprecated
    VariableIdIter variableIdsEnd()const{
        return VariableIdIter(nVar_);
    }

    FactorProxy factor(const FactorDescriptor facDesc)const{
        const uint64_t uu = facDesc*2;
        const uint64_t vv = uu+1;
        return FactorProxy(nVar_,edges_[uu],edges_[vv],beta_[uu/2]);
    }

    LabelType nLabels(const uint64_t variabeId)const{
        return LabelType(nVar_); 
    }


private:
    uint64_t nVar_;
    std::vector<Vi> edges_;
    std::vector<ValueType> beta_;


};

} // end namespace inferno::models
} // inferno

#endif 
