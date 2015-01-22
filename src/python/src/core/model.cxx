#define PY_ARRAY_UNIQUE_SYMBOL inferno_core_PyArray_API
#define NO_IMPORT_ARRAY



// helper classes
#include "model_helper.hxx"



namespace inferno{

    namespace bp = boost::python;

    void exportModel(){


        FactorSharedPtr  (Model::*getFactor)(const int64_t ) const = &Model::getFactor;

        MixedFactorSharedPtr     (Model::*getMixedFactor)    (const int64_t , const MixedTag) const = &Model::getFactor;
        DiscreteFactorSharedPtr  (Model::*getDiscreteFactor) (const int64_t , const DiscreteTag) const = &Model::getFactor;
        ContinousFactorSharedPtr (Model::*getContinousFactor)(const int64_t , const ContinousTag) const = &Model::getFactor;

        bp::class_<ModelWrap, boost::noncopyable>("Model")
            .def("getFactor", bp::pure_virtual(getFactor))
            .def("getFactor", bp::pure_virtual(getMixedFactor))
            .def("getFactor", bp::pure_virtual(getDiscreteFactor))
            .def("getFactor", bp::pure_virtual(getContinousFactor))
            .def(ModelDefVisitor())
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
        //bp::register_ptr_to_python<  std::shared_ptr<Factor> >();
        //bp::register_ptr_to_python<  std::shared_ptr<DiscreteFactor> >();
        //bp::register_ptr_to_python<  std::shared_ptr<ContinousFactor> >();
        //bp::register_ptr_to_python<  std::shared_ptr<MixedFactor> >();
        //// SHARED PTRS TO CONCRETE CLASSES
        //bp::register_ptr_to_python<  std::shared_ptr<TwoClassUnary> >();
    }
}



