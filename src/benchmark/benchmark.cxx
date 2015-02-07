#include <benchmark/benchmark.h>

#include "inferno/inferno.hxx"
#include "inferno/model/sparse_discrete_model.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"



// Define another benchmark
static void GeneralDiscreteGraphicalModelBenchmark(benchmark::State& state) {

    while (state.KeepRunning()){
        state.PauseTiming();
        const inferno::LabelType nLabes = state.range_y();
        const inferno::Vi nVar = state.range_x();
        state.ResumeTiming();
        inferno::GeneralDiscreteGraphicalModel model(nVar, nLabes);
        // second order
        for(inferno::Vi vi=0; vi<nVar-1; ++vi){
            auto vti = model.addValueTable(new inferno::value_tables::PottsValueTable(nLabes, 1.0));
            auto fi = model.addFactor(vti ,{vi, vi+1});
        }
    }

}
BENCHMARK(GeneralDiscreteGraphicalModelBenchmark)->RangePair(8, 8<<10, 2, 512);



// Define another benchmark
static void SparseDiscreteGraphicalModelBenchmark(benchmark::State& state) {

    while (state.KeepRunning()){
        state.PauseTiming();
        const inferno::LabelType nLabes = state.range_y();
        const inferno::Vi nVar = state.range_x();
        state.ResumeTiming();
        inferno::SparseDiscreteGraphicalModel model;
        for(size_t i=0; i<nVar; ++i)
            model.addVariable(i,nLabes);
        // second order
        for(inferno::Vi vi=0; vi<nVar-1; ++vi){
            auto vti = model.addValueTable(vi,new inferno::value_tables::PottsValueTable(nLabes, 1.0));
            auto fi = model.addFactor(vti, vi, {vi, vi+1});
        }
    }

}
BENCHMARK(SparseDiscreteGraphicalModelBenchmark)->RangePair(8, 8<<10, 2, 512);




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
