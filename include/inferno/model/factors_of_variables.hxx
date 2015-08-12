/** \file factors_of_variables.hxx 
    \brief  default implementation of inferno::FactorsOfVariables is implemented
    within this header
*/
#ifndef INFERNO_MODEL_FACTORS_OF_VARIABLES_HXX
#define INFERNO_MODEL_FACTORS_OF_VARIABLES_HXX

#include <initializer_list>

#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "inferno/inferno.hxx"
#include "inferno/utilities/vector_set.hxx"


namespace inferno{
namespace models{


/** \brief Container one can access
    the variables which are connected to a 
    certain variable.
    This relation is stored for all 
    variables within this container.

    \warning It is highly recommended that
    any model should specialize this call,
    if this relation is already part
    of the model, or can be computed on the fly
    without any overhead.

*/
template<class MODEL>
class VariablesNeighbours{

public:
    typedef MODEL Model;
    typedef typename Model::VariableDescriptor VariableDescriptor;
    typedef typename Model::FactorDescriptor FactorDescriptor;
    typedef VectorSet<VariableDescriptor> VariableNeighbours;

    VariablesNeighbours(const Model & model)
    : storage_(model){
        for(const auto fac : model.factorDescriptors()){
            const auto factor = model[fac];
            const auto arity = factor->arity();
            for(uint32_t va=0; va<arity-1; ++va){
                const auto varA = factor->variable(va);
                for(uint32_t vb=va+1; vb<arity; ++vb){
                    const auto varB = factor->variable(vb);
                    storage_[varA].insert(varB);
                    storage_[varB].insert(varA);
                }
            }
        };
    };

    const VariableNeighbours & operator[](const VariableDescriptor varDesc)const{
        return storage_[varDesc];
    }
    
    const VariableNeighbours & operator[](const VariableDescriptor varDesc){
        return storage_[varDesc];
    }


private:
    typedef typename Model:: template VariableMap<VariableNeighbours>  Storage;
    Storage storage_;
};





/** \brief Container one can access
    the factors which are connected to a 
    certain variable.
    This relation is stored for all 
    variables within this container.

    \warning It is highly recommended that
    any model should specialize this call,
    if this relation is already part
    of the model, or can be computed on the fly
    without any overhead.

*/
template<class MODEL>
class FactorsOfVariables{

public:
    typedef MODEL Model;
    typedef typename Model::VariableDescriptor VariableDescriptor;
    typedef typename Model::FactorDescriptor FactorDescriptor;
    
    typedef VectorSet<FactorDescriptor> FiSet;
    typedef FiSet FactorsOfVariable;

    FactorsOfVariables(const Model & model)
    : storage_(model){
        for(const auto facDesc : model.factorDescriptors()){
            const auto factor = model.factor(facDesc);
            const auto arity = factor->arity();
            for(auto v=0; v<arity; ++v){
                storage_[factor->variable(v)].insert(facDesc);
            };
        };
    };

    const FactorsOfVariable & operator[](const VariableDescriptor var)const{
        return storage_[var];
    }
    
    const FactorsOfVariable & operator[](const VariableDescriptor var){
        return storage_[var];
    }


private:
    typedef typename Model:: template VariableMap<FactorsOfVariable>  Storage;

    Storage storage_;
};

/** \brief Container one can access
    the factors which are connected to a 
    certain variable.
    This relation is stored for all 
    variables within this container.

    \warning It is highly recommended that
    any model should specialize this call,
    if this relation is already part
    of the model, or can be computed on the fly
    without any overhead.

*/
template<class MODEL>
class HigherOrderAndUnaryFactorsOfVariables{

public:
    typedef MODEL Model;
    typedef typename Model::VariableDescriptor VariableDescriptor;
    typedef typename Model::FactorDescriptor FactorDescriptor;

    typedef VectorSet<FactorDescriptor> FiSet;
    typedef FiSet HigherOrderFactorsOfVariable;
    typedef FiSet UnaryFactorsOfVariable;

    struct Facs{
    public:
        void insert(const FactorDescriptor facDesc, const uint32_t arity){
            if(arity == 1){
                unaryFactors_.insert(facDesc);
            }
            else{
                higherOrderFactors_.insert(facDesc);
            }
        }
        const UnaryFactorsOfVariable & unaryFactors()const{
            return unaryFactors_;
        }
        const HigherOrderFactorsOfVariable & higherOrderFactors()const{
            return higherOrderFactors_;
        }

    private:
        UnaryFactorsOfVariable unaryFactors_;
        HigherOrderFactorsOfVariable higherOrderFactors_;
    };

    HigherOrderAndUnaryFactorsOfVariables(const Model & model)
    : storage_(model){
        for(const auto facDesc : model.factorDescriptors()){
            const auto factor = model[facDesc];
            const auto fi = model.factorId(facDesc);
            const auto arity = factor->arity();
            for(auto v=0; v<arity; ++v){
                storage_[factor->variable(v)].insert(facDesc, arity);
            };
        };
    };

    const Facs & operator[](const VariableDescriptor var)const{
        return storage_[var];
    }
    
    const Facs & operator[](const VariableDescriptor var){
        return storage_[var];
    }


private:
    typedef typename Model:: template VariableMap<Facs>  Storage;

    Storage storage_;
};


/** \brief Data-structure to access all factors
    which are connected to a set of variables.

    The set of variables can be increased
    by calling addVariables, and reseted
    whith reset.
    
    This class has been tailor made for
    the Movemaker class.
*/
template<class MODEL>
class FactorsOfMultipleVariables{
private:
    typedef MODEL Model;
    typedef typename Model::VariableDescriptor VariableDescriptor;
    typedef typename Model::FactorDescriptor FactorDescriptor;
    typedef HigherOrderAndUnaryFactorsOfVariables<Model> FactorsOfVars;
    typedef typename Model:: template FactorMap<unsigned char> UsedFac;
    typedef typename std::vector<FactorDescriptor>::const_iterator const_iterator;

public:
    FactorsOfMultipleVariables(const Model & model, 
                               const FactorsOfVariables<Model> & factorsOfVariables)
    :   factorsOfVariables_(factorsOfVariables),
        facToRecomp_(model.nFactors()),
        usedFac_(model, 0),
        nFactors_(0){

    }

    void reset(){
        for(size_t i=0; i<nFactors_;++ i){
            usedFac_[i] = 0;
        }
        nFactors_ = 0;
    };

    const_iterator begin()const{
        return facToRecomp_.begin();
    }

    const_iterator end()const{
        return facToRecomp_.begin() + nFactors_;
    }

    template<class VAR_ITER>
    void addVariables(VAR_ITER varDescBegin, VAR_ITER varDescEnd){
        for( ;varDescBegin!=varDescEnd; ++varDescBegin){
            const auto varDesc = *varDescBegin;
            const auto & fset = factorsOfVariables_[varDesc];
            for(const auto fDesc: fset){
                if(usedFac_[fDesc] == 0){
                    facToRecomp_[nFactors_] = fDesc;
                    ++nFactors_;
                    usedFac_[fDesc] = 1;
                }
                // do nothing if this factors 
                // is already in the set
                // else{}
            }
        }
    }

private:
    const FactorsOfVariables<MODEL> &   factorsOfVariables_;
    std::vector<FactorDescriptor>       facToRecomp_;
    UsedFac                             usedFac_;
    uint64_t                            nFactors_;
};


}
}

#endif 
