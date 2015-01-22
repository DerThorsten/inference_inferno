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
#include "inferno/factor.hxx"
#include "inferno/small_vector.hxx"



namespace inferno{


   namespace bp = boost::python;


    class FactorDefVisitor : public bp::def_visitor<FactorDefVisitor>
    {
        friend class bp::def_visitor_access;

        template <class classT>
        void visit(classT& c) const
        {
            c
                .def("__getitem__",&evalFromSmallVec< SmallVector<DiscreteLabel> >)
            ;
        }


        template<class VEC>
        static FunctionValueType evalFromSmallVec(Factor * factor, const VEC & vec){
            return factor->eval(vec.data());
        }
    
    };

    struct FactorWrap : Factor, bp::wrapper<Factor>
    {

        void bounds(size_t d, MixedLabelBounds & bounds) const{
            this->get_override("bounds")(bounds);
        }
        void bounds(size_t d, DiscreteLabelBounds & bounds) const{
            this->get_override("bounds")(bounds);
        }
        void bounds(size_t d, ContinousLabelBounds & bounds) const{
            this->get_override("bounds")(bounds);
        }
        FunctionValueType   eval(const MixedLabel * conf) const{
            return this->get_override("eval")(conf);
        }
        FunctionValueType   eval(const DiscreteLabel * conf) const{
            return this->get_override("eval")(conf);
        }
        FunctionValueType   eval(const ContinousLabel * conf) const{
            return this->get_override("eval")(conf);
        }

        size_t arity() const{
            return this->get_override("arity")();
        }
        int64_t vi(const size_t d)const{
            return this->get_override("vi")(d);
        }
    };

    struct DiscreteFactorWrap : DiscreteFactor, bp::wrapper<DiscreteFactor>
    {
        int f()
        {
            return this->get_override("f")();
        }


        void bounds(size_t d, MixedLabelBounds & bounds) const{
            this->get_override("bounds")(bounds);
        }
        void bounds(size_t d, DiscreteLabelBounds & bounds) const{
            this->get_override("bounds")(bounds);
        }
        void bounds(size_t d, ContinousLabelBounds & bounds) const{
            this->get_override("bounds")(bounds);
        }
        FunctionValueType   eval(const MixedLabel * conf) const{
            return this->get_override("eval")(conf);
        }
        FunctionValueType   eval(const DiscreteLabel * conf) const{
            return this->get_override("eval")(conf);
        }
        FunctionValueType   eval(const ContinousLabel * conf) const{
            return this->get_override("eval")(conf);
        }

        size_t arity() const{
            return this->get_override("arity")();
        }
        int64_t vi(const size_t d)const{
            return this->get_override("vi")(d);
        }
    };




    std::shared_ptr<Factor>  twoClassUnaryFactory(const int64_t vi , const FunctionValueType v0, const FunctionValueType v1){
        return std::shared_ptr<Factor>(new TwoClassUnary(vi, v0, v1));
    }
    void exportFactor(){

        //FunctionValueType    (Factor::*eval_ml_ptr)(const MixedLabel * conf) const = &Factor::eval;


        bp::class_<FactorWrap, boost::noncopyable>("Factor")
            .def("arity", bp::pure_virtual(&Factor::arity))
            .def("vi", bp::pure_virtual(&Factor::vi))
            .def(FactorDefVisitor())
        ;





        bp::class_<DiscreteFactorWrap,bp::bases<Factor>, boost::noncopyable>("DiscreteFactor")
            //.def("eval", bp::pure_virtual(eval_ml_ptr))
        ;

        bp::class_<TwoClassUnary,bp::bases<DiscreteFactor>, boost::noncopyable>
            ("TwoClassUnary",bp::no_init)
            //.def(FactorDefVisitor())
        ;

        // factory
        bp::def("twoClassUnary",&twoClassUnaryFactory)
        ;


        // SHARED PTRS TO ABSTRACT BASE CLASES
        bp::register_ptr_to_python<  std::shared_ptr<Factor> >();
        bp::register_ptr_to_python<  std::shared_ptr<DiscreteFactor> >();
        bp::register_ptr_to_python<  std::shared_ptr<ContinousFactor> >();
        bp::register_ptr_to_python<  std::shared_ptr<MixedFactor> >();

        // SHARED PTRS TO CONCRETE CLASSES
        bp::register_ptr_to_python<  std::shared_ptr<TwoClassUnary> >();
    }
}



