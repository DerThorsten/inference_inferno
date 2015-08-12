#define BOOST_TEST_MODULE TlModelTest

#include <boost/test/unit_test.hpp>
#include "inferno_test/test.hxx"

#include "inferno/inferno.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"
#include "inferno/model/general_discrete_tl_model.hxx"




BOOST_AUTO_TEST_CASE(TestAddFunctionVti)
{
    using namespace inferno;

    typedef models::TlModel<
        value_tables::UnaryValueTable,
        value_tables::PottsValueTable
    > Model;

    Model model(10, 2);

    {
        const auto vti = model.addValueTable(value_tables::PottsValueTable(2, 1.0));
        BOOST_CHECK_EQUAL(vti.first, 1);
        BOOST_CHECK_EQUAL(vti.second, 0);
    }

    {
        const auto vti = model.addValueTable(value_tables::UnaryValueTable( {1.0, 2.0} ));
        BOOST_CHECK_EQUAL(vti.first, 0);
        BOOST_CHECK_EQUAL(vti.second, 0);
    }
    {
        const auto vti = model.addValueTable(value_tables::UnaryValueTable( {1.0, 2.0} ));
        BOOST_CHECK_EQUAL(vti.first, 0);
        BOOST_CHECK_EQUAL(vti.second, 1);
    }
    {
        const auto vti = model.addValueTable(value_tables::UnaryValueTable( {1.0, 2.0} ));
        BOOST_CHECK_EQUAL(vti.first, 0);
        BOOST_CHECK_EQUAL(vti.second, 2);
    }

    {
        const auto vti = model.addValueTable(value_tables::PottsValueTable(2, 1.0));
        BOOST_CHECK_EQUAL(vti.first, 1);
        BOOST_CHECK_EQUAL(vti.second, 1);
    }
}

BOOST_AUTO_TEST_CASE(TestAddFactorSimple)
{
    using namespace inferno;

    typedef models::TlModel<
        value_tables::UnaryValueTable,
        value_tables::PottsValueTable
    > Model;

    Model model(10, 2);
    {
        value_tables::PottsValueTable pvt(2, 1.0);
        const auto vti = model.addValueTable(pvt);
        BOOST_CHECK_EQUAL(vti.first, 1);
        BOOST_CHECK_EQUAL(vti.second, 0);

        Vi vis[2] = {0,1};
        const auto fi = model.addFactor(vti, vis,vis+2);
        BOOST_CHECK_EQUAL(fi, 0);
        const auto factor = model.factor(0);
        //const auto vt = factor->valueTable();
        ValueType beta;
        factor->isPotts(beta);
        BOOST_CHECK_CLOSE(factor->eval(0l,0l),0.0, TEST_EPS);
        BOOST_CHECK_CLOSE(factor->eval(0l,1l),1.0, TEST_EPS);
    }

   
}
