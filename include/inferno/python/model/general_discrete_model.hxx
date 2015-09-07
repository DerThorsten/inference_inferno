#ifndef INFERNO_PTYHON_MODEL_GENERAL_DISCRETE_MODEL_HXX
#define INFERNO_PTYHON_MODEL_GENERAL_DISCRETE_MODEL_HXX


// std
#include <string>


// vigra
#include <vigra/multi_array.hxx>
    
// inferno
#include "inferno/python/model/models.hxx"
#include "inferno/model/general_discrete_model.hxx"

namespace inferno{
    namespace models{



    typedef GeneralDiscreteModel  PyGeneralDiscreteModel;

    template<class MODEL>
    struct ModelName;

    template<>
    struct ModelName<PyGeneralDiscreteModel>{
        static std::string name(){
            return "GeneralDiscreteModel";
        }
    };
    

} // end namespace inferno::models
} // end namespace inferno

#endif /*INFERNO_PTYHON_MODEL_GENERAL_DISCRETE_MODEL_HXX*/
