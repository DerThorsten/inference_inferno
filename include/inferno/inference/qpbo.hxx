/** \file qpbo.hxx 
    \brief  Implementation of inferno::inference::Qpbo  

    \warning To include this header one needs qpbo
    from  inferno_externals
    (in CMake ''WITH_QPBO'')
*/
#ifndef INFERNO_INFERENCE_QPBO_HXX
#define INFERNO_INFERENCE_QPBO_HXX

#include <unordered_map>

#include "inferno/model/discrete_model_base.hxx"
#include "inferno/inference/discrete_inference_base.hxx"

#ifdef WITH_QPBO
#include <inferno_externals/qpbo/QPBO.h>
#else 
#error "To include inferno/infernece/qpbo.hxx, WITH_QPBO needs to be" 
#endif 

namespace inferno{
namespace inference{

    
    template<class MODEL>
    class Qpbo  : public DiscreteInferenceBase<MODEL> {
    public:
        typedef MODEL Model;
        typedef Qpbo<MODEL> Self;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor Visitor;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;
        typedef inferno::models::DenseVariableIds<Model> DenseVarIds;
        typedef kolmogorov::qpbo::QPBO<ValueType> QpboSolver;

        struct Options{
            Options(      
                const bool useProbing = false,
                const bool useImproving = false ,
                const bool strongPersistency = false,
                const bool saveMem = false,
                const uint64_t nThreads = 0
            )
            :   useProbing_(useProbing),
                useImproving_(useImproving),
                strongPersistency_(strongPersistency),
                saveMem_(saveMem),
                nThreads_(nThreads)
            {
            }
            bool strongPersistency_;
            bool useProbing_;
            bool useImproving_;
            bool saveMem_;
            uint64_t nThreads_;
            
        };



        Qpbo(const Model & model, const Options & options = Options() )
        :   BaseInf(),
            model_(model),
            denseVarIds_(model),
            options_(options),
            stopInference_(false),
            qpbo_(),
            constTerm_(0.0),
            bound_(-1.0*infVal()),
            value_(infVal()){


            // check for valid models
            {
                INFERNO_CHECK_OP(model.maxArity(),<=,2,
                    "Qpbo is only allowed for models with maxArity<=2");

                LabelType nLabels;
                const bool simpleLabelSpace  =  model_.hasSimpleLabelSpace(nLabels);
                INFERNO_CHECK(simpleLabelSpace && nLabels == 2, "HigherOrderQpbo is only implemented for models "
                                                                "where all variables  have a binary label space");
            } 
            
            const Vi nVar = model_.nVariables();

            // construct qpbo
            qpbo_ = new QpboSolver(nVar, model_.nPairwiseFactors());
            qpbo_->AddNode(nVar);


            for(const auto unary : model_.unaries()){
                const int qpboVi0 = denseVarIds_.toDenseId(unary->variable());
                qpbo_->AddUnaryTerm(qpboVi0, unary->eval(0l), unary->eval(1l));
            }
            for(const auto factor : model_.factors()){
                const auto arity = factor->arity();
                if(arity == 2){
                    const int qpboVi0 = denseVarIds_.toDenseId(factor->variable(0));
                    const int qpboVi1 = denseVarIds_.toDenseId(factor->variable(1));
                    INFERNO_CHECK_OP(qpboVi0,!=,qpboVi1,"");
                    qpbo_->AddPairwiseTerm(qpboVi0, qpboVi1,factor->eval(0,0), factor->eval(0,1),
                        factor->eval(1,0), factor->eval(1,1));
                }
                else{
                    throw RuntimeError("INTERNAL ERROR: model_.maxArity() must have a bug");
                }
            }
            qpbo_->MergeParallelEdges();
        }

        ~Qpbo(){
            delete qpbo_;
        }

        // MUST HAVE INTERACE
        virtual std::string name() const {
            return "Qpbo";
        }
        // inference
        virtual void infer( Visitor  * visitor  = NULL) {


            if(visitor!=NULL)
                visitor->begin(this);

            qpbo_->Solve();
            if(!options_.strongPersistency_)
                qpbo_->ComputeWeakPersistencies();
            
            value_ = constTerm_ + 0.5 * qpbo_->ComputeTwiceEnergy();
            bound_ = constTerm_ + 0.5 * qpbo_->ComputeTwiceLowerBound();

            if(options_.useProbing_){
                throw NotImplementedException("useProbing will be implemented soon");
            }
            if(options_.useImproving_){
                throw NotImplementedException("useImproving will be implemented soon");
            }

            if(visitor!=NULL)
                visitor->end(this);
        }
        // get result
        virtual void conf(Conf & confMap ) {
            for(const auto varDesc : model_.variableDescriptors()){
                const auto vi = model_.variableId(varDesc);
                const auto qpboLabel = qpbo_->GetLabel(denseVarIds_.toDenseId(vi));
                if(qpboLabel==0)
                    confMap[vi] = 0;
                if(qpboLabel==1)
                    confMap[vi] = 1;
                else 
                    confMap[vi] = 0;
            }
        }
        virtual DiscreteLabel label(const Vi vi ) {
            const auto qpboLabel = qpbo_->GetLabel(denseVarIds_.toDenseId(vi));
            if(qpboLabel==0)
                return 0;
            if(qpboLabel==1)
                return 1;
            else 
                return 0;
        }

        virtual bool isPartialOptimal(const Vi vi){
            const auto qpboLabel = qpbo_->GetLabel(denseVarIds_.toDenseId(vi));
            return qpboLabel == 0 || qpboLabel == 1;
        }

        // get model
        virtual const Model & model() const{
            return model_;
        }
        // stop inference (via visitor)
        virtual void stopInference(){
            stopInference_ = true;
        }
        // get results optional interface
        virtual ValueType lowerBound(){
            return bound_;
        }
        virtual ValueType upperBound(){
            return value_;
        }
        virtual ValueType energy(){
            return value_;
        }

    private:
        const Model & model_;
        DenseVarIds   denseVarIds_;
        Options       options_;
        bool          stopInference_;
        QpboSolver *  qpbo_;
        ValueType     constTerm_;
        ValueType     bound_;
        ValueType     value_;
    };


} // end namespace inference
} // end namespace inferno

#endif /* INFERNO_INFERENCE_QPBO_HXX */
