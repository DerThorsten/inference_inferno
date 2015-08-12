#define BOOST_TEST_MODULE ValueTableTest

#include <boost/test/unit_test.hpp>
#include "inferno_test/test.hxx"

#include "inferno/inferno.hxx"
#include "inferno/value_tables/sum_constraint.hxx"


typedef inferno::value_tables::DiscreteValueTableBase Vtb;
typedef inferno::ValueMarray Marray;
typedef inferno::ValueMarrayView MarrayView;

BOOST_AUTO_TEST_CASE(TestSumConstraint)
{
    using namespace inferno;

    typedef value_tables::SumConstraint Vt;

    {
        const auto lb = 2;
        const auto ub = 4;
        // to test
        Vt * vt = new Vt({2,3,4}, lb, ub);

        // true table
        Marray trueVt({2,3,4}, 0.0);

        for(DiscreteLabel x0=0; x0<2; ++x0)
        for(DiscreteLabel x1=0; x1<3; ++x1)
        for(DiscreteLabel x2=0; x2<4; ++x2){

            const auto sum = x0 + x1 + x2;
            if(sum<lb || sum>ub){
                trueVt(x0, x1, x2) = infVal();
            }
        }

        // run tests
        INFERNO_TEST_VT(vt, trueVt);
    }
}
