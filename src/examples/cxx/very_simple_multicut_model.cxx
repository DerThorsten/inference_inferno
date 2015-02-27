#include "inferno/inferno.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/model/implicit_multicut_model.hxx"
//#include "inferno/inference/multicut.hxx"

int main(){

   const inferno::Vi nVar = 4;

    //  0 | 1
    //  _ | _
    //  2 | 3

    inferno::ImplicitMulticutModel model(nVar);
        
    model.addFactor(0,1, 1.0);
    model.addFactor(2,3, 1.0);
    model.addFactor(0,2, 1.0);
    model.addFactor(1,3, 1.0);


    typedef inferno::ImplicitMulticutModel          Model;
    //typedef inferno::inference::Multicut<Model>     Solver;
    //typedef Solver::Options                         SolverOptions; 
    //SolverOptions options;
    //Solver solver(model, options);
    //solver.infer();
    //inferno::DiscreteLabel conf[4];
    //solver.conf(conf);
}
