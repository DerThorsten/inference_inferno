#define BOOST_TEST_MODULE MulticutTest
#include <boost/test/unit_test.hpp>

#include <random>
#include <iostream>

#include "inferno/model/implicit_multicut_model.hxx"
#include "inferno/inference/multicut.hxx"
#define TEST_EPS 0.00001



BOOST_AUTO_TEST_CASE(TestMulticutInferenceTrivial1)
{
    using namespace inferno;


    const Vi nVar = 4;

    //  0 | 1
    //  _ | _
    //  2 | 3

    ImplicitMulticutModel model(nVar);
        
    model.addFactor(0,1, 1.0);
    model.addFactor(2,3, 1.0);
    model.addFactor(0,2, 1.0);
    model.addFactor(1,3, 1.0);

    for(auto fiter=model.factorIdsBegin(); fiter != model.factorIdsEnd(); ++fiter){
        std::cout<<*fiter<<" "<<io::varibleIds(model[*fiter])<<"\n"<<io::valueTable(model[*fiter])<<"\n";
    }

    typedef ImplicitMulticutModel       Model;
    typedef inference::Multicut<Model>  Solver;
    typedef Solver::Options             SolverOptions; 

    SolverOptions options;
    Solver solver(model, options);
    solver.infer();

    DiscreteLabel conf[4];
    solver.conf(conf);

    BOOST_CHECK_EQUAL(conf[0],conf[1]);
    BOOST_CHECK_EQUAL(conf[0],conf[2]);
    BOOST_CHECK_EQUAL(conf[0],conf[3]);
}



BOOST_AUTO_TEST_CASE(TestMulticutInferenceTrivial2)
{
    using namespace inferno;


    const Vi nVar = 4;

    //  0 | 1
    //  _ | _
    //  2 | 3

    ImplicitMulticutModel model(nVar);
        
    model.addFactor(0,1, -1.0);
    model.addFactor(2,3, -1.0);
    model.addFactor(0,2, 1.0);
    model.addFactor(1,3, 1.0);

    for(auto fiter=model.factorIdsBegin(); fiter != model.factorIdsEnd(); ++fiter){
        std::cout<<*fiter<<" "<<io::varibleIds(model[*fiter])<<"\n"<<io::valueTable(model[*fiter])<<"\n";
    }

    typedef ImplicitMulticutModel       Model;
    typedef inference::Multicut<Model>  Solver;
    typedef Solver::Options             SolverOptions; 

    SolverOptions options;
    Solver solver(model, options);
    solver.infer();

    DiscreteLabel conf[4];
    solver.conf(conf);

    BOOST_CHECK_EQUAL(conf[0],conf[2]);
    BOOST_CHECK_EQUAL(conf[1],conf[3]);

    BOOST_CHECK(conf[0] != conf[1]);
    BOOST_CHECK(conf[2] != conf[3]);
}


BOOST_AUTO_TEST_CASE(TestMulticutInferenceTrivial3)
{
    using namespace inferno;


    const Vi nVar = 4;

    //  0 | 1
    //  _ | _
    //  2 | 3

    ImplicitMulticutModel model(nVar);
        
    model.addFactor(0,1, -1.0);
    model.addFactor(2,3, 0.5);
    model.addFactor(0,2, 1.0);
    model.addFactor(1,3, 1.0);

    for(auto fiter=model.factorIdsBegin(); fiter != model.factorIdsEnd(); ++fiter){
        std::cout<<*fiter<<" "<<io::varibleIds(model[*fiter])<<"\n"<<io::valueTable(model[*fiter])<<"\n";
    }

    typedef ImplicitMulticutModel       Model;
    typedef inference::Multicut<Model>  Solver;
    typedef Solver::Options             SolverOptions; 

    SolverOptions options;
    Solver solver(model, options);
    solver.infer();

    DiscreteLabel conf[4];
    solver.conf(conf);

    BOOST_CHECK_EQUAL(conf[0],conf[2]);
    BOOST_CHECK_EQUAL(conf[1],conf[3]);

    BOOST_CHECK(conf[0] != conf[1]);
    BOOST_CHECK(conf[2] != conf[3]);
}

BOOST_AUTO_TEST_CASE(TestMulticutInferenceTrivial4)
{
    using namespace inferno;


    const Vi nVar = 4;

    //  0 | 1
    //  _ | _
    //  2 | 3

    ImplicitMulticutModel model(nVar);
        
    model.addFactor(0,1, -0.5);
    model.addFactor(2,3, 1.0);
    model.addFactor(0,2, 1.0);
    model.addFactor(1,3, 1.0);

    for(auto fiter=model.factorIdsBegin(); fiter != model.factorIdsEnd(); ++fiter){
        std::cout<<*fiter<<" "<<io::varibleIds(model[*fiter])<<"\n"<<io::valueTable(model[*fiter])<<"\n";
    }

    typedef ImplicitMulticutModel       Model;
    typedef inference::Multicut<Model>  Solver;
    typedef Solver::Options             SolverOptions; 

    SolverOptions options;
    Solver solver(model, options);
    solver.infer();

    DiscreteLabel conf[4];
    solver.conf(conf);

    BOOST_CHECK_EQUAL(conf[0],conf[1]);
    BOOST_CHECK_EQUAL(conf[0],conf[2]);
    BOOST_CHECK_EQUAL(conf[0],conf[3]);
}
