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
static void PottsBenchmarkInfernoTl(benchmark::State& state) {

    state.PauseTiming();

    const inferno::LabelType nLabels = state.range_y();
    const inferno::Vi gridShape[2] = {state.range_x(),state.range_x()};
    const inferno::Vi nVar = gridShape[0]*gridShape[1];
    std::vector<inferno::ValueType> uVals(nLabels); 
    std::uniform_real_distribution<float> distribution(-1,1); //Values between -1 and 1
    std::mt19937 engine; // Mersenne twister MT19937
    auto generator = std::bind(distribution, engine);


    inferno::TlModel<inferno::value_tables::PottsValueTable, inferno::value_tables::UnaryValueTable> model(nVar, nLabels);



    // second order
    for(auto y=0; y< gridShape[1]; ++y)
    for(auto x=0; x< gridShape[0]; ++x){

        if(x+1 <gridShape[0]){
            // get the flat variable index
            const inferno::Vi vi0 = x + y*gridShape[0];
            const inferno::Vi vi1 = x + 1 + y*gridShape[0];
            // add the value table to the model
            auto vti = model.addValueTable(inferno::value_tables::PottsValueTable(nLabels, 1.0));
            // add the factor connected to the just added value table
            // and the 2 variable indices 
            auto fi = model.addFactor(vti ,{vi0, vi1});
        }
        if(y+1 <gridShape[1]){
            // get the flat variable index
            const inferno::Vi vi0 = x + y*gridShape[0];
            const inferno::Vi vi1 = x + (1 + y)*gridShape[0];
            // add the value table which encodes the second order function to the model
            auto vti = model.addValueTable(inferno::value_tables::PottsValueTable(nLabels, 1.0));
            // add the factor connected to the just added value table
            // and the 2 variable indices 
            auto fi = model.addFactor(vti ,{vi0, vi1});
        }
    }



    //inferno::GeneralDiscreteGraphicalModel::VariableMap<inferno::DiscreteLabel>  labels(model, 0);
    inferno::ValueType res = 0;
    std::vector<inferno::DiscreteLabel> labels(model.nVariables(),0);
    state.ResumeTiming();
    while (state.KeepRunning()){
        res += model.eval(labels.begin());
    }

}



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
        if(x + 1 < gridShape[0]) { // (x, y) -- (x + 1, y)
            const inferno::Vi vi0 = x + y*gridShape[0];
            const inferno::Vi vi1 = x + 1 + y*gridShape[0];
            const inferno::Vi vis[2] = {vi0,vi1};
            opengm::PottsFunction<double> f(nLabels, nLabels, 0.0, 1.0);
            Model::FunctionIdentifier fid = gm.addFunction(f);
            gm.addFactor(fid, vis, vis + 2);
        }
        if(y + 1 < gridShape[1]) { // (x, y) -- (x, y + 1)
            const inferno::Vi vi0 = x + y*gridShape[0];
            const inferno::Vi vi1 = x + (1 + y)*gridShape[0];
            const inferno::Vi vis[2] = {vi0,vi1};
            opengm::PottsFunction<double> f(nLabels, nLabels, 0.0, 1.0);
            Model::FunctionIdentifier fid = gm.addFunction(f);
            gm.addFactor(fid, vis, vis + 2);
        }
    }   




    std::vector<size_t> labels(gm.numberOfVariables(), 0);
    inferno::ValueType res = 0;
    
    state.ResumeTiming();
    while (state.KeepRunning()){
        res += gm.evaluate(labels);
    }

}


BENCHMARK(PottsBenchmarkOpengm)->RangePair(8, 2000, 2, 16);
BENCHMARK(PottsBenchmarkInfernoTl)->RangePair(8, 2000, 2,16);



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
