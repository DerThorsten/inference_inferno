/** \file general_discrete_model.hxx 
    \brief  functionality for inferno::models::GeneralDiscreteModel is implemented in this header.
*/
#ifndef INFERNO_MODEL_GENERAL_DISCRETE_MODEL_HXX
#define INFERNO_MODEL_GENERAL_DISCRETE_MODEL_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/model/base_discrete_model.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{
namespace models{

    
class GeneralDiscreteModel;


/** \brief Factor class for the GeneralDiscreteModel
*/
template<class MODEL>
class GeneralDiscreteGraphicalModelFactor : public DiscreteFactorBase<GeneralDiscreteGraphicalModelFactor<MODEL> > {
public:


    GeneralDiscreteGraphicalModelFactor()
    :   model_(NULL),
        vt_(NULL),
        arity_(0),
        visOffset_(0){

    }






    GeneralDiscreteGraphicalModelFactor(const MODEL * model ,
                                        const value_tables::DiscreteValueTableBase * vt,
                                        const uint64_t visOffset, 
                                        const size_t arity)
    :   model_(model),
        vt_(vt),
        visOffset_(visOffset),
        arity_(arity){

    }
    const value_tables::DiscreteValueTableBase * valueTable()const{
        return vt_;
    }   
    uint32_t arity()const{
        return arity_;
    }
    LabelType shape(const size_t d)const{
        return model_->facShape(visOffset_, d);
    }

