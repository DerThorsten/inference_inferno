/** \file qpbo.hxx 
    \brief  Implementation of inferno::inference::Ehc  
*/
#ifndef INFERNO_INFERENCE_EHC_HXX
#define INFERNO_INFERENCE_EHC_HXX

// std
#include <vector>

// vigra
#include <vigra/adjacency_list_graph.hxx>
#include <vigra/merge_graph_adaptor.hxx>
#include <vigra/hierarchical_clustering.hxx>
#include <vigra/priority_queue.hxx>
#include <vigra/graph_algorithms.hxx>

// inferno
#include "inferno/model/discrete_model_base.hxx"
#include "inferno/inference/discrete_inference_base.hxx"



namespace inferno{
namespace inference{

    
    template<class MODEL>
    class Ehc;

    /// \cond
    namespace detail_ehc{

        template<class MODEL >
        class McClusterOp{
        public:

            typedef DiscreteInferenceBase<MODEL> BaseInf;
            typedef typename BaseInf::Visitor Visitor;

            typedef vigra::AdjacencyListGraph Graph;
            typedef vigra::MergeGraphAdaptor< Graph > MergeGraph;

            typedef typename MergeGraph::Edge Edge;
            typedef ValueType WeightType;
            typedef int64_t index_type;
            struct Options
            {
                Options(
                    const ValueType stopWeight = 0.0,
                    const ValueType bias = 0.0
                )
                :   stopWeight_(stopWeight),
                    bias_(bias){
                }
                ValueType stopWeight_;
                ValueType bias_;
            };


            McClusterOp(const Graph & graph , MergeGraph & mergegraph, 
                        const Options & options = Options())
            :
                graph_(graph),
                mergeGraph_(mergegraph),
                pq_(graph.edgeNum()),
                options_(options),
                rWeights_(graph.edgeNum()){

            }

            void reset(){
                pq_.reset();
            }

            void setWeights(const std::vector<ValueType> & weights){
                for(size_t i=0; i<graph_.edgeNum(); ++i){
                    pq_.push(i, weights[i] + options_.bias_);
                }
            }

            Edge contractionEdge(){
                index_type minLabel = pq_.top();
                while(mergeGraph_.hasEdgeId(minLabel)==false){
                    pq_.deleteItem(minLabel);
                    minLabel = pq_.top();
                }
                return Edge(minLabel);
            }

            /// \brief get the edge weight of the edge which should be contracted next
            WeightType contractionWeight(){
                index_type minLabel = pq_.top();
                while(mergeGraph_.hasEdgeId(minLabel)==false){
                    pq_.deleteItem(minLabel);
                    minLabel = pq_.top();
                }
                return pq_.topPriority();
            }

            /// \brief get a reference to the merge
            MergeGraph & mergeGraph(){
                return mergeGraph_;
            }

            bool done()const{
                const bool doneByWeight = pq_.topPriority()<=ValueType(options_.stopWeight_);
                const bool doneByNodeNum = mergeGraph_.nodeNum()<= 1;
                const bool doneByInf = ehc_->stopInference_;
                return doneByWeight || doneByNodeNum || doneByInf;
            }

            void mergeEdges(const Edge & a,const Edge & b){
                rWeights_[a.id()]+=rWeights_[b.id()];
                pq_.push(a.id(), rWeights_[a.id()]);
                pq_.deleteItem(b.id());
            }

            void eraseEdge(const Edge & edge){

                if(visitor_ != nullptr){
                    ehc_->confFromHc(*this, ehc_->conf_);
                    visitor_->visit(*ehc_);
                }
                
                pq_.deleteItem(edge.id());
            }

            const Graph & graph_;
            MergeGraph & mergeGraph_;
            vigra::ChangeablePriorityQueue< ValueType ,std::greater<ValueType> > pq_;

            Options options_;
            std::vector<ValueType> rWeights_;

            Visitor * visitor_;
            Ehc<MODEL> * ehc_;
        };
    } // end namespace inferno::inference::detail_ehc
    /// \endcond




    template<class MODEL>
    class Ehc  : public DiscreteInferenceBase<MODEL> {
    public:
        friend class detail_ehc::McClusterOp<MODEL >;

        typedef MODEL Model;
        typedef Ehc<MODEL> Self;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor Visitor;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;
    private:
        typedef vigra::AdjacencyListGraph Graph;
        typedef vigra::MergeGraphAdaptor< Graph > MGraph;
        typedef detail_ehc::McClusterOp<Model > Cop;
        typedef typename Cop::Options CopOptions;
        typedef vigra::HierarchicalClustering< Cop > HC;
        typedef typename HC::Parameter HcOptions;
    public:

