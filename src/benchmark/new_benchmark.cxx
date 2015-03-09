

#include "inferno/inferno.hxx"
#include "inferno/model/general_discrete_tl_model.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"
#include "inferno/value_tables/explicit.hxx"

#include "benchmark.hxx"
#include "reference_models.hxx"




int main(int argc, const char* argv[]) {

    using namespace inferno;


    typedef models::GeneralDiscreteModel Model;
    typedef Benchmark<Model> BenchType;
    typedef models::PottsGrid2d<Model> ModelBuilder;
    typedef ModelBuilder::ModelParam ModelParam;
    typedef ModelBuilder::BuilderParam BuilderParam;
    
    BenchType benchFunctor;

    for(size_t x=1;x<10;++x){
        models::PottsGridParam mp{DiscreteLabel(2),x*10,x*10,1.0,true};
        ModelBuilder::buildModelCallFunctor(mp, BuilderParam(),0, benchFunctor);
    }
        


}
