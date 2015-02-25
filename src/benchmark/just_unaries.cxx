#include <benchmark/benchmark.h>

#include "inferno/inferno.hxx"
#include "inferno/model/general_discrete_tl_model.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"
#include "inferno/value_tables/explicit.hxx"

#include <opengm/graphicalmodel/graphicalmodel.hxx>
#include <opengm/graphicalmodel/space/simplediscretespace.hxx>
#include <opengm/functions/potts.hxx>
#include <opengm/operations/adder.hxx>

#include <opengm/functions/explicit_function.hxx>
#include <opengm/functions/absolute_difference.hxx>
#include <opengm/functions/potts.hxx>
#include <opengm/functions/pottsn.hxx>
#include <opengm/functions/pottsg.hxx>
#include <opengm/functions/squared_difference.hxx>
#include <opengm/functions/truncated_absolute_difference.hxx>
#include <opengm/functions/truncated_squared_difference.hxx>
#include <opengm/functions/sparsemarray.hxx>



// Define another benchmark
static void InfernoUnary(benchmark::State& state) {

    state.PauseTiming();

    const inferno::LabelType nLabels = state.range_x();
   
    inferno::value_tables::UnaryValueTable vt(nLabels);
    inferno::ValueType res = 0;

    state.ResumeTiming();
    while (state.KeepRunning()){
        res += vt.eval1(0);
        res += vt.eval1(1);
        res += vt.eval1(0);
        res += vt.eval1(1);
    }
    std::cout<<"res"<<res<<"\n";
}


static void InfernoHeapUnary(benchmark::State& state) {

    state.PauseTiming();

    const inferno::LabelType nLabels = state.range_x();
   
    inferno::value_tables::UnaryValueTable * vt = new inferno::value_tables::UnaryValueTable(nLabels);
    inferno::ValueType res = 0;

    state.ResumeTiming();
    while (state.KeepRunning()){
        res += vt->eval1(0);
        res += vt->eval1(1);
        res += vt->eval1(0);
        res += vt->eval1(1);
    }
    std::cout<<"res"<<res<<"\n";
    delete vt;
}



#if 0
// Define another benchmark
static void PottsBenchmarkOpengm(benchmark::State& state) {

    state.PauseTiming();

    const inferno::LabelType nLabels = state.range_y();
    const inferno::Vi gridShape[2] = {state.range_x(),state.range_x()};
    const inferno::Vi nVar = gridShape[0]*gridShape[1];


    std::uniform_real_distribution<float> distribution(-1,1); //Values between -1 and 1
    std::mt19937 engine; // Mersenne twister MT19937
    auto generator = std::bind(distribution, engine);



    typedef double ValueType;
    typedef size_t IndexType;
    typedef size_t LabelType;
    typedef opengm::ExplicitFunction                      <ValueType,IndexType,LabelType> PyExplicitFunction;
    typedef opengm::PottsFunction                         <ValueType,IndexType,LabelType> PyPottsFunction;
    typedef opengm::PottsNFunction                        <ValueType,IndexType,LabelType> PyPottsNFunction;
    typedef opengm::PottsGFunction                        <ValueType,IndexType,LabelType> PyPottsGFunction;
    typedef opengm::TruncatedAbsoluteDifferenceFunction   <ValueType,IndexType,LabelType> PyTruncatedAbsoluteDifferenceFunction;
    typedef opengm::TruncatedSquaredDifferenceFunction    <ValueType,IndexType,LabelType> PyTruncatedSquaredDifferenceFunction;
    typedef opengm::SparseFunction                        <ValueType,IndexType,LabelType> PySparseFunction; 

    typedef  opengm::meta::TypeListGenerator<
        PyExplicitFunction,
        PyPottsNFunction,
        PyPottsGFunction,
        PyTruncatedAbsoluteDifferenceFunction,
        PyTruncatedSquaredDifferenceFunction,
        PySparseFunction,
        PyPottsFunction
    >::type Typelist;



    typedef opengm::DiscreteSpace<size_t, size_t> Space;
    typedef opengm::GraphicalModel<double, opengm::Adder, Typelist    , Space> Model;
    std::vector<size_t> spaceVec(nVar,nLabels);



    Space space(spaceVec.begin(), spaceVec.end());
    Model gm(space);

    



    for(auto y=0; y< gridShape[1]; ++y)
    for(auto x=0; x< gridShape[0]; ++x){
        // function
        const size_t shape[] = {size_t(nLabels)};
        opengm::ExplicitFunction<double> f(shape, shape + 1);
        Model::FunctionIdentifier fid = gm.addFunction(f);

        // factor
        size_t variableIndices[] = { size_t(x + y*gridShape[0])};
        gm.addFactor(fid, variableIndices, variableIndices + 1);
    }






    std::vector<size_t> labels(gm.numberOfVariables(), 0);
    inferno::ValueType res = 0;
    
    state.ResumeTiming();
    while (state.KeepRunning()){
        res += gm.evaluate(labels);
    }

}
#endif

BENCHMARK(InfernoUnary)->Range(2, 256);
BENCHMARK(InfernoHeapUnary)->Range(2, 256);
//BENCHMARK(PottsBenchmarkInfernoTl)->RangePair(8, 2000, 2,16);




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
