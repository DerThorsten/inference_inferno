#ifndef INFERNO_PYTHON_LEARNING_LOSS_FUNCTIONS_EXPORT_PARTITION_HAMMING_HXX
#define INFERNO_PYTHON_LEARNING_LOSS_FUNCTIONS_EXPORT_PARTITION_HAMMING_HXX

// boost
#include <boost/python/def_visitor.hpp>
#include <boost/python/suite/indexing/indexing_suite.hpp>
#include <boost/python/copy_non_const_reference.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/mpl/if.hpp>
#include <boost/python/pure_virtual.hpp>

// inferno 
#include "inferno/python/learning/loss_functions/export_loss_functions.hxx"
#include "inferno/learning/loss_functions/partition_hamming.hxx"


namespace inferno{
namespace learning{
namespace loss_functions{


    namespace bp = boost::python;


    




    template<class MODEL>
    struct ExportPartitionHamming{
        typedef MODEL Model;
        typedef PartitionHamming<Model> LossFunction;
        typedef typename  LossFunction::LossAugmentedModel LossAugmentedModel;

        typedef DecomposableLossFunctionBase<Model, LossAugmentedModel> Base;
        typedef DecomposableLossFunctionBaseWrap<Model, LossAugmentedModel> BaseWrap;
        
        typedef std::auto_ptr<Base> BasePtrType;
        typedef std::auto_ptr<LossFunction> PtrType;
        typedef typename LossFunction::FactorWeightMap FactorWeightMap;

        static void exportLossFunction(const std::string modelClsName){

            const auto baseClsName = modelClsName + 
               std::string("PartitionHamming");


            // the class
            bp::class_<LossFunction, 
                bp::bases<Base>, 
                boost::noncopyable
            >( baseClsName.c_str(),bp::no_init)
            ;

            // the shared ptr
            //bp::register_ptr_to_python< std::unique_ptr<Base> >();

            // the factories
            bp::def("partitionHamming",&factory,
                (
                    bp::arg("model"),
                    bp::arg("factorWeightMap"),
                    bp::arg("rescale") = 1.0,
                    bp::arg("underseg") = 1.0,
                    bp::arg("overseg") = 1.0,
                    bp::arg("useIgnoreLabel") = false,
                    bp::arg("ignoreLabel") = -1
                )
            );
            bp::def("partitionHamming",&factory2,
                (
                    bp::arg("model"),
                    bp::arg("rescale") = 1.0,
                    bp::arg("underseg") = 1.0,
                    bp::arg("overseg") = 1.0,
                    bp::arg("useIgnoreLabel") = false,
                    bp::arg("ignoreLabel") = -1
                )
            );

        }


        static BasePtrType factory(
            const Model & model,
            const FactorWeightMap & factorWeightMap,
            const double rescale,
            const double underseg,
            const double overseg,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel
        ){
            Base * ptr = new LossFunction(model, rescale, underseg, 
                                        overseg, useIgnoreLabel, 
                                        ignoreLabel, factorWeightMap);
            return BasePtrType(ptr);
        }
        static BasePtrType factory2(
            const Model & model,
            const double rescale,
            const double underseg,
            const double overseg,
            const bool useIgnoreLabel,
            const DiscreteLabel ignoreLabel     
        ){
            Base * ptr = new LossFunction(model, rescale, underseg, 
                                        overseg, useIgnoreLabel, 
                                        ignoreLabel);
            return BasePtrType(ptr);
        }
    };



} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_PYTHON_LEARNING_LOSS_FUNCTIONS_EXPORT_PARTITION_HAMMING_HXX */
