#ifndef INFERNO_MODEL_SPARSE_DISCRETE_MODEL_HXX
#define INFERNO_MODEL_SPARSE_DISCRETE_MODEL_HXX

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/model/base_discrete_model.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{

template<typename T1, typename T2> 
struct TakeFirst{
    typedef  T1  result_type;
    T1 operator()(const std::pair<T1, T2> & aPair) const
    {
      return  aPair.first;
    }
};


/** \brief  Very flexible  sparse graphical
    model.

    Sparse means variable and factor ids can be sparse and unsorted

    \ingroup models
    \ingroup discrete_models
*/
class SparseDiscreteGraphicalModel : 
public DiscreteGraphicalModelBase<SparseDiscreteGraphicalModel>{

private:
    typedef TakeFirst<Vi, GeneralDiscreteGraphicalModelFactor>  FacStorageKeyAccessor;
    typedef TakeFirst<Vi, DiscreteLabel>                        NlStorageKeyAccessor;

    typedef std::unordered_map<Fi, GeneralDiscreteGraphicalModelFactor>   FactorStorage;
    typedef std::unordered_map<Vi, DiscreteLabel>                         NlStorage;
public:
   
    typedef boost::transform_iterator<FacStorageKeyAccessor,
        typename FactorStorage::const_iterator> FactorIdIter;

    typedef boost::transform_iterator<NlStorageKeyAccessor,
        typename NlStorage::const_iterator> VariableIdIter;




    typedef const GeneralDiscreteGraphicalModelFactor * FactorProxy;
    typedef FactorProxy FactorProxyRef;

    const static bool SortedVariableIds = false;
    const static bool SortedFactorIds = false;

    
    /** \brief container which can store an instance
        of T for any variable id.

        \warning changing the number of the variables in
        this model will invalidate any instance VariableMap.
    */
    template<class T>
    class VariableMap : public std::unordered_map<Vi, T>{
    public:
        VariableMap(const SparseDiscreteGraphicalModel & m, const T & val)
        : std::unordered_map<Vi, T>(){
            for(const auto vi : m.variableIds()){
                this->operator[](vi) = val;
            }
        }//
        VariableMap(const SparseDiscreteGraphicalModel & m)
        : std::unordered_map<Vi, T>(){
        }
    };
    /** \brief container which can store an instance
        of T for any factor id.

        \warning Adding additional factors or changing the number of the factors in
        this model will invalidate any instance FactorMap.
    */
    template<class T>
    class FactorMap : public std::unordered_map<Fi, T>{
    public:
        FactorMap(const SparseDiscreteGraphicalModel & m, const T & val)
        : std::unordered_map<Fi, T>(m.nFactors(),val){
            for(const auto vi : m.factorIds()){
                this->operator[](vi) = val;
            }
        }
        FactorMap(const SparseDiscreteGraphicalModel & m)
        : std::unordered_map<Fi, T>(m.nFactors()){
        }
    };


    FactorIdIter factorIdsBegin()const{
        return FactorIdIter(factors_.begin(), FacStorageKeyAccessor());
    }
    FactorIdIter factorIdsEnd()const{
        return FactorIdIter(factors_.end(), FacStorageKeyAccessor());
    }
    VariableIdIter variableIdsBegin()const{
        return VariableIdIter(numberOfLabels_.begin(), NlStorageKeyAccessor());
    }
    VariableIdIter variableIdsEnd()const{
        return VariableIdIter(numberOfLabels_.end(), NlStorageKeyAccessor());
    }

    FactorProxy operator[](const uint64_t factorId)const{
        const auto it = factors_.find(factorId);
        return & it->second;
    }

    LabelType nLabels(const uint64_t variabeId)const{
        const auto it = numberOfLabels_.find(variabeId);
        return it->second;
    }


    SparseDiscreteGraphicalModel()
    :   numberOfLabels_(),
        valueTables_(),
        factors_()
    {
    }

    void addVariable(const Vi vi, const DiscreteLabel nl){
        numberOfLabels_[vi] = nl;
    }   

    uint64_t addValueTable( value_tables::DiscreteValueTableBase * vt){
        //valueTables_.push_back(vt);
        //return valueTables_.size()-1;
    }   
    template<class VI_ITER>
    uint64_t addFactor(const uint64_t vti , VI_ITER viBegin, VI_ITER viEnd){
        //factors_.push_back(GeneralDiscreteGraphicalModelFactor(valueTables_[vti], viBegin, viEnd));
        //return factors_.size()-1;
    }
    template<class VI_T>
    uint64_t addFactor(const uint64_t vti , std::initializer_list<VI_T>  list){
        //factors_.push_back(GeneralDiscreteGraphicalModelFactor(valueTables_[vti], list));
        //return factors_.size()-1;
    }
private:
    std::unordered_map<Vi, LabelType>                                   numberOfLabels_;
    std::unordered_map<Vi, value_tables::DiscreteValueTableBase * >     valueTables_;
    FactorStorage                                                       factors_;
    

};





}

#endif 
