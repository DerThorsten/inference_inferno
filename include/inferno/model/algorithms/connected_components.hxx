#ifndef INFERNO_MODEL_ALGORITHMS_CONNECTED_COMPONENTS_HXX
#define INFERNO_MODEL_ALGORITHMS_CONNECTED_COMPONENTS_HXX

#include "inferno/inferno.hxx"
#include "inferno/model/discrete_model_base.hxx"
#include "inferno/utilities/ufd.hxx"


namespace inferno{
namespace models{


    template<class MODEL>
    class ConnectedComponents{
    public:
        typedef MODEL Model;
        struct Options{
            Options(const bool makeDense = true)
            :   makeDense_(makeDense){
            }
            bool makeDense_;
        };

        ConnectedComponents(const Model & model, const Options & options = Options())
        :   model_(model),
            options_(options),
            denseIds_(model),
            ufd_(model_.nVariables()),
            relabeling_(),
            anchors_(model_.nVariables())
        {
            relabeling_.reserve(model_.nVariables());
        } 

        template<class CONF>
        Vi run(CONF & conf){
            for(const auto factor : model_.factors()){
                const auto arity = factor->arity();
                for(auto v0=0; v0<arity -1; ++v0){
                    const auto vi0 = factor->variable(v0);
                    const auto l0 = conf[vi0];
                    for(auto v1=v0+1; v1<arity; ++v1){
                        const auto vi1 = factor->variable(v1);
                        const auto l1 = conf[vi1];
                        if(l0 == l1){
                            ufd_.merge(denseIds_.toDense(vi0),denseIds_.toDense(vi1));
                        }
                    }
                }
            }
            anchors_.resize(0);
            const auto makeDense = options_.makeDense_;
            if(makeDense)
                ufd_.representativeLabeling(relabeling_);
            DiscreteLabel maxLabel = 0;
            for(const auto varDesc : model_.variableDescriptors()){
                const auto vi = model_.variableId(varDesc);
                const auto denseVi = denseIds_.toDense(vi);
                const DiscreteLabel reprLabel = ufd_.find(denseVi);
                if(reprLabel == denseVi){
                    anchors_.push_back(vi);
                }
                conf[vi] = makeDense ? relabeling_[reprLabel] : reprLabel;
                maxLabel = std::max(maxLabel, conf[vi]);
            }
            if(makeDense){
                INFERNO_CHECK_OP(maxLabel+1,==,relabeling_.size(),"");
                INFERNO_CHECK_OP(maxLabel+1,==,ufd_.numberOfSets(),"");
            }
            return maxLabel;
        }

        const std::vector<Vi> & anchors() const{
            return anchors_;
        }
    private:
        typedef inferno::models::DenseVariableIds<Model> DenseVarIds;


        const Model & model_;
        Options options_;
        DenseVarIds denseIds_;
        inferno::utilities::Ufd<DiscreteLabel> ufd_;
        std::unordered_map<DiscreteLabel, DiscreteLabel> relabeling_;
        std::vector<Vi> anchors_;

    };


} // end namespace inferno::models
} // inferno

#endif /* INFERNO_MODEL_ALGORITHMS_CONNECTED_COMPONENTS_HXX */
