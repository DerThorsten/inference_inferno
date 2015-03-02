#ifndef INFERNO_INFERENCE_UTILITIES_MOVEMAKER_HXX
#define INFERNO_INFERENCE_UTILITIES_MOVEMAKER_HXX

#include <algorithm>
#include <vector>


#include "inferno/inferno.hxx"
#include "inferno/model/factors_of_variables.hxx"

namespace inferno {
namespace inference {


/** \brief A helper class for move making algorithms.
    
    With Movemaker one can compute optimal moves of a 
    subset of variables.
    Furthermore one can evaluate the energy
    of a move without doing the actual move.
    
    \ingroup inference_utilities
*/
template<class MODEL>
class Movemaker {

public:

    /** \brief Rebind the Model to get the Movemaker class for a different 
        Model Type.
    */
    template<class _MODEL>
        struct RebindGm{
        /** \var typedef type 
            \brief Movemaker type for new model type_MODEL
        */
        typedef Movemaker<_MODEL> type;
    };

    typedef MODEL Model;
    typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;

    Movemaker(const Model &); 
    template<class StateIterator>

    /// \todo remove this function and use energy
    ValueType value() const{
        return energy_;
    }

    ValueType energy() const{
        return energy_;
    }

    template<class IndexIterator, class StateIterator>
    ValueType valueAfterMove(IndexIterator, IndexIterator, StateIterator);

    DiscreteLabel state(const Vi vi) const{
        return state_[vi];
    }

    DiscreteLabel label(const Vi vi) const{
        return state_[vi];
    }

    void reset();
    template<class CONF_MAP>
    void initialize(const CONF_MAP &);
    template<class IndexIterator, class StateIterator>
    ValueType move(IndexIterator, IndexIterator, StateIterator);
    template<class IndexIterator>
    ValueType moveOptimally(IndexIterator, IndexIterator);

    void conf(Conf & confMap){
        for(const auto vi: model_.variableIds())
            confMap[vi] = state_[vi];
    }

    const FactorsOfVariables<Model> & factorsOfVariabes()const{
        return factorsOfVariables_;
    }

private:



    template<class FactorIndexIterator>
    ValueType evaluateFactors(FactorIndexIterator, FactorIndexIterator, const std::vector<DiscreteLabel>&) ;

    const Model & model_;
    FactorsOfVariables<Model> factorsOfVariables_;
    Conf state_;
    Conf stateBuffer_; // always equal to state_ (invariant)
    Conf bestStateBuffer_; // used in moveOptimaly
    ValueType energy_; // energy of state state_ (invariant)

    std::vector<DiscreteLabel> currentFState_,destFState_; // buffers for the factors  configs

