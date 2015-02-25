#define BOOST_TEST_MODULE ValueTableTest

#include <boost/test/unit_test.hpp>
#include "inferno_test/test.hxx"

#include "inferno/inferno.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"
#include "inferno/value_tables/explicit.hxx"


typedef inferno::value_tables::DiscreteValueTableBase Vtb;
typedef inferno::ValueMarray Marray;
typedef inferno::ValueMarrayView MarrayView;


BOOST_AUTO_TEST_CASE(TestUnaryValueTable)
{
    using namespace inferno;

    typedef value_tables::UnaryValueTable Vt;

    {
        // to test
        Vtb * vt = new Vt({2.5, 5.0 , 7.0});

        // true table
        Marray trueVt({3});
        trueVt(0) = 2.5;
        trueVt(1) = 5.0;
        trueVt(2) = 7.0;

        // run tests
        INFERNO_TEST_VT(vt, trueVt);
        ValueType beta;
        BOOST_CHECK_EQUAL(vt->isPotts(beta),false);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),false);

    }
    {
        // to test
        const ValueType data[]  ={-1.0, 2.0, -3.0, 4.5};
        Vtb * vt = new Vt(data, data+4);

        // true table
        Marray trueVt({4});
        for(size_t i=0; i<4; ++i)
            trueVt(i) = data[i];
        // run tests
        ValueType beta;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(beta),false);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),false);

    }
    {
        // to test
        Vtb * vt = new Vt({-1.0, 1.0});

        // true table
        Marray trueVt({2});
        trueVt(0) = -1.0;
        trueVt(1) =  1.0;

        // run tests
        ValueType beta;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(beta),false);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),false);

    }
}



BOOST_AUTO_TEST_CASE(TestPottsValueTable)
{
    using namespace inferno;

    typedef value_tables::PottsValueTable Vt;

    {
        // to test
        const DiscreteLabel nl = 3;
        const ValueType beta = 2.0;
        Vtb * vt = new Vt(nl, beta);

        // true table
        Marray trueVt({nl,nl}, beta);
        for(size_t i=0; i<nl; ++i)
            trueVt(i, i) = 0;

        // run tests
        ValueType betat;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(betat),true);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),true);

    }
}


BOOST_AUTO_TEST_CASE(TestExplicitValueTable)
{
    using namespace inferno;

    typedef value_tables::Explicit Vt;

    // some 1. order
    {
        // true table
        Marray trueVt({20});
        for(size_t i=0; i<trueVt.size(); ++i)
            trueVt(i) = i;

        // to test
        Vtb * vt = new Vt(trueVt);

        ValueType beta;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(beta),false);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),false);

    }
    // some 4. order
    {
        // true table
        Marray trueVt({2,3,4,5});
        for(size_t i=0; i<trueVt.size(); ++i)
            trueVt(i) = i;

        // to test
        Vtb * vt = new Vt(trueVt);

        // run tests
        ValueType beta;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(beta),false);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),false);

    }
    // 2. potts
    {
        // true table
        Marray trueVt({10,10}, 1.0);
        for(size_t i=0; i<10; ++i)
            trueVt(i, i) = 0;
        // to test
        Vtb * vt = new Vt(trueVt);

        // run tests
        ValueType beta;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(beta),true);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),true);
    }
    // 2. constant
    {
        // true table
        Marray trueVt({10,10}, 0.0);
        for(size_t i=0; i<10; ++i)
            trueVt(i, i) = 0;
        // to test
        Vtb * vt = new Vt(trueVt);

        // run tests
        ValueType beta;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(beta),true);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),true);
    }
    // 3. order potts
    {
        // true table
        Marray trueVt({10,10,10}, 1.0);
        for(size_t i=0; i<10; ++i)
            trueVt(i, i, i) = 0;
        // to test
        Vtb * vt = new Vt(trueVt);

        // run tests
        ValueType beta;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(beta),true);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),true);

    }
}

BOOST_AUTO_TEST_CASE(TestExplicitViewValueTable)
{
    using namespace inferno;

    typedef value_tables::ExplicitView Vt;

    // some 1. order
    {
        // true table
        Marray trueVt({20});
        MarrayView view(trueVt);
        for(size_t i=0; i<trueVt.size(); ++i)
            trueVt(i) = i;

        // to test
        Vtb * vt = new Vt(view);

        // run tests
        ValueType beta;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(beta),false);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),false);

    }
    // some 4. order
    {
        // true table
        Marray trueVt({2,3,4,5});
        MarrayView view(trueVt);
        for(size_t i=0; i<trueVt.size(); ++i)
            trueVt(i) = i;

        // to test
        Vtb * vt = new Vt(view);

        // run tests
        ValueType beta;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(beta),false);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),false);

    }
    // 2. potts
    {
        // true table
        Marray trueVt({10,10}, 1.0);
        MarrayView view(trueVt);
        for(size_t i=0; i<10; ++i)
            trueVt(i, i) = 0;
        // to test
        Vtb * vt = new Vt(view);

        // run tests
        ValueType beta;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(beta),true);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),true);
    }
    // 2. constant
    {
        // true table
        Marray trueVt({10,10}, 0.0);
        MarrayView view(trueVt);
        for(size_t i=0; i<10; ++i)
            trueVt(i, i) = 0;
        // to test
        Vtb * vt = new Vt(view);

        // run tests
        ValueType beta;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(beta),true);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),true);
    }
    // 3. order potts
    {
        // true table
        Marray trueVt({10,10,10}, 1.0);
        MarrayView view(trueVt);
        for(size_t i=0; i<10; ++i)
            trueVt(i, i, i) = 0;
        // to test
        Vtb * vt = new Vt(view);

        // run tests
        ValueType beta;
        INFERNO_TEST_VT(vt, trueVt);
        BOOST_CHECK_EQUAL(vt->isPotts(beta),true);
        BOOST_CHECK_EQUAL(vt->isGeneralizedPotts(),true);

    }
}
