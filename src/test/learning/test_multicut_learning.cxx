#define BOOST_TEST_MODULE MulticutLearningTest
#include <boost/test/unit_test.hpp>
#include "inferno_test/test.hxx"

#include <random>
#include <iostream>



#include "inferno/model/parametrized_multicut_model.hxx"
#include "inferno/learning/dataset/explicit_dataset.hxx"
#include "inferno/learning/loss_functions/loss_functions.hxx"
#include "inferno/learning/learners/stochastic_gradient.hxx"
#include "inferno/inference/qpbo.hxx"


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


    typedef models::ParametrizedMulticutModel<EdgeArrayView, FeatureArrayView> Model;
    typedef Model::VariableMap<DiscreteLabel> Conf;
    typedef learning::loss_functions::VariationOfInformation<Model> LossFunction;
    typedef typename LossFunction::VariableSizeMap VarSize;
    typedef learning::dataset::VectorDataset<LossFunction> Dataset;

 
    Dataset dataset;
    dataset.resize(1);

    // graph       // gt
    // 0 | 1       // A | B
    // __ __       // __ __
    // 2 | 3       // A | B
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

    // make weight vector
    auto weights = learning::WeightVector(4);

    dataset.model(0) =  Model(4, edges, features, weights);
    auto model = dataset.model(0);

    Conf gt(model);
    gt[0] = 1;
    gt[1] = 2;
    gt[2] = 1;
    gt[2] = 2;
    dataset.groundTruth(0) = gt;

    VarSize varSize(model, 1.0);
    dataset.lossFunction(0).assign(model, varSize);

    
    auto trainingInstance = dataset[0];


    typedef learning::learners::StochasticGradient<Dataset> Learner;


    typedef inference::BaseDiscreteInferenceFactory<Model> FactoryBase;
    typedef inference::Qpbo<Model> Inference;
    typedef inference::DiscreteInferenceFactory<Inference> ConcreteFactory;

    FactoryBase * factory = new ConcreteFactory();
    Learner learner(dataset); 

    learner.learn(factory, weights);

}





