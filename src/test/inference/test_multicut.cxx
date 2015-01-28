#define BOOST_TEST_MODULE MulticutTest
#include <boost/test/unit_test.hpp>

#include <random>
#include <iostream>

#include "inferno/model/implicit_multicut_model.hxx"
#include "inferno/inference/multicut.hxx"
#define TEST_EPS 0.00001



BOOST_AUTO_TEST_CASE(TestMulticutInference)
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
}



