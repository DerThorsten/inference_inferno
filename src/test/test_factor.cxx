#define BOOST_TEST_MODULE FactorTest
#include <boost/test/unit_test.hpp>

#include "inferno/factor.hxx"
#include "inferno/model.hxx"
#include "inferno/model_fac_info.hxx"
#include "inferno/model_var_info.hxx"

#define TEST_EPS 0.00001

BOOST_AUTO_TEST_CASE(TestCasting)
{
    using namespace inferno;
    DiscreteFactor * df =  new TwoClassUnary(0, 0.0, 1.0);
    BOOST_CHECK_EQUAL(df->arity(),1);

    Factor * f  = df;
    BOOST_CHECK_EQUAL(f->arity(),1);
}


BOOST_AUTO_TEST_CASE(TestCasting2)
{
    using namespace inferno;
    Factor * df =  new TwoClassUnary(0, 0.0, 1.0);
    BOOST_CHECK_EQUAL(df->arity(),1);

    DiscreteFactor * f  = dynamic_cast<DiscreteFactor *>(df);
    BOOST_CHECK_EQUAL(f->arity(),1);
    BOOST_CHECK_EQUAL(f!=NULL, true);
}


BOOST_AUTO_TEST_CASE(TestCasting3)
{
    using namespace inferno;
    Factor * df =  new TwoClassUnary(0, 0.0, 1.0);
    BOOST_CHECK_EQUAL(df->arity(),1);

    ContinousFactor * f  = dynamic_cast<ContinousFactor *>(df);
    BOOST_CHECK_EQUAL(f==NULL,true);
}

