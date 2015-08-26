#ifndef INFERNO_MODEL_SIMPLE_DISCRETE_MODEL_BASE_HXX
#define INFERNO_MODEL_SIMPLE_DISCRETE_MODEL_BASE_HXX

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

#include "inferno/model/discrete_model_base.hxx"
#include "inferno/model/maps/map_iterators.hxx"
#include "inferno/model/maps/model_maps.hxx"


namespace inferno{
namespace models{





template<class MODEL>
class SimpleDiscreteGraphicalModelBase : 
    public DiscreteGraphicalModelBase<MODEL>
{
public:

    // 
    typedef Fi       FactorDescriptor;
    typedef Vi       VariableDescriptor;
    typedef Vi       UnaryDescriptor;

    typedef boost::counting_iterator<Vi>       UnaryDescriptorIter;
    typedef boost::counting_iterator<Fi>       FactorDescriptorIter;
    typedef boost::counting_iterator<Vi>       VariableDescriptorIter;


    /** \brief container which can store an instance
    of T for any variable descriptor.

    \warning changing the number of the variables in
    this model will invalidate any instance of this class.
    */
    template<class T>
    class VariableMap : public  maps::SimpleVariableMap<MODEL, T>{
        typedef maps::SimpleVariableMap<MODEL, T> Base;
    public:
        VariableMap()
        :   Base(){
        }
        VariableMap(const MODEL & m, const T & val = T())
        :   Base(m, val){
        }
    };

    /** \brief container which can store an instance
    of T for any factor descriptor.

    \warning changing the number of the variables in
    this model will invalidate any instance of this class.
    */
    template<class T>
    class FactorMap : public  maps::SimpleFactorMap<MODEL, T>{
        typedef maps::SimpleFactorMap<MODEL, T> Base;
    public:
        FactorMap()
        :   Base(){
        }
        FactorMap(const MODEL & m, const T & val = T())
        :   Base(m, val){
        }
    };

    /** \brief container which can store an instance
    of T for any factor descriptor.

    \warning changing the number of the variables in
    this model will invalidate any instance of this class.
    */
    template<class T>
    class UnaryMap : public  maps::SimpleUnaryMap<MODEL, T>{
        typedef maps::SimpleUnaryMap<MODEL, T> Base;
    public:
        UnaryMap()
        :   Base(){
        }
        UnaryMap(const MODEL & m, const T & val = T())
        :   Base(m, val){
        }
    };

    // descriptor iterators
    FactorDescriptorIter factorDescriptorsBegin()const{
        return FactorDescriptorIter(0);
    }
    FactorDescriptorIter factorDescriptorsEnd()const{
        return FactorDescriptorIter(model().nFactors());
    }
    UnaryDescriptorIter unaryDescriptorsBegin()const{
        return UnaryDescriptorIter(0);
    }
    UnaryDescriptorIter unaryDescriptorsEnd()const{
        return UnaryDescriptorIter(model().nUnaries());
    }
    VariableDescriptorIter variableDescriptorsBegin()const{
        return VariableDescriptorIter(0);
    }
    VariableDescriptorIter variableDescriptorsEnd()const{
        return VariableDescriptorIter(model().nVariables());
    }

    // convert between id's and descriptors

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


    Vi minVarId()const{
        return 0;
    }
    Vi maxVarId()const{
        return model().nVariables()-1;
    }
    Fi minFactorId()const{
        return 0;
    }
    Fi maxFactorId()const{
        return model().nFactors()-1;
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





} // end namespace inferno::models
} // end namespace inferno

#endif 
