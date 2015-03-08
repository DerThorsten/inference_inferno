/** \file qpbo.hxx 
    \brief  Implementation of inferno::inference::Qpbo  

    \warning To include this header one needs qpbo
    from  inferno_externals
    (in CMake ''WITH_QPBO'')
*/
#ifndef INFERNO_INFERENCE_QPBO_HXX
#define INFERNO_INFERENCE_QPBO_HXX

#include <unordered_map>

#include "inferno/inference/base_discrete_inference.hxx"

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
        typedef models::DenseVariableIds<Model> DenseVarIds;
        typedef kolmogorov::qpbo::QPBO<ValueType> QpboSolver;

        struct Options{
            Options(           
            ){
                useProbing_ = true;
                useImproving_ = false ;         
                strongPersistency_ = false;
                saveMem_ = false;
                nThreads_ = 0;     
            }
            Options(const InferenceOptions & options)
            {
                Options  defaultOpt;
                if(options.checkOptions()){
                    auto keys = options.keys();
                    options.getOption("useProbing", defaultOpt.useProbing_, useProbing_, keys);
                    options.getOption("useImproving", defaultOpt.useImproving_, useImproving_, keys);                   
                    options.getOption("strongPersistency", defaultOpt.strongPersistency_, strongPersistency_, keys);
                    options.getOption("saveMem", defaultOpt.saveMem_, saveMem_, keys);
                    options.getOption("nThreads", defaultOpt.nThreads_, nThreads_, keys);
                    options. template checkForLeftovers<Self>(keys);
                }
                else{
                    options.getOption("useProbing", defaultOpt.useProbing_, useProbing_);
                    options.getOption("useImproving", defaultOpt.useImproving_, useImproving_);  
                    options.getOption("strongPersistency", defaultOpt.strongPersistency_, strongPersistency_);
                    options.getOption("saveMem", defaultOpt.saveMem_, saveMem_);
                    options.getOption("nThreads", defaultOpt.nThreads_, nThreads_);
                }
            }
            ValueType strongPersistency_;
            bool useProbing_;
            ValueType useImproving_;
            uint64_t nThreads_;
            bool saveMem_;
        };

        static void defaultOptions(InferenceOptions & options){
            Options defaultOpt;
            options.set("useProbing",defaultOpt.useProbing_);
            options.set("useImproving",defaultOpt.useImproving_);
            options.set("strongPersistency",defaultOpt.strongPersistency_);
            options.set("saveMem",defaultOpt.saveMem_);
            options.set("nThreads",defaultOpt.nThreads_);
        }


        Qpbo(const Model & model, const InferenceOptions & infParam = InferenceOptions())
        :   BaseInf(),
            model_(model),
            denseVarIds_(model),
            stopInference_(false),
            qpbo_(),
            constTerm_(0.0),
            bound_(-1.0*infVal()),
            value_(infVal()){

            INFERNO_CHECK_OP(model_.maxArity(),<=,2,
                "qpbo is only allowed for models with maxArity<=2");
            
            const Vi nVar = model_.nVariables();

            // construct qpbo
            qpbo_ = new QpboSolver(nVar, model_.nPairwiseFactors());
            qpbo_->AddNode(nVar);


             for(const auto factor : model_.factors()){
                const auto arity = factor->arity();
                if(arity == 0){
                    INFERNO_CHECK(false, "constant factors are not allowed anymore!");
                    constTerm_ += factor->eval(0l);
                }
                else if(arity == 1){
                    const int qpboVi0 = denseVarIds_.toDense(factor->vi(0));
                    qpbo_->AddUnaryTerm(qpboVi0, factor->eval1(0l), factor->eval1(1l));
                }
                else if(arity == 2){
                    const int qpboVi0 = denseVarIds_.toDense(factor->vi(0));
                    const int qpboVi1 = denseVarIds_.toDense(factor->vi(1));
                    qpbo_->AddPairwiseTerm(qpboVi0, qpboVi1,factor->eval2(0,0), factor->eval2(0,1),
                        factor->eval2(1,0), factor->eval2(1,1));
                }
                else
                    throw RuntimeError("INTERNAL ERROR: model_.maxArity() must have a bug");
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

            if(options_.useProbing_)
                throw NotImplementedException("useProbing will be implemented soon");
            if(options_.useImproving_)
                throw NotImplementedException("useImproving will be implemented soon");

            if(visitor!=NULL)
                visitor->end(this);
        }
        // get result
        virtual void conf(Conf & confMap ) {
            for(const Vi vi : model_.variableIds()){
                const auto qpboLabel = qpbo_->GetLabel(denseVarIds_.toDense(vi));
                if(qpboLabel==0)
                    confMap[vi] = 0;
                if(qpboLabel==1)
                    confMap[vi] = 1;
                else 
                    confMap[vi] = 0;
            }
        }
        virtual DiscreteLabel label(const Vi vi ) {
            const auto qpboLabel = qpbo_->GetLabel(denseVarIds_.toDense(vi));
            if(qpboLabel==0)
                return 0;
            if(qpboLabel==1)
                return 1;
            else 
                return 0;
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
