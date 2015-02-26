

#include "inferno/inferno.hxx"
#include "inferno/model/general_discrete_tl_model.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"
#include "inferno/value_tables/explicit.hxx"

#include "benchmark.hxx"




int main(int argc, const char* argv[]) {

    using namespace inferno;


    const LabelType nLabels = 200;
    const Vi gridShape[2] = {2000, 2000};
    const Vi nVar = gridShape[0]*gridShape[1];
    std::vector<ValueType> uVals(nLabels); 
    std::uniform_real_distribution<float> distribution(-1,1); //Values between -1 and 1
    std::mt19937 engine; // Mersenne twister MT19937
    auto generator = std::bind(distribution, engine);


    GeneralDiscreteGraphicalModel model(nVar, nLabels, false);

    const auto nUnaries = nVar; 
    const auto n2Order  = ((gridShape[0]-1) * gridShape[1]) + ((gridShape[1]-1) * gridShape[0]);
    const auto sU = sizeof(value_tables::UnaryViewValueTable);
    const auto sSO = sizeof(value_tables::PottsValueTable);


    const auto memsize = (sU + sizeof(ValueType)*nLabels )*nUnaries + sSO*n2Order;
    uint8_t * memh = new uint8_t[memsize];
    uint8_t * mem = memh;

    // unary factors
    for(auto y=0; y< gridShape[1]; ++y)
    for(auto x=0; x< gridShape[0]; ++x){
        const Vi vi = x + y*gridShape[0];
        for(auto & v : uVals){
            v = generator();
        }
        const ValueType * data = reinterpret_cast<const ValueType *>(mem+sU);
        auto vti = model.addValueTable(new (mem) value_tables::UnaryViewValueTable(nLabels, data) );
        mem += sU + sizeof(ValueType)*nLabels;
        auto fi = model.addFactor(vti ,{vi});
    }

    // second order
    for(auto y=0; y< gridShape[1]; ++y)
    for(auto x=0; x< gridShape[0]; ++x){
        if(x+1 <gridShape[0]){
            const Vi vi0 = x + y*gridShape[0];
            const Vi vi1 = x + 1 + y*gridShape[0];
            auto vti = model.addValueTable(new(mem) value_tables::PottsValueTable(nLabels, 1.0));
            mem += sSO;
            auto fi = model.addFactor(vti ,{vi0, vi1});
        }
        if(y+1 <gridShape[1]){
            // get the flat variable index
            const Vi vi0 = x + y*gridShape[0];
            const Vi vi1 = x + (1 + y)*gridShape[0];
            auto vti = model.addValueTable(new(mem) value_tables::PottsValueTable(nLabels, 1.0));
            mem += sSO;
            auto fi = model.addFactor(vti ,{vi0, vi1});
        }
    }

    typedef Benchmark<GeneralDiscreteGraphicalModel> BenchType;

    BenchType::run(model);

    delete[] memh;



}
