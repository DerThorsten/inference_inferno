/** \file general_discrete_model.hxx 
    \brief  functionality for inferno::models::GeneralDiscreteModel is implemented in this header.
*/
#ifndef INFERNO_MODEL_GENERAL_DISCRETE_MODEL_HXX
#define INFERNO_MODEL_GENERAL_DISCRETE_MODEL_HXX


//boost
#include <boost/iterator/counting_iterator.hpp>

// std

// inferno
#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/value_tables/discrete_unary_value_table_base.hxx"
#include "inferno/model/discrete_factor_base.hxx"
#include "inferno/model/discrete_unary_base.hxx"
#include "inferno/model/discrete_model_base.hxx"
#include "inferno/model/simple_discrete_model_base.hxx"
#include "inferno/model/maps/model_maps.hxx"


namespace inferno{
namespace models{

    
class GeneralDiscreteModel;


/** \brief Factor class for the GeneralDiscreteModel
*/
template<class MODEL>
class GeneralDiscreteGraphicalModelFactor :
public DiscreteFactorBase<GeneralDiscreteGraphicalModelFactor<MODEL>, MODEL> {
public:

    typedef typename MODEL::VariableDescriptor VariableDescriptor;



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

    VariableDescriptor variable(const size_t d)const{
        return model_->facVis(visOffset_, d);
    }

private:
    const MODEL * model_;
    const value_tables::DiscreteValueTableBase * vt_;
    uint64_t visOffset_;
    size_t arity_;

};


/** \brief Factor class for the GeneralDiscreteModel
*/
template<class MODEL>
class GeneralDiscreteGraphicalModelUnary :
    public DiscreteUnaryBase<GeneralDiscreteGraphicalModelUnary<MODEL>, MODEL> {
public:
    typedef DiscreteUnaryBase<GeneralDiscreteGraphicalModelUnary<MODEL>, MODEL > BaseType;
    typedef typename MODEL::VariableDescriptor VariableDescriptor;

    using BaseType::shape;
    using BaseType::variable;

    GeneralDiscreteGraphicalModelUnary()
    :   model_(NULL),
    vt_(NULL),
        var_(){

    }


    GeneralDiscreteGraphicalModelUnary(const MODEL * model ,
                                       const value_tables::DiscreteUnaryValueTableBase * vt,
                                       const VariableDescriptor var)
    :   model_(model),
        vt_(vt),
        var_(var){

    }
    const value_tables::DiscreteUnaryValueTableBase * valueTable()const{
        return vt_;
    }   
    uint32_t arity()const{
        return 1;
    }
    LabelType shape( )const{
        return model_->nLabels(var_);
    }

    VariableDescriptor variable()const{
        return var_;
    }

private:
    const MODEL * model_;
    const value_tables::DiscreteUnaryValueTableBase * vt_;
    VariableDescriptor var_;

};



/** \brief  Very flexible graphical
    model.

    \ingroup models
    \ingroup discrete_models

    \todo move this to the namespace inferno::models
*/
class GeneralDiscreteModel : 
public SimpleDiscreteGraphicalModelBase<GeneralDiscreteModel>{
    typedef GeneralDiscreteModel Self;
    friend class GeneralDiscreteGraphicalModelFactor<Self>;
private:
    typedef  GeneralDiscreteGraphicalModelFactor<Self> Ftype;
    typedef  GeneralDiscreteGraphicalModelUnary<Self> Utype;
public:

    typedef const GeneralDiscreteGraphicalModelUnary<Self> * UnaryProxy;
    typedef UnaryProxy UnaryProxyRef;

    typedef const GeneralDiscreteGraphicalModelFactor<Self> * FactorProxy;
    typedef FactorProxy FactorProxyRef;


    GeneralDiscreteModel(const GeneralDiscreteModel&) = delete;
    GeneralDiscreteModel& operator=(const GeneralDiscreteModel&) = delete;

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

    uint64_t nVariables()const{
        return nVar_;
    }
    uint64_t nFactors()const{
        return factors_.size();
    }
    uint64_t nUnaries()const{
        return unaries_.size();
    } 

    template<class CONFIG>
    double eval(const CONFIG  & conf)const{
        double sum = 0.0;
        for(const auto unary : this->unaries()){
            sum += unary->eval(conf[unary->variable()]);
        }
        switch(maxArity_){
            case 1 :{

                return sum;
            }
            case 2 : {
                for(size_t i=0; i<factors_.size(); ++i){
                    const Ftype & fac = factors_[i];
                    sum+= fac.eval(conf[fac.variable(0)],conf[fac.variable(1)]);
                }
                return sum;
            }
            case 3 : {
                for(size_t i=0; i<factors_.size(); ++i){
                    const Ftype & fac = factors_[i];
                    const uint32_t arity = fac.arity();
                    switch(arity){
                        case 2:
                            sum+= fac.eval(conf[fac.variable(0)],conf[fac.variable(1)]);
                            break;
                        case 3:
                            sum+= fac.eval(conf[fac.variable(0)],conf[fac.variable(1)],conf[fac.variable(2)]);
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



    FactorProxy factor(const FactorDescriptor factorDescriptor)const{
        return &factors_[factorDescriptor];
    }
    UnaryProxy unary(const UnaryDescriptor unaryDescriptor)const{
        return &unaries_[unaryDescriptor];
    }

    LabelType nLabels(const VariableDescriptor variableDescriptor)const{
        return variableDescriptor >= numberOfLabels_.size() ? numberOfLabels_[0] : numberOfLabels_[variableDescriptor]; 
    }

    Vti nValueTables()const{
        return valueTables_.size();
    }

    GeneralDiscreteModel(const uint64_t nVar = 0 , const LabelType nLabes = 0, const bool functionOwner = true)
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
        const uint64_t visOffset = facVarDesc_.size();
        for( ;viBegin!=viEnd; ++viBegin){
            const Vi vi = *viBegin;
            INFERNO_CHECK_OP(nLabels(vi),==,valueTables_[vti]->shape(arity),
                "number of labels does not match value tables shape");
            facVarDesc_.push_back(vi);
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


    uint64_t addUnaryValueTable( value_tables::DiscreteUnaryValueTableBase * uvt){
        unaryValueTables_.push_back(uvt);
        return unaryValueTables_.size()-1;
    }

    uint64_t addUnary(const uint64_t vti , VariableDescriptor var){
        size_t arity=0;
        
        INFERNO_CHECK_OP(nLabels(var),==,unaryValueTables_[vti]->size(),
            "numberOfLabels(var) does not match size of unary value table");
        const auto unary = Utype(this,unaryValueTables_[vti], var);        
        unaries_.push_back(unary);
        maxArity_  = std::max(size_t(1), maxArity_);
        return unaries_.size()-1;
    }


    size_t maxArity()const{
        return maxArity_;
    }
private:

    VariableDescriptor facVis(uint64_t index, const size_t v)const{
        return facVarDesc_[index + v];
    }
    DiscreteLabel facShape(uint64_t index, const size_t v)const{
        return this->nLabels(facVarDesc_[index + v]);
    }

    const uint64_t nVar_;
    std::vector<LabelType>              numberOfLabels_;

    std::vector<value_tables::DiscreteValueTableBase * >  valueTables_;
    std::vector<value_tables::DiscreteUnaryValueTableBase * >  unaryValueTables_;

    std::vector<GeneralDiscreteGraphicalModelFactor<GeneralDiscreteModel> > factors_;
    std::vector<GeneralDiscreteGraphicalModelUnary<GeneralDiscreteModel> > unaries_;

    typedef std::vector<VariableDescriptor> FacVarStorage;
    FacVarStorage facVarDesc_;
    size_t maxArity_;
    bool functionOwner_;

};





} // end namespace inferno::models
} // inferno


#endif 
