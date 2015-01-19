#define BOOST_TEST_MODULE ModelTest
#include <boost/test/unit_test.hpp>


#include "inferno/model.hxx"

#define TEST_EPS 0.00001

BOOST_AUTO_TEST_CASE(TestModel)
{
    using namespace inferno;
    SharedDiscreteModelPtr dmodel(new ExplicitDiscreteModel(10, DiscreteLabelBounds(0,1)));
    SharedModelPtr model = dmodel;
}

