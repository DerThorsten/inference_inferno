#include <benchmark/benchmark.h>

#include "inferno/inferno.hxx"
#include "inferno/model/sparse_discrete_model.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"


#define  ADD_BENCHMARK(BNAME) \
int main(int argc, const char* argv[]) { \
    std::cout<<" NAME "<<BNAME<<"\n"; \
    return 0; \
} \



struct Benchmark{
    
};


ADD_BENCHMARK("model/model_setup/grid/general_discrete_model");
