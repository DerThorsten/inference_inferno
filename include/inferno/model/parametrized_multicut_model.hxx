#ifndef INFERNO_MODEL_PARAMETRIZED_MULTICUT_MODEL_HXX
#define INFERNO_MODEL_PARAMETRIZED_MULTICUT_MODEL_HXX

#include <boost/iterator/counting_iterator.hpp>

#include "inferno/inferno.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/value_tables/discrete_unary_value_table_base.hxx"
#include "inferno/model/discrete_factor_base.hxx"
#include "inferno/model/discrete_unary_base.hxx"
#include "inferno/model/simple_discrete_model_base.hxx"

namespace inferno{

    // forward declarations
    namespace models{
    template<class MODEL>
        class ParametrizedMulticutModelFactor;

        template<class EDGES, class FEATURES>
        class ParametrizedMulticutModel;
    }

    namespace value_tables{

        template<class MODEL>
        class  ParametrizedMulticutModelPottsValueTable : public DiscreteValueTableBase{
        public:
            friend class inferno::models::ParametrizedMulticutModelFactor<MODEL>;
            using DiscreteValueTableBase::eval;
            ParametrizedMulticutModelPottsValueTable(const MODEL & model, const uint64_t edge)
            :   DiscreteValueTableBase(),
                model_(&model),
                edge_(edge)
            {
            }
            ValueType eval(const LabelType *conf)const{
                return conf[0] == conf[1] ? 0.0 : beta();
            }
            ValueType eval(const LabelType l1, const LabelType l2)const{
                return l1==l2 ? 0 : beta();
            }
            LabelType shape(const uint32_t d) const{
                return model_->nVar_;
            }
            uint32_t  arity()const{
                return 2;
            }
            bool isGeneralizedPotts() const{
                return true;
            }
            bool isPotts(ValueType & beta) const{
                beta = this->beta();
                return true;
            }

            void facToVarMsg(const ValueType ** inMsgs, ValueType ** outMsgs)const{
                pottsFacToVarMsg(this->shape(0), this->beta(), inMsgs, outMsgs);
            }


            // learning
            virtual uint64_t nWeights()const{
                return model_->currentWeights_->size();
            }
            //global is local here
            virtual int64_t weightIndex(const size_t i)const{
                return i;
            }

            virtual WeightType weightGradient(const size_t vtsWeightIndex, const DiscreteLabel * conf)const{
                return conf[0]==conf[1] ? 0.0 : model_->features_(edge_, vtsWeightIndex);
            }

            virtual void updateWeights(const learning::Weights & weights)const{
            }


        private:
            const MODEL & model()const{
                return *model_;
            }
            const uint64_t edgeDescriptor()const{
                return edge_;
            }
            ValueType beta()const{
                return model_->betas_[edge_];
            }
            const MODEL * model_;
            const uint64_t edge_;
            
        };

    }

namespace models{

    template<class MODEL>
    class ParametrizedMulticutModelFactor
    : public  DiscreteFactorBase<ParametrizedMulticutModelFactor<MODEL>, MODEL> {

        typedef typename  MODEL::VariableDescriptor VariableDescriptor;
        typedef inferno::value_tables::ParametrizedMulticutModelPottsValueTable<MODEL> Vt;
    public:
        ParametrizedMulticutModelFactor(const MODEL & model, const uint64_t edge)
        :   vt_(model, edge){   
        }

        const value_tables::DiscreteValueTableBase * valueTable()const{
            return &vt_;
        }   

        ArityType arity()const{
            return 2;
        }

        DiscreteLabel shape(const size_t d)const{
            return vt_.shape(d);
        }

        VariableDescriptor variable(const size_t d)const{
            return vt_.model().edges_[vt_.edgeDescriptor()][d];
        }
    private:
        Vt vt_;
    };



    template<class EDGES, class FEATURES>
    class ParametrizedMulticutModel :
    public SimpleDiscreteGraphicalModelBase<ParametrizedMulticutModel<EDGES, FEATURES> >{

        typedef ParametrizedMulticutModel<EDGES, FEATURES> Self;
        friend class ParametrizedMulticutModelFactor< Self >;
        friend class inferno::value_tables::ParametrizedMulticutModelPottsValueTable< Self >;
       
        typedef ParametrizedMulticutModelFactor<Self> FactorImpl;
        typedef DeadCodeUnary<Self> UnaryImpl;
    public:
        typedef FactorImpl FactorProxy;
        typedef UnaryImpl UnaryProxy;



        ParametrizedMulticutModel()
        :   nVar_(0),
            edges_(),
            features_(),
            betas_(),
            currentWeights_(nullptr){
        }

        ParametrizedMulticutModel(const uint64_t nVar, const EDGES & edges, const FEATURES &features,
                                  const learning::WeightVector & weights)
        :   nVar_(nVar),
            edges_(edges),
            features_(features),
            betas_(edges.shape(0)),
            currentWeights_(&weights){
                this->makeBetas();
        }

        void assign( const uint64_t nVar, const EDGES & edges, const FEATURES &features,
                const learning::WeightVector & weights)
        {
            nVar_ = nVar;
            edges_ = edges;
            features_ = features;
            betas_.reshape(  edges.shape());
            currentWeights_ = & weights;
            this->makeBetas();
        }




        /** \brief get the number of labels for variable

            get the number of labels for the variable associated
            with the passed variableDescriptor
        */
        DiscreteLabel nLabels(const typename ParametrizedMulticutModel::VariableDescriptor var)const{
            return nVar_;
        }

        FactorProxy factor(const typename ParametrizedMulticutModel::FactorDescriptor factorDescriptor)const{
            const auto & uv = edges_(factorDescriptor);
            return FactorProxy(*this, factorDescriptor);
        }
        UnaryProxy unary(const typename ParametrizedMulticutModel::UnaryDescriptor unaryDescriptor)const{
            return UnaryProxy();
        }


        // overloads / specializations
        // Non of these are required
        bool hasSimpleLabelSpace(DiscreteLabel & l)const{
            l=nVar_;
            return true;
        }

        ArityType maxArity() const {
            return 2;
        }
        void minMaxNLabels(DiscreteLabel & minNumberOfLabels, DiscreteLabel & maxNumberOfLabels)const{
            minNumberOfLabels = nVar_;
            maxNumberOfLabels = nVar_;
        }

        uint64_t nVariables()const{
            return nVar_;
        }
        uint64_t nFactors()const{
            return betas_.size();
        }
        uint64_t nUnaries()const{
            return 0;
        }

       
        bool denseVariableIds()const{
            return true;
        }
        bool hasUnaries()const{
            return false;
        }
        Fi nPairwiseFactors()const{
            return this->nFactors();
        }


        void updateWeights(const learning::WeightVector & weights)const{
            currentWeights_ = &weights;
            makeBetas();
        }



    private:

        void makeBetas()const{
            //std::cout<<"make betas\n";
            for(size_t e=0; e<betas_.shape(0); ++e){
                ValueType val = 0;
                for(size_t f=0; f<features_.shape(1); ++f){
                    val += features_(e,f)*(*currentWeights_)[f];
                }
                //std::cout<<"e "<<e<<" beta "<<val<<"\n";
                betas_[e] = val;
            }
        }
        uint64_t nVar_;
        EDGES edges_;
        FEATURES features_;
        mutable vigra::MultiArray<1, ValueType> betas_;
        mutable const learning::WeightVector * currentWeights_;
    };

} // end namespace models
} // end namespace inferno




#endif /*INFERNO_MODEL_PARAMETRIZED_MULTICUT_MODEL_HXX*/
