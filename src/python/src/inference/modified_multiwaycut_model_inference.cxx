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
#include "inferno/model/modified_multiwaycut_model.hxx"

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

    void exportModifiedMultiwaycutModelInference(){
        typedef inferno::models::ModifiedMultiwaycutModel Model;
        typedef DiscreteInferenceBase<Model> BaseInf;
        const std::string modelName = "ModifiedMultiwaycutModel";


        // export the base class for discrete inference
        export_helper::exportDiscreteInferenceBase<Model>(modelName);

        // export concrete solvers (via macro)
        INFERNO_EXPORT_INFERENCE(Model, modelName, Hmmwc<Model>, "Hmmwc");

    }
    

} // end namespace inferno::inference
} // end namespace inferno



