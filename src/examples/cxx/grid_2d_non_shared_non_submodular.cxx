#include <random>

#include "inferno/inferno.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/explicit.hxx"
#include "inferno/value_tables/unary.hxx"

int main(){

    const inferno::LabelType nLabes = 4;
    const inferno::Vi gridShape[2] = {64,48};
    const inferno::Vi nVar = gridShape[0]*gridShape[1];

    inferno::GeneralDiscreteGraphicalModel model(nVar, nLabes);
    
    // random gen (just for this example)
    std::uniform_real_distribution<float> distribution(-1,1); //Values between -1 and 1
    std::mt19937 engine; // Mersenne twister MT19937
    auto generator = std::bind(distribution, engine);
   
    // vector to hold random numbers for unaries
    // (just for this example)
    std::vector<inferno::ValueType> uVals(nLabes); 



    // unary factors
    for(auto y=0; y< gridShape[1]; ++y)
    for(auto x=0; x< gridShape[0]; ++x){
        
        // get the flat variable index
        const inferno::Vi vi = x + y*gridShape[0];

        // fill random numbers 
        // for unaries
        for(auto & v : uVals)
            v = generator();
        // add the value table which stores the unary function to the model
        auto vti = model.addValueTable(new inferno::value_tables::UnaryValueTable(uVals.begin(), uVals.end()) );
        // add the factor connected to the just added value table
        // and the 1 variable indices 
        auto fi = model.addFactor(vti ,{vi});
    }

    // second order
    for(auto y=0; y< gridShape[1]; ++y)
    for(auto x=0; x< gridShape[0]; ++x){

        if(x+1 <gridShape[0]){
            // fill random numbers 
            // for for 2-orders
            inferno::ValueMarray  val2Order({nLabes, nLabes});
            for(auto & v : val2Order)
                v = generator();
            // get the flat variable index
            const inferno::Vi vi0 = x + y*gridShape[0];
            const inferno::Vi vi1 = x + 1 + y*gridShape[0];
            // add the value table to the model
            auto vti = model.addValueTable(new inferno::value_tables::Explicit(val2Order));
            // add the factor connected to the just added value table
            // and the 2 variable indices 
            auto fi = model.addFactor(vti ,{vi0, vi1});
        }
        if(y+1 <gridShape[1]){
            // fill random numbers 
            // for for 2-orders
            inferno::ValueMarray  val2Order({nLabes, nLabes});
            for(auto & v : val2Order)
                v = generator();
            // get the flat variable index
            const inferno::Vi vi0 = x + y*gridShape[0];
            const inferno::Vi vi1 = x + (1 + y)*gridShape[0];
            // add the value table which encodes the second order function to the model
            auto vti = model.addValueTable(new inferno::value_tables::Explicit(val2Order));
            // add the factor connected to the just added value table
            // and the 2 variable indices 
            auto fi = model.addFactor(vti ,{vi0, vi1});
        }
    }
}
