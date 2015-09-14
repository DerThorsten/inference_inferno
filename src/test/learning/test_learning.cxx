#define BOOST_TEST_MODULE LearningTest
#include <boost/test/unit_test.hpp>
#include "inferno_test/test.hxx"

#include <random>
#include <iostream>



#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"




BOOST_AUTO_TEST_CASE(TestLearning_1)
{
    using namespace inferno;
    auto weightVector = inferno::learning::WeightVector(10);

    BOOST_CHECK_EQUAL(weightVector.size(), 10);
    weightVector = 0;
    BOOST_CHECK_EQUAL(weightVector.size(), 10);
}





