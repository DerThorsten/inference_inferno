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
#include "inferno/python/model/parametrized_multicut_model.hxx"

// solvers
//#include "inferno/inference/icm.hxx"
//#include "inferno/inference/mp.hxx"

#ifdef WITH_QPBO
#include "factories/qpbo.hxx"
#endif
#include "factories/multicut.hxx"
#include "factories/ehc.hxx"

//#include "inferno/inference/hmmwc.hxx"

namespace inferno{
namespace inference{

    //namespace bp = boost::python;

    void exportParametrizedMulticutModelInference(){

        typedef inferno::models::PyParametrizedMulticutModel Model;

        typedef DiscreteInferenceBase<Model> BaseInf;
        const std::string modelName = "ParametrizedMulticutModel";


        // export the base class for discrete inference
        exportDiscreteInferenceBase<Model>(modelName);

        // export concrete solvers (via macro)
        //INFERNO_EXPORT_INFERENCE(Model, modelName, Icm<Model>,            "Icm");
        //INFERNO_EXPORT_INFERENCE(Model, modelName, MessagePassing<Model>, "MessagePassing");
        //INFERNO_EXPORT_INFERENCE(Model, modelName, Hmmwc<Model>, "Hmmwc");

        //#ifdef WITH_QPBO
        //INFERNO_EXPORT_INFERENCE(Model, modelName, Qpbo<Model>,            "Qpbo");
        ////INFERNO_EXPORT_INFERENCE(Model, modelName, HigherOrderQpbo<Model>, "HigherOrderQpbo");
        //#endif

        typedef OpengmInference<
            Model, opengm::Multicut<ogm::GraphicalModel,opengm::Minimizer>
        > Multicut;
        INFERNO_EXPORT_INFERENCE(Model, modelName, Multicut, "Multicut");

        typedef Ehc<Model> EhcType;
        INFERNO_EXPORT_INFERENCE(Model, modelName, EhcType,  "Ehc");
    }
    

} // end namespace inferno::inference
} // end namespace inferno



