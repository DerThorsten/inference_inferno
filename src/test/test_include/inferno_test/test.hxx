#ifndef INFERNO_TEST_TEST_HXX
#define INFERNO_TEST_TEST_HXX

#define INFERNO_DEBUG 

#include <vector>
#include <random>
#include <cmath>

#include <boost/iterator/counting_iterator.hpp>

#include "inferno/inferno.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"

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


#define _CHECK_CLOSE(aa, bb, tol) { \
    if(std::isinf(aa) && std::isinf(bb)){ \
        BOOST_CHECK_EQUAL(aa, bb); \
    } \
    else{ \
        BOOST_CHECK_CLOSE(aa, bb, tol);\
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
        _CHECK_CLOSE(vtVal, trueVal, TEST_EPS); \
    } \
    if(arity==1){ \
        for(auto l=0; l < vt->shape(0); ++l) \
            _CHECK_CLOSE(vt->eval(l), trueVt(l), TEST_EPS); \
    } \
    if(arity==2){ \
        for(auto l1=0; l1 < vt->shape(1); ++l1) \
        for(auto l0=0; l0 < vt->shape(0); ++l0) \
            _CHECK_CLOSE(vt->eval(l0,l1), trueVt(l0,l1), TEST_EPS); \
    } \
    if(arity==3){ \
        for(auto l2=0; l2 < vt->shape(2); ++l2) \
        for(auto l1=0; l1 < vt->shape(1); ++l1) \
        for(auto l0=0; l0 < vt->shape(0); ++l0) \
            _CHECK_CLOSE(vt->eval(l0,l1, l2), trueVt(l0,l1, l2), TEST_EPS); \
    } \
    if(arity==4){ \
        for(auto l3=0; l3 < vt->shape(3); ++l3) \
        for(auto l2=0; l2 < vt->shape(2); ++l2) \
        for(auto l1=0; l1 < vt->shape(1); ++l1) \
        for(auto l0=0; l0 < vt->shape(0); ++l0) \
            _CHECK_CLOSE(vt->eval(l0,l1, l2, l3), trueVt(l0,l1, l2, l3), TEST_EPS); \
    } \
    if(arity==5){ \
        for(auto l4=0; l4 < vt->shape(4); ++l4) \
        for(auto l3=0; l3 < vt->shape(3); ++l3) \
        for(auto l2=0; l2 < vt->shape(2); ++l2) \
        for(auto l1=0; l1 < vt->shape(1); ++l1) \
        for(auto l0=0; l0 < vt->shape(0); ++l0) \
            _CHECK_CLOSE(vt->eval(l0,l1, l2, l3, l4), trueVt(l0,l1, l2, l3, l4), TEST_EPS); \
    } \
    std::vector<inferno::DiscreteLabel> shapeBuffer(arity); \
    vt->bufferShape(shapeBuffer.data()); \
    for(auto a=0;a<arity; ++a) \
         BOOST_CHECK_EQUAL(shapeBuffer[a], trueVt.shape(a)); \
    std::vector<inferno::ValueType> valueBuffer(vt->size()); \
    vt->bufferValueTable(valueBuffer.data()); \
    auto c = 0; \
    for(const auto & conf : vt->confs()){ \
        _CHECK_CLOSE(valueBuffer[c], trueVt(c), TEST_EPS); \
        ++c; \
    } \
    /* no test / or very simple tests  for next functions */ \
    ValueType beta; \
    const auto isPotts = vt->isPotts(beta); \
    const auto isGeneralizedPotts = vt->isGeneralizedPotts(); \
    if(isPotts && arity>=1 && arity<=4)\
        BOOST_CHECK_EQUAL(isPotts, isGeneralizedPotts); \
    if(arity == 1){ \
        BOOST_CHECK_EQUAL(isGeneralizedPotts, false); \
    } \
} 


#define INFERNO_CHECK_EMPTY_MODEL(model)\
    BOOST_CHECK_EQUAL(model.nVariables(),0); \
    BOOST_CHECK_EQUAL(model.nFactors(),0); \


#define INFERNO_CHECK_MODEL_VIS(MODEL, model, varIds) \
{ \
    const auto SortedVariableIds = MODEL::SortedVariableIds; \
    const auto SortedFactorIds = MODEL::SortedFactorIds; \
    std::vector<inferno::Vi> vis(model.variableIdsBegin(), model.variableIdsEnd()); \
    BOOST_CHECK_EQUAL(vis.size(),model.nVariables());\
    if(SortedVariableIds){ \
        INFERNO_CHECK_EQ_COLLECTION(vis, varIds); \
        const auto modelMinVarId = model.minVarId();\
        const auto modelMaxVarId = model.maxVarId();\
        BOOST_REQUIRE_EQUAL(modelMinVarId,vis.front());\
        BOOST_REQUIRE_EQUAL(modelMaxVarId,vis.back());\
        const auto realMin = *std::min_element(vis.begin(), vis.end());\
        const auto realMax = *std::max_element(vis.begin(), vis.end());\
        BOOST_REQUIRE_EQUAL(modelMinVarId,realMin);\
        BOOST_REQUIRE_EQUAL(modelMaxVarId,realMax);\
    } \
    else{ \
        const auto modelMinVarId = model.minVarId();\
        const auto modelMaxVarId = model.maxVarId();\
        const auto realMin = *std::min_element(vis.begin(), vis.end());\
        const auto realMax = *std::max_element(vis.begin(), vis.end());\
        BOOST_REQUIRE_EQUAL(modelMinVarId,realMin);\
        BOOST_REQUIRE_EQUAL(modelMaxVarId,realMax);\
    } \
} \


namespace inferno{
namespace models{

    template<class T>
    GeneralDiscreteModel grid2d(
        std::initializer_list<T> shape, 
        const DiscreteLabel  nLabels,
        const ValueType beta = 1.0,
        const bool submodular = false
    ){
        std::vector<DiscreteLabel> vShape(shape.begin(),shape.end());
        const Vi nVar = vShape[0]*vShape[1];
        GeneralDiscreteModel model(nVar, nLabels);

        std::mt19937 engine; 

        std::uniform_real_distribution<ValueType> dUnary(0,1); //Values between -1 and 1
        auto uGen = std::bind(dUnary, engine);

        std::uniform_real_distribution<ValueType> dBinary(submodular ? 0 : -beta,beta); //Values between -1 and 1
        auto bGen = std::bind(dBinary, engine);

        std::vector<ValueType> values(nLabels);

        for(Vi vi=0; vi<nVar; ++vi){
            for(auto & v : values)
                v = uGen();
            auto vti = model.addValueTable(new value_tables::UnaryValueTable(values.begin(), values.end()) );
            auto fi = model.addFactor(vti ,{vi});

        }

        for(int y=0; y<vShape[1]; ++y)
        for(int x=0; x<vShape[0]; ++x){
            const auto vi0  = x + y*vShape[0];
            if(x + 1 < vShape[0]){
                const auto vi1  = x + 1 + y*vShape[0];
                auto vti = model.addValueTable(new value_tables::PottsValueTable(nLabels, bGen( )));
                auto fi = model.addFactor(vti ,{vi0, vi1});
            }
            if(x + 1 < vShape[0]){
                const auto vi1  = x + 1 + y*vShape[0];
                auto vti = model.addValueTable(new value_tables::PottsValueTable(nLabels, bGen()));
                auto fi = model.addFactor(vti ,{vi0, vi1});
            }
        }
        return model;
    }

} // end namespace models
} // end namespace inferno


#endif/* INFERNO_TEST_TEST_HXX */
