#define PY_ARRAY_UNIQUE_SYMBOL inferno_inference_PyArray_API
#define NO_IMPORT_ARRAY


// inferno relatex
#include "inferno/inferno_python.hxx"

// vigra numpy array converters
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>

// functions and macros to export inference
#include "exportInference.hxx"

// model type
#include "inferno/model/general_discrete_model.hxx"

// solvers
#include "inferno/inference/icm.hxx"
#include "inferno/inference/mp.hxx"
#ifdef WITH_QPBO
#include "inferno/inference/qpbo.hxx"
#include "inferno/inference/higher_order_qpbo.hxx"
#endif
#include "inferno/inference/hmmwc.hxx"

namespace inferno{
namespace inference{

    //namespace bp = boost::python;

    void exportGeneralDiscreteModelInference(){
        typedef inferno::models::GeneralDiscreteModel Model;
        typedef DiscreteInferenceBase<Model> BaseInf;
        const std::string modelName = "GeneralDiscreteModel";


        // export the base class for discrete inference
        exportDiscreteInferenceBase<Model>(modelName);

        // export concrete solvers (via macro)
        INFERNO_EXPORT_INFERENCE(Model, modelName, Icm<Model>,            "Icm");
        INFERNO_EXPORT_INFERENCE(Model, modelName, MessagePassing<Model>, "MessagePassing");
        INFERNO_EXPORT_INFERENCE(Model, modelName, Hmmwc<Model>, "Hmmwc");

        #ifdef WITH_QPBO
        INFERNO_EXPORT_INFERENCE(Model, modelName, Qpbo<Model>,            "Qpbo");
        INFERNO_EXPORT_INFERENCE(Model, modelName, HigherOrderQpbo<Model>, "HigherOrderQpbo");
        #endif
    }
    

} // end namespace inferno::inference
} // end namespace inferno



