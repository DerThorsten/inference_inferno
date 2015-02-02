#define BOOST_TEST_MODULE ModelTest
#include <boost/test/unit_test.hpp>

#include <random>
#include <iostream>

#include "inferno/model/sparse_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"
#define TEST_EPS 0.00001








BOOST_AUTO_TEST_CASE(TestSparseModel)
{
    using namespace inferno;

    const LabelType nLabes = 2;
    const Vi nVar = 10;

    SparseDiscreteGraphicalModel model;
    
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