    Vi vi(const size_t d)const{
        return model_->facVis(visOffset_, d);
    }

private:
    const MODEL * model_;
    const value_tables::DiscreteValueTableBase * vt_;
    uint64_t visOffset_;
    size_t arity_;

};



/** \brief  Very flexible graphical
    model.

    \ingroup models
    \ingroup discrete_models

    \todo move this to the namespace inferno::models
*/
class GeneralDiscreteModel : 
public DiscreteGraphicalModelBase<GeneralDiscreteModel>{

    friend class GeneralDiscreteGraphicalModelFactor<GeneralDiscreteModel>;
private:
    typedef  GeneralDiscreteGraphicalModelFactor<GeneralDiscreteModel> Ftype;
public:
    typedef boost::counting_iterator<uint64_t> FactorIdIter;
    typedef boost::counting_iterator<Vi> VariableIdIter;
    typedef const GeneralDiscreteGraphicalModelFactor<GeneralDiscreteModel> * FactorProxy;
    typedef FactorProxy FactorProxyRef;


    /** \brief destructor for model

        \warning this will delete all value
        table have been added.
    */
    ~GeneralDiscreteModel(){
        if(functionOwner_){
            for(auto vt : valueTables_)
                delete vt;
        }
    }


    /** \brief container which can store an instance
        of T for any variable id.

        \warning changing the number of the variables in
        this model will invalidate any instance VariableMap.
    */
    template<class T>
    class VariableMap : public std::vector<T>{
    public:
        VariableMap(const GeneralDiscreteModel & m, const T & val)
        : std::vector<T>(m.nVariables(),val){
        }//
        VariableMap(const GeneralDiscreteModel & m)
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
        FactorMap(const GeneralDiscreteModel & m, const T & val)
        : std::vector<T>(m.nFactors(),val){
        }
        FactorMap(const GeneralDiscreteModel & m)
        : std::vector<T>(m.nFactors()){
        }
    };

    /*
    template<class CONFIG>
    double eval(const CONFIG  &conf)const{
        double sum = 0.0;
        std::vector<LabelType> confBuffer(maxArity_);
        for(size_t i=0; i<factors_.size(); ++i){
            const Ftype & fac = factors_[i];
            // get the configuration of the factor
            fac.getFactorConf(conf, confBuffer.begin());
            sum += fac.eval(confBuffer.data());
        }
        return sum;
    }
    */

    template<class CONFIG>
    double eval(const CONFIG  & conf)const{
        double sum = 0.0;
        switch(maxArity_){
            case 1 :{
                for(size_t i=0; i<factors_.size(); ++i){
                    const Ftype & fac = factors_[i];
                    sum += fac.eval1(conf[fac.vi(0)]);
                }
                return sum;
            }
            case 2 : {
                for(size_t i=0; i<factors_.size(); ++i){
                    const Ftype & fac = factors_[i];
                    switch(fac.arity()){
                        case 1:
                            sum+= fac.eval1(conf[fac.vi(0)]);
                            break;
                        case 2:
                            sum+= fac.eval2(conf[fac.vi(0)],conf[fac.vi(1)]);
                            break;
                    }
                }
                return sum;
            }
            case 3 : {
                for(size_t i=0; i<factors_.size(); ++i){
                    const Ftype & fac = factors_[i];
                    const uint32_t arity = fac.arity();
                    switch(arity){
                        case 1:
                            sum+= fac.eval1(conf[fac.vi(0)]);
                            break;
                        case 2:
                            sum+= fac.eval2(conf[fac.vi(0)],conf[fac.vi(1)]);
                            break;
                        case 3:
                            sum+= fac.eval3(conf[fac.vi(0)],conf[fac.vi(1)],conf[fac.vi(2)]);
                            break;
                    }
                }
                return sum;
            }
            default : {
                std::vector<DiscreteLabel> confBuffer(maxArity_);
                for(size_t i=0; i<factors_.size(); ++i){
                    const Ftype & fac = factors_[i];
                    fac.getFactorConf(conf, confBuffer.begin());
                    sum += fac.eval(confBuffer.data());
                }
                return sum;
            }
        }
    }




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

    Vti nValueTables()const{
        return valueTables_.size();
    }

    GeneralDiscreteModel(const uint64_t nVar, const LabelType nLabes, const bool functionOwner = true)
    :   nVar_(nVar),
        numberOfLabels_(1, nLabes),
        valueTables_(),
        factors_(),
        maxArity_(0),
        functionOwner_(functionOwner){
    }
    uint64_t addValueTable( value_tables::DiscreteValueTableBase * vt){
        valueTables_.push_back(vt);
        return valueTables_.size()-1;
    }   
    template<class VI_ITER>
    uint64_t addFactor(const uint64_t vti , VI_ITER viBegin, VI_ITER viEnd){
        size_t arity=0;
        const uint64_t visOffset = facVis_.size();
        for( ;viBegin!=viEnd; ++viBegin){
            const Vi vi = *viBegin;
            INFERNO_CHECK_OP(nLabels(vi),==,valueTables_[vti]->shape(arity),
                "number of labels does not match value tables shape");
            facVis_.push_back(vi);
            ++arity;
        }
        INFERNO_CHECK_OP(arity,==,valueTables_[vti]->arity(),
            "dist(viBegin,viEnd) does not match vt's arity");
        factors_.push_back(GeneralDiscreteGraphicalModelFactor<GeneralDiscreteModel>(this,valueTables_[vti], visOffset, arity));
        maxArity_  = std::max(arity, maxArity_);
        return factors_.size()-1;
    }
    template<class VI_T>
    uint64_t addFactor(const uint64_t vti , std::initializer_list<VI_T>  list){
        return this->addFactor(vti, list.begin(), list.end());
    }
    size_t maxArity()const{
        return maxArity_;
    }
private:

    Vi facVis(uint64_t index, const size_t v)const{
        return facVis_[index + v];
    }
    DiscreteLabel facShape(uint64_t index, const size_t v)const{
        return this->nLabels(facVis_[index + v]);
    }

    const uint64_t nVar_;
    std::vector<LabelType>              numberOfLabels_;
    std::vector<value_tables::DiscreteValueTableBase * >  valueTables_;
    std::vector<GeneralDiscreteGraphicalModelFactor<GeneralDiscreteModel> >         factors_;
    std::vector<Vi> facVis_;
    size_t maxArity_;
    bool functionOwner_;

};





} // end namespace inferno::models
} // inferno


#endif 
