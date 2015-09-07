#ifndef INFERNO_PTYHON_MODEL_PARAMETRIZED_MULTICUT_MODEL_HXX
#define INFERNO_PTYHON_MODEL_PARAMETRIZED_MULTICUT_MODEL_HXX


// std
#include <string>


// vigra
#include <vigra/multi_array.hxx>
#include <vigra/tinyvector.hxx>  
// inferno
#include "inferno/python/model/models.hxx"
#include "inferno/model/parametrized_multicut_model.hxx"

namespace inferno{
    namespace models{

    // forward declaration
    //template<class EDGES, class FEATURES>
    //class ParametrizedMulticutModel;


    namespace detail_python{
        typedef vigra::MultiArray<1, vigra::TinyVector<uint64_t,2> > EdgeArray;
        typedef vigra::MultiArray<2, ValueType>                      FeatureArray;
    }   
    typedef ParametrizedMulticutModel<
        detail_python::EdgeArray, 
        detail_python::FeatureArray
    > PyParametrizedMulticutModel;


    template<class MODEL>
    struct ModelName;

    template<>
    struct ModelName<PyParametrizedMulticutModel>{
        static std::string name(){
            return "ParametrizedMulticutModel";
        }
    };

    

} // end namespace inferno::models
} // end namespace inferno

#endif /*INFERNO_PTYHON_MODEL_PARAMETRIZED_MULTICUT_MODEL_HXX*/