    inferno::FactorsOfMultipleVariables<Model> facToRecomp_;
};



template<class MODEL>
Movemaker<MODEL>::Movemaker
(
   const Model & model
)
:  model_(model),
   factorsOfVariables_(model),
   state_(model,0),
   stateBuffer_(model),
   bestStateBuffer_(model),
   energy_(model.eval(state_)),
   currentFState_(),
   destFState_(),
   facToRecomp_(model, factorsOfVariables_)
{
    std::cout<<"mm . start\n";
    const auto maxArity = model_.maxArity();
    currentFState_.resize(maxArity);
    destFState_.resize(maxArity);
    std::cout<<"mm . end\n";
}


template<class MODEL>
template<class CONF_MAP>
void Movemaker<MODEL>::initialize
(
   const CONF_MAP & conf
) {
   energy_ = model_.eval(conf); 
   for (const auto vi : model_.variableIds()) {
      state_[vi] = conf[vi];
      stateBuffer_[vi] = conf[vi];
   }
}

template<class MODEL>
void
Movemaker<MODEL>::reset() {
   for (const auto vi : model_.varibleIds()) {
      state_[vi] = 0;
      stateBuffer_[vi] = 0;
   }
   energy_ = model_.eval(state_);
}



template<class MODEL>
template<class IndexIterator, class StateIterator>
ValueType
Movemaker<MODEL>::valueAfterMove
(
   IndexIterator begin,
   IndexIterator end,
   StateIterator destLabels
) { 

    // do partial update 

    // set stateBuffer_ to destLabels, and determine factors to recompute
    for (auto it = begin; it != end; ++it, ++destLabels) {
        const auto vi = *it;
        const auto dLabel = *destLabels;
        if (state_[vi] != dLabel) {
            stateBuffer_[vi] = dLabel;
        }
    }
    facToRecomp_.reset();
    facToRecomp_.addVariables(begin, end);

    // \todo consider buffering the values of ALL factors at the current state!
    ValueType destinationValue = energy_;



    for (const auto fi: facToRecomp_) {

        const auto factor = model_[fi];
        factor->getFactorConf(state_, currentFState_.begin());
        factor->getFactorConf(stateBuffer_, destFState_.begin());
        destinationValue -= factor->eval(currentFState_.data());
        destinationValue += factor->eval(destFState_.data());
    }

    // restore stateBuffer_
    for (auto  it = begin; it != end; ++it) {
        stateBuffer_[*it] = state_[*it];
    }

    return destinationValue;
}

template<class MODEL>
template<class IndexIterator, class StateIterator>
inline ValueType
Movemaker<MODEL>::move
(
   IndexIterator begin,
   IndexIterator end,
   StateIterator sit
) {
   energy_ = valueAfterMove(begin, end, sit); // tests for assertions
   while (begin != end) {
      state_[*begin] = *sit;
      stateBuffer_[*begin] = *sit;
      ++begin;
      ++sit;
   }
   return energy_;
}


/// for a subset of variables, move to a labeling that is optimal 
/// \param variableIndices random access iterator to the beginning of a sequence of variable indices
/// \param variableIndicesEnd random access iterator to the end of a sequence of variable indices
/// \return new value
template<class MODEL>
template<class IndexIterator>
inline ValueType
Movemaker<MODEL>::moveOptimally
(
    IndexIterator variableIndices,
    IndexIterator variableIndicesEnd
) {
    // determine factors to recompute
    facToRecomp_.reset();
    facToRecomp_.addVariables(variableIndices, variableIndicesEnd);

    // find an optimal move and the corresponding energy of factors to recompute
    size_t numberOfVariables = std::distance(variableIndices, variableIndicesEnd);
    ValueType initialEnergy = evaluateFactors(facToRecomp_.begin(),facToRecomp_.end(),state_);

    ValueType bestEnergy = initialEnergy;


    for (size_t j=0; j<numberOfVariables; ++j) {
        const size_t vi = variableIndices[j];
        stateBuffer_[vi] = 0;
    }
    for (;;) {
        // compute energy
        ValueType energy = evaluateFactors(facToRecomp_.begin(),facToRecomp_.end(),stateBuffer_);
        if(energy<bestEnergy) {
            // update energy and state
            bestEnergy = energy;
            for (size_t j = 0; j < numberOfVariables; ++j) {
                bestStateBuffer_[j] = stateBuffer_[variableIndices[j]];
            }
        }
        // increment buffered state
        for (size_t j = 0; j < numberOfVariables; ++j) {
            const size_t vi = variableIndices[j];
            if (stateBuffer_[vi] < model_.nLabels(vi) - 1) {
                ++stateBuffer_[vi];
                break;
            } 
            else {
                if (j < numberOfVariables - 1) {
                    stateBuffer_[vi] = 0;
                } 
                else {
                    goto overflow;
                }
            }
        }
    }
    overflow:
        ;

    if (bestEnergy < initialEnergy ) {
        // update state_ and stateBuffer_
        for (size_t j = 0; j < numberOfVariables; ++j) {
            const size_t vi = variableIndices[j];
            state_[vi] = bestStateBuffer_[j];
            stateBuffer_[vi] = bestStateBuffer_[j];
        }
        energy_ -= initialEnergy;
        energy_ += bestEnergy;

    } 
    else {
        // restore stateBuffer_
        for (size_t j = 0; j < numberOfVariables; ++j) {
            const size_t vi = variableIndices[j];
            stateBuffer_[vi] = state_[vi];
        }
    }

   return energy_;
}



template<class MODEL>
template<class FactorIndexIterator>
inline ValueType
Movemaker<MODEL>::evaluateFactors
(
    FactorIndexIterator begin,
    FactorIndexIterator end,
    const std::vector<DiscreteLabel>& state
)  {
    ValueType value = ValueType(0.0);
    for(; begin != end; ++begin) {
        const auto fi = *begin;
        const auto factor = model_[fi];
        const auto arity = factor->arity();
        factor->getFactorConf(state, currentFState_.begin());
        value += factor->eval(currentFState_.data());
    }
    return value;
}

} // end namespace inference
} // end inference namespace inferno

#endif // #ifndef INFERNO_INFERENCE_UTILITIES_MOVEMAKER_HXX
