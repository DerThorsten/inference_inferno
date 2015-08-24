/** \file qpbo.hxx 
    \brief  Implementation of inferno::inference::Qpbo  

    \warning To include this header one needs qpbo
    from  inferno_externals
    (in CMake ''WITH_QPBO'')
*/
#ifndef INFERNO_INFERENCE_OPENGM_INFERENCE_HXX
#define INFERNO_INFERENCE_OPENGM_INFERENCE_HXX


#include "inferno/model/discrete_model_base.hxx"
#include "inferno/inference/discrete_inference_base.hxx"

#include <opengm/opengm.hxx>
#include <opengm/functions/potts.hxx>
#include <opengm/graphicalmodel/graphicalmodel.hxx>

namespace inferno{
namespace inference{

    namespace ogm{

        typedef opengm::DiscreteSpace<uint64_t, uint64_t>           Space;
        typedef opengm::ExplicitFunction<double,uint64_t,uint64_t>  ExplicitFunction;
        typedef opengm::PottsFunction<double,uint64_t,uint64_t>    PottsFunction;
        typedef opengm::meta::TypeListGenerator<ExplicitFunction, PottsFunction>::type FunctionTypes;
        typedef opengm::GraphicalModel<double, opengm::Adder, FunctionTypes, Space> GraphicalModel;
    }

    typedef ogm::GraphicalModel OgmModel;

    



    template<class MODEL, class OGM_INF>
    class OpengmInference  : public DiscreteInferenceBase<MODEL> {
    public:
        typedef MODEL Model;
        typedef OpengmInference<MODEL, OGM_INF> Self;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor Visitor;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;
        typedef inferno::models::DenseVariableIds<Model> DenseVarIds;
        typedef OGM_INF OpengmSolver;
        typedef typename OpengmSolver::Parameter OpengmSolverOptions;
        
        typedef OpengmSolverOptions Options;


        OpengmInference(const Model & model, const Options & options = Options() )
        :   BaseInf(),
            model_(model),
            denseVarIds_(model),
            options_(options),
            stopInference_(false),
            constTerm_(0.0),
            bound_(-1.0*infVal()),
            value_(infVal()),
            conf_(model,0),
            ogmModel_()
        {

            // setup space
            std::vector<uint64_t> nLabels(model_.nVariables());
            for(const auto var : model_.variableDescriptors()){
                const auto dId = denseVarIds_.toDenseId(var);
                nLabels[dId] = model_.nLabels(var);
            }
            ogm::Space space(nLabels.begin(), nLabels.end());
            ogmModel_.assign(space);

            // add unary-factors
            for(const auto unary : model_.unaries()){
                const DiscreteLabel nL = unary->shape();
                ogm::ExplicitFunction e(&nL, &nL+1);
                for(DiscreteLabel l=0; l<nL; ++l){
                    e(l) = unary->eval(l);
                }
                const uint64_t vi = unary.variable();
                auto fid = ogmModel_.addFunction(e);
                ogmModel_.addFactor(fid, &vi,&vi+1);
            }

            for(const auto factor : model_.factors()){
                const auto arity = factor->arity();
                INFERNO_CHECK_OP(arity,<=,2, "currently only factors with order 2 are allowed");

                if(arity == 2){

                    const Vi visInferno[2] = {
                        denseVarIds_.toDenseId(factor->variable(0)),
                        denseVarIds_.toDenseId(factor->variable(1))
                    };
                    const DiscreteLabel shapeInferno[2] = {
                       model_.nLabels(factor->variable(0)),
                       model_.nLabels(factor->variable(1))
                    };
                    const bool isSorted = visInferno[0] < visInferno[1];
                    const Vi visOgm[2] = {
                        isSorted ? visInferno[0] : visInferno[1],
                        isSorted ? visInferno[1] : visInferno[0]
                    };
                    const DiscreteLabel shapeOgm[2] = {
                        isSorted ? shapeInferno[0] : shapeInferno[1],
                        isSorted ? shapeInferno[1] : shapeInferno[0]
                    };


                    ValueType beta;
                    if(factor->isPotts(beta)){
                        ogm::PottsFunction pf(shapeOgm[0],shapeOgm[1], 0.0, beta);
                        auto fid = ogmModel_.addFunction(pf);
                        ogmModel_.addFactor(fid, visOgm,visOgm+2);
                    }
                    else{
                        ogm::ExplicitFunction ef(shapeOgm,shapeOgm+2);
                        for(DiscreteLabel l0=0; l0<shapeInferno[0]; ++l0)
                        for(DiscreteLabel l1=0; l1<shapeInferno[1]; ++l1){
                            const auto val = factor->eval(l0,l1);
                            if(isSorted){
                                ef(l0,l1) = val;
                            }
                            else{
                                ef(l1,l0) = val;
                            }
                        }
                        auto fid = ogmModel_.addFunction(ef);
                        ogmModel_.addFactor(fid, visOgm,visOgm+2);
                    }
                }
            }
           
        }

        ~OpengmInference(){
           
        }

        // MUST HAVE INTERACE
        virtual std::string name() const {
            return "OpengmInference";
        }
        // inference
        virtual void infer( Visitor  * visitor  = NULL) {

            if(visitor!=NULL)
                visitor->begin(this);
           

            // create opengm solver
            OGM_INF inf(ogmModel_, options_);
            inf.infer();
            // extract arg
            std::vector<uint64_t> ogmArg;
            inf.arg(ogmArg);


            // extract conf
            for(const auto var: model_.variableDescriptors()){
                const auto vid = model_.variableId(var);
                const auto dvid = denseVarIds_.toDenseId(vid);

                conf_[var] = ogmArg[dvid];
            }

            if(visitor!=NULL)
                visitor->end(this);
        }

        // get result
        virtual void conf(Conf & confMap ) {
            for(const auto var: model_.variableDescriptors()){
                confMap[var] = conf_[var];
            }
        }
        virtual DiscreteLabel label(const Vi vi ) {
            return conf_[model_.variableDescriptor(vi)];
        }

        virtual bool isPartialOptimal(const Vi vi){
            return false;
        }

        // get model
        virtual const Model & model() const{
            return model_;
        }
        // stop inference (via visitor)
        virtual void stopInference(){
           stopInference_ = true;
        }

        // // get results optional interface
        // virtual ValueType lowerBound(){
            
        // }
        // virtual ValueType upperBound(){
        // }
        // virtual ValueType energy(){
        // }

    private:


        const Model & model_;
        DenseVarIds   denseVarIds_;
        Options       options_;
        bool          stopInference_;
        ValueType     constTerm_;
        ValueType     bound_;
        ValueType     value_;

        // 
        Conf conf_;

        // opengm
        OgmModel ogmModel_;

    };


} // end namespace inference
} // end namespace inferno

#endif /* INFERNO_INFERENCE_OPENGM_INFERENCE_HXX */
