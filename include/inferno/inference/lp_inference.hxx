/** \file LpInference.hxx 
    \brief  Implementation of inferno::inference::LpInference  
*/
#ifndef INFERNO_INFERENCE_LP_INFERENCE_HXX
#define INFERNO_INFERENCE_LP_INFERENCE_HXX

#include <unordered_map>

#include "inferno/model/discrete_model_base.hxx"
#include "inferno/inference/discrete_inference_base.hxx"
#include "inferno/inference/utilities/lp/lp_solver_base.hxx"


namespace inferno{
namespace inference{

    

    template<class MODEL>
    class LpInference  : public DiscreteInferenceBase<MODEL> {
    public:
        typedef MODEL Model;
        typedef LpInference<MODEL> Self;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor Visitor;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;

        typedef typename MODEL:: template VariableMap<int64_t> VarsNode;

        typedef kolmogorov::LpInference::LpInference<ValueType> QpboSolver;

        struct Options{
            Options(      

            )
            {
            }
            
            bool integerConstraint_;
        };



        LpInference(const Model & model, const Options & options = Options() )
        :   BaseInf(),
            model_(&model),
            options_(options),
            stopInference_(false),
            bound_(-1.0*infVal()),
            value_(infVal())
        {

            lpSolver_ = new LpSolverGurobi();
            
            
            

            
            VarsNode varsNode(*model_);
            const auto nVarNodes = model_->nVariableIndicatorVariables(varsNode);
            const auto nFacNodes = model_->nFactorIndicatorVariables();
            std::vector<LpValueType> obj(std::max(nVarNodes,nFacNodes), 0.0);

            ////////////////////////////////////////////
            //  ADD VARIABLES AND OBJECTIVE
            ////////////////////////////////////////////

            // add variable indicator nodes
            obj.resize(nVarNodes);
            for(const auto unary : model_->unaries()){
                const auto var = unary.variable();
                const auto offset = varsNode[var];
                for(DiscreteLabel l=0; l<model_.nLabels(var); ++l)
                    objUnaries[offset + l] += unary->eval(l);
            }
            if( options_.integerConstraint_)
                lpSolver_->addVariables(lp::LpVariableType::Binary,obj, 0, 1);
            else
                lpSolver_->addVariables(lp::LpVariableType::Continius,obj, 0, 1);


            // add factor indicator nodes
            obj.resize(nFacNodes);
            int64_t c = 0;
            for(const auto  factor : model_->factors()){
                factor->bufferValueTable(obj.data()+c);
                c += factor->size();
            }
            lpSolver_->addVariables(lp::LpVariableType::Continius,obj, 0, 1);


            ////////////////////////////////////////////
            //  ADD LOCAL-POLYTOP CONSTRAINTS
            ////////////////////////////////////////////
            DiscreteLabel minNLabels, maxNLabels;
            model_->minMaxNLabels(minNLabels,maxNLabels);


            std::vector<int64_t>     vars;
             std::vector<uint64_t>    cSizes;
            std::vector<LpValueType> coeffs;
            std::vector<LpValueType> lowerBounds;
            std::vector<LpValueType> upperBounds;

            // var nodes
            for(const auto var : model_->variableDescriptors()){
                const auto  nLabels = model_.nLabels(var)
                lowerBounds.push_back(0.0);
                upperBounds.push_back(1.0);    
                cSizes.push_back(nLabels)
                for(DiscreteLabel l=0; l<nLabels; ++l){
                    vars.push_back(varsNode[var]+l);
                    coeffs.push_back(1.0);
                }
            }

            // fac nodes
            c = 0;
            for(const auto factor : model_->factors()){
                const auto arity = factor->arity();
                const auto size = factor->size();
                marray::Marray<size_t> temp(factor.shapeBegin(), factor.shapeEnd());
                for(auto auto & mit :tmp)
                    mit = c++;
                for(size_t v=0; v<arity ++v){
                    const auto var = factor->variable(v);
                    for(DiscreteLabel l=0; l<model_->nLabels(var); ++l){

                        
                        auto view = temp.boundView(v, l);

                        lowerBounds.push_back(0.0);
                        upperBounds.push_back(0.0); 
                        cSizes.push_back(view.size()+1);
                        vars.push_back(varsNode[node]+l);
                        coeffs.push_back(-1.0);

                        for(const auto vit : view){
                            vars.push_back(vit);
                            coeffs.push_back(1.0);
                        }
                    }
                }
            }
            auto nC = lowerBounds.size();
            lpSolver_->addConstraints(nC, cSizes.data(), vars.data(),
                                      coeffs.data(), lowerBounds.data(),
                                      upperBounds.data());
        }

        ~LpInference(){
            delete lpSolver_;
        }

        // MUST HAVE INTERACE
        virtual std::string name() const {
            return "LpInference";
        }
        // inference
        virtual void infer( Visitor  * visitor  = NULL) {


            if(visitor!=NULL)
                visitor->begin(this);

           

            if(visitor!=NULL)
                visitor->end(this);
        }

        // get result
        virtual void conf(Conf & confMap ) {
          
        }
        virtual DiscreteLabel label(const Vi vi ) {
            
        }

    

        // get model
        virtual const Model & model() const{
            return *model_;
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
        Model *       model_;
        Options       options_;
        bool          stopInference_;
        ValueType     bound_;
        ValueType     value_;

        LpSolverBase * lpSolver_;
    };


} // end namespace inference
} // end namespace inferno

#endif /* INFERNO_INFERENCE_LP_INFERENCE_HXX */
