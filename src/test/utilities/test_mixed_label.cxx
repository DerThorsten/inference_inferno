#define BOOST_TEST_MODULE MixedLabelTest
#include <boost/test/unit_test.hpp>

#include "inferno/utilities/mixed_label.hxx"

#define TEST_EPS 0.00001

BOOST_AUTO_TEST_CASE(EmptyConstructorTest)
{
    inferno::MixedLabel l;
    BOOST_CHECK_EQUAL(l.d(), 0);
    BOOST_CHECK_CLOSE(l.c(), 0.0, TEST_EPS);    
    BOOST_CHECK_EQUAL(l.isDiscrete(), true);
    
}

BOOST_AUTO_TEST_CASE(ConstructorTest)
{
    {
        inferno::MixedLabel l(1);
        BOOST_CHECK_EQUAL(l.d(), 1);
        BOOST_CHECK_EQUAL(inferno::DiscreteLabel(l), 1);
        BOOST_CHECK_CLOSE(l.c(), 1.0, TEST_EPS);  
        BOOST_CHECK_CLOSE(inferno::ContinousLabel(l), 1.0, TEST_EPS);  
        BOOST_CHECK_EQUAL(l.isDiscrete(), true);
    }
    {
        inferno::MixedLabel l(1.5);
        BOOST_CHECK_EQUAL(l.d(), 2);
        BOOST_CHECK_EQUAL(inferno::DiscreteLabel(l), 2);
        BOOST_CHECK_CLOSE(l.c(), 1.5, TEST_EPS);    
        BOOST_CHECK_CLOSE(inferno::ContinousLabel(l), 1.5, TEST_EPS);  
        BOOST_CHECK_EQUAL(l.isDiscrete(), false);
    }
    {
        inferno::MixedLabel l(-1);
        BOOST_CHECK_EQUAL(l.d(), -1);
        BOOST_CHECK_EQUAL(inferno::DiscreteLabel(l), -1);
        BOOST_CHECK_CLOSE(l.c(), -1.0, TEST_EPS);    
        BOOST_CHECK_CLOSE(inferno::ContinousLabel(l), -1.0, TEST_EPS);  
        BOOST_CHECK_EQUAL(l.isDiscrete(), true);
    }
    {
        inferno::MixedLabel l(-1.9999);
        BOOST_CHECK_EQUAL(l.d(), -2);
        BOOST_CHECK_EQUAL(inferno::DiscreteLabel(l), -2);
        BOOST_CHECK_CLOSE(l.c(), -1.9999, TEST_EPS);    
        BOOST_CHECK_CLOSE(inferno::ContinousLabel(l), -1.9999, TEST_EPS);  
        BOOST_CHECK_EQUAL(l.isDiscrete(), false);
    }
}
