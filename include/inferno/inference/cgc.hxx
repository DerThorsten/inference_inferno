/** \file qpbo.hxx 
    \brief  Implementation of inferno::inference::Cgc  

    \warning To include this header one needs qpbo
    from  inferno_externals
    (in CMake ''WITH_QPBO'')
*/
#ifndef INFERNO_INFERENCE_QPBO_HXX
#define INFERNO_INFERENCE_QPBO_HXX

#include <unordered_map>

#include "inferno/model/discrete_model_base.hxx"
#include "inferno/inference/discrete_inference_base.hxx"

#ifdef WITH_QPBO
#include <inferno_externals/qpbo/QPBO.h>
#else 
#error "To include inferno/infernece/cgc.hxx, WITH_QPBO needs to be" 
#endif 

namespace inferno{
namespace inference{

    
    template<class MODEL>
    class Cgc  : public DiscreteInferenceBase<MODEL> {
    public:
        typedef MODEL Model;
        typedef Cgc<MODEL> Self;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor Visitor;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;

        typedef typename MODEL:: template VariableMap<DiscreteLabel>    ArgPrimal;
        typedef typename MODEL:: template FactorMap<DiscreteLabel>      ArgDual;
        typedef typename MODEL:: template FactorMap<ValueType>          Betas;

        std::vector<VariableDescriptor> VarVec;

        typedef inferno::models::DenseVariableIds<Model> DenseVarIds;
        typedef kolmogorov::qpbo::QPBO<ValueType> CgcSolver;

        typedef  std::pair<VariableDescriptor, VariableDescriptor> VarPair;
        typedef  std::pair<DiscreteLabel, DiscreteLabel> LabelPair;

        struct Options{
            Options(
                const uint64_t maxIterations = 1
            ) 
            :   maxIterations_(maxIterations)
            {
            }
            uint64_t maxIterations_;
        };



        Cgc(const Model & model, const Options & options = Options() )
        :   BaseInf(),
            model_(model),
            bestConf_(model,0),
            denseVarIds_(model),
            options_(options),
            stopInference_(false),
            bound_(-1.0*infVal()),
            value_(infVal()){


            // check for valid models
            {
                INFERNO_CHECK_OP(model.maxArity(),<=,2,
                    "Cgc is only allowed for models with maxArity<=2");

                LabelType nLabels;
                const bool simpleLabelSpace  =  model_.hasSimpleLabelSpace(nLabels);
                INFERNO_CHECK(simpleLabelSpace && nLabels == 2, "Cgc is only implemented for models "
                                                                "where all variables  have a binary label space");
            } 
            
            const Vi nVar = model_.nVariables();

            // construct qpbo
            qpbo_ = new CgcSolver(nVar, model_.nPairwiseFactors());
            qpbo_->AddNode(nVar);


            for(const auto unary : model_.unaries()){
                const int qpboVi0 = denseVarIds_.toDenseId(unary->variable());
                qpbo_->AddUnaryTerm(qpboVi0, unary->eval(0l), unary->eval(1l));
            }
            for(const auto factor : model_.factors()){
                const auto arity = factor->arity();
                if(arity == 2){
                    const int qpboVi0 = denseVarIds_.toDenseId(factor->variable(0));
                    const int qpboVi1 = denseVarIds_.toDenseId(factor->variable(1));
                    INFERNO_CHECK_OP(qpboVi0,!=,qpboVi1,"");
                    qpbo_->AddPairwiseTerm(qpboVi0, qpboVi1,factor->eval(0,0), factor->eval(0,1),
                        factor->eval(1,0), factor->eval(1,1));
                }
                else{
                    throw RuntimeError("INTERNAL ERROR: model_.maxArity() must have a bug");
                }
            }
            qpbo_->MergeParallelEdges();
        }

        ~Cgc(){
            delete qpbo_;
        }

