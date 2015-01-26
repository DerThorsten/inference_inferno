#ifndef INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX
#define INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX

#include "inferno/inferno.hxx"

namespace inferno{

    template<class INFERENCE, class MODEL>
    class DiscreteInferenceBase{

    public:
        // setup before inference
        void setModel(const MODEL & model);
        void setSemiRing(const MODEL & model);
        void setUpperBound(const ValueType ub);
        void setLowerBound(const ValueType lb);
        template<class ITER>
        void setWarmStart(ITER warmStartConf);

        // actual inference
        void infer();
        template<class VI_ITER, class LABEL_ITER>
        void inferWithFixedVariables(VI_ITER fixedVars, LABEL_ITER fixedVarsLabels);
        template<class VI_ITER, class EXLUDED_LABEL_ITER>
        void inferWithPartialFixedVariables(VI_ITER partialFixedVars, EXLUDED_LABEL_ITER excludedLabelsIter);


        



    private:
        const INFERENCE & inference()const{
            return static_cast<const INFERENCE *>(this);
        }
        INFERENCE & inference(){
            return static_cast<INFERENCE *>(this);
        }
    };
}


#endif /* INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX */
