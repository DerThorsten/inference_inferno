#ifndef INFERNO_INFERENCE_UTILITIES_LP_SOLVER_BASE_HXX
#define INFERNO_INFERENCE_UTILITIES_LP_SOLVER_BASE_HXX

#include <unordered_map>

#include "inferno/model/discrete_model_base.hxx"
#include "inferno/inference/discrete_inference_base.hxx"
#include "inferno/utilities/linear_constraint.hxx"

#include <ilcplex/ilocplex.h>
#include <gurobi_c++.h>



namespace inferno{
namespace inference{
namespace lp{

    typedef utilities::LinearConstraint<uint64_t, double, double> LpConstraint;

    enum class LpVariableType {
        Continous,
        Binary, 
        Integer
    }; 

   

    class LpOptions{
        LpOptions(
            const bool minimize = true
        )
        : minimize_(minimize){

        }
    public:

        bool minimize_;

    };



    class LpSolverBase{
    public:

        virtual void setOptions(const LpOptions & options) = 0;

        virtual int64_t addVariables(
            const LpVariableType varType, 
             const LpValueType * objective, 
             const uint64_t    nVars,
            const LpValueType lowerBound, 
            const LpValueType upperBound
        ) = 0;

        virtual int64_t addVariables(
            const LpVariableType varType, 
            const std::vector<LpValueType> & objective,
            const LpValueType lowerBound, 
            const LpValueType upperBound
        ) {
            return this->addVariables(varType, objective.data(), 
                                      objective.size(), lowerBound, 
                                      upperBound);
        }



        virtual void addConstraints(
            const uint64_t nConstraints,
            const uint64_t * constraintSizes,
            const int64_t  * variables,
            const LpValueType * coefficients,
            const LpValueType * lowerBounds,
            const LpValueType * upperBounds
        ) = 0;




        virtual int64_t addConstraint(
           const int64_t *         variables,
           const LpValueType *     coefficients,
           const uint64_t          constraintSize,
           const LpValueType    lowerBound,
           const LpValueType    upperBound
        ) = 0;

        virtual void updateVariablesAdded() = 0;
        virtual void updateConstraintsAdded() = 0;
        virtual void optimize() = 0;
        virtual LpValueType objectiveValue()const = 0;
        virtual LpValueType objectiveBound()const = 0;

        virtual LpValueType state(int64_t variable) const = 0;
    };


    class LpSolverGurobi : public LpSolverBase{
    public:

        LpSolverGurobi()
        :   env_(),
            model_(env_){
        }


        virtual void setOptions(const LpOptions & options) override{

        }

        virtual int64_t addVariables(
             const LpVariableType varType, 
             const LpValueType * objective, 
             const uint64_t    nVar,
             const LpValueType lowerBound, 
             const LpValueType upperBound
        )
        {
            char vt = GRB_CONTINUOUS;
            if(varType == LpVariableType::Binary)
                vt = GRB_BINARY;
            else if(varType == LpVariableType::Integer)
                vt = GRB_INTEGER;

            // TODO speedupds are possible by setting some pointers to NULL 
            // and use the gurobi defaults
            std::vector<LpValueType> lb(nVar, lowerBound);
            std::vector<LpValueType> ub(nVar, upperBound);
            std::vector<char> vtype(nVar,vt);
            auto vars = model_.addVars(&lb[0],&ub[0],objective,&vtype[0],NULL,nVar);
            for(uint64_t v=0; v<nVar; ++v){
                variables_.push_back(vars[v]); 
            }
            delete vars;
            auto ret = nVariables_;
            nVariables_ += nVar;
            return ret;
        }


        virtual void addConstraints(
            const uint64_t nConstraints,
            const uint64_t * constraintSizes,
            const int64_t  * variables,
            const LpValueType * coefficients,
            const LpValueType * lowerBounds,
            const LpValueType * upperBounds
        ) override {

            auto vars   = variables;
            auto coeffs = coefficients;

            for(size_t i=0; i<nConstraints; ++i){
                auto size = constraintSizes[i];

                GRBLinExpr expr;
                for(size_t j=0; j<size; ++j){
                    expr += variables_[vars[j]] * coeffs[i];
                }
                this->addConstraint(expr, lowerBounds[i], upperBounds[i]);
                vars += size;
                coeffs += size;
            }
        }



        virtual int64_t addConstraint(
            const int64_t *         variables,
            const LpValueType *     coefficients,
            const uint64_t          constraintSize,
            const LpValueType    lowerBound,
            const LpValueType    upperBound
        ) override{
            GRBLinExpr expr;
            for(size_t i=0; i<constraintSize; ++i){
                expr += variables_[variables[i]] * coefficients[i];
            }

            this->addConstraint(expr, lowerBound, upperBound);
        }

        virtual void updateVariablesAdded() override{
            model_.update();
        }

        virtual void updateConstraintsAdded() override{
            model_.update();
        }

        virtual void optimize()override{
            try {
                model_.optimize();
            }
            catch(GRBException e) {
                //std::cout << "Error code = " << e.getErrorCode() << "\n";
                //std::cout << e.getMessage() <<"\n";
                throw RuntimeError("Gurobi Error");
            } catch(...) {
                throw RuntimeError("Gurobi Error");
            }
        }

        virtual LpValueType objectiveValue()const override{
            return model_.get(GRB_DoubleAttr_ObjVal);
        }

        virtual LpValueType objectiveBound()const override{
            return model_.get(GRB_DoubleAttr_ObjBound);
        }

        virtual LpValueType state(int64_t variable) const override{
            return variables_[variable].get(GRB_DoubleAttr_X);
        }

    private:

        void addConstraint(
            const GRBLinExpr & expr,
            const LpValueType lowerBound,
            const LpValueType upperBound
        ){
            if(floatEqual(lowerBound, upperBound))
                model_.addConstr(expr, GRB_EQUAL, lowerBound, NULL);
            else{
                if(lowerBound > -1.0*infVal())
                    model_.addConstr(expr, GRB_GREATER_EQUAL, lowerBound, NULL);
                if(upperBound < infVal())
                    model_.addConstr(expr, GRB_LESS_EQUAL, upperBound, NULL);
            }
        }

        GRBEnv                       env_;
        GRBModel                     model_; 
        uint64_t                     nVariables_;
        std::vector<GRBVar>          variables_;
    };





    

}
}
}



#endif /* INFERNO_INFERENCE_UTILITIES_LP_SOLVER_BASE_HXX */
