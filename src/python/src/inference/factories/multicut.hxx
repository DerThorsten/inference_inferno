#ifndef INFERNO_PYTHON_MULTICUT_FACTORY
#define INFERNO_PYTHON_MULTICUT_FACTORY

#include "../exportInference.hxx"

//#ifdef WITH_QPBO
#define WITH_BOOST
#include "inferno/inference/opengm_inference.hxx"
#include "opengm/inference/multicut.hxx"
//#endif

namespace inferno{
namespace inference{

    template<class INF>
    struct ExportInferenceFactory;

    //template<class MODEL>
    //using OgmMcInference<MODEL> = OpengmInference<
    //    opengm::Multicut<ogm::GraphicalModel>,
    //    opengm::Minimizer
    //>;


    template< class MODEL>
    struct ExportInferenceFactory<
        OpengmInference<
            MODEL, opengm::Multicut<ogm::GraphicalModel,opengm::Minimizer>
        >
     >{
        typedef OpengmInference<
           MODEL, opengm::Multicut<ogm::GraphicalModel,opengm::Minimizer>
        > Inference;
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
                    bp::arg("workFlow") = defaultOptions.workFlow_
                )
            );
        }

        static std::shared_ptr<BaseInfFactory>  pyFactoryFactory(
            const std::string & workFlow
        ){      
            Options opt;
            opt.workFlow_ = workFlow;
            return  std::make_shared<InfFactory>(opt);
        }
    };
} 
}

#endif // INFERNO_PYTHON_MULTICUT_FACTORY
