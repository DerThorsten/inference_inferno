#ifndef INFERNO_PYTHON_LEARNING_LOSS_FUNCTIONS_EXPORT_VARIATION_OF_INFORMATION_HXX
#define INFERNO_PYTHON_LEARNING_LOSS_FUNCTIONS_EXPORT_VARIATION_OF_INFORMATION_HXX

// boost
#include <boost/python/def_visitor.hpp>
#include <boost/python/suite/indexing/indexing_suite.hpp>
#include <boost/python/copy_non_const_reference.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/mpl/if.hpp>
#include <boost/python/pure_virtual.hpp>

// inferno 
#include "inferno/python/learning/loss_functions/export_loss_functions.hxx"
#include "inferno/learning/loss_functions/variation_of_information.hxx"


namespace inferno{
namespace learning{
namespace loss_functions{


    namespace bp = boost::python;


    




    template<class MODEL>
    struct ExportVariationOfInformation2{
        typedef MODEL Model;
        typedef NonDecomposableLossFunctionBase<Model> Base;
        typedef NonDecomposableLossFunctionBaseWrap<Model> BaseWrap;
        typedef VariationOfInformation2<Model> LossFunction;
        typedef std::auto_ptr<Base> BasePtrType;
        typedef std::auto_ptr<LossFunction> PtrType;
        typedef typename LossFunction::VariableSizeMap VariableSizeMap;

        static void exportLossFunction(const std::string modelClsName){

            const auto baseClsName = modelClsName + 
               std::string("VariationOfInformation2");


            // the class
            bp::class_<LossFunction, 
                bp::bases<Base> //, 
               // PtrType
            >( baseClsName.c_str() )
            ;

            // the shared ptr
            //bp::register_ptr_to_python< std::unique_ptr<Base> >();

            // the factories
            bp::def("variationOfInformation2",&factory,
                (
                    bp::arg("model"),
                    bp::arg("variableSizeMap"),
                    bp::arg("beta") = 0.5,
                    bp::arg("useIgnoreLabel") = false,
                    bp::arg("ignoreLabel") = -1
                )
            );
            bp::def("variationOfInformation2",&factoryWithoutVariableSizeMap,
                (
                    bp::arg("model"),
                    bp::arg("beta") = 0.5,
                    bp::arg("useIgnoreLabel") = false,
                    bp::arg("ignoreLabel") = -1
                )
            );
        }


        static BasePtrType factory(
            const Model & model,
            const VariableSizeMap & variableSizeMap,
            const double beta,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel
        ){
            Base * basePtr =  new LossFunction(model, useIgnoreLabel, ignoreLabel, variableSizeMap);
            return BasePtrType(basePtr);
        }

        static BasePtrType factoryWithoutVariableSizeMap(
            const Model & model,
            const double beta,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel
        ){
            Base * basePtr =  new LossFunction(model, useIgnoreLabel, ignoreLabel);
            return BasePtrType(basePtr);
        }
    };



} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_PYTHON_LEARNING_LOSS_FUNCTIONS_EXPORT_VARIATION_OF_INFORMATION_HXX */
