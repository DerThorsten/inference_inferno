#ifndef INFERNO_TEST_TEST_HXX
#define INFERNO_TEST_TEST_HXX

#include <vector>

#include "inferno/inferno.hxx"

#define TEST_EPS 0.00001

// Have to make it a macro so that it reports exact line numbers when checks fail.
#define INFERNO_CHECK_EQ_COLLECTION(aa, bb) { \
    using std::distance; \
    using std::begin; \
    using std::end; \
    auto a = begin(aa), ae = end(aa); \
    auto b = begin(bb); \
    auto d1 = distance(a, ae);\
    auto d2 = distance(b, end(bb));\
    BOOST_REQUIRE_EQUAL(d1,d2); \
    for(; a != ae; ++a, ++b) { \
        BOOST_CHECK_EQUAL(*a, *b); \
    } \
}


#define INFERNO_TEST_VT(vt, trueVt) \
{ \
    BOOST_CHECK_EQUAL(vt->arity(), trueVt.dimension()); \
    BOOST_CHECK_EQUAL(vt->size(), trueVt.size()); \
    const auto arity = vt->arity(); \
    for(auto a=0;a<arity; ++a) \
         BOOST_CHECK_EQUAL(vt->shape(a), trueVt.shape(a)); \
    for(const auto & conf : vt->confs()){ \
        const auto vtVal = vt->eval(conf.data()); \
        const auto trueVal = trueVt(conf.data()); \
        BOOST_CHECK_CLOSE(vtVal, trueVal, TEST_EPS); \
    } \
    if(arity==1){ \
        for(auto l=0; l < vt->shape(0); ++l) \
            BOOST_CHECK_CLOSE(vt->eval(l), trueVt(l), TEST_EPS); \
    } \
    if(arity==2){ \
        for(auto l1=0; l1 < vt->shape(1); ++l1) \
        for(auto l0=0; l0 < vt->shape(0); ++l0) \
            BOOST_CHECK_CLOSE(vt->eval(l0,l1), trueVt(l0,l1), TEST_EPS); \
    } \
    if(arity==3){ \
        for(auto l2=0; l2 < vt->shape(2); ++l2) \
        for(auto l1=0; l1 < vt->shape(1); ++l1) \
        for(auto l0=0; l0 < vt->shape(0); ++l0) \
            BOOST_CHECK_CLOSE(vt->eval(l0,l1, l2), trueVt(l0,l1, l2), TEST_EPS); \
    } \
    if(arity==4){ \
        for(auto l3=0; l3 < vt->shape(3); ++l3) \
        for(auto l2=0; l2 < vt->shape(2); ++l2) \
        for(auto l1=0; l1 < vt->shape(1); ++l1) \
        for(auto l0=0; l0 < vt->shape(0); ++l0) \
            BOOST_CHECK_CLOSE(vt->eval(l0,l1, l2, l3), trueVt(l0,l1, l2, l3), TEST_EPS); \
    } \
    if(arity==5){ \
        for(auto l4=0; l4 < vt->shape(4); ++l4) \
        for(auto l3=0; l3 < vt->shape(3); ++l3) \
        for(auto l2=0; l2 < vt->shape(2); ++l2) \
        for(auto l1=0; l1 < vt->shape(1); ++l1) \
        for(auto l0=0; l0 < vt->shape(0); ++l0) \
            BOOST_CHECK_CLOSE(vt->eval(l0,l1, l2, l3, l4), trueVt(l0,l1, l2, l3, l4), TEST_EPS); \
    } \
    std::vector<inferno::DiscreteLabel> shapeBuffer(arity); \
    vt->bufferShape(shapeBuffer.data()); \
    for(auto a=0;a<arity; ++a) \
         BOOST_CHECK_EQUAL(shapeBuffer[a], trueVt.shape(a)); \
    std::vector<inferno::ValueType> valueBuffer(vt->size()); \
    vt->bufferValueTable(valueBuffer.data()); \
    auto c = 0; \
    for(const auto & conf : vt->confs()){ \
        BOOST_CHECK_CLOSE(valueBuffer[c], trueVt(c), TEST_EPS); \
        ++c; \
    } \
    /* no test / or very simple tests  for next functions */ \
    const auto isPotts = vt->isPotts(); \
    const auto isGeneralizedPotts = vt->isGeneralizedPotts(); \
    if(isPotts && arity>=1 && arity<=4)\
        BOOST_CHECK_EQUAL(isPotts, isGeneralizedPotts); \
    if(arity == 1){ \
        BOOST_CHECK_EQUAL(isGeneralizedPotts, false); \
    } \
} 



namespace inferno{

}

#endif/* INFERNO_TEST_TEST_HXX */
