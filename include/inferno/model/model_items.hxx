#ifndef INFERNO_MODEL_MODEL_ITEMS_HXX
#define INFERNO_MODEL_MODEL_ITEMS_HXX

#include <boost/iterator/iterator_facade.hpp>

#include "inferno/inferno.hxx"
#include "inferno/utilities/utilities.hxx"
#include "inferno/value_tables/discrete_value_table_base.hxx"
#include "inferno/utilities/shape_walker.hxx"

namespace inferno{
namespace models{

    template<class MODEL>
    struct ModelFactorItems{

        template<class T>
        struct Map{
            typedef typename MODEL:: template FactorMap<T> type;
        };

        static std::string name(){
            return "Factor";
        }

        static uint64_t size(const MODEL & model){
            return model.nFactors();
        }
    };
    template<class MODEL>
    struct ModelUnaryItems{

        template<class T>
        struct Map{
            typedef typename MODEL:: template UnaryMap<T> type;
        };
        static std::string name(){
            return "Unary";
        }
        static uint64_t size(const MODEL & model){
            return model.nUnaries();
        }
    };
    template<class MODEL>
    struct ModelVariableItems{

        template<class T>
        struct Map{
            typedef typename MODEL:: template VariableMap<T> type;
        };
        static std::string name(){
            return "Variable";
        }
        static uint64_t size(const MODEL & model){
            return model.nVariables();
        }
    };


} // end namespace python
} // end namespace models

#endif /* INFERNO_MODEL_MODEL_ITEMS_HXX */
