/** \file discrete_inference_base.hxx 
    \brief  Functionality which is shared between all inference algorithms
    is implemented in this header.

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX
#define INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>

#include <boost/any.hpp> 

#include "inferno/inferno.hxx"
#include "inferno/utilities/delegate.hxx"
#include "inferno/inference/utilities/movemaker.hxx"
#include "inferno/inference/inference_options.hxx"
#include "inferno/model/factors_of_variables.hxx"
#include "inferno/learning/weights.hxx"

namespace inferno{

/** \namespace inferno::inference
    \brief subnamespace of inferno
    where all inference related
    functionality is implemented.

    All inference algorithms and related
    functionality is implemented 
    within inferno::inference.
*/
namespace inference{


    template<class MODEL>
    class DiscreteInferenceBase{
    public:
        typedef MODEL Model;
        typedef typename Model::VariableDescriptor   VariableDescriptor;
        typedef typename Model::FactorDescriptor     FactorDescriptor;
        typedef typename Model::UnaryDescriptor      UnaryDescriptor;
        typedef DiscreteInferenceBase<MODEL> Self;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;

        class Visitor{
        public:

            typedef DiscreteInferenceBase<MODEL> BaseInf;
            typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;

            typedef inferno::delegate1<void , BaseInf * > BeginCallBack;
            typedef inferno::delegate1<void , BaseInf * > VisitCallBack;
            typedef inferno::delegate3<void , BaseInf * , const std::string &, const ValueType> LoggingCallBack;          
            typedef inferno::delegate1<void , BaseInf * > EndCallBack;

            void begin(BaseInf * inf){
                if(bool(beginCallBack))
                    beginCallBack(inf);
            }
            void visit(BaseInf * inf){
                if(bool(visitCallBack))
                    visitCallBack(inf);
            }
            void logging(BaseInf * inf, const std::string & name, const ValueType value){
                if(bool(loggingCallBack))
                    loggingCallBack(inf, name, value);
            }
            void end(BaseInf * inf){
                if(bool(endCallBack))
                    endCallBack(inf);
            }

            BeginCallBack beginCallBack;
            VisitCallBack visitCallBack;
            LoggingCallBack loggingCallBack;
            EndCallBack endCallBack;
        };


        // MUST HAVE INTERACE
        //
        virtual std::string name()const = 0;
        // inference
        virtual void infer( Visitor  * visitor  = NULL) = 0 ;
        // get result
        virtual void conf(Conf & conf ) = 0;

        /** \brief get the label
            \deprecated will be changed soon to descriptors
        */
        virtual DiscreteLabel label(const Vi vi ) = 0;
        // get model
        virtual const Model & model() const=0;
        // stop inference (via visitor)
        virtual void stopInference() = 0;

       


        // OPTIONAL INTERFACE

        // warm start related (do nothing default)
        virtual void setConf(const Conf & conf){}
        virtual void setUpperBound(const ValueType valueType){}
        virtual void setLowerBound(const ValueType valueType){}

        // get results optional interface
        virtual ValueType upperBound(){
            Conf confMap(this->model());
            this->conf(confMap);
            return this->model().eval(confMap);
        }
        virtual ValueType energy(){
            Conf confMap(this->model());
            this->conf(confMap);
            return this->model().eval(confMap);
        }
        virtual ValueType lowerBound(){
            return -1.0*std::numeric_limits<ValueType>::infinity();
        }

        /// 
        /** \brief check if a certain variable is partial optimal 
            \deprecated will be changed soon to descriptors
        */
        virtual bool isPartialOptimal(const Vi vi){
            return false;
        }

        /** \brief 

            \deprecated will be changed soon to descriptors

            some solvers can guarantee that some labels
            can be excluded in the (unknown) global optimal solutions
        */
        virtual void excludedLabels(
            const Vi vi, 
            VectorSet<DiscreteLabel> & excludedLabes
        ){
            excludedLabes.clear();
            if(this->isPartialOptimal(vi)){
                const auto optLabel = this->label(vi);
                const auto & model = this->model();
                const auto nLabels = model.nLabels(vi);
                excludedLabes.reserve(nLabels-1);
                for(DiscreteLabel l=0; l<nLabels; ++l){
                    if(l!=optLabel)
                        excludedLabes.insert(l);
                }
            }
        }

        /** \brief inform a solver that the graph has been changed
        
            throws a NotImplementedException exception
        */
        virtual void graphChange() {
            throw NotImplementedException(this->name() +
                std::string("does not support \"graphChange\" so far"));
        }

        /** \brief inform a solver that the energy has been changed
        
            throws a NotImplementedException exception
        */
        virtual void energyChange() {
            throw NotImplementedException(this->name() +
                std::string("does not support \"energyChange\" so far"));
        }

        /** \brief inform a solver that the weights have been updated
        
            default implementation calls 
            DiscreteInferenceBase::energyChange
        */
        virtual void updateWeights(const learning::WeightVector & weights){
            this->energyChange();
        }

        /** \brief inform a solver that the energy has partially changed
        
            default implementation calls 
            DiscreteInferenceBase::energyChange
        */
        virtual void partialEnergyChange(
            const FactorDescriptor * changedFactorsBegin, 
            const FactorDescriptor * changedFactorsEnd,
            const UnaryDescriptor *  changedUnariesBegin,
            const UnaryDescriptor *  changedUnariesEnd 
        ) {
            this->energyChange();
        }
    };


}
}


#endif /* INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX */
