#define BOOST_TEST_MODULE ModelTest
#include <boost/test/unit_test.hpp>

#include <random>
#include <iostream>

#include "inferno/model/sparse_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"
#define TEST_EPS 0.00001








BOOST_AUTO_TEST_CASE(TestSparseModel1)
{
    using namespace inferno;

    const LabelType nLabes = 2;
    const Vi nVar = 10;

    models::SparseDiscreteGraphicalModel model;
    
    BOOST_CHECK_EQUAL(model.nVariables(),0);
    BOOST_CHECK_EQUAL(model.nFactors(),0);

    {
        auto viterBegin = model.variableIdsBegin();
        auto viterEnd = model.variableIdsEnd();
        BOOST_CHECK_EQUAL(std::distance(viterBegin, viterEnd), 0);
    }

    model.addVariable(11, nLabes);

    {
        auto viterBegin = model.variableIdsBegin();
        auto viterEnd = model.variableIdsEnd();
        BOOST_CHECK_EQUAL(std::distance(viterBegin, viterEnd), 1);
    }

    BOOST_CHECK_EQUAL(model.nVariables(),1);
    BOOST_CHECK_EQUAL(*(model.variableIdsBegin()), 11);
    
    BOOST_CHECK_EQUAL(model.minVarId(),11);
    BOOST_CHECK_EQUAL(model.maxVarId(),11);
}


BOOST_AUTO_TEST_CASE(TestSparseModelGrid2x2_AddAndErase)
{
    using namespace inferno;
    typedef value_tables::PottsValueTable Pvt;

    const LabelType nLabes = 3;
    const Vi nVar = 10;

    models::SparseDiscreteGraphicalModel model;
    
    BOOST_CHECK_EQUAL(model.nVariables(),0);
    BOOST_CHECK_EQUAL(model.nFactors(),0);

    // 1 | 3
    // -----
    // 5 | 7

    // add variables
    model.addVariable(3, nLabes);
    BOOST_CHECK_EQUAL(model.minVarId(),3);
    BOOST_CHECK_EQUAL(model.maxVarId(),3);
    BOOST_CHECK_EQUAL(model.nVariables(),1);
    BOOST_CHECK_EQUAL(model.nFactors(),0);

    model.addVariable(1, nLabes);
    BOOST_CHECK_EQUAL(model.minVarId(),1);
    BOOST_CHECK_EQUAL(model.maxVarId(),3);
    BOOST_CHECK_EQUAL(model.nVariables(),2);
    BOOST_CHECK_EQUAL(model.nFactors(),0);

    model.addVariable(7, nLabes);
    BOOST_CHECK_EQUAL(model.minVarId(),1);
    BOOST_CHECK_EQUAL(model.maxVarId(),7);
    BOOST_CHECK_EQUAL(model.nVariables(),3);
    BOOST_CHECK_EQUAL(model.nFactors(),0);
    
    model.addVariable(5, nLabes);
    BOOST_CHECK_EQUAL(model.minVarId(),1);
    BOOST_CHECK_EQUAL(model.maxVarId(),7);
    BOOST_CHECK_EQUAL(model.nVariables(),4);
    BOOST_CHECK_EQUAL(model.nFactors(),0);



    // add two potts functions (horizontal (1) and vertical (3) )
    const auto ph = model.addValueTable(1,new Pvt(nLabes, 0.5));
    const auto pv = model.addValueTable(3,new Pvt(nLabes, 1.5));
    BOOST_CHECK_EQUAL(model.minVarId(),1);
    BOOST_CHECK_EQUAL(model.maxVarId(),7);
    BOOST_CHECK_EQUAL(model.nVariables(),4);
    BOOST_CHECK_EQUAL(model.nFactors(),0);


    // add h potts factors
    auto fi_p13 = model.addFactor(ph, 11,  {1l, 3l});
    BOOST_CHECK_EQUAL(model.factor(fi_p13)->variable(0),1);
    BOOST_CHECK_EQUAL(model.factor(fi_p13)->variable(1),3);
    BOOST_CHECK_EQUAL(model.minVarId(),1);
    BOOST_CHECK_EQUAL(model.maxVarId(),7);
    BOOST_CHECK_EQUAL(model.minFactorId(),11);
    BOOST_CHECK_EQUAL(model.maxFactorId(),11);
    BOOST_CHECK_EQUAL(model.nVariables(),4);
    BOOST_CHECK_EQUAL(model.nFactors(),1);

    auto fi_p57 = model.addFactor(ph, 9, {5l, 7l});
    BOOST_CHECK_EQUAL(model.factor(fi_p57)->variable(0),5);
    BOOST_CHECK_EQUAL(model.factor(fi_p57)->variable(1),7);
    BOOST_CHECK_EQUAL(model.minVarId(),1);
    BOOST_CHECK_EQUAL(model.maxVarId(),7);
    BOOST_CHECK_EQUAL(model.minFactorId(),9);
    BOOST_CHECK_EQUAL(model.maxFactorId(),11);
    BOOST_CHECK_EQUAL(model.nVariables(),4);
    BOOST_CHECK_EQUAL(model.nFactors(),2);

    // add v potts factors
    auto fi_p15 = model.addFactor(pv, 13,  {1l, 5l});
    BOOST_CHECK_EQUAL(model.factor(fi_p15)->variable(0),1);
    BOOST_CHECK_EQUAL(model.factor(fi_p15)->variable(1),5);
    BOOST_CHECK_EQUAL(model.minVarId(),1);
    BOOST_CHECK_EQUAL(model.maxVarId(),7);
    BOOST_CHECK_EQUAL(model.minFactorId(),9);
    BOOST_CHECK_EQUAL(model.maxFactorId(),13);
    BOOST_CHECK_EQUAL(model.nVariables(),4);
    BOOST_CHECK_EQUAL(model.nFactors(),3);

    auto fi_p37 = model.addFactor(pv, 15, {3l, 7l});
    BOOST_CHECK_EQUAL(model.factor(fi_p37)->variable(0),3);
    BOOST_CHECK_EQUAL(model.factor(fi_p37)->variable(1),7);
    BOOST_CHECK_EQUAL(model.minVarId(),1);
    BOOST_CHECK_EQUAL(model.maxVarId(),7);
    BOOST_CHECK_EQUAL(model.minFactorId(),9);
    BOOST_CHECK_EQUAL(model.maxFactorId(),15);
    BOOST_CHECK_EQUAL(model.nVariables(),4);
    BOOST_CHECK_EQUAL(model.nFactors(),4);


    BOOST_CHECK_EQUAL(model.nConnectedFactorsForVti(ph),2);
    BOOST_CHECK_EQUAL(model.nConnectedFactorsForVti(pv),2);

    BOOST_CHECK_EQUAL(model.nConnectedFactorsForVi(1),2);
    BOOST_CHECK_EQUAL(model.nConnectedFactorsForVi(5),2);
    BOOST_CHECK_EQUAL(model.nConnectedFactorsForVi(7),2);
    BOOST_CHECK_EQUAL(model.nConnectedFactorsForVi(3),2);

    // erase a variable 1
    // this will remove fac 11 and 13
    BOOST_TEST_CHECKPOINT("erase var 1");
    model.eraseVariable(1);
    BOOST_CHECK_EQUAL(model.minVarId(),3);
    BOOST_CHECK_EQUAL(model.maxVarId(),7);
    BOOST_CHECK_EQUAL(model.minFactorId(),9);
    BOOST_CHECK_EQUAL(model.maxFactorId(),15);
    BOOST_CHECK_EQUAL(model.nVariables(),3);
    BOOST_CHECK_EQUAL(model.nFactors(),2);

    //     3
    //   ---
    // 5 | 7
    BOOST_CHECK_EQUAL(model.nConnectedFactorsForVti(ph),1);
    BOOST_CHECK_EQUAL(model.nConnectedFactorsForVti(pv),1);

    BOOST_CHECK_EQUAL(model.nConnectedFactorsForVi(5),1);
    BOOST_CHECK_EQUAL(model.nConnectedFactorsForVi(7),2);
    BOOST_CHECK_EQUAL(model.nConnectedFactorsForVi(3),1);
    BOOST_CHECK_EQUAL(model.factorsVti(fi_p37),pv);
    BOOST_CHECK_EQUAL(model.factorsVti(fi_p57),ph);

    BOOST_CHECK_EQUAL(model.factor(fi_p37)->variable(0),3);
    BOOST_CHECK_EQUAL(model.factor(fi_p37)->variable(1),7);
    BOOST_CHECK_EQUAL(model.factor(fi_p57)->variable(0),5);
    BOOST_CHECK_EQUAL(model.factor(fi_p57)->variable(1),7);

    // erase a variable 5
    // this will remove fac 9
    BOOST_TEST_CHECKPOINT("erase var 5");
    model.eraseVariable(5);
    BOOST_CHECK_EQUAL(model.minVarId(),3);
    BOOST_CHECK_EQUAL(model.maxVarId(),7);
    BOOST_CHECK_EQUAL(model.minFactorId(),15);
    BOOST_CHECK_EQUAL(model.maxFactorId(),15);
    BOOST_CHECK_EQUAL(model.nVariables(),2);
    BOOST_CHECK_EQUAL(model.nFactors(),1);
    
}
