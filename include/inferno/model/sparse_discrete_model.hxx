/** \file sparse_discrete_model.hxx 
    \brief  functionality for inferno::SparseDiscreteGraphicalModel is implemented in this header.
    \todo add namespae
*/
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

/** \brief Factor class for the SparseDiscreteGraphicalModel
*/
class SparseDiscreteGraphicalModelFactor : public DiscreteFactorBase<SparseDiscreteGraphicalModelFactor> {
public:


    SparseDiscreteGraphicalModelFactor()
    :   vis_(),
        vt_(NULL),
        arity_(0){

    }

    //SparseDiscreteGraphicalModelFactor & operator = (const SparseDiscreteGraphicalModelFactor & other){
    //    if(this!= &other){
    //        vis_ = other.vis_;
    //        vt_ = other.vt_;
    //    }
    //    return *this;
    //}

    template<class VI_T>
    SparseDiscreteGraphicalModelFactor(const value_tables::DiscreteValueTableBase * vt,
                   std::initializer_list<VI_T> list )
    :   vis_(list.begin(), list.end()),
        vt_(vt),
        arity_(vt->arity()){

    }

    template<class VI_ITER>
    SparseDiscreteGraphicalModelFactor(const value_tables::DiscreteValueTableBase * vt,
                   const VI_ITER viBegin, 
                   const VI_ITER viEnd)
    :   vis_(viBegin, viEnd),
        vt_(vt),
        arity_(vt->arity()){

            INFERNO_ASSERT_OP(vis_.size(),==,vt_->arity());
    }
    const value_tables::DiscreteValueTableBase * valueTable()const{
        return vt_;
    }   
    size_t arity()const{
        return arity_;
    }
    LabelType shape(const size_t d)const{
        return vt_->shape(d);
    }
    Vi vi(const size_t d)const{
        return vis_[d];
    }

    /// \brief not part of the actual api
    const std::vector<Vi> & visVector()const{
        return vis_;
    }
private:
    std::vector<Vi> vis_;
    const value_tables::DiscreteValueTableBase * vt_;
    size_t arity_;

};



