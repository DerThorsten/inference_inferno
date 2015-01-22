#define PY_ARRAY_UNIQUE_SYMBOL inferno_core_PyArray_API
#define NO_IMPORT_ARRAY


// boost python related
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/def_visitor.hpp>

// vigra numpy array converters
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>
#include <boost/python/exception_translator.hpp>

// standart c++ headers (whatever you need (string and vector are just examples))
#include <string>
#include <vector>

// inferno relatex
#include "inferno/inferno.hxx"
#include "inferno/model.hxx"




namespace inferno{


   namespace bp = boost::python;


    class ModelDefVisitor : public bp::def_visitor<ModelDefVisitor>
    {
        friend class bp::def_visitor_access;
        template <class classT>
        void visit(classT& c) const
        {
            c
                //.def("__getitem__",&evalFromSmallVec< SmallVector<DiscreteLabel> >)
            ;
        } //
    };

    struct ModelWrap : Model, bp::wrapper<Model>
    {
        FactorSharedPtr getFactor(const int64_t fi) const{
            return this->get_override("getFactor")(fi);
        }//

        VariablesInfo variablesInfo()const{
            return this->get_override("variablesInfo")();
        }

        FactorsInfo factorsInfo()const{
            return this->get_override("factorsInfo")();
        }

        bool isVariableId(const int64_t id) const{
            return this->get_override("isVariableId")(id);
        }

        bool isFactorId(const int64_t id) const{
            return this->get_override("isFactorId")(id);
            VariablesInfo info;
            info.maxVariableId();
        }


        DiscreteFactorSharedPtr  getFactor(const int64_t fi, const DiscreteTag tag) const{
            if (bp::override f = this->get_override("getFactor"))
                return f(); // *note*
            return Model::getFactor(fi, tag);
        }
        ContinousFactorSharedPtr getFactor(const int64_t fi, const ContinousTag tag) const{
            if (bp::override f = this->get_override("getFactor"))
                return f(); // *note*
            return Model::getFactor(fi, tag);
        }
        MixedFactorSharedPtr     getFactor(const int64_t fi, const MixedTag tag) const{
            if (bp::override f = this->get_override("getFactor"))
                return f(); // *note*
            return Model::getFactor(fi, tag);
        }


        // default impls
        DiscreteFactorSharedPtr  defaultGetFactor(const size_t fi, const DiscreteTag tag) const{
       
            return this->Model::getFactor(fi, tag);
        }
        ContinousFactorSharedPtr defaultGetFactor(const size_t fi, const ContinousTag tag) const{
             return this-> Model::getFactor(fi, tag);
        }
        MixedFactorSharedPtr     defaultGetFactor(const size_t fi, const MixedTag tag) const{
             return this-> Model::getFactor(fi, tag);
        }
    };

    struct DiscreteModelWrap : DiscreteModel, bp::wrapper<DiscreteModel>
    {
        
    };




  
    void exportModel(){


        FactorSharedPtr  (Model::*getFactor)(const int64_t ) const = &Model::getFactor;

        bp::class_<ModelWrap, boost::noncopyable>("Model")
            .def("getFactor", bp::pure_virtual(getFactor))
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



