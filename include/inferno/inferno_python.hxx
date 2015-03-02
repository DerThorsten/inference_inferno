/** \file inferno_python.hxx 
    \brief Main header of inferno python.
    Any project using inferno python should 
    include this header.

*/
#ifndef INFERNO_INFERNO_PYTHON_HXX
#define INFERNO_INFERNO_PYTHON_HXX



// boost python related
//#include <boost/python.hpp>
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/def_visitor.hpp>
#include <boost/python/exception_translator.hpp>


namespace inferno{

    namespace bp = boost::python;
    typedef bp::manage_new_object NewObj;

    template<class T> using RetValPol =
        bp::return_value_policy<T>; 
    typedef RetValPol<NewObj> RetValPolNewObj;
    template<unsigned int A, unsigned int B,class T> using CustWardPost =
        bp::with_custodian_and_ward_postcall<A,B, T>; 

    class ScopedGILRelease
    {
    // C & D -------------------------------------------------------------------------------------------
    public:
        inline ScopedGILRelease()
        {
            m_thread_state = PyEval_SaveThread();
        }

        inline ~ScopedGILRelease()
        {
            PyEval_RestoreThread(m_thread_state);
            m_thread_state = NULL;
        }

    private:
        PyThreadState * m_thread_state;
    };
}

#endif
