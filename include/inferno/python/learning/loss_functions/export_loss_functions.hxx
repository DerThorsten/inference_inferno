#ifndef INFERNO_PYTHON_LEARNING_LOSS_FUNCTIONS_EXPORT_LOSS_FUNCTIONS
#define INFERNO_PYTHON_LEARNING_LOSS_FUNCTIONS_EXPORT_LOSS_FUNCTIONS

// boost
#include <boost/python/def_visitor.hpp>
#include <boost/python/suite/indexing/indexing_suite.hpp>
#include <boost/python/copy_non_const_reference.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/mpl/if.hpp>
#include <boost/python/pure_virtual.hpp>

// inferno 
#include "inferno/learning/loss_functions/loss_function_base.hxx"


namespace inferno{
namespace learning{
namespace loss_functions{


    namespace bp = boost::python;


    template<class MODEL, class LOSS_AUGMENTED_MODEL>
    class DecomposableLossFunctionBaseWrap :
        public DecomposableLossFunctionBase<MODEL, LOSS_AUGMENTED_MODEL>, 
        public bp::wrapper<DecomposableLossFunctionBase<MODEL, LOSS_AUGMENTED_MODEL> >
    {
    public:

        typedef MODEL Model;
        typedef LOSS_AUGMENTED_MODEL LossAugmentedModel;
        typedef DecomposableLossFunctionBase<Model, LossAugmentedModel> Base;
        typedef typename Model:: template VariableMap<DiscreteLabel> ConfMap;

        virtual ~DecomposableLossFunctionBaseWrap(){
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

        virtual void makeLossAugmentedModel(
            const Model & model, 
            const ConfMap & confGt, 
            LossAugmentedModel & lossAugmentedModel
        ) const {
            return this->get_override("makeLossAugmentedModel")(
                model, confGt, lossAugmentedModel
            );
        }


        // with default implementation
        virtual LossType maximumLoss()const {
            auto possibleOverride = this->get_override("maximumLoss");
            if(possibleOverride)
                return possibleOverride();
            else
                Base::maximumLoss();

        }

        LossType defaultMaximumLoss()const{
            return this->Base::maximumLoss();
        }

    };


    template<class MODEL>
    class NonDecomposableLossFunctionBaseWrap :
        public NonDecomposableLossFunctionBase<MODEL>, 
        public bp::wrapper<NonDecomposableLossFunctionBase<MODEL> >
    {
    public:
        typedef MODEL Model;
        typedef NonDecomposableLossFunctionBase<Model> Base;
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
                Base::maximumLoss();

        }

        LossType defaultMaximumLoss()const{
            return this->Base::maximumLoss();
        }

    };



    template<class MODEL, class LOSS_AUGMENTED_MODEL>
    void exportDecomposableLossFunctionBase(
        const std::string modelClsName
    ){
        typedef MODEL Model;
        typedef LOSS_AUGMENTED_MODEL LossAugmentedModel;
        typedef DecomposableLossFunctionBase<Model, LossAugmentedModel> Base;
        typedef DecomposableLossFunctionBaseWrap<Model, LossAugmentedModel> BaseWrap;

        const auto baseClsName = modelClsName + 
            std::string("DecomposableLossFunctionBase");

        // the class
        bp::class_<BaseWrap, boost::noncopyable>(baseClsName.c_str())
            .def("eval", 
                bp::pure_virtual(&Base::eval)
            )
            .def("name", 
                bp::pure_virtual(&Base::name)
            )
            .def("makeLossAugmentedModel", 
                bp::pure_virtual(&Base::makeLossAugmentedModel)
            )
            .def("maximumLoss", 
                &Base::maximumLoss, 
                &BaseWrap::defaultMaximumLoss
            )
        ;

        // the shared ptr
        bp::register_ptr_to_python< std::shared_ptr<Base> >();
    }

    template<class MODEL>
    void exportNonDecomposableLossFunctionBase(
        const std::string modelClsName
    ){
        typedef MODEL Model;
        typedef NonDecomposableLossFunctionBase<Model> Base;
        typedef NonDecomposableLossFunctionBaseWrap<Model> BaseWrap;

        const auto baseClsName = modelClsName + 
            std::string("NonDecomposableLossFunctionBase");

        bp::class_<BaseWrap, boost::noncopyable>(baseClsName.c_str())
            .def("eval",
                bp::pure_virtual(&Base::eval)
            )
            .def("name",
                bp::pure_virtual(&Base::name)
            )
            .def("maximumLoss", 
                &Base::maximumLoss,
                &BaseWrap::defaultMaximumLoss
            )
        ;
    }




} // end namespace inferno::learning::loss_functions
} // end namespace inferno::learning
} // end namespace inferno


#endif /* INFERNO_PYTHON_LEARNING_LOSS_FUNCTIONS_EXPORT_LOSS_FUNCTIONS */
