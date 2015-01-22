#define PY_ARRAY_UNIQUE_SYMBOL inferno_core_PyArray_API
#define NO_IMPORT_ARRAY



// helper classes
#include "model_helper.hxx"



namespace inferno{

    namespace bp = boost::python;


    std::shared_ptr<ExplicitDiscreteModel> explicitDiscreteModelFactorySimple(
        const uint64_t nVar,
        const int64_t minLabel,
        const int64_t maxLabel
    ){
        return std::shared_ptr<ExplicitDiscreteModel>
            (new ExplicitDiscreteModel(nVar, DiscreteLabelBounds(minLabel, maxLabel)));
    }

    void exportModel(){


        FactorSharedPtr  (Model::*getFactor)(const int64_t ) const = &Model::getFactor;

        MixedFactorSharedPtr     (Model::*getMixedFactor)    (const int64_t , const MixedTag)     const = &Model::getFactor;
        DiscreteFactorSharedPtr  (Model::*getDiscreteFactor) (const int64_t , const DiscreteTag)  const = &Model::getFactor;
        ContinousFactorSharedPtr (Model::*getContinousFactor)(const int64_t , const ContinousTag) const = &Model::getFactor;


        void (Model::*varBoundsMixed    ) (const int64_t , MixedLabelBounds & )     const = &Model::varBounds;
        void (Model::*varBoundsDiscrete ) (const int64_t , DiscreteLabelBounds & )  const = &Model::varBounds;
        void (Model::*varBoundsContinous) (const int64_t , ContinousLabelBounds & ) const = &Model::varBounds;


        bp::class_<ModelWrap, boost::noncopyable>("Model")
             // get the factors
            .def("getFactor", bp::pure_virtual(getFactor         ))
            .def("getFactor", bp::pure_virtual(getMixedFactor    ))
            .def("getFactor", bp::pure_virtual(getDiscreteFactor ))
            .def("getFactor", bp::pure_virtual(getContinousFactor))
            // get var and fac info
            .def("variablesInfo", bp::pure_virtual(&ModelWrap::variablesInfo))
            .def("factorsInfo", bp::pure_virtual(&ModelWrap::variablesInfo))

            // get variable bounds
            .def("varBounds", bp::pure_virtual(varBoundsMixed     ))
            .def("varBounds", bp::pure_virtual(varBoundsDiscrete  ))
            .def("varBounds", bp::pure_virtual(varBoundsContinous ))
            // def visitor
            .def(ModelDefVisitor())
        ;



        bp::class_<DiscreteModelWrap, bp::bases<Model>, boost::noncopyable>("DiscreteModel")
        ;


        bp::class_<ExplicitDiscreteModel,bp::bases<DiscreteModel>, boost::noncopyable>("ExplicitDiscreteModel",bp::no_init)
            .def("addFactor",& ExplicitDiscreteModel::addFactor)
        ;

        // factory
        bp::def("explicitDiscreteModel",&explicitDiscreteModelFactorySimple)
        ;

        //bp::class_<DiscreteModelWrap,bp::bases<Model>, boost::noncopyable>("DiscreteModel")
        //    //.def("eval", bp::pure_virtual(eval_ml_ptr))
        //;

        //bp::class_<TwoClassUnary,bp::bases<DiscreteFactor>, boost::noncopyable>
        //    ("TwoClassUnary",bp::no_init)
        //    //.def(ModelDefVisitor())
        //;
        //// factory
        //bp::def("twoClassUnary",&twoClassUnaryFactory)
        //;


        // SHARED PTRS TO ABSTRACT BASE CLASES
        bp::register_ptr_to_python<  std::shared_ptr<Model> >();
        bp::register_ptr_to_python<  std::shared_ptr<DiscreteModel> >();
        bp::register_ptr_to_python<  std::shared_ptr<ContinousModel> >();
        bp::register_ptr_to_python<  std::shared_ptr<MixedModel> >();

        // SHARED PTRS TO CONCRETE CLASSES
        bp::register_ptr_to_python<  std::shared_ptr<ExplicitDiscreteModel> >();

    }
}



