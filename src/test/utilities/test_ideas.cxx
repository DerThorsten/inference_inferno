#define BOOST_TEST_MODULE MovemakerTest
#include <boost/test/unit_test.hpp>

#include <random>
#include <iostream>

#include "inferno/model/general_discrete_model.hxx"
#include "inferno/inference/utilities/movemaker.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"
#include "inferno/value_tables/explicit.hxx"

#include "inferno/inferno_keywords.hxx"

#include <boost/parameter/name.hpp>
#include <boost/parameter/preprocessor.hpp>
#include <string>

#define TEST_EPS 0.00001





BOOST_AUTO_TEST_CASE(TestMove)
{
    using namespace inferno;
    using namespace inferno::keywords;

    typedef std::tuple<
        tag::temperature
    > Tuple;

    std::cout<<tag::temperature::keyword_name<<"\n";

    
}
