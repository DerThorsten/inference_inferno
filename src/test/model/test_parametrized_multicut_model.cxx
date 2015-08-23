#define BOOST_TEST_MODULE MulticutLearningTest
#include <boost/test/unit_test.hpp>
#include "inferno_test/test.hxx"

#include <random>
#include <iostream>



#include "inferno/model/parametrized_multicut_model.hxx"
#include "inferno/learning/dataset/explicit_dataset.hxx"
#include "inferno/learning/loss_functions/loss_functions.hxx"



typedef vigra::TinyVector<uint64_t, 2> Edge;
typedef vigra::MultiArray<1, vigra::TinyVector<uint64_t, 2> > EdgeArray;
typedef vigra::MultiArrayView<1, vigra::TinyVector<uint64_t, 2> > EdgeArrayView;
typedef typename EdgeArray::difference_type EdgeArrayShape;

typedef vigra::MultiArray<2, double> FeatureArray;
typedef vigra::MultiArrayView<2, double> FeatureArrayView;
typedef typename FeatureArray::difference_type FeatureArrayShape;



BOOST_AUTO_TEST_CASE(TestMulticutLearning_1)
{
    using namespace inferno;

    // graph
    // 0 | 1
    // __ __
    // 2 | 3

    // gt
    // A | B
    // __ __
    // A | B

    // *--------*-------*-------*
    // | edgeId | uv    | edgeGt|
    // *--------*-------*-------|
    // | 0      | (0,1) | 1     |
    // | 1      | (0,2) | 0     |
    // | 2      | (1,3) | 1     |
    // | 3      | (2,3) | 0     |
    // *--------*-------*-------*
    auto edges = EdgeArray(EdgeArrayShape(4));
    edges[0] = Edge(0,1);
    edges[1] = Edge(0,2);
    edges[2] = Edge(1,3);
    edges[3] = Edge(2,3);

    // 5 features per edge (including constant 1 feature)
    auto features = FeatureArray(FeatureArrayShape(4, 5),0.0);

    // make const feature
    features.bindOuter(4) = 1.0;



    auto weights = learning::WeightVector(4);

    typedef models::ParametrizedMulticutModel<EdgeArrayView, FeatureArrayView> Model;

    Model model(4, edges, features, weights);

    typedef Model::VariableMap<DiscreteLabel> Conf;
    Conf conf(model);

    conf[0] = 0;
    conf[0] = 0;
    conf[0] = 0;
    conf[0] = 0;

    BOOST_CHECK_CLOSE(model.eval(conf),0.0, 0.0001);

    for( auto cv : model.variableMapIter(conf)){
        
    }

    // check api of model
    BOOST_CHECK_EQUAL(model.nVariables(),4);
    BOOST_CHECK_EQUAL(model.nFactors(),4);
    BOOST_CHECK_EQUAL(model.nUnaries(),0);
    BOOST_CHECK_EQUAL(model.maxArity(), 2);
    DiscreteLabel l,l2;
    BOOST_CHECK_EQUAL(model.hasSimpleLabelSpace(l), true);
    BOOST_CHECK_EQUAL(l,model.nVariables());
    model.minMaxNLabels(l,l2);
    BOOST_CHECK_EQUAL(l,model.nVariables());
    BOOST_CHECK_EQUAL(l2,model.nVariables());

    BOOST_CHECK_EQUAL(model.minVarId(),0);
    BOOST_CHECK_EQUAL(model.maxVarId(),3);

    BOOST_CHECK_EQUAL(model.minFactorId(),0);
    BOOST_CHECK_EQUAL(model.maxFactorId(),3);

    BOOST_CHECK_EQUAL(model.denseVariableIds(), true);
    BOOST_CHECK_EQUAL(model.hasUnaries(), false);
    BOOST_CHECK_EQUAL(model.nPairwiseFactors(),4);

    const auto trueVis =  {0,1,2,3};

    CHECK_FACTOR_NUM_LABELS_SANITY(model);


    // check api of factors
    for(auto factor : model.factors()){
        BOOST_CHECK_EQUAL(factor->arity(),  2);
        BOOST_CHECK_EQUAL(factor->shape(0), 4);
        BOOST_CHECK_EQUAL(factor->shape(1), 4);
        BOOST_CHECK_EQUAL(factor->size(),  16);

        auto vt = factor->valueTable();
        BOOST_CHECK_EQUAL(vt->arity(),  2);
        BOOST_CHECK_EQUAL(vt->shape(0), 4);
        BOOST_CHECK_EQUAL(vt->shape(1), 4);
        BOOST_CHECK_EQUAL(vt->size(),  16);

    }




    /// 
}





