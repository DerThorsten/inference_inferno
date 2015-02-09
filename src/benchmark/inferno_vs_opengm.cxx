#include <benchmark/benchmark.h>

#include "inferno/inferno.hxx"
#include "inferno/model/sparse_discrete_model.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"

#include <opengm/graphicalmodel/graphicalmodel.hxx>
#include <opengm/graphicalmodel/space/simplediscretespace.hxx>
#include <opengm/functions/potts.hxx>
#include <opengm/operations/adder.hxx>

// Define another benchmark
static void PottsBenchmarkInferno(benchmark::State& state) {

    //state.PauseTiming();

    const inferno::LabelType nLabes = state.range_y();
    const inferno::Vi gridShape[2] = {state.range_x(),state.range_x()};
    const inferno::Vi nVar = gridShape[0]*gridShape[1];
    std::vector<inferno::ValueType> uVals(nLabes); 
    std::uniform_real_distribution<float> distribution(-1,1); //Values between -1 and 1
    std::mt19937 engine; // Mersenne twister MT19937
    auto generator = std::bind(distribution, engine);


    inferno::GeneralDiscreteGraphicalModel model(nVar, nLabes);

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
    inferno::GeneralDiscreteGraphicalModel::VariableMap<inferno::DiscreteLabel>  labels(model, 0);
    inferno::ValueType res = 0;
    //state.ResumeTiming();
    while (state.KeepRunning()){
        res += model.eval(labels);
    }

}
BENCHMARK(PottsBenchmarkInferno)->RangePair(8, 8<<10, 2, 8);



// Define another benchmark
static void PottsBenchmarkOpengm(benchmark::State& state) {

    //state.PauseTiming();

    const inferno::LabelType nLabes = state.range_y();
    const inferno::Vi gridShape[2] = {state.range_x(),state.range_x()};
    const inferno::Vi nVar = gridShape[0]*gridShape[1];


    std::uniform_real_distribution<float> distribution(-1,1); //Values between -1 and 1
    std::mt19937 engine; // Mersenne twister MT19937
    auto generator = std::bind(distribution, engine);

    typedef opengm::SimpleDiscreteSpace<size_t, size_t> Space;
    typedef opengm::GraphicalModel<double, opengm::Adder, opengm::ExplicitFunction<double>  , Space> Model;
    Space space(nVar, nLabes);
    Model gm(space);
    



    for(auto y=0; y< gridShape[1]; ++y)
    for(auto x=0; x< gridShape[0]; ++x){
        // function
        const size_t shape[] = {size_t(nLabes)};
        opengm::ExplicitFunction<double> f(shape, shape + 1);
        for(size_t s = 0; s < nLabes; ++s) {
            f(s) = generator();
        }
        Model::FunctionIdentifier fid = gm.addFunction(f);

        // factor
        size_t variableIndices[] = { size_t(x + y*gridShape[0])};
        gm.addFactor(fid, variableIndices, variableIndices + 1);
    }






    std::vector<size_t> labels(gm.numberOfVariables(), 0);
    inferno::ValueType res = 0;
    //state.ResumeTiming();

    while (state.KeepRunning()){
        res += gm.evaluate(labels);
    }

}
BENCHMARK(PottsBenchmarkOpengm)->RangePair(8, 8<<10, 2, 8);





// Augment the main() program to invoke benchmarks if specified
// via the --benchmarks command line flag.  E.g.,
//       my_unittest --benchmark_filter=all
//       my_unittest --benchmark_filter=BM_StringCreation
//       my_unittest --benchmark_filter=String
//       my_unittest --benchmark_filter='Copy|Creation'
int main(int argc, const char* argv[]) {
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  return 0;
}
