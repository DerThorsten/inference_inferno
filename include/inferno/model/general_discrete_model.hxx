/** \file general_discrete_model.hxx 
    \brief  functionality for inferno::GeneralDiscreteGraphicalModel is implemented in this header.
*/
#ifndef INFERNO_MODEL_GENERAL_DISCRETE_MODEL_HXX
#define INFERNO_MODEL_GENERAL_DISCRETE_MODEL_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/model/base_discrete_model.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{


/** \brief Factor class for the GeneralDiscreteGraphicalModel
*/
class GeneralDiscreteGraphicalModelFactor : public DiscreteFactorBase<GeneralDiscreteGraphicalModelFactor> {
public:


    GeneralDiscreteGraphicalModelFactor()
    :   vis_(),
        vt_(NULL),
        arity_(0){

    }

    //GeneralDiscreteGraphicalModelFactor & operator = (const GeneralDiscreteGraphicalModelFactor & other){
    //    if(this!= &other){
    //        vis_ = other.vis_;
    //        vt_ = other.vt_;
    //    }
    //    return *this;
    //}

    template<class VI_T>
    GeneralDiscreteGraphicalModelFactor(const value_tables::DiscreteValueTableBase * vt,
                   std::initializer_list<VI_T> list )
    :   vis_(list),
        vt_(vt),
        arity_(vt->arity()){

    }

    template<class VI_ITER>
    GeneralDiscreteGraphicalModelFactor(const value_tables::DiscreteValueTableBase * vt,
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



/** \brief  Very flexible graphical
    model.

    \ingroup models
    \ingroup discrete_models

    \todo move this to the namespace inferno::models
*/
class GeneralDiscreteGraphicalModel : 
public DiscreteGraphicalModelBase<GeneralDiscreteGraphicalModel>{


public:
    typedef boost::counting_iterator<uint64_t> FactorIdIter;
    typedef boost::counting_iterator<Vi> VariableIdIter;
    typedef const GeneralDiscreteGraphicalModelFactor * FactorProxy;
    typedef FactorProxy FactorProxyRef;

    const static bool SortedVariableIds = true;
    const static bool SortedFactorIds = true;

    /** \brief destructor for model

        \warning this will delete all value
        table have been added.
    */
    ~GeneralDiscreteGraphicalModel(){
        for(auto vt : valueTables_)
            delete vt;
    }


    /** \brief container which can store an instance
        of T for any variable id.

        \warning changing the number of the variables in
        this model will invalidate any instance VariableMap.
    */
    template<class T>
    class VariableMap : public std::vector<T>{
    public:
        VariableMap(const GeneralDiscreteGraphicalModel & m, const T & val)
        : std::vector<T>(m.nVariables(),val){
        }//
        VariableMap(const GeneralDiscreteGraphicalModel & m)
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
        FactorMap(const GeneralDiscreteGraphicalModel & m, const T & val)
        : std::vector<T>(m.nFactors(),val){
        }
        FactorMap(const GeneralDiscreteGraphicalModel & m)
        : std::vector<T>(m.nFactors()){
        }
    };


    FactorIdIter factorIdsBegin()const{
        return FactorIdIter(0);
    }
    FactorIdIter factorIdsEnd()const{
        return FactorIdIter(factors_.size());
    }
    VariableIdIter variableIdsBegin()const{
        return VariableIdIter(0);
    }
    VariableIdIter variableIdsEnd()const{
        return VariableIdIter(nVar_);
    }

    FactorProxy operator[](const uint64_t factorId)const{
        return &factors_[factorId];
    }

    LabelType nLabels(const uint64_t variabeId)const{
        return variabeId >= numberOfLabels_.size() ? numberOfLabels_[0] : numberOfLabels_[variabeId]; 
    }


    GeneralDiscreteGraphicalModel(const uint64_t nVar, const LabelType nLabes)
    :   nVar_(nVar),
        numberOfLabels_(1, nLabes),
        valueTables_(),
        factors_(),
        maxArity_(0){
    }
    uint64_t addValueTable( value_tables::DiscreteValueTableBase * vt){
        valueTables_.push_back(vt);
        return valueTables_.size()-1;
    }   
    template<class VI_ITER>
    uint64_t addFactor(const uint64_t vti , VI_ITER viBegin, VI_ITER viEnd){
        factors_.push_back(GeneralDiscreteGraphicalModelFactor(valueTables_[vti], viBegin, viEnd));
        maxArity_  = std::max(factors_.back()->arity(), maxArity_);
        return factors_.size()-1;
    }
    template<class VI_T>
    uint64_t addFactor(const uint64_t vti , std::initializer_list<VI_T>  list){
        factors_.push_back(GeneralDiscreteGraphicalModelFactor(valueTables_[vti], list));
        maxArity_  = std::max(factors_.back()->arity(), maxArity_);
        return factors_.size()-1;
    }
    size_t maxArity()const{
        return maxArity_;
    }
private:
    const uint64_t nVar_;
    std::vector<LabelType>              numberOfLabels_;
    std::vector<value_tables::DiscreteValueTableBase * >  valueTables_;
    std::vector<GeneralDiscreteGraphicalModelFactor>         factors_;
    size_t maxArity_;

};





}

#endif 
