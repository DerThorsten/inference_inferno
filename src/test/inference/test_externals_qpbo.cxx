#define BOOST_TEST_MODULE QpboExteralTest
#include <boost/test/unit_test.hpp>

#include <random>
#include <iostream>

#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"
#include "inferno/inference/qpbo.hxx"
#define TEST_EPS 0.00001



BOOST_AUTO_TEST_CASE(TestQpboInference)
{
 using namespace inferno;

    const LabelType nLabes = 2;
    const Vi nVar = 10;

    GeneralDiscreteGraphicalModel model(nVar, nLabes);
    

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1, 1);



    
    std::uniform_real_distribution<float> distribution(-1,1); //Values between -1 and 1
    std::mt19937 engine; // Mersenne twister MT19937
    auto generator = std::bind(distribution, engine);
   
    std::vector<ValueType> values(nLabes); 
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
        std::cout<<beta<<"\n";
        auto vti = model.addValueTable(new value_tables::PottsValueTable(nLabes, beta));
        auto fi = model.addFactor(vti ,{vi, vi+1});
    }

    




    typedef GeneralDiscreteGraphicalModel   Model;
    typedef inference::Qpbo<Model>          Solver;
    typedef Solver::Options                 SolverOptions; 

    SolverOptions options;
    Solver solver(model, options);
    solver.infer();

    std::vector<LabelType> conf(nVar);
    solver.conf(conf.begin());

    for( auto c : conf)
        std::cout<<"conf"<<c<<"\n";
}



