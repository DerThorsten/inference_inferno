#define BOOST_TEST_MODULE ModelTest
#include <boost/test/unit_test.hpp>

#include "inferno/model/base_discrete_factor.hxx"
#define TEST_EPS 0.00001



BOOST_AUTO_TEST_CASE(TestFactor)
{
    using namespace inferno;
    /*
    BOOST_TEST_CHECKPOINT("construct PottsValueTable");
    DiscreteValueTable  * vt = new PottsValueTable(2, 1.0);
    Vi vis[2] = {0, 1};
    BOOST_TEST_CHECKPOINT("construct Factor from PottsValueTable");
    DiscreteFactor f(vis,vis+2, vt);

    LabelType l00[] = {0,0};
    LabelType l01[] = {0,1};
    BOOST_TEST_CHECKPOINT("check virtual call of eval(confptr)");
    BOOST_CHECK_CLOSE(f.valueTable()->eval(l00), 0.0, TEST_EPS);
    BOOST_CHECK_CLOSE(f.valueTable()->eval(l01), 1.0, TEST_EPS);

    BOOST_TEST_CHECKPOINT("check virtual call of eval(l0, l1)");
    BOOST_CHECK_CLOSE(f.valueTable()->eval(0,0), 0.0, TEST_EPS);
    BOOST_CHECK_CLOSE(f.valueTable()->eval(0,1), 1.0, TEST_EPS);

    //BOOST_CHECK_CLOSE(f(0,1), 1.0, TEST_EPS);

    BOOST_TEST_CHECKPOINT("check (maybe) non virtual call of operator (l0, l1)");
    BOOST_CHECK_CLOSE(f(0,0), 0.0, TEST_EPS);
    BOOST_CHECK_CLOSE(f(0,1), 1.0, TEST_EPS);
    */
}

