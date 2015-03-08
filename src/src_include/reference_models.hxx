#include "inferno/inferno.hxx"
#include "inferno/model/sparse_discrete_model.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/model/general_discrete_tl_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"

#include "inferno/utilities/timer.hxx"

#include "rep_rand.hxx"

namespace inferno{
namespace models{


    template<class MODEL>
    struct PottsGrid2d;

    struct PottsGridParam{

        //PottsGridParam(){
        //    nl_        = 2;
        //    sizeX_     = 10;
        //    sizeY_     = 10;
        //    betaScale_ = 1.0;
        //    submodular_ = true;
        //}

        DiscreteLabel nl_;
        size_t        sizeX_;
        size_t        sizeY_;
        double        betaScale_;
        bool          submodular_;
    };


    template< >
    struct PottsGrid2d<GeneralDiscreteGraphicalModel>{
        typedef GeneralDiscreteGraphicalModel Model;
        typedef PottsGridParam ModelParam;
        struct BuilderParam{
            BuilderParam(){
                continousModel_ = true;
            }
            bool continousModel_;
        };

        template<class FUNCTOR>
        static void buildModelCallFunctor(const ModelParam & mp, 
                                          const BuilderParam & bp, 
                                          const size_t seed,
                                          FUNCTOR & functor){

            utilities::ReprRandom rgen(seed);

            const LabelType nLabels = mp.nl_;
            const Vi gridShape[2] = {Vi(mp.sizeX_), Vi(mp.sizeY_)};
            const Vi nVar = gridShape[0]*gridShape[1];

            Model model(nVar, nLabels, !bp.continousModel_);

            uint8_t * memh;
            if(bp.continousModel_){

                const auto nUnaries = nVar; 
                const auto n2Order  = ((gridShape[0]-1) * gridShape[1]) + ((gridShape[1]-1) * gridShape[0]);
                const auto sU = sizeof(inferno::value_tables::UnaryViewValueTable);
                const auto sSO = sizeof(inferno::value_tables::PottsValueTable);


                const auto memsize = (sU + sizeof(inferno::ValueType)*nLabels )*nUnaries + sSO*n2Order;
                memh = new uint8_t[memsize];
                uint8_t * mem = memh;

                // unary factors
                for(auto y=0; y< gridShape[1]; ++y)
                for(auto x=0; x< gridShape[0]; ++x){
                    
                    // get the flat variable index
                    const inferno::Vi vi = x + y*gridShape[0];
                    inferno::ValueType * data = reinterpret_cast<inferno::ValueType *>(mem+sU);
                    for(DiscreteLabel l=0; l<nLabels; ++l){
                        data[l] = rgen.rDouble();
                    }
                    auto vti = model.addValueTable(new (mem) inferno::value_tables::UnaryViewValueTable(nLabels, data) );
                    mem += sU + sizeof(inferno::ValueType)*nLabels;
                    auto fi = model.addFactor(vti ,{vi});
                }


                // second order
                for(auto y=0; y< gridShape[1]; ++y)
                for(auto x=0; x< gridShape[0]; ++x){
                    if(x+1 <gridShape[0]){
                        double beta = mp.submodular_ ? rgen.rDouble()*mp.betaScale_ : (rgen.rDouble()-0.5)*2.0*mp.betaScale_ ;
                        const inferno::Vi vi0 = x + y*gridShape[0];
                        const inferno::Vi vi1 = x + 1 + y*gridShape[0];
                        auto vti = model.addValueTable(new(mem) inferno::value_tables::PottsValueTable(nLabels, beta));
                        mem += sSO; 
                        auto fi = model.addFactor(vti ,{vi0, vi1});
                    }
                    if(y+1 <gridShape[1]){
                        double beta = mp.submodular_ ? rgen.rDouble()*mp.betaScale_ : (rgen.rDouble()-0.5)*2.0*mp.betaScale_ ;
                        const inferno::Vi vi0 = x + y*gridShape[0];
                        const inferno::Vi vi1 = x + (1 + y)*gridShape[0];
                        auto vti = model.addValueTable(new(mem) inferno::value_tables::PottsValueTable(nLabels, 1.0));
                        mem += sSO;
                        auto fi = model.addFactor(vti ,{vi0, vi1});
                    }
                }

                // MODEL IS SETUP

            }



            functor(model);
            if(bp.continousModel_){
                delete[] memh;
             }

        }


    };

}
}
