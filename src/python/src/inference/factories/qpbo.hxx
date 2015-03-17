#ifndef INFERNO_PYTHON_QPBO_FACTORY
#define INFERNO_PYTHON_QPBO_FACTORY

#include "../exportInference.hxx"

#ifdef WITH_QPBO
#include "inferno/inference/qpbo.hxx"
#endif

namespace inferno{
namespace inference{

    template<class INF>
    struct ExportInferenceFactory;

    template< class MODEL>
    struct ExportInferenceFactory<Qpbo<MODEL> >{

        typedef Qpbo<MODEL> Inference;
        typedef typename Inference::Options Options;
        typedef typename Inference::Model Model;
        typedef BaseDiscreteInferenceFactory<Model> BaseInfFactory;
        typedef DiscreteInferenceFactory<Inference> InfFactory;

        static void op(const std::string  & modelName, const std::string & infName){

            namespace bp = boost::python;
            const std::string clsName =  infName + std::string("Factory") + modelName;

            bp::class_<InfFactory, bp::bases<BaseInfFactory> >(clsName.c_str(),bp::no_init);
            const Options defaultOptions;
            bp::def(lowerFirst(clsName).c_str(), &pyFactoryFactory,
                (
                    bp::arg("useProbing") = defaultOptions.useProbing_,
                    bp::arg("useImproving") = defaultOptions.useImproving_,
                    bp::arg("strongPersistency") = defaultOptions.strongPersistency_,
                    bp::arg("saveMem") = defaultOptions.saveMem_,
                    bp::arg("nThreads") = defaultOptions.nThreads_
                )
            );
        }

        static std::shared_ptr<BaseInfFactory>  pyFactoryFactory(
            const bool useProbing,
            const bool useImproving,
            const bool strongPersistency,
            const bool saveMem,
            const uint64_t nThreads
        ){      
            const Options opt(useProbing, useImproving, strongPersistency, saveMem, nThreads);
            return  std::make_shared<InfFactory>(opt);
        }
    };
} 
}

#endif // INFERNO_PYTHON_QPBO_FACTORY
