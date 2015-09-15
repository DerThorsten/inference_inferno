#ifndef INFERNO_MODELS_MAPS_MODEL_MAPS_HXX
#define INFERNO_MODELS_MAPS_MODEL_MAPS_HXX

namespace inferno{
namespace models{
namespace maps{

    /// \cond
    namespace detail_maps{
        struct FactorHelper{
            template<class MODEL>
            static uint64_t size(const MODEL & m){
                return m.nFactors();
            }
        };
        struct VariableHelper{
            template<class MODEL>
            static uint64_t size(const MODEL & m){
                return m.nVariables();
            }
        };
        struct UnaryHelper{
            template<class MODEL>
            static uint64_t size(const MODEL & m){
                return m.nUnaries();
            }
        };
        struct ConstraintHelper{
            template<class MODEL>
            static uint64_t size(const MODEL & m){
                return m.nConstraints();
            }
        };

        template<class MODEL, class T, class SIZE_HELPER>
        class SimpleMapBase :
            public std::vector<T>
        {
            typedef MODEL Model;
        public:
            SimpleMapBase()
            :   std::vector<T>(),
                model_(nullptr){
            }
            SimpleMapBase(const MODEL & model, const T & val = T())
            :   std::vector<T>(SIZE_HELPER::size(model),val),
                model_(&model){
            }

            void assign(const MODEL & model, const T & val = T()){
                model_ = & model;
                this->resize(0);
                this->resize(SIZE_HELPER::size(model), val);
            }

            const MODEL & model()const{
                return *model_;
            }

        private:
            const MODEL * model_;
        };

    } // end namespace inferno::models::maps::detail_maps
    /// \endcond

    template<class MODEL, class T>
    class SimpleVariableMap : public detail_maps::SimpleMapBase<MODEL, T, detail_maps::VariableHelper>{
        typedef detail_maps::SimpleMapBase<MODEL, T, detail_maps::VariableHelper> Base;
    public:
        SimpleVariableMap()
        :   Base(){
        }
        SimpleVariableMap(const MODEL & model, const T val = T())
        :   Base(model, val){
        }
    };

    template<class MODEL, class T>
    class SimpleFactorMap : public detail_maps::SimpleMapBase<MODEL, T, detail_maps::FactorHelper>{
        typedef detail_maps::SimpleMapBase<MODEL, T, detail_maps::FactorHelper> Base;
    public:
        SimpleFactorMap()
        :   Base(){
        }
        SimpleFactorMap(const MODEL & model, const T val = T())
        : Base(model, val){
        }
    };

    template<class MODEL, class T>
    class SimpleUnaryMap : public detail_maps::SimpleMapBase<MODEL, T, detail_maps::UnaryHelper>{
        typedef detail_maps::SimpleMapBase<MODEL, T, detail_maps::UnaryHelper> Base;
    public:
        SimpleUnaryMap()
        :   Base(){
        }
        SimpleUnaryMap(const MODEL & model, const T val = T())
        : Base(model, val){
        }
    };

    template<class MODEL, class T>
    class SimpleConstraintMap : public detail_maps::SimpleMapBase<MODEL, T, detail_maps::ConstraintHelper>{
        typedef detail_maps::SimpleMapBase<MODEL, T, detail_maps::ConstraintHelper> Base;
    public:
        SimpleConstraintMap()
        :   Base(){
        }
        SimpleConstraintMap(const MODEL & model, const T val = T())
        : Base(model, val){
        }
    };


} // end namespace inferno::models::maps
} // end namespace inferno::models
} // end namespace inferno


#endif /* INFERNO_MODELS_MAPS_MODEL_MAPS_HXX */
