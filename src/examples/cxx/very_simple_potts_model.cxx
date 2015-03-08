#include <random>

#include "inferno/inferno.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"

int main(){

    const inferno::LabelType nLabes = 2;
    const inferno::Vi nVar = 10;

    inferno::models::GeneralDiscreteGraphicalModel model(nVar, nLabes);
    
    // random gen (just for this example)
    std::uniform_real_distribution<float> distribution(-1,1); //Values between -1 and 1
    std::mt19937 engine; // Mersenne twister MT19937
    auto generator = std::bind(distribution, engine);
   
    // vector to hold random numbers
    std::vector<inferno::ValueType> values(nLabes); 

    // unary factors
    for(inferno::Vi vi=0; vi<nVar; ++vi){
        // fill random numbers 
        // for unaries
        for(auto & v : values)
            v = generator();
        auto vti = model.addValueTable(new inferno::value_tables::UnaryValueTable(values.begin(), values.end()) );
        auto fi = model.addFactor(vti ,{vi});
    }

    // second order
    for(inferno::Vi vi=0; vi<nVar-1; ++vi){
        auto beta = generator( );
        std::cout<<beta<<"\n";
        auto vti = model.addValueTable(new inferno::value_tables::PottsValueTable(nLabes, beta));
        auto fi = model.addFactor(vti ,{vi, vi+1});

        const auto factor = model[fi];
    }
}
