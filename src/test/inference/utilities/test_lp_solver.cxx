#define BOOST_TEST_MODULE LpSolverTest
#include <boost/test/unit_test.hpp>
#include "inferno_test/test.hxx"

#include <random>
#include <iostream>

#include "inferno/inference/utilities/lp/lp_solver_base.hxx"



BOOST_AUTO_TEST_CASE(TestCompile_1)
{
   
    using namespace inferno;

    typedef inference::lp::LpSolverBase LpSolver;
    typedef inference::lp::LpValueType LpValueType;


    LpSolver *  solver = new inference::lp::LpSolverGurobi();


    std::vector<LpValueType> obj = {-1.0, 0.0, 1.0};

    solver->addVariables(inference::lp::LpVariableType::Binary, obj, 0, 1);
    solver->updateVariablesAdded();

    solver->optimize();

    auto value = solver->objectiveValue();
    auto bound = solver->objectiveBound();

    auto state0 = solver->state(0);
    auto state1 = solver->state(1);
    auto state2 = solver->state(2);


}
