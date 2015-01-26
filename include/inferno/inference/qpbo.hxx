#ifndef INFERNO_INFERENCE_QPBO_HXX
#define INFERNO_INFERENCE_QPBO_HXX

#include "inferno/inference/base_discrete_inference.hxx"
#include <inferno_externals/qpbo/QPBO.h>

namespace inferno{

    ///Option object for Qpbo
    struct QpboOption {
        /// using probeing technique
        bool useProbeing;
        /// forcing strong persistency
        bool strongPersistency;
        /// using improving technique
        bool useImproveing;

        /// \brief constructor
        QpboOption() {
           strongPersistency = true;
           useImproveing = false;
           useProbeing = false;
        }
    };

    template<class MODEL>
    class Qpbo : public DiscreteInferenceBase<Qpbo<MODEL>, MODEL > {
        typedef kolmogorov::qpbo::QPBO<ValueType> QpboSolver;

    public:
        typedef MODEL Model;
        Qpbo()
        : model_(NULL),
          qpbo_(NULL){
        }

        ~Qpbo(){
            delete qpbo_;
        }


        // INTERFACE
        void setModel(const Model & model){
            model_ = &model;
        }

        void infer(){
            
        }

    private:
        void setupQpbo(const Model & model){
            const auto nVar = model_->nVariables();
            INFERNO_CHECK_OP(model_->maxArity(),<=,2, "qpbo allows only models with a maximal arity of 2");
            qpbo_ = new QpboSolver(nVar, model_->nPairwiseFactors());
            qpbo_->AddNode(nVar);
            auto fiter = model_->factorIdsBegin();
            auto fiterEnd = model_->factorIdsEnd();
            for(;fiter!=fiterEnd; ++fiter){
                const auto factorId = *fiter;
                auto factor = model->operator[](factorId);
                const auto arity = factor->arity();
                if(arity == 1){
                    qpbo_->AddUnaryTerm(int(factor->vi(0)), factor->eval(0l), factor->eval(1l));
                }
                else if(arity == 2){
                    qpbo_->AddPairwiseTerm(int(factor->vi(0)), int(factor->vi(1)),
                                           factor->eval(0,0), factor->eval(0,1),
                                           factor->eval(1,0), factor->eval(1,1));
                }
                else{
                    throw RuntimeError("INTERNAL ERROR: model_.maxArity() must have a bug");
                }
            }
            qpbo_->MergeParallelEdges();
        }


    private:
        const Model * model_;
        QpboSolver * qpbo_;
    };
}


#endif /* INFERNO_INFERENCE_QPBO_HXX */
