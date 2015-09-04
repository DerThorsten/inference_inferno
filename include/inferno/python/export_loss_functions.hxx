#ifndef INFERNO_PYTHON_EXPORT_MODEL_MAPS_HXX
#define INFERNO_PYTHON_EXPORT_MODEL_MAPS_HXX

// boost
#include <boost/python/def_visitor.hpp>
#include <boost/python/suite/indexing/indexing_suite.hpp>
#include <boost/python/copy_non_const_reference.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/mpl/if.hpp>

// vigra numpy array converters
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>

// inferno 
#include "inferno/model/maps/model_maps.hxx"
#include "inferno/model/model_items.hxx"



namespace inferno{
namespace learning{
namespace loss_functions{


    namespace bp = boost::python;


    template<class MODEL>
    class NonDecomposableLossFunctionBaseWrap :
        public NonDecomposableLossFunctionBase, 
        public bp::wrapper<NonDecomposableLossFunctionBase>
    {
    public:
        typedef MODEL Model;
        typedef typename Model:: template VariableMap<DiscreteLabel> ConfMap;

        virtual ~NonDecomposableLossFunctionBaseWrap(){
        }

        // pure virtual interface
        virtual std::string name() const {
            return this->get_override("name")();
        }

        virtual LossType eval(
            const Model & model, 
            const ConfMap & confGt, 
            const ConfMap & conf
        ) const {
            return this->get_override("eval")(model, confGt, conf);
        }




        // with default implementation
        virtual LossType maximumLoss()const {
            auto possibleOverride = this->get_override("maximumLoss");
            if(possibleOverride)
                return possibleOverride();
            else
                NonDecomposableLossFunctionBase::maximumLoss();

        }

        LossType default_maximumLoss()const{
            return this->NonDecomposableLossFunctionBase::maximumLoss();
        }

    };

} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno
