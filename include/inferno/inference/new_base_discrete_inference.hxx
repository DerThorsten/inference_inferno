/** \file base_discrete_inference.hxx 
    \brief  Functionality which is shared between all inference algorithms
    is implemented in this header.

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX
#define INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX

#include "inferno/inferno.hxx"

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
    class DiscreteInferenceBase;


    template<class MODEL>
    class Visitor{
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;

        typedef delegate1<void , BaseInf * > BeginCallBack;
        typedef delegate1<void , BaseInf * > VisitCallBack;
        typedef delegate1<void , BaseInf * > EndCallBack;
    };


    template<class MODEL>
    class DiscreteInferenceBase{

        typedef MODEL Model;
        typedef DiscreteInferenceBase<MODEL> Self;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;



        // MUST HAVE INTERACE
        //
        virtual std::string name()const = 0;
        // inference
        virtual infer( Visitor<Model>  * visitor  = NULL) = 0 ;
        // get result
        virtual ValueType conf(Conf & conf ) = 0;
        // get model
        virtual Model & model() const=0;
        // stop inference (via visitor)
        virtual void stopInference() = 0;



        // OPTIONAL INTERFACE

        // warm start related (do nothing default)
        virtual void setConf(const Conf & conf){}
        virtual void setUpperBound(const ValueType valueType){}
        virtual void setLowerBound(const ValueType valueType){}

        // get results optional interface
        virtual ValueType upperBound(){
            return this->model().eval(this->conf());
        }
        virtual ValueType energy(){
            return this->model().eval(this->conf());
        }
        virtual ValueType lowerBound(){
            return -1.0*std::numeric_limits<ValueType>::infinity();
        }

        // partial optimality
        virtual void partialOptimalVariables()

        // model has changed
        virtual void graphChange() {
            throw NotImplementedException(this->name()+std::string("does nographChange\" so far");
        }
        virtual void energyChange() {
            throw NotImplementedException(this->name()+std::string("does not support \"graphChanged\" so far");
        }
        virtual void partialEnergyChange(const Fi * changedFacBegin, const Fi * changedFacEnd ) {
            throw NotImplementedException(this->name()+std::string("does not support \"partialEneryChange\" so far");
        }
        

    };

    /** \brief base class for inference options

        any options object of an inference algorithm
        should derive from this class.
    */
    class InferenceOptions{
    public:
        InferenceOptions(){
        }
    };

  


}
}


#endif /* INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX */
