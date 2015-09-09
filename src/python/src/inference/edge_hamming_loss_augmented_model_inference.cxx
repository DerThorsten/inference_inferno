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
#include "inferno/learning/loss_functions/edge_hamming.hxx"
#include "inferno/python/model/general_discrete_model.hxx"
#include "inferno/python/model/parametrized_multicut_model.hxx"

// solvers
//#include "inferno/inference/icm.hxx"
//#include "inferno/inference/mp.hxx"

#ifdef WITH_QPBO
#include "factories/qpbo.hxx"
#endif
#include "factories/multicut.hxx"
//#include "inferno/inference/hmmwc.hxx"

namespace inferno{
namespace inference{

    //namespace bp = boost::python;

    template<class BASE_MODEL>
    void exportEdgeHammingLossAugmentedModelInferenceT(const std::string baseModelName){

        typedef BASE_MODEL BaseModel;
        typedef learning::loss_functions::EdgeHamming<BaseModel> EdgeHammingLossFunction;
        typedef typename EdgeHammingLossFunction::LossAugmentedModel Model;

        typedef DiscreteInferenceBase<Model> BaseInf;
        const std::string modelName = "EdgeHammingLossAugmented" + baseModelName;


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
        INFERNO_EXPORT_INFERENCE(Model, modelName, Multicut,            "Multicut");
    }
    

    void exportEdgeHammingLossAugmentedModelInference(){
        exportEdgeHammingLossAugmentedModelInferenceT<models::GeneralDiscreteModel>("GeneralDiscreteModel");    
        exportEdgeHammingLossAugmentedModelInferenceT<models::PyParametrizedMulticutModel>("ParametrizedMulticutModel");
    }

} // end namespace inferno::inference
} // end namespace inferno



