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

    /** \brief base class for inference options

        any options object of an inference algorithm
        should derive from this class.
    */
    class InferenceOptions{
    public:
        InferenceOptions(const SemiRing sr = MinSum)
        : semiRing(sr){
        }
        SemiRing semiRing;
    };

    /** \brief base class for any discrete inference class
        implemented with
        <a href="http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern"> CRT-pattern</a>.  
    
        Any discrete inference function should derive from
        this class with <a href="http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern"> CRT-pattern</a>.
        

    */
    template<class INFERENCE, class MODEL>
    class DiscreteInferenceBase{

    public:
        // setup before inference
        void setUpperBound(const ValueType ub);
        void setLowerBound(const ValueType lb);
        template<class ITER>
        void setWarmStart(ITER warmStartConf);

        /** \brief run inference


            \warning This function must be implemented by INFERENCE.
        */
        void infer();


        /** \brief get the result labeling / state-vector

            DiscreteInferenceBase::infer() must be called
            before calling DiscreteInferenceBase<INFERENCE,MODEL>::conf

            \warning This function must be implemented by INFERENCE.
        */
        template<class OUT_ITER>
        void conf(OUT_ITER);





    private:
        const INFERENCE & inference()const{
            return static_cast<const INFERENCE *>(this);
        }
        INFERENCE & inference(){
            return static_cast<INFERENCE *>(this);
        }
    };


}
}


#endif /* INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX */
