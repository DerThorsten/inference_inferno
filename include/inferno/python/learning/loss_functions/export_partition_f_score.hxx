#ifndef INFERNO_PYTHON_LEARNING_LOSS_FUNCTIONS_EXPORT_PARTITION_F_SCORE
#define INFERNO_PYTHON_LEARNING_LOSS_FUNCTIONS_EXPORT_PARTITION_F_SCORE

// boost
#include <boost/python/def_visitor.hpp>
#include <boost/python/suite/indexing/indexing_suite.hpp>
#include <boost/python/copy_non_const_reference.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/mpl/if.hpp>
#include <boost/python/pure_virtual.hpp>

// inferno 
#include "inferno/python/learning/loss_functions/export_loss_functions.hxx"
#include "inferno/learning/loss_functions/partition_f_score.hxx"


namespace inferno{
namespace learning{
namespace loss_functions{


    namespace bp = boost::python;


    




    template<class MODEL>
    struct ExportPartitonFScore{
        typedef MODEL Model;
        typedef NonDecomposableLossFunctionBase<Model> Base;
        typedef NonDecomposableLossFunctionBaseWrap<Model> BaseWrap;
        typedef PartitionFScore<Model> LossFunction;
        typedef std::shared_ptr<LossFunction> SharedPtr;
        typedef typename LossFunction::FactorWeightMap FactorWeightMap;

        static void exportLossFunction(const std::string modelClsName){

            const auto baseClsName = modelClsName + 
               std::string("PartitionFScore");


            // the class
            bp::class_<LossFunction, bp::bases<Base> >( baseClsName.c_str() )
            ;

            // the shared ptr
            bp::register_ptr_to_python< std::shared_ptr<Base> >();

            // the factories
            bp::def("partitionFScore",&factory,
                (
                    bp::arg("model"),
                    bp::arg("factorWeightMap"),
                    bp::arg("beta") = 0.5,
                    bp::arg("useIgnoreLabel") = false,
                    bp::arg("ignoreLabel") = -1
                )
            );
            bp::def("partitionFScore",&factoryWithoutFactorWeightMap,
                (
                    bp::arg("model"),
                    bp::arg("beta") = 0.5,
                    bp::arg("useIgnoreLabel") = false,
                    bp::arg("ignoreLabel") = -1
                )
            );
        }


        static SharedPtr factory(
            const Model & model,
            const FactorWeightMap & factorWeightMap,
            const double beta,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel
        ){
            return std::make_shared<LossFunction>(
                model, beta, useIgnoreLabel, ignoreLabel, factorWeightMap
            );
        }

        static SharedPtr factoryWithoutFactorWeightMap(
            const Model & model,
            const double beta,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel
        ){
            return std::make_shared<LossFunction>(
                model, beta, useIgnoreLabel, ignoreLabel
            );
        }
    };



} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_PYTHON_LEARNING_LOSS_FUNCTIONS_EXPORT_PARTITION_F_SCORE */
