#ifndef INFERNO_MODEL_STRUCTURE_VIEW_MODEL_BASE_HXX
#define INFERNO_MODEL_STRUCTURE_VIEW_MODEL_BASE_HXX

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








template<class MODEL, class BASE_MODEL>
class StructureViewModelBase : 
    public DiscreteGraphicalModelBase<MODEL>
{
public:

    typedef BASE_MODEL BaseModel;
    typedef typename BaseModel::FactorDescriptor     FactorDescriptor;
    typedef typename BaseModel::VariableDescriptor   VariableDescriptor;
    typedef typename BaseModel::UnaryDescriptor      UnaryDescriptor;
    typedef typename BaseModel::ConstraintDescriptor ConstraintDescriptor;

    typedef typename BaseModel::FactorDescriptorIter    FactorDescriptorIter;
    typedef typename BaseModel::VariableDescriptorIter  VariableDescriptorIter;
    typedef typename BaseModel::UnaryDescriptorIter     UnaryDescriptorIter;
    typedef typename BaseModel::ConstraintDescriptorIter     ConstraintDescriptorIter;

    /** \brief container which can store an instance
    of T for any variable descriptor.

    \warning changing the number of the variables in
    this model will invalidate any instance of this class.
    */
    template<class T>
    class VariableMap {
    private:
        typedef typename BASE_MODEL:: template VariableMap<T> BaseModelMap;

        typedef typename BaseModelMap::value_type        value_type;
        typedef typename BaseModelMap::reference         reference;
        typedef typename BaseModelMap::const_reference   const_reference;

    public:
        VariableMap()
        :   model_(nullptr),
            map_(){
        }
        VariableMap(const MODEL & m, const T & val = T())
        :   model_(&m),
            map_(m.baseModel(), val){
        }

        void assign(const MODEL & m, const T & val = T()){
            model_ = &m;
            map_.assign(m.baseModel(), val);
        }

        const MODEL & model()const{
            return *model_;
        }
        template<class D>
        reference operator[](const D & d){
            return map_[d];
        }

        template<class D>
        const_reference operator[](const D & d)const{
            return map_[d];
        }

    private:
        const MODEL * model_;
        BaseModelMap map_;
    };

    template<class T>
    class FactorMap {
    private:
        typedef typename BASE_MODEL:: template FactorMap<T> BaseModelMap;

        typedef typename BaseModelMap::value_type        value_type;
        typedef typename BaseModelMap::reference         reference;
        typedef typename BaseModelMap::const_reference   const_reference;

    public:
        FactorMap()
        :   model_(nullptr),
            map_(){
        }
        FactorMap(const MODEL & m, const T & val = T())
        :   model_(&m),
            map_(m.baseModel(), val){
        }

        void assign(const MODEL & m, const T & val = T()){
            model_ = &m;
            map_.assign(m.baseModel(), val);
        }

        const MODEL & model()const{
            return *model_;
        }
        template<class D>
        reference operator[](const D & d){
            return map_[d];
        }

        template<class D>
        const_reference operator[](const D & d)const{
            return map_[d];
        }

    private:
        const MODEL * model_;
        BaseModelMap map_;
    };

    template<class T>
    class UnaryMap {
    private:
        typedef typename BASE_MODEL:: template UnaryMap<T> BaseModelMap;

        typedef typename BaseModelMap::value_type        value_type;
        typedef typename BaseModelMap::reference         reference;
        typedef typename BaseModelMap::const_reference   const_reference;

    public:
        UnaryMap()
        :   model_(nullptr),
            map_(){
        }
        UnaryMap(const MODEL & m, const T & val = T())
        :   model_(&m),
            map_(m.baseModel(), val){
        }

        void assign(const MODEL & m, const T & val = T()){
            model_ = &m;
            map_.assign(m.baseModel(), val);
        }

        const MODEL & model()const{
            return *model_;
        }
        template<class D>
        reference operator[](const D & d){
            return map_[d];
        }

        template<class D>
        const_reference operator[](const D & d)const{
            return map_[d];
        }

    private:
        const MODEL * model_;
        BaseModelMap map_;
    };

    template<class T>
    class ConstraintMap {
    private:
        typedef typename BASE_MODEL:: template ConstraintMap<T> BaseModelMap;

        typedef typename BaseModelMap::value_type        value_type;
        typedef typename BaseModelMap::reference         reference;
        typedef typename BaseModelMap::const_reference   const_reference;

    public:
        ConstraintMap()
        :   model_(nullptr),
            map_(){
        }
        ConstraintMap(const MODEL & m, const T & val = T())
        :   model_(&m),
            map_(m.baseModel(), val){
        }

        void assign(const MODEL & m, const T & val = T()){
            model_ = &m;
            map_.assign(m.baseModel(), val);
        }

        const MODEL & model()const{
            return *model_;
        }
        template<class D>
        reference operator[](const D & d){
            return map_[d];
        }

        template<class D>
        const_reference operator[](const D & d)const{
            return map_[d];
        }

    private:
        const MODEL * model_;
        BaseModelMap map_;
    };



    DiscreteLabel nLabels(const VariableDescriptor var)const{
        return model().baseModel().nLabels(var);
    }

    // descriptor iterators
    FactorDescriptorIter factorDescriptorsBegin()const{
        return model().baseModel().factorDescriptorsBegin();
    }
    FactorDescriptorIter factorDescriptorsEnd()const{
        return model().baseModel().factorDescriptorsEnd();
    }
    UnaryDescriptorIter unaryDescriptorsBegin()const{
        return model().baseModel().unaryDescriptorsBegin();
    }
    UnaryDescriptorIter unaryDescriptorsEnd()const{
        return model().baseModel().unaryDescriptorsEnd();
    }
    VariableDescriptorIter variableDescriptorsBegin()const{
        return model().baseModel().variableDescriptorsBegin();
    }
    VariableDescriptorIter variableDescriptorsEnd()const{
        return model().baseModel().variableDescriptorsEnd();
    }

    ConstraintDescriptorIter constraintDescriptorsBegin()const{
        return model().baseModel().constraintDescriptorsBegin();
    }
    ConstraintDescriptorIter constraintDescriptorsEnd()const{
        return model().baseModel().constraintDescriptorsEnd();
    }

    // convert between id's and descriptors

    /// \brief convert factor descriptor into factor id
    ///
    /// For this type of graphical model, factor ids and descriptors
    /// are equivalent
    Fi factorId(const FactorDescriptor factorDescriptor)const{
        return model().baseModel().factorId(factorDescriptor);
    }

    /// \brief convert variable descriptor into variable id
    ///
    /// For this type of graphical model, variable ids and descriptors
    /// are equivalent
    Vi variableId(const VariableDescriptor variableDescriptor)const{
        return model().baseModel().variableId(variableDescriptor);
    }


    /// \brief convert constraint descriptor into constraint id
    ///
    /// For this type of graphical model, constraint ids and descriptors
    /// are equivalent
    Ci constraintId(const ConstraintDescriptor constraintDescriptor)const{
        return model().baseModel().constraintId(constraintDescriptor);
    }

    /// \brief convert factor id into factor descriptor
    ///
    /// For this type of graphical model, factor ids and descriptors
    /// are equivalent
    FactorDescriptor factorDescriptor(const Fi fi)const{
        return model().baseModel().factorDescriptor(fi);
    }

    /// \brief convert factor id into factor descriptor
    ///
    /// For this type of graphical model, factor ids and descriptors
    /// are equivalent
    VariableDescriptor variableDescriptor(const Vi vi)const{
        return model().baseModel().variableDescriptor(vi);
    }

    /// \brief convert factor id into factor descriptor
    ///
    /// For this type of graphical model, factor ids and descriptors
    /// are equivalent
    ConstraintDescriptor constraintDescriptor(const Ci ci)const{
        return model().baseModel().constraintDescriptor(ci);
    }

    uint64_t nVariables()const{
        return model().baseModel().nVariables();
    }
    uint64_t nFactors()const{
        return model().baseModel().nFactors();
    }
    uint64_t nUnaries()const{
        return model().baseModel().nUnaries();
    }
    Vi minVarId()const{
        return model().baseModel().minVarId();
    }
    Vi maxVarId()const{
        return model().baseModel().maxVarId();
    }
    Fi minFactorId()const{
        return model().baseModel().minFactorId();
    }
    Fi maxFactorId()const{
        return model().baseModel().maxFactorId();
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
