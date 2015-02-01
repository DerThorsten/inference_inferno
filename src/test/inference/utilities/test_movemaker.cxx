#define BOOST_TEST_MODULE MovemakerTest
#include <boost/test/unit_test.hpp>

#include <random>
#include <iostream>

#include "inferno/model/general_discrete_model.hxx"
#include "inferno/inference/utilities/movemaker.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"
#include "inferno/value_tables/explicit.hxx"
#define TEST_EPS 0.00001



BOOST_AUTO_TEST_CASE(TestMovemaker_ConstructorCompiles)
{
    using namespace inferno;

    const LabelType nLabels = 2;
    const Vi nVar = 10;
    typedef GeneralDiscreteGraphicalModel Model;
    Model model(nVar, nLabels);
    

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1, 1);



    
    std::uniform_real_distribution<float> distribution(-1,1); //Values between -1 and 1
    std::mt19937 engine; // Mersenne twister MT19937
    auto generator = std::bind(distribution, engine);
   
    std::vector<ValueType> values(nLabels); 
    // unary factors


    
    for(auto vi : model.variableIds() ){
        for(auto & v : values)
            v = generator();
        auto vti = model.addValueTable(new value_tables::UnaryValueTable(values.begin(), values.end()) );
        auto fi = model.addFactor(vti ,{vi});
    }

    // second order
    for(Vi vi=0; vi<nVar-1; ++vi){
        auto beta = generator( );
        auto vti = model.addValueTable(new value_tables::PottsValueTable(nLabels, beta));
        auto fi = model.addFactor(vti ,{vi, vi+1});
    }

    
    inference::Movemaker<Model> movemaker(model);



   
}



BOOST_AUTO_TEST_CASE(TestMove)
{
    using namespace inferno;

    size_t numLabels[] = {3, 3, 3, 3, 3};
    const LabelType nLabels = 3;
    const Vi nVar = 5;
    typedef GeneralDiscreteGraphicalModel Model;
    Model model(nVar, nLabels);

    // 1 order
    for(const auto vi : model.variableIds()){
        ValueMarray f1(&nLabels, &nLabels+1);
        f1(0) = 0.0;
        f1(1) = 0.2;
        f1(2) = 0.2;
        auto vti = model.addValueTable(new value_tables::Explicit(f1));
        auto fi = model.addFactor(vti ,{vi});
    }

    // 2nd order factors
    for (Vi j = 0; j < model.nVariables() - 1; ++j) {
        ValueMarray f2(numLabels, numLabels + 2);
        for (size_t j = 0; j < 9; ++j) {
            f2(j) = static_cast<ValueType> (j) / 10;
        }
        auto vti = model.addValueTable(new value_tables::Explicit(f2));
        auto fi = model.addFactor(vti ,{j, j + 1});
    }

    // 3rd order factor
    {
        ValueMarray f3(numLabels, numLabels + 3, 1.0);
        for (size_t j = 0; j < 27; ++j) {
            f3(j) = static_cast<ValueType> (j) / 20;
        }
        auto vti = model.addValueTable(new value_tables::Explicit(f3));
        auto fi = model.addFactor(vti , {0l, 2l, 4l} );;
    }

    inference::Movemaker<Model> movemaker(model);


    std::vector<DiscreteLabel> stateVec(model.nVariables(),0);
    Model::VariableMap<DiscreteLabel> stateMap(model,0);

    std::vector<Vi> vis(model.nVariables());
    auto c=0;
    for(auto & vi : vis){
        vi = c;
        ++c;
    }

    auto overflow = false;
    while (!overflow) {
        const auto mmVal = movemaker.valueAfterMove(vis.begin(), 
                                                    vis.end(), 
                                                    stateVec.begin());

        const auto modelVal = model.eval(stateMap);

        BOOST_CHECK_CLOSE(modelVal, mmVal, TEST_EPS);

        for(const auto vi : model.variableIds()){
            if(stateVec[vi] + 1 < model.nLabels(vi)){
                ++stateVec[vi];
                ++stateMap[vi];
                break;
            }
            else{
                stateVec[vi] = 0;
                stateMap[vi] = 0;
                if (vi == model.nVariables() - 1) {
                    overflow = true;
                }
            }
        }
    }
    //
}
