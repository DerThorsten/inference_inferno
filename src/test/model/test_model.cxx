#define BOOST_TEST_MODULE ModelTest
#include <boost/test/unit_test.hpp>
#include "inferno_test/test.hxx"

#include <random>
#include <iostream>



#include "inferno/model/general_discrete_model.hxx"
#include "inferno/model/general_discrete_tl_model.hxx"
#include "inferno/model/sparse_discrete_model.hxx"
#include "inferno/model/implicit_multicut_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/new_unary.hxx"





/*
BOOST_AUTO_TEST_CASE(TestImplicitMulticutModel)
{
    using namespace inferno;


    const Vi nVar = 4;

    //  0 | 1
    //  _ | _
    //  2 | 3
    BOOST_TEST_CHECKPOINT("construct model");
    models::ImplicitMulticutModel model(nVar);


    BOOST_TEST_CHECKPOINT("minVarId");
    BOOST_CHECK_EQUAL(model.minVarId(),0);
    BOOST_TEST_CHECKPOINT("maxVarId");
    BOOST_CHECK_EQUAL(model.maxVarId(),3);
    
    BOOST_TEST_CHECKPOINT("addFactor0");     
    model.addFactor(0,1, 1.0);
    BOOST_TEST_CHECKPOINT("addFactor1");     
    model.addFactor(2,3, 1.0);
    BOOST_TEST_CHECKPOINT("addFactor2");     
    model.addFactor(0,2, 1.0);
    BOOST_TEST_CHECKPOINT("addFactor3");     
    model.addFactor(1,3, 1.0);

    BOOST_TEST_CHECKPOINT("minFactorId");     
    BOOST_CHECK_EQUAL(model.minFactorId(),0);
    BOOST_TEST_CHECKPOINT("maxFactorId");     
    BOOST_CHECK_EQUAL(model.maxFactorId(),3);

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

}


BOOST_AUTO_TEST_CASE(TestDifferentTypesGrid)
{
    using namespace inferno;
    typedef models::TlModel<
        value_tables::UnaryValueTable,
        value_tables::PottsValueTable
    > TlModel;
    typedef models::GeneralDiscreteModel VModel;
    typedef models::SparseDiscreteGraphicalModel  SModel;

    const LabelType nLabes = 3;
    const DiscreteLabel gridShape[2] = {3l,3l};
    const Vi nVar = gridShape[0]*gridShape[1];

    TlModel tlModel(nVar, nLabes);
    VModel  vModel(nVar, nLabes);
}

*/

BOOST_AUTO_TEST_CASE(TestGeneralDiscreteModel_1)
{
    using namespace inferno;

    const LabelType nLabes = 2;
    const Vi nVar = 10;

    models::GeneralDiscreteModel model(nVar, nLabes);
    

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
        auto vti = model.addUnaryValueTable(new value_tables::UnaryValueTable(values.begin(), values.end()) );
        auto fi = model.addUnary(vti ,vi);
    }

    // second order
    for(Vi vi=0; vi<nVar-1; ++vi){
        auto beta = generator( );
        auto vti = model.addValueTable(new value_tables::PottsValueTable(nLabes, beta));
        auto fi = model.addFactor(vti ,{vi, vi+1});

        const auto factor = model.factor(fi);

    }
}



BOOST_AUTO_TEST_CASE(TestGeneralDiscreteModel_2)
{
    using namespace inferno;

    const LabelType nLabes = 2;
    const Vi nVar = 2;

    models::GeneralDiscreteModel model(nVar, nLabes);
    
    auto v0 = new value_tables::UnaryValueTable({1.0,2.0});
    auto v1 = new value_tables::UnaryValueTable({3.0,7.0});
    auto v01 = new value_tables::PottsValueTable(2, 3.0);

    auto i0  = model.addUnaryValueTable(v0);
    auto i1  = model.addUnaryValueTable(v1);
    auto i01 = model.addValueTable(v01);

    auto u0 = model.addUnary(i0, 0);
    auto u1 = model.addUnary(i1, 1);
    auto f10 = model.addFactor(i01, {0,1});


    CHECK_FACTOR_NUM_LABELS_SANITY(model);

    BOOST_CHECK_EQUAL(model.nFactors(),1);
    BOOST_CHECK_EQUAL(model.nUnaries(),2);

    BOOST_CHECK_EQUAL(model.unary(0)->variable(),0);
    BOOST_CHECK_EQUAL(model.unary(1)->variable(),1);

    BOOST_CHECK_EQUAL(model.factor(0)->variable(0),0);
    BOOST_CHECK_EQUAL(model.factor(0)->variable(1),1);

    {
        DiscreteLabel conf[2] = {0,0};
        BOOST_CHECK_CLOSE(model.eval(conf),4.0, TEST_EPS);
    }
    {
        DiscreteLabel conf[2] = {0,1};
        BOOST_CHECK_CLOSE(model.eval(conf),11.0, TEST_EPS);
    }
    {
        DiscreteLabel conf[2] = {1,1};
        BOOST_CHECK_CLOSE(model.eval(conf),9.0, TEST_EPS);
    }
    {
        DiscreteLabel conf[2] = {1,0};
        BOOST_CHECK_CLOSE(model.eval(conf),8.0, TEST_EPS);
    }




}