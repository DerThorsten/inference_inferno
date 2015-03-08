/** \file qpbo.hxx 
    \brief  Implementation of inferno::inference::HigherOrderQpbo  

    \warning To include this header one needs qpbo
    from  inferno_externals
    (in CMake ''WITH_QPBO'')
*/
#ifndef INFERNO_INFERENCE_HIGHER_ORDER_QPBO_HXX
#define INFERNO_INFERENCE_HIGHER_ORDER_QPBO_HXX

#include <unordered_map>

#include "inferno/inference/base_discrete_inference.hxx"
#include "inferno/inference/utilities/fix-fusion/fusion-move.hpp"

#ifdef WITH_QPBO
#include <inferno_externals/qpbo/QPBO.h>
#else 
#error "To include inferno/infernece/qpbo.hxx, WITH_QPBO needs to be" 
#endif 

namespace inferno{
namespace inference{

    
    template<class MODEL>
    class HigherOrderQpbo  : public DiscreteInferenceBase<MODEL> {
    public:
        typedef MODEL Model;
        typedef HigherOrderQpbo<MODEL> Self;
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


        HigherOrderQpbo(const Model & model, const InferenceOptions & infParam = InferenceOptions())
        :   BaseInf(),
            model_(model),
            denseVarIds_(model),
            stopInference_(false),
            conf_(model, 0),
            hoe_(),
            constTerm_(0.0),
            bound_(-1.0*infVal()){

            const auto maxArity = model_.maxArity();
            INFERNO_CHECK_OP(maxArity,<=,2,
                "qpbo is only allowed for models with maxArity<=2");
            
            const Vi nVar = model_.nVariables();

            hoe_.AddVars(model_.nVariables());

            std::vector<ValueType> coeffs(1 << maxArity);
            std::vector<DiscreteLabel> cliqueLabels(maxArity);

            for (const auto fi : model_.factorIds()){
                const auto  factor = model_[fi];
                const ArityType arity = factor->arity();
                const LabelType l0 = 0;
                const LabelType l1 = 1;
                if (arity == 0){
                    constTerm_ += factor->eval1(l0);
                    continue;
                }
                else if (arity == 1){
                    const Vi var = denseVarIds_.toDense(factor->vi(0));
                    const ValueType e0 = factor->eval1(l0);
                    const ValueType e1 = factor->eval1(l1);
                    hoe_.AddUnaryTerm(var, e1 - e0);
                }
                else{
                    unsigned int numAssignments = 1 << arity;
                    //ValueType coeffs[numAssignments];
                    for (unsigned int subset = 1; subset < numAssignments; ++subset){
                        coeffs[subset] = 0;
                    }
                    // For each boolean assignment, get the clique energy at the
                    // corresponding labeling
                    for (unsigned int assignment = 0;  assignment < numAssignments; ++assignment){
                        for (unsigned int i = 0; i < arity; ++i){
                            if (assignment & (1 << i)){
                                cliqueLabels[i] = l1;
                            }
                            else{
                                cliqueLabels[i] = l0;
                            }
                        }
                        const ValueType energy = factor->eval(cliqueLabels.data());
                        for (unsigned int subset = 1; subset < numAssignments; ++subset){
                            if (assignment & ~subset){
                                continue;
                            }
                            else{
                                int parity = 0;
                                for (unsigned int b = 0; b < arity; ++b){
                                    parity ^=  (((assignment ^ subset) & (1 << b)) != 0);
                                }
                                coeffs[subset] += parity ? -energy : energy;
                            }
                        }
                    }
                    typename HigherOrderEnergy<ValueType, 10>::VarId vars[10];
                    for (unsigned int subset = 1; subset < numAssignments; ++subset){
                        int degree = 0;
                        for (unsigned int b = 0; b < arity; ++b){
                            if (subset & (1 << b)){
                                vars[degree++] = denseVarIds_.toDense(factor->vi(b));
                            }
                        }
                        std::sort(vars, vars + degree);
                        hoe_.AddTerm(coeffs[subset], degree, vars);
                    }
                }
            }
        }

        ~HigherOrderQpbo(){
            //delete qpbo_;
        }

        // MUST HAVE INTERACE
        virtual std::string name() const {
            return "HigherOrderQpbo";
        }
        // inference
        virtual void infer( Visitor  * visitor  = NULL) {

            if(visitor!=NULL)
                visitor->begin(this);

            QpboSolver qr(model_.nVariables(), 0);
            hoe_.ToQuadratic(qr);
            qr.Solve();

            for (const auto sparseVi : model_.variableIds()){
                const auto denseVi = denseVarIds_.toDense(sparseVi);
                int label = qr.GetLabel(denseVi);
                if (label == 0 ) {
                    conf_[sparseVi] = 0;
                }
                else if (label == 1){
                    conf_[sparseVi] = 1;
                }
                else{
                    conf_[sparseVi] = 0;
                }
            }
            bound_ = constTerm_ + 0.5 * qr.ComputeTwiceLowerBound();


            if(visitor!=NULL)
                visitor->end(this);
        }
        // get result
        virtual void conf(Conf & confMap ) {
            for(const Vi vi : model_.variableIds()){
                confMap[vi] = conf_[vi];
            }
        }
        virtual DiscreteLabel label(const Vi vi ) {
            return conf_[vi];
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
        //virtual ValueType upperBound(){
        //    return value_;
        //}
        //virtual ValueType energy(){
        //    return value_;
        //}

    private:
        const Model & model_;
        DenseVarIds   denseVarIds_;
        Options       options_;
        bool          stopInference_;
        Conf          conf_;
        HigherOrderEnergy<ValueType, 10> hoe_;
        ValueType     constTerm_;
        ValueType     bound_;
        ValueType     value_;
    };


} // end namespace inference
} // end namespace inferno

#endif /* INFERNO_INFERENCE_HIGHER_ORDER_QPBO_HXX */