/** \brief  Very flexible  sparse graphical
    model.
    \warning This model is more or less just 
        to test the API.  Really try to stick
        with dense graphical models, this makes
        live a lot easier.

    Sparse means variable and factor ids can be sparse and unsorted
    
    \todo rename in "editable graphical model"
    \todo move this to the namespace inferno::models
    \ingroup models
    \ingroup discrete_models
*/
class SparseDiscreteGraphicalModel : 
public DiscreteGraphicalModelBase<SparseDiscreteGraphicalModel>{

private:
    template<typename T1, typename T2> 
    struct TakeFirst{
        typedef  T1  result_type;
        T1 operator()(const std::pair<T1, T2> & aPair) const
        {
          return  aPair.first;
        }
    };
    typedef std::pair<SparseDiscreteGraphicalModelFactor, Vti>     FacVtiPair; 
    typedef TakeFirst<Vi, FacVtiPair>                               FacStorageKeyAccessor;
    typedef TakeFirst<Vi, DiscreteLabel>                            NlStorageKeyAccessor;

    typedef std::unordered_map<Fi, FacVtiPair>                            FactorStorage;
    typedef std::unordered_map<Vi, DiscreteLabel>                         NlStorage;
public:
   
    typedef boost::transform_iterator<FacStorageKeyAccessor,
        typename FactorStorage::const_iterator> FactorIdIter;

    typedef boost::transform_iterator<NlStorageKeyAccessor,
        typename NlStorage::const_iterator> VariableIdIter;




    typedef const SparseDiscreteGraphicalModelFactor * FactorProxy;
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
        return & it->second.first;
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

    Vti addValueTable(const Vti vti,value_tables::DiscreteValueTableBase * vt){
        valueTables_[vti] = vt;
        return vti;
    }   
    template<class VI_ITER>
    Fi addFactor(const Vti vti, const Fi fi , VI_ITER viBegin, VI_ITER viEnd){
        INFERNO_CHECK(factors_.find(fi)==factors_.end(), "factor already exists");
        INFERNO_CHECK(factorsOfValueTables_[vti].find(fi)==factorsOfValueTables_[vti].end(), "factor already exists in vti's factors");
        factorsOfValueTables_[vti].insert(fi);
        factors_[fi] = FacVtiPair(SparseDiscreteGraphicalModelFactor(valueTables_[vti], viBegin, viEnd),vti);
        for(const auto vi : factors_[fi].first.visVector()){
            factorsOfVariables_[vi].insert(fi);
        }
        return fi;
    }
    template<class VI_T>
    Fi addFactor(const Vti vti,const Fi fi, std::initializer_list<VI_T>  list){
        INFERNO_CHECK(factors_.find(fi)==factors_.end(), "factor already exists");
        INFERNO_CHECK(factorsOfValueTables_[vti].find(fi)==factorsOfValueTables_[vti].end(), "factor already exists in vti's factors");
        factorsOfValueTables_[vti].insert(fi);
        for(const auto vi : list){
            factorsOfVariables_[vi].insert(fi);
        }
        factors_[fi] = FacVtiPair(SparseDiscreteGraphicalModelFactor(valueTables_[vti],list),vti);
        return fi;
    }

    size_t nConnectedFactorsForVti(const Vti vti)const{
        const auto it = factorsOfValueTables_.find(vti);
        INFERNO_CHECK(it!=factorsOfValueTables_.end(),"no such vti");
        return it->second.size();
    }

    size_t nConnectedFactorsForVi(const Vi vi)const{
        const auto it = factorsOfVariables_.find(vi);
        INFERNO_CHECK(it!=factorsOfVariables_.end(),"no such vi");
        return it->second.size();
    }

    Vti factorsVti(const Fi fi)const{
        const auto it = factors_.find(fi);
        INFERNO_CHECK(it!=factors_.end(),"no such fi");
        return it->second.second;
    }

    void eraseVariable(const Vi vi){

        numberOfLabels_.erase(vi);

        // erase all factors which depend on vi
        for(const auto fi : factorsOfVariables_.find(vi)->second){
            
            // get the factor/vti pair
            const auto & facVtiPair = factors_.find(fi)->second;
            const auto vti = facVtiPair.second;

            // get all factors connected to this vti
            // and check how many factors are connected to this vti
            INFERNO_ASSERT(factorsOfValueTables_.find(vti)!=factorsOfVariables_.end());
            auto & vtisFacs = factorsOfValueTables_.find(vti)->second;
            const auto nConnected = vtisFacs.size();


            if(nConnected == 1){
                // only this factor is connected to the vti
                // => the vti can be deleted
                INFERNO_ASSERT_OP(fi,==, (*(vtisFacs.begin())));
                delete valueTables_.find(vti)->second;
                factorsOfValueTables_.erase(vti);

            }
            else{
                // remove fi from factorsOfValueTables_[vti]
                vtisFacs.erase(fi);
                INFERNO_CHECK_OP(vtisFacs.size(),>=,1,"error here");
            }
            // remove the factor
            // all other vars which depend on this factor
            const auto & visVec = factors_[fi].first.visVector();
            for(const auto otherVis : visVec){
                if(otherVis!=vi){
                    factorsOfVariables_[otherVis].erase(fi);
                }
            }
            factors_.erase(fi);
        }
        // remove vi from factorsOfVariables_
        factorsOfVariables_.erase(vi);

    }

private:
    std::unordered_map<Vi, LabelType>                                   numberOfLabels_;
    std::unordered_map<Vti, value_tables::DiscreteValueTableBase * >    valueTables_;
    FactorStorage                                                       factors_;
    

    std::unordered_map<Vi, std::unordered_set<Fi> > factorsOfVariables_;
    std::unordered_map<Vti, std::unordered_set<Fi> > factorsOfValueTables_; 

};





}

#endif 
