/** \file inferno_python.hxx 
    \brief Main header of inferno python.
    Any project using inferno python should 
    include this header.

*/
#ifndef INFERNO_INFERNO_PYTHON_HXX
#define INFERNO_INFERNO_PYTHON_HXX


// std
#include <string>


// vigra
#include <vigra/multi_array.hxx>
    
// vigra numpy array converters
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>


// boost python related
//#include <boost/python.hpp>
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/def_visitor.hpp>
#include <boost/python/exception_translator.hpp>

#include "inferno/inferno.hxx"
//#include "inferno/model/parametrized_multicut_model.hxx"
#include "inferno/model/general_discrete_model.hxx"

namespace inferno{



    namespace models{

        // forward declaration
        template<class EDGES, class FEATURES>
        class ParametrizedMulticutModel;

        

        namespace detail_python{
            typedef vigra::MultiArray<1, vigra::TinyVector<uint64_t,2> > EdgeArray;
            typedef vigra::MultiArray<2, ValueType>                      FeatureArray;
        }   
        typedef ParametrizedMulticutModel<
            detail_python::EdgeArray, 
            detail_python::FeatureArray
        > PyParametrizedMulticutModel;

        typedef GeneralDiscreteModel  PyGeneralDiscreteModel;

        //template<class MODEL>
        //std::string name()const{
        //}

        template<class MODEL>
        struct ModelName;

        template<>
        struct ModelName<PyGeneralDiscreteModel>{
            static std::string name(){
                return "GeneralDiscreteModel";
            }
        };
        template<>
        struct ModelName<PyParametrizedMulticutModel>{
            static std::string name(){
                return "ParametrizedMulticutModel";
            }
        };

        

        /*
        template<class MODEL>
        class ModelNumpyArrayConverter{
        public:

            template<class T>
            struct NodeMap{
                typedef vigra::NumpyArray<T, 1>                     NumpyArrayType;
                typedef vigra::MultiArray<T, 1>                     MultiArrayType;
                typedef vigra::MultiArrayView<T, 1>                 MultiArrayViewType;
                typedef typename  MODEL:: template VariableMap<T>   NativeType;


                static void fromPython(const MODEL & m &,
                                       MultiArrayViewType view & , 
                                       NativeType & native){
                    for(auto var : m.variables()){
                        native[var] = view[var];
                    }
                };            
            };

        };
        */


    }

    namespace learing{
        namespace dataset{


        }
    }





    namespace bp = boost::python;
    typedef bp::manage_new_object NewObj;

    template<class T> using RetValPol =
        bp::return_value_policy<T>; 
    typedef RetValPol<NewObj> RetValPolNewObj;

    template<unsigned int A, unsigned int B,class T> using CustWardPost =
        bp::with_custodian_and_ward_postcall<A,B, T>; 

    template<unsigned int A, unsigned int B,class T> using CustWard =
        bp::with_custodian_and_ward<A,B, T>; 

    template<unsigned int A, unsigned int B> using CustWardPostEnd =
    bp::with_custodian_and_ward_postcall<A,B>; 

    template<unsigned int A, unsigned int B> using CustWardEnd =
    bp::with_custodian_and_ward<A,B>; 

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
