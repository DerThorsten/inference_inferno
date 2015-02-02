#define BOOST_TEST_MODULE ModelTest
#include <boost/test/unit_test.hpp>

#include <random>
#include <iostream>

#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"
#include "inferno/model/implicit_multicut_model.hxx"
#define TEST_EPS 0.00001







BOOST_AUTO_TEST_CASE(TestImplicitMulticutModel)
{
    using namespace inferno;


    const Vi nVar = 4;

    //  0 | 1
    //  _ | _
    //  2 | 3

    ImplicitMulticutModel model(nVar);

    BOOST_CHECK_EQUAL(model.minVarId(),0);
    BOOST_CHECK_EQUAL(model.maxVarId(),3);
        
    model.addFactor(0,1, 1.0);
    model.addFactor(2,3, 1.0);
    model.addFactor(0,2, 1.0);
    model.addFactor(1,3, 1.0);

    BOOST_CHECK_EQUAL(model.minFactorId(),0);
    BOOST_CHECK_EQUAL(model.maxFactorId(),3);

    for(auto fiter=model.factorIdsBegin(); fiter != model.factorIdsEnd(); ++fiter){
        //std::cout<<*fiter<<" "<<io::varibleIds(model[*fiter])<<"\n"<<io::valueTable(model[*fiter])<<"\n";
    }
    

    //  0 | 0
    //  _ | _
    //  0 | 0
    BOOST_CHECK_CLOSE(model.eval({0,0,0,0}), 0.0, TEST_EPS);
    //  1 | 1
    //  _ | _
    //  1 | 1
    BOOST_CHECK_CLOSE(model.eval({1,1,1,1}), 0.0, TEST_EPS);

    //  1 | 1
    //  _ | _
    //  0 | 0
    BOOST_CHECK_CLOSE(model.eval({1,1,0,0}), 2.0, TEST_EPS);

    //  2 | 0
    //  _ | _
    //  0 | 0
    BOOST_CHECK_CLOSE(model.eval({2,0,0,0}), 2.0, TEST_EPS);

    //  1 | 2
    //  _ | _
    //  1 | 0
    BOOST_CHECK_CLOSE(model.eval({1,2,1,0}), 3.0, TEST_EPS);

    //  3 | 2
    //  _ | _
    //  1 | 0
    BOOST_CHECK_CLOSE(model.eval({3,2,1,0}), 4.0, TEST_EPS);

}



BOOST_AUTO_TEST_CASE(TestModel)
{
    using namespace inferno;

    const LabelType nLabes = 2;
    const Vi nVar = 10;

    GeneralDiscreteGraphicalModel model(nVar, nLabes);
    

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1, 1);



    
    std::uniform_real_distribution<float> distribution(-1,1); //Values between -1 and 1
    std::mt19937 engine; // Mersenne twister MT19937
    auto generator = std::bind(distribution, engine);
   
    std::vector<ValueType> values(nLabes); 
    // unary factors


    
    for(Vi vi=0; vi<nVar; ++vi){
        for(auto & v : values)
            v = generator();
        auto vti = model.addValueTable(new value_tables::UnaryValueTable(values.begin(), values.end()) );
        auto fi = model.addFactor(vti ,{vi});
    }

    // second order
    for(Vi vi=0; vi<nVar-1; ++vi){
        auto beta = generator( );
        auto vti = model.addValueTable(new value_tables::PottsValueTable(nLabes, beta));
        auto fi = model.addFactor(vti ,{vi, vi+1});

        const auto factor = model[fi];

    }


    for(auto fiter=model.factorIdsBegin(); fiter != model.factorIdsEnd(); ++fiter){
        //std::cout<<*fiter<<" "<<io::varibleIds(model[*fiter])<<"\n"<<io::valueTable(model[*fiter])<<"\n";
    }
    

}
