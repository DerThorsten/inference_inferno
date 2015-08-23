#ifndef INFERNO_MODELS_TRANSFORMATIONS_CONVERT_BINARY_TO_SECOND_ORDER_BINARY_HXX
#define INFERNO_MODELS_TRANSFORMATIONS_CONVERT_BINARY_TO_SECOND_ORDER_BINARY_HXX


#include "inferno/inference/utilities/fix-fusion/fusion-move.hpp"


namespace inferno{
namespace models{
namespace transformations
    

    template<class MODEL_IN, class MODEL_OUT>
    class ConvertBinaryToSecondOrderBinary{
    public:
        typedef models::DenseVariableIds<Model> DenseVarIds;
        typedef MODEL_IN  ModelIn;
        typedef MODEL_OUT ModelOut;

        struct Options{

        };
        ConvertBinaryToSecondOrderBinary(
            const ModelIn & modelIn,
            ModelOut & modelOut,
            const Options & options
        )
        :   modelIn_(modelIn),
            denseVarIds_(modelIn),
            modelOut_(modelOut),
            options_(options),
            coeffs_(),
            cliqueLabels_(){


            const auto maxArity = model_.maxArity();
            // check for valid models
            {
                INFERNO_CHECK_OP(maxArity,<=,10,
                    "ConvertBinaryToSecondOrderBinary is only implemented for models with maxArity<=10");
                DiscreteLabel nLabels;
                const bool simpleLabelSpace  =  model_.hasSimpleLabelSpace(nLabels);
                INFERNO_CHECK(simpleLabelSpace && nLabels == 2, "ConvertBinaryToSecondOrderBinary is only implemented for models "
                                                                "where all variables  have a binary label space");
            }
            coeffs_.resize(1 << maxArity);
            cliqueLabels_.resize(maxArity);

        }

        void convert(){

            // convert arbitrary energy function (with binary labels, and arity<=10)
            // to a higher order pseudo boolean function
            this->setupHoe();

            // convert higher order pseudo boolean function
            // into a second order pseudo boolean graphical model
            this->toQuadratic();

        }


    private:

        void setupHoe(){
            for (const auto factor : modelIn_.factors()){
                const Vi var = denseVarIds_.toDense(factor->variable(0));
                hoe_.AddUnaryTerm(var, factor->eval(1) - factor->eval(0));
            }
        }

        void toQuadratic(){
            hoe_.toQuadratic(modelOut_);
        }

        const ModelIn & modelIn_;
        DenseVarIds   denseVarIds_;
        ModelOut & modelOut_;
        Options options_;

        HigherOrderEnergy<ValueType, 10> hoe_;
        std::vector<ValueType> coeffs_;
        std::vector<DiscreteLabel> cliqueLabels_;
    };


} // end namespace inferno::models::transformations
} // end namespace inferno::models
} // inferno

#endif /* INFERNO_MODELS_TRANSFORMATIONS_CONVERT_BINARY_TO_SECOND_ORDER_BINARY_HXX /*
