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
    typedef VectorSet<Vi> ViSet;
    typedef ViSet VariableNeighbours;

    VariablesNeighbours(const Model & model)
    : storage_(model){
        for(const auto fi : model.factorIds()){
            const auto factor = model[fi];
            const auto arity = factor->arity();

            for(uint32_t va=0; va<arity-1; ++va){
                const auto viA = factor->vi(va);
                for(uint32_t vb=va+1; vb<arity; ++vb){
                    const auto viB = factor->vi(vb);
                    storage_[viA].insert(viB);
                    storage_[viB].insert(viA);
                }
            }
        };
    };

    const VariableNeighbours & operator[](const Vi vi)const{
        return storage_[vi];
    }
    
    const VariableNeighbours & operator[](const Vi vi){
        return storage_[vi];
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
    typedef VectorSet<Fi> FiSet;
    typedef FiSet FactorsOfVariable;

    FactorsOfVariables(const Model & model)
    : storage_(model){
        for(const auto fi : model.factorIds()){
            const auto factor = model[fi];
            const auto arity = factor->arity();
            for(auto v=0; v<arity; ++v){
                storage_[factor->vi(v)].insert(fi);
            };
        };
    };

    const FactorsOfVariable & operator[](const Vi vi)const{
        return storage_[vi];
    }
    
    const FactorsOfVariable & operator[](const Vi vi){
        return storage_[vi];
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
    typedef VectorSet<Fi> FiSet;
    typedef FiSet HigherOrderFactorsOfVariable;
    typedef FiSet UnaryFactorsOfVariable;

    struct Facs{
    public:
        void insert(const Fi fi, const uint32_t arity){
            if(arity == 1){
                unaryFactors_.insert(fi);
            }
            else{
                higherOrderFactors_.insert(fi);
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
        for(const auto fi : model.factorIds()){
            const auto factor = model[fi];
            const auto arity = factor->arity();
            for(auto v=0; v<arity; ++v){
                storage_[factor->vi(v)].insert(fi, arity);
            };
        };
    };

    const Facs & operator[](const Vi vi)const{
        return storage_[vi];
    }
    
    const Facs & operator[](const Vi vi){
        return storage_[vi];
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
    typedef HigherOrderAndUnaryFactorsOfVariables<Model> FactorsOfVars;
    typedef typename Model:: template FactorMap<unsigned char> UsedFac;
    typedef std::vector<Fi>::const_iterator const_iterator;

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

    template<class VI_ITER>
    void addVariables(VI_ITER viBegin, VI_ITER viEnd){
        for( ;viBegin!=viEnd; ++viBegin){
            const auto vi = *viBegin;
            const auto & fset = factorsOfVariables_[vi];
            for(const auto fi: fset){
                if(usedFac_[fi] == 0){
                    facToRecomp_[nFactors_] = fi;
                    ++nFactors_;
                    usedFac_[fi] = 1;
                }
                // do nothing if this factors 
                // is already in the set
                // else{}
            }
        }
    }

private:
    const FactorsOfVariables<MODEL> & factorsOfVariables_;
    std::vector<Fi>             facToRecomp_;
    UsedFac                     usedFac_;
    uint64_t                    nFactors_;
};


}
}

#endif 
