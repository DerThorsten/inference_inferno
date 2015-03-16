/** \file higher_order_qpbo.hxx 
    \brief  Implementation of inferno::inference::HigherOrderQpbo  

    \warning To include this header one needs qpbo
    from  inferno_externals
    (in CMake ''WITH_QPBO'')
*/
#ifndef INFERNO_INFERENCE_HIGHER_ORDER_QPBO_HXX
#define INFERNO_INFERENCE_HIGHER_ORDER_QPBO_HXX

#include "inferno/inference/base_discrete_inference.hxx"
#include "inferno/inference/utilities/fix-fusion/fusion-move.hpp"

#ifdef WITH_QPBO
#include <inferno_externals/qpbo/QPBO.h>
#else 
#error "To include inferno/infernece/higher_order_qpbo.hxx, WITH_QPBO needs to be" 
#endif 

namespace inferno{
namespace inference{

    template<class MODEL>
    class HigherOrderQpbo  : public DiscreteInferenceBase<MODEL> {
    public:
        typedef MODEL Model;
    private:
        typedef HigherOrderQpbo<MODEL> Self;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor Visitor;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;
        typedef models::DenseVariableIds<Model> DenseVarIds;
        typedef kolmogorov::qpbo::QPBO<ValueType> QpboSolver;
    public:
        struct Options{
            Options(           
            ){
                useProbing_ = true;
                useImproving_ = false ;         
                strongPersistency_ = false;
                saveMem_ = false;
                nThreads_ = 0;     
            }
            
            ValueType strongPersistency_;
            bool useProbing_;
            ValueType useImproving_;
            uint64_t nThreads_;
            bool saveMem_;
        };


        HigherOrderQpbo(const Model & model, const Options & infParam = Options())
        :   BaseInf(),
            model_(model),
            denseVarIds_(model),
            stopInference_(false),
            conf_(model, 0),
            hoe_(),
            constTerm_(0.0),
            bound_(-1.0*infVal()){

            const auto maxArity = model_.maxArity();
            // check for valid models
            {
                INFERNO_CHECK_OP(maxArity,<=,10,
                    "HigherOrderQpbo is only implemented for models with maxArity<=10");

                LabelType nLabels;
                const bool simpleLabelSpace  =  model_.hasSimpleLabelSpace(nLabels);
                INFERNO_CHECK(simpleLabelSpace && nLabels == 2, "HigherOrderQpbo is only implemented for models "
                                                                "where all variables  have a binary label space");
            }   

            hoe_.AddVars(model_.nVariables());

            std::vector<ValueType> coeffs(1 << maxArity);
            std::vector<DiscreteLabel> cliqueLabels(maxArity);

            for (const auto fi : model_.factorIds()){
                const auto  factor = model_[fi];
                const ArityType arity = factor->arity();
                const LabelType l0 = 0;
                const LabelType l1 = 1;
                INFERNO_CHECK(arity>0, "0-arity factor are deprecated and not allowed anymore.\n"
                                       "Please contact the developers to inform them how you "
                                       "could create a model with zero arity factor");
                if (arity == 1){
                    const Vi var = denseVarIds_.toDense(factor->vi(0));
                    const ValueType e0 = factor->eval1(l0);
                    const ValueType e1 = factor->eval1(l1);
                    hoe_.AddUnaryTerm(var, e1 - e0);
                }
                else{
                    const unsigned int numAssignments = 1 << arity;
                    for (unsigned int subset = 1; subset < numAssignments; ++subset){
                        coeffs[subset] = 0;
                    }
                    // For each boolean assignment, get the clique energy at the
                    // corresponding labeling
                    for (unsigned int assignment = 0;  assignment < numAssignments; ++assignment){
                        for (unsigned int i = 0; i < arity; ++i){
                            if (assignment & (1 << i))
                                cliqueLabels[i] = l1;
                            else
                                cliqueLabels[i] = l0;
                        }
                        const ValueType energy = factor->eval(cliqueLabels.data());
                        for (unsigned int subset = 1; subset < numAssignments; ++subset){
                            if (assignment & ~subset)
                                continue;
                            else{
                                int parity = 0;
                                for (unsigned int b = 0; b < arity; ++b)
                                    parity ^=  (((assignment ^ subset) & (1 << b)) != 0);
                                coeffs[subset] += parity ? -energy : energy;
                            }
                        }
                    }
                    typename HigherOrderEnergy<ValueType, 10>::VarId vars[10];
                    for (unsigned int subset = 1; subset < numAssignments; ++subset){
                        int degree = 0;
                        for (unsigned int b = 0; b < arity; ++b)
                            if (subset & (1 << b))
                                vars[degree++] = denseVarIds_.toDense(factor->vi(b));

                        // this sort could be redundant
                        /// \todo add policy check 
                        /// here which checks if the variable
                        /// ids of a factor are sorted
                        /// (or even a policy within the densifier)
                        std::sort(vars, vars + degree);
                        hoe_.AddTerm(coeffs[subset], degree, vars);
                    }
                }
            }
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

            if(!options_.strongPersistency_)
                qr.ComputeWeakPersistencies();
            if(options_.useProbing_)
                throw NotImplementedException("useProbing will be implemented soon");
            if(options_.useImproving_)
                throw NotImplementedException("useImproving will be implemented soon");

            bound_ = constTerm_ + 0.5 * qr.ComputeTwiceLowerBound();

            for (const auto vi : model_.variableIds()){
                int label = qr.GetLabel(denseVarIds_.toDense(vi));
                if (label == 0 || label == 1)
                    conf_[vi] == static_cast<DiscreteLabel>(label);
                else
                    conf_[vi] = 0;
            }
            if(visitor!=NULL)
                visitor->end(this);

        }
        // get result
        virtual void conf(Conf & confMap ) {
            for(const Vi vi : model_.variableIds())
                confMap[vi] = conf_[vi];
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

    private:
        const Model & model_;
        DenseVarIds   denseVarIds_;
        Options       options_;
        bool          stopInference_;
        Conf          conf_;
        HigherOrderEnergy<ValueType, 10> hoe_;
        ValueType     constTerm_;
        ValueType     bound_;
    };


} // end namespace inference
} // end namespace inferno

#endif /* INFERNO_INFERENCE_HIGHER_ORDER_QPBO_HXX */
