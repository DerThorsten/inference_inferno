#ifndef INFERNO_PYTHON_EXPORT_NON_COPYABLE_VECTOR_HXX
#define INFERNO_PYTHON_EXPORT_NON_COPYABLE_VECTOR_HXX

#include <boost/python/def_visitor.hpp>
#include <boost/python/suite/indexing/indexing_suite.hpp>
#include <boost/python/copy_non_const_reference.hpp>
#include <boost/python/return_value_policy.hpp>

namespace inferno{
namespace python{

namespace bp = boost::python;

template<class VECTOR_CLASS>
class NonCopyableVectorVisitor : 
    public bp::def_visitor<NonCopyableVectorVisitor<VECTOR_CLASS> >
{
    friend class bp::def_visitor_access;

    typedef VECTOR_CLASS Vector;
    typedef typename Vector::value_type value_type;
    typedef typename Vector::reference reference;
    typedef typename Vector::const_reference const_reference;

    template <class classT>
    void visit(classT& c) const
    {
        c
            .def("__getitem__", &getItem, bp::return_internal_reference<>())
            .def("__len__", &Vector::size)
            .def("__iter__", bp::iterator<Vector,  bp::return_internal_reference<> >())
        ;
    }

    static reference getItem(Vector & vec, int64_t key){
        if(key<0)
            key += vec.size();
        return vec[key];
    }  
};





} // end namespace python
} // end namespace inferno


#endif /* INFERNO_PYTHON_EXPORT_NON_COPYABLE_VECTOR_HXX */