        // MUST HAVE INTERACE
        virtual std::string name() const {
            return "Cgc";
        }
        // inference
        virtual void infer( Visitor  * visitor  = NULL) {

            this->initPhase();

            auto preCut = this->energy();;
            auto postCut = preCut;
            auto preGlue = preCut;
            auto postGlue = preCut;

            for(const auto iter : range(options_.maxIterations_)){
        
                if(i==0){
                    this->cutPhase();
                    this->glueAndCutPhase();
                }
            }

        }
        // get result
        virtual void conf(Conf & confMap ) {
            
        }
        virtual DiscreteLabel label(const Vi vi ) {

        }



        // get model
        virtual const Model & model() const{
            return model_;
        }

        virtual void setConf(const Conf & conf){
            for(const auto var : model_.variableDescriptors())
                bestConf_[var] = conf[var];
        }

        // stop inference (via visitor)
        virtual void stopInference(){
            stopInference_ = true;
        }
        // get results optional interface
        virtual ValueType lowerBound(){
            return bound_;
        }
        virtual ValueType upperBound(){
            return value_;
        }
        virtual ValueType energy(){
            return value_;
        }

    private:

        VarPair uvInternalSorted(const uint64_t internalEdge)const{

        }
        LabelPair sortedLabelPair(const VarPair varPair)const{

        }
        ValueType betaInternal(const uint64_t internalEdge)const{

        }


        bool isCut(const VarPair & varPair)const{
            
        }
        bool isCut(const uint64_t internalEdge )const{
            
        }
        uint64_t nInternalEdges()const{
        }

        void startFromHc(Visitor * visitor ){
        }
        void initPhase(Visitor  * visitor ){
        }
        void cutPhase(Visitor * visitor ){
        }


        void fillMap(std::map<LabelPair, ValAndFac> & map){
            map.clear();
            for(const auto ei : range(nInternalEdges())){
                const auto uv =  uvInternalSorted(ei);
                const auto luv = labelPair(uv);
                if(luv.first !=  luv.second){
                    const auto beta = betaInternal(ei);
                    auto iter = ccMap.find(luv);
                    if(iter == ccMap.end()){
                        ccMap.insert(std::make_pair(luv, ValAndFac(beta,ei)));
                    }
                    else{
                        iter->second.first +=beta;
                    }
                }
            } 
        }

        bool glueAndCutPhase(Visitor * visitor ){

            typedef std::pair<ValueType, uint64_t> ValAndFac;
            std::map<LabelPair, ValAndFac> ccMap;

            bool anyImprovment = false
            bool improvmentInRound = true;

            while(improvmentInRound){
                
                improvmentInRound = false;
                this->fillMap(ccMap);

                // do we have any cc's
                if(ccMap.size() >= 2){  
                    for(const auto iter : ccMap){
                        const auto ei = iter->second->second;
                        if(isCut(ei)){
                            const bool improvment = doSingleGlueAndCut(ei);
                            if(improvment){
                                anyImprovment = true;
                                improvmentInRound= true;
                            }
                        }
                    }
                }   
            }
            return anyImprovment;
        }


        void doSingleGlueAndCut(const uint64_t eInternal){

            // initialize this move
            this->preMove();

            // setup the submodel
            this->setupModel2();

            // solve submodel
            this->solveSubmodel();

            // finish this move
            this->postMove()
        }

        void getGlueVarInCC(const uint64_t eInternal, VarVec & varVec){

        }


        void setupModel1(const uint64_t eInternal){

        }
        void setupModel2(const uint64_t eInternal){
            
        }
        void solveSubmodel(){

            // qpbo

            // planar max cut

            // hc

            // sw

            // optimal(?)

        }



        void preMove(){
            varVec_.resize(0);
        }

        void postMove(){

        }


        const Model & model_;
        
        ArgPrimal     bestConf_;

        DenseVarIds   denseVarIds_;
        Options       options_;
        bool          stopInference_;
        ValueType     bound_;
        ValueType     value_;

        VarVec varVec_;
    };


} // end namespace inference
} // end namespace inferno

#endif /* INFERNO_INFERENCE_QPBO_HXX */
