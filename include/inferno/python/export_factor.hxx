#ifndef INFERNO_PYTHON_FACTOR_HXX
#define INFERNO_PYTHON_FACTOR_HXX

#include <boost/python/def_visitor.hpp>
#include <boost/python/suite/indexing/indexing_suite.hpp>
#include <boost/python/copy_non_const_reference.hpp>
#include <boost/python/return_value_policy.hpp>

#include <boost/mpl/if.hpp>

namespace inferno{
namespace python{

namespace bp = boost::python;





template<class FACTOR_CLASS>
class FactorVisitor : 
    public bp::def_visitor<FactorVisitor<FACTOR_CLASS> >
{
    friend class bp::def_visitor_access;

    typedef FACTOR_CLASS FactorType;

    template <class classT>
    void visit(classT& c) const
    {
        c

        ;
    }

 
};



template<class MODEL>
struct ExportFactor_Ptr{

    static void exportFactor(const std::string & clsName){
        typedef MODEL Model;
        typedef typename Model::FactorType FactorType;
        bp::class_<FactorType,FactorType *,boost::noncopyable>(clsName.c_str(),bp::no_init)
            .def(FactorVisitor<FactorType>())
        ;
    }
};

template<class MODEL>
struct ExportFactor_Ref{
    
    static void exportFactor(const std::string & clsName){
        typedef MODEL Model;
        typedef typename Model::FactorType FactorType;
        bp::class_<FactorType,boost::noncopyable>(clsName.c_str(),bp::no_init)
            .def(FactorVisitor<FactorType>())
        ;
    }
};

template<class MODEL>
struct ExportFactor_TmpObj{
    typedef MODEL Model;
    typedef typename Model::FactorType FactorType;

    typedef const FactorType * FacArgType;
    static void exportFactor(const std::string & clsName){
        bp::class_<FactorType,boost::noncopyable>(clsName.c_str(),bp::no_init)
            .def(FactorVisitor<FactorType>())
            
            // properties
            .add_property("size", &FactorType::size)
            .add_property("arity", &FactorType::arity)
            .add_property("nWeights", &FactorType::size)

            // functions
            .def("variable",&FactorType::variable)
            .def("variables",&FactorType::variables)

            // special methods
            .def("__getitem__",&eval2)
            .def("__getitem__",&eval3)
            .def("__getitem__",&eval4)
            .def("__getitem__",&eval5)
        ;
    }

    /// get all variables as list
    static bp::list variables(FacArgType factor){
        bp::list list;
        for(ArityType a=0; a<factor->arity(); ++a){
            list.append(factor->variable(a));
        }
        return list;
    }

    /// evaluate
    static ValueType eval2(
        FacArgType factor,
        const DiscreteLabel l0,
        const DiscreteLabel l1
    ){
        return factor->eval(l0, l1);
    }
    static ValueType eval3(
        FacArgType factor,
        const DiscreteLabel l0,
        const DiscreteLabel l1,
        const DiscreteLabel l2
    ){
        return factor->eval(l0, l1, l2);
    }
    static ValueType eval4(
        FacArgType factor,
        const DiscreteLabel l0,
        const DiscreteLabel l1,
        const DiscreteLabel l2,
        const DiscreteLabel l3
    ){
        return factor->eval(l0, l1, l2, l3);
    }
    static ValueType eval5(
        FacArgType factor,
        const DiscreteLabel l0,
        const DiscreteLabel l1,
        const DiscreteLabel l2,
        const DiscreteLabel l3,
        const DiscreteLabel l4
    ){
        return factor->eval(l0, l1, l2, l3, l4);
    }
};







template<class MODEL>
void exportFactor(const std::string & modelClsName){
    const auto clsName = modelClsName + std::string("Factor");
    

    typedef MODEL Model;
    typedef typename Model::FactorDescriptor FactorDescriptor;
    typedef typename Model::FactorType FactorType;

    typedef typename std::result_of<
        decltype(&Model::factor)(Model, const FactorDescriptor )
    >::type ReturnedFactorType;

    typedef boost::mpl::bool_<std::is_reference<ReturnedFactorType>::value> IsRef;
    typedef boost::mpl::bool_<std::is_pointer<ReturnedFactorType>::value> IsPtr;

    typedef typename boost::mpl::if_<
        IsRef,
        ExportFactor_Ref<MODEL>,
        typename boost::mpl::if_<
            IsPtr,
            ExportFactor_Ptr<MODEL>,
            ExportFactor_TmpObj<MODEL>
        >::type
    >::type Exporter;

    Exporter::exportFactor(clsName);
}




} // end namespace python
} // end namespace inferno


#endif /* INFERNO_PYTHON_FACTOR_HXX */
