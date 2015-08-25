
namespace inferno{
namespace python{


template<class VECTOR_CLASS>
class NonCopyableVectorVisitor : 
    boost::python::def_visitor<my_def_visitor>
{
    friend class def_visitor_access;

    template <class classT>
    void visit(classT& c) const
    {
        c
            .def("foo", &my_def_visitor::foo)
            .def("bar", &my_def_visitor::bar)
        ;
    }

    static void foo(X& self);
    static void bar(X& self);
};

} // end namespace python
} // end namespace inferno