        struct Options{
            Options(){
            }
        };

        Ehc(const Model & model, const Options & options = Options() )
        :   BaseInf(),
            model_(model),
            options_(options),
            stopInference_(false),
            value_(infVal()),
            graph_(),
            mgraph_(nullptr),
            clusterOp_(nullptr),
            weights_(model_.nFactors(),0),
            conf_(model)
        {
            this->setupVigraGraphs();
        }

        ~Ehc(){
            delete clusterOp_;
            delete mgraph_;
        }


        /// \brief update if graph has changed
        ///
        /// Updating the solver after a structural
        /// change of the graph is 
        virtual void graphChange() override{
            delete clusterOp_;
            delete mgraph_;

            mgraph_ = nullptr;
            clusterOp_ = nullptr;
            graph_.clear();

            conf_.assign(model_);
            this->weights_.resize(model_.nFactors());
            std::fill(weights_.begin(), weights_.end(), 0);
            this->setupVigraGraphs();
        }

        /// \brief update if energy has changed
        /// 
        /// Updating the solver after an energy / weight change
        /// is cheap for this solver
        virtual void energyChange() override{
            INFERNO_CHECK(mgraph_!=nullptr,"internal error");
            INFERNO_CHECK(clusterOp_!=nullptr,"internal error");
            std::fill(weights_.begin(), weights_.end(), 0);
            for(const auto  factor : model_.factors()){
                ValueType beta;
                factor->isPotts(beta);
                const auto v0 = model_.variableId(factor->variable(0));
                const auto v1 = model_.variableId(factor->variable(1));
                const auto edge = graph_.findEdge(v0,v1);
                weights_[edge.id()] += beta;
            }
        }

        // MUST HAVE INTERACE
        virtual std::string name() const override {
            return "Ehc";
        }
        // inference
        virtual void infer( Visitor  * visitor  = NULL)override {

            clusterOp_->visitor_ = visitor;
            clusterOp_->ehc_ = this;

            if(visitor!=NULL)
                visitor->begin(this);

            HcOptions p;
            p.verbose_=false;
            p.buildMergeTreeEncoding_=false;

            HC hc(*clusterOp_,p);
            hc.cluster();

            // get conf from hd
            this->confFromHc(hc, conf_);

            if(visitor!=NULL)
                visitor->end(this);
        }
        // get result
        virtual void conf(Conf & confMap ) override{
            for(const auto var : model_.variableDescriptors()){
                confMap[var] = conf_[var];
            }
        }
        virtual DiscreteLabel label(const Vi vi )override {
            return conf_[model_.variableDescriptor(vi)];
        }

        // get model
        virtual const Model & model() const override{
            return model_;
        }
        // stop inference (via visitor)
        virtual void stopInference()override{
            stopInference_ = true;
        }
    private:

        // this function is also called within
        // the hc cluster operator
        void confFromHc(HC & hc, Conf & conf){
            for(const auto var : model_.variableDescriptors()){
                conf[var] = hc.reprNodeId(model_.variableId(var));
            }     
        }
        void setupVigraGraphs(){
            INFERNO_CHECK(mgraph_==nullptr,"internal error");
            INFERNO_CHECK(clusterOp_==nullptr,"internal error");

            INFERNO_CHECK(model_.isSecondOrderMulticutModel(),
                "model must be second order multicut model");

            graph_.reserveMaxNodeId(model_.maxVarId());
            graph_.reserveEdges(model_.nFactors());

            for(const auto var : model_.variableDescriptors()){
                graph_.addNode(model_.variableId(var));
            }
            for(const auto  factor : model_.factors()){
                ValueType beta;
                factor->isPotts(beta);
                const auto v0 = model_.variableId(factor->variable(0));
                const auto v1 = model_.variableId(factor->variable(1));
                const auto edge = graph_.addEdge(v0,v1);
                weights_[edge.id()] += beta;
            }

            mgraph_ = new MGraph(graph_);
            clusterOp_ = new Cop(graph_, *mgraph_ );
            clusterOp_->setWeights(weights_);

        }

        const Model & model_;
        Options       options_;
        bool          stopInference_;
        ValueType     value_;

        vigra::AdjacencyListGraph graph_;
        MGraph * mgraph_;
        Cop * clusterOp_;
        std::vector<ValueType> weights_;
        Conf conf_;
    };


} // end namespace inference
} // end namespace inferno

#endif /* INFERNO_INFERENCE_EHC_HXX */
