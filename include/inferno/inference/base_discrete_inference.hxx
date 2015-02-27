/** \file base_discrete_inference.hxx 
    \brief  Functionality which is shared between all inference algorithms
    is implemented in this header.

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX
#define INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX

#include "inferno/inferno.hxx"
#include "inferno/utilities/delegate.hxx"
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
    public:
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;

        typedef inferno::delegate1<void , BaseInf * > BeginCallBack;
        typedef inferno::delegate1<void , BaseInf * > VisitCallBack;
        typedef inferno::delegate1<void , BaseInf * > EndCallBack;


        void begin(BaseInf * inf);
        void visit(BaseInf * inf);
        void end(BaseInf * inf);
    };


    template<class MODEL>
    class DiscreteInferenceBase{
    public:
        typedef MODEL Model;
        typedef DiscreteInferenceBase<MODEL> Self;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;



        // MUST HAVE INTERACE
        //
        virtual std::string name()const = 0;
        // inference
        virtual void infer( Visitor<Model>  * visitor  = NULL) = 0 ;
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

        // partial optimality
        // ???

        // model has changed
        virtual void graphChange() {
            throw NotImplementedException(this->name()+std::string("does not support \"graphChange\" so far"));
        }
        virtual void energyChange() {
            throw NotImplementedException(this->name()+std::string("does not support \"graphChanged\" so far"));
        }
        virtual void partialEnergyChange(const Fi * changedFacBegin, const Fi * changedFacEnd ) {
            throw NotImplementedException(this->name()+std::string("does not support \"partialEneryChange\" so far"));
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

    /*
    template<class MODEL>
    class Icm  : public<DiscreteInferenceBase> {
    public:
        typedef MODEL Model;
        typedef Icm<MODEL> Self;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;



        // MUST HAVE INTERACE
        //
        virtual std::string name()const = 0;
        // inference
        virtual void infer( Visitor<Model>  * visitor  = NULL) = 0 ;
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

        // partial optimality
        // ???

        // model has changed
        virtual void graphChange() {
            throw NotImplementedException(this->name()+std::string("does not support \"graphChange\" so far"));
        }
        virtual void energyChange() {
            throw NotImplementedException(this->name()+std::string("does not support \"graphChanged\" so far"));
        }
        virtual void partialEnergyChange(const Fi * changedFacBegin, const Fi * changedFacEnd ) {
            throw NotImplementedException(this->name()+std::string("does not support \"partialEneryChange\" so far"));
        }
        
    };
  
    */

}
}


#endif /* INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX */
