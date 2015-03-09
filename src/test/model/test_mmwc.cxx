#define BOOST_TEST_MODULE ModelTest
#include <boost/test/unit_test.hpp>

#include <random>
#include <iostream>


#include "inferno/model/modified_multiwaycut_model.hxx"

#define TEST_EPS 0.00001





BOOST_AUTO_TEST_CASE(TestImplicitMulticutModel)
{
    using namespace inferno;


    const Vi nVar = 4;

    //  0 | 1
    //  _ | _
    //  2 | 3
    BOOST_TEST_CHECKPOINT("construct model");
    models::ModifiedMultiwaycutModel model(nVar, 3);


    BOOST_TEST_CHECKPOINT("minVarId");
    BOOST_CHECK_EQUAL(model.minVarId(),0);
    BOOST_TEST_CHECKPOINT("maxVarId");
    BOOST_CHECK_EQUAL(model.maxVarId(),3);
    
    BOOST_TEST_CHECKPOINT("addPottsFactor0");     
    model.addPottsFactor(0,1, -1.0);
    BOOST_TEST_CHECKPOINT("addPottsFactor1");     
    model.addPottsFactor(2,3, -1.0);
    BOOST_TEST_CHECKPOINT("addPottsFactor2");     
    model.addPottsFactor(0,2, -1.0);
    BOOST_TEST_CHECKPOINT("addPottsFactor3");     
    model.addPottsFactor(1,3, -1.0);

   
    BOOST_TEST_CHECKPOINT("minFactorId");     
    BOOST_CHECK_EQUAL(model.minFactorId(),0);
    BOOST_TEST_CHECKPOINT("maxFactorId");     
    BOOST_CHECK_EQUAL(model.maxFactorId(),3);


    // add unaries (var 2 does not get an unary )
    model.addUnaryFactor(0, {-1.0, 0.0, 0.0});
    model.addUnaryFactor(1, { 0.0, 0.0, 0.0});
    model.addUnaryFactor(3, {-1.0, 1.0, 0.0});


    BOOST_TEST_CHECKPOINT("minFactorId");     
    BOOST_CHECK_EQUAL(model.minFactorId(),0);
    BOOST_TEST_CHECKPOINT("maxFactorId");     
    BOOST_CHECK_EQUAL(model.maxFactorId(),6);


    #if 0
    BOOST_TEST_CHECKPOINT("factorLoop");     
    for(auto fiter=model.factorIdsBegin(); fiter != model.factorIdsEnd(); ++fiter){
        //std::cout<<*fiter<<" "<<io::varibleIds(model[*fiter])<<"\n"<<io::valueTable(model[*fiter])<<"\n";
    }
    
    BOOST_TEST_CHECKPOINT("modelEval");
    //  0 | 0
    //  _ | _
    //  0 | 0
    {
        DiscreteLabel conf[4] = {0,0,0,0};
        BOOST_CHECK_CLOSE(model.eval(conf), 0.0, TEST_EPS);
    }
    
    //  1 | 1
    //  _ | _
    //  1 | 1
    {
        DiscreteLabel conf[4] = {1,1,1,1};
        BOOST_CHECK_CLOSE(model.eval(conf), 0.0, TEST_EPS);
    }

    //  1 | 1
    //  _ | _
    //  0 | 0
    {
        DiscreteLabel conf[4] = {1,1,0,0};
        BOOST_CHECK_CLOSE(model.eval(conf), 2.0, TEST_EPS);
    }

    //  2 | 0
    //  _ | _
    //  0 | 0
    {
        DiscreteLabel conf[4] = {2,0,0,0};
        BOOST_CHECK_CLOSE(model.eval(conf), 2.0, TEST_EPS);
    }

    //  1 | 2
    //  _ | _
    //  1 | 0
    {
        DiscreteLabel conf[4] = {1,2,1,0};
        BOOST_CHECK_CLOSE(model.eval(conf), 3.0, TEST_EPS);
    }

    //  3 | 2
    //  _ | _
    //  1 | 0
    {
        DiscreteLabel conf[4] = {3,2,1,0};
        BOOST_CHECK_CLOSE(model.eval(conf), 4.0, TEST_EPS);
    }
    
    #endif
}


