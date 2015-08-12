/** \file general_discrete_model.hxx 
    \brief  functionality for inferno::models::GeneralDiscreteModel is implemented in this header.
*/
#ifndef INFERNO_MODEL_GENERAL_DISCRETE_MODEL_HXX
#define INFERNO_MODEL_GENERAL_DISCRETE_MODEL_HXX

#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/model/discrete_model_base.hxx"
#include <boost/iterator/counting_iterator.hpp>

namespace inferno{
namespace models{

    
class GeneralDiscreteModel;


/** \brief Factor class for the GeneralDiscreteModel
*/
template<class MODEL>
class GeneralDiscreteGraphicalModelFactor : public DiscreteFactorBase<GeneralDiscreteGraphicalModelFactor<MODEL> > {
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
class GeneralDiscreteGraphicalModelUnary : public DiscreteFactorBase<GeneralDiscreteGraphicalModelUnary<MODEL> > {
public:

    typedef typename MODEL::VariableDescriptor VariableDescriptor;

    GeneralDiscreteGraphicalModelUnary()
    :   model_(NULL),
        vt_(NULL),
        var_(){

    }


    GeneralDiscreteGraphicalModelUnary(const MODEL * model ,
                                             const value_tables::DiscreteValueTableBase * vt,
                                             const VariableDescriptor var)
    :   model_(model),
        vt_(vt),
        var_(var){

    }
    const value_tables::DiscreteValueTableBase * valueTable()const{
        return vt_;
    }   
    uint32_t arity()const{
        return 1;
    }
    LabelType shape(const size_t d)const{
        return model_->nLabels(var_);
    }

    VariableDescriptor variable(const size_t d)const{
        return var_;
    }

private:
    const MODEL * model_;
    const value_tables::DiscreteValueTableBase * vt_;
    VariableDescriptor var_;

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


    typedef Fi       FactorDescriptor;
    typedef Vi       VariableDescriptor;
    typedef Vi       UnaryDescriptor;

    typedef boost::counting_iterator<Vi>       UnaryDescriptorIter;
    typedef boost::counting_iterator<Fi>       FactorDescriptorIter;
    typedef boost::counting_iterator<Vi>       VariableDescriptorIter;


    typedef const GeneralDiscreteGraphicalModelUnary<GeneralDiscreteModel> * UnaryProxy;
    typedef UnaryProxy UnaryProxyRef;

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
        of T for any variable Descriptor.

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
        of T for any factor Descriptor.

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


    /** \brief container which can store an instance
        of T for any unary Descriptor.

        \warning Adding additional factors or changing the number of the factors in
        this model will invalidate any instance UnaryMap.
    */
    template<class T>
    class UnaryMap : public std::vector<T>{
    public:
        UnaryMap(const GeneralDiscreteModel & m, const T & val)
        : std::vector<T>(m.nUnaries(),val){
        }
        UnaryMap(const GeneralDiscreteModel & m)
        : std::vector<T>(m.nUnaries()){
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
                    sum += fac.eval(conf[fac.variable(0)]);
                }
                return sum;
            }
            case 2 : {
                for(size_t i=0; i<factors_.size(); ++i){
                    const Ftype & fac = factors_[i];
                    switch(fac.arity()){
                        case 1:
                            sum+= fac.eval(conf[fac.variable(0)]);
                            break;
                        case 2:
                            sum+= fac.eval(conf[fac.variable(0)],conf[fac.variable(1)]);
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
                            sum+= fac.eval(conf[fac.variable(0)]);
                            break;
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


    // new descriptor iterators
    FactorDescriptorIter factorDescriptorsBegin()const{
        return FactorDescriptorIter(0);
    }
    FactorDescriptorIter factorDescriptorsEnd()const{
        return FactorDescriptorIter(factors_.size());
    }
    UnaryDescriptorIter unaryDescriptorsBegin()const{
        return UnaryDescriptorIter(0);
    }
    UnaryDescriptorIter unaryDescriptorsEnd()const{
        return UnaryDescriptorIter(factors_.size());
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

    size_t nUnaries()const{
        return unaries_.size();
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
