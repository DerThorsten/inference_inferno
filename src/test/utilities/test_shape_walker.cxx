#define BOOST_TEST_MODULE ShapeWalkerTest
#include <boost/test/unit_test.hpp>

#include "inferno/utilities/shape_walker.hxx"

#define TEST_EPS 0.00001

// Have to make it a macro so that it reports exact line numbers when checks fail.
#define CHECK_EQ_COLLECTION(aa, bb) { \
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


struct TestShape_2_1_3{
    size_t operator()(const size_t d)const{
        const int s[3] = {2,1,3};
        return s[d];
    }
};

struct TestShape_1_2_3_4_5{
    size_t operator()(const size_t d)const{
        const int s[5] = {1,2,3,4,5};
        return s[d];
    }
};

BOOST_AUTO_TEST_CASE(Test_ShapeWalker)
{
    std::vector<int> vec = {0, 0, 0};
    typedef std::vector<int>::iterator Iter;

    TestShape_2_1_3 shape;

    inferno::ShapeWalker< TestShape_2_1_3, Iter> walker(shape, 3, vec.begin());

    auto should = {0,0,0};


                should = {0,0,0}; CHECK_EQ_COLLECTION(vec, should);
    ++walker;   should = {1,0,0}; CHECK_EQ_COLLECTION(vec, should);
    ++walker;   should = {0,0,1}; CHECK_EQ_COLLECTION(vec, should);
    ++walker;   should = {1,0,1}; CHECK_EQ_COLLECTION(vec, should);
    ++walker;   should = {0,0,2}; CHECK_EQ_COLLECTION(vec, should);
    ++walker;   should = {1,0,2}; CHECK_EQ_COLLECTION(vec, should);
}


BOOST_AUTO_TEST_CASE(Test_ShapeWalker2)
{
    std::vector<int> vec = {0, 0, 0, 0, 0};
    typedef std::vector<int>::iterator Iter;

    TestShape_1_2_3_4_5 shape;

    inferno::ShapeWalker< TestShape_1_2_3_4_5, Iter> walker(shape, 5, vec.begin());

    int should[5] = {0,0,0,0,0};

    for(should[4]=0; should[4]<shape(4); ++should[4])
    for(should[3]=0; should[3]<shape(3); ++should[3])
    for(should[2]=0; should[2]<shape(2); ++should[2])
    for(should[1]=0; should[1]<shape(1); ++should[1])
    for(should[0]=0; should[0]<shape(0); ++should[0]){
        CHECK_EQ_COLLECTION(vec, should);
        ++walker;
    }

}

