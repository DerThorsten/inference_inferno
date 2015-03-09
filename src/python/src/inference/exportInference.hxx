#ifndef  INFERNO_SRC_PYTHON_INFERENCE_EXPORT_INFERENCE_HXX
#define INFERNO_SRC_PYTHON_INFERENCE_EXPORT_INFERENCE_HXX

#include <cctype>

#include <boost/python/def_visitor.hpp> 

#include "inferno/inferno.hxx"
#include "inferno/inferno_python.hxx"
#include "inferno/inference/base_discrete_inference.hxx"


#define INFERNO_EXPORT_INFERENCE(MODEL, modelName, INF, infName) \
    boost::python::class_< INF,boost::python::bases< DiscreteInferenceBase<MODEL>   >,boost::noncopyable > \
    (( std::string(infName) + modelName ).c_str(),boost::python::no_init) \
        .def(export_helper::ExportInferenecAPI<INF>(modelName, infName)) 



namespace inferno{
namespace inference{
namespace export_helper{

    namespace bp = boost::python;

    template<class MODEL>
    struct BaseInfWrap : 
        public DiscreteInferenceBase<MODEL>, 
        bp::wrapper<DiscreteInferenceBase<MODEL> >
    {
        typedef MODEL Model;
        typedef typename MODEL :: template VariableMap<DiscreteLabel> Conf;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor  VisitorType;


        std::string name()const{
            return this->get_override("name")();
        }
        // inference
        void infer( VisitorType  * visitor  = NULL) {
            this->get_override("infer")(visitor);
        }
        // get result
        void conf(Conf & conf ){
            this->get_override("__conf")(conf);
        }

        DiscreteLabel label(const Vi vi ) {
           return this->get_override("label")(vi);
        }

        // get model
        const Model & model() const{
            return this->get_override("model")();
        }
        // stop inference (via visitor)
        void stopInference(){
            this->get_override("stopInference")();
        }
    };


    template<class BASE_INF>
    vigra::NumpyAnyArray getConf(BASE_INF * inf, vigra::NumpyArray<1, DiscreteLabel> out){
        const auto & model = inf->model();
        vigra::TinyVector<vigra::MultiArrayIndex, 1> shape(model.maxVarId()+1);
        out.reshapeIfEmpty(shape);
        {
            ScopedGILRelease _gil;
            for(const auto vi : model.variableIds())
                out(vi) = inf->label(vi);
        }
        return out;

    }

    template<class INF,class MODEL>
    InferenceOptions getDefaultOptions(const MODEL & model){
        InferenceOptions  options;
        INF::defaultOptions(options);
        return options;
    }



    template<class MODEL>
    void exportDiscreteInferenceBase(const std::string modelName){


        const std::string baseClsName = std::string("DiscreteInferenceBase") + modelName;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        bp::class_<export_helper::BaseInfWrap<MODEL>, boost::noncopyable>(baseClsName.c_str())
            // pure virtual functions
            .def("name", bp::pure_virtual( &BaseInf::name))
            .def("infer", bp::pure_virtual(&BaseInf::infer),(bp::arg("visitor")=NULL))
            .def("__conf", bp::pure_virtual(&BaseInf::conf))
            .def("conf", vigra::registerConverters(getConf<BaseInf>), 
                (bp::arg("out") = bp::object() )
            )
            .def("label", bp::pure_virtual(&BaseInf::label))
            .def("model", bp::pure_virtual(&BaseInf::model),bp::return_internal_reference<>())
            .def("stopInference", bp::pure_virtual(&BaseInf::stopInference))
        ;
        #if 0
        {
            // export icm
            typedef Icm<MODEL> Inference;
            const std::string infClsName = std::string("Icm") + modelName;
            bp::class_< Inference,bp::bases<BaseInf>,boost::noncopyable >(infClsName.c_str(),bp::no_init)
                .def(export_helper::ExportInferenecAPI<Inference>(modelName,"Icm"))
            ; 
            //// export factory
            //bp::def("icmOptions", &getDefaultOptions<Inference, MODEL>);
            //bp::def("icm", & inferenceFactory<Inference>,
            //    (
            //        bp::arg("model"),
            //        bp::arg("options") = InferenceOptions(),
            //        bp::arg("checkOptions") = true
            //    ),
            //    CustWardPost< 0,1 ,RetValPolNewObj>()  
            //);
        }
        {
            // export mp
            typedef MessagePassing<MODEL> Inference;
            const std::string infClsName = std::string("MessagePassing") + modelName;
            bp::class_< Inference,bp::bases<BaseInf>,boost::noncopyable >(infClsName.c_str(),bp::no_init)
            ; 
            // export factory
            bp::def("messagePassingOptions", &getDefaultOptions<Inference, MODEL>);
            bp::def("messagePassing", & inferenceFactory< Inference>,
                (
                    bp::arg("model"),
                    bp::arg("options") = InferenceOptions(),
                    bp::arg("checkOptions") = true
                ),
                CustWardPost< 0,1 ,RetValPolNewObj>()  
            );
        }
        {   
            #ifdef WITH_QPBO
            // export qpbo
            typedef Qpbo<MODEL> Inference;
            const std::string infClsName = std::string("Qpbo") + modelName;
            bp::class_< Inference,bp::bases<BaseInf>,boost::noncopyable >(infClsName.c_str(),bp::no_init)
            ; 
            // export factory
            bp::def("qpboOptions", &getDefaultOptions<Inference, MODEL>);
            bp::def("qpbo", & inferenceFactory< Inference>,
                (
                    bp::arg("model"),
                    bp::arg("options") = InferenceOptions(),
                    bp::arg("checkOptions") = true
                ),
                CustWardPost< 0,1 ,RetValPolNewObj>()  
            );
            #endif
        }
        {   
            #ifdef WITH_QPBO
            // export qpbo
            typedef HigherOrderQpbo<MODEL> Inference;
            const std::string infClsName = std::string("HigherOrderQpbo") + modelName;
            bp::class_< Inference,bp::bases<BaseInf>,boost::noncopyable >(infClsName.c_str(),bp::no_init)
            ; 
            // export factory
            bp::def("higherOrderQpboOptions", &getDefaultOptions<Inference, MODEL>);
            bp::def("higherOrderQpbo", & inferenceFactory< Inference>,
                (
                    bp::arg("model"),
                    bp::arg("options") = InferenceOptions(),
                    bp::arg("checkOptions") = true
                ),
                CustWardPost< 0,1 ,RetValPolNewObj>()  
            );
            #endif
        }

        {
            // export hmmwc
            typedef Hmmwc<MODEL> Inference;
            const std::string infClsName = std::string("Hmmwc") + modelName;
            bp::class_< Inference,bp::bases<BaseInf>,boost::noncopyable >(infClsName.c_str(),bp::no_init)
            ; 
            // export factory
            bp::def("hmmwcOptions", &getDefaultOptions<Inference, MODEL>);
            bp::def("hmmwc", & inferenceFactory< Inference>,
                (
                    bp::arg("model"),
                    bp::arg("options") = InferenceOptions(),
                    bp::arg("checkOptions") = true
                ),
                CustWardPost< 0,1 ,RetValPolNewObj>()  
            );
        }
        #endif
    }


    template<class INF>
    class ExportInferenecAPI  : public boost::python::def_visitor<ExportInferenecAPI<INF> >
    {
    public:
        typedef INF Inference;
        typedef typename Inference::Model Model;
        typedef typename Model:: template VariableMap<inferno::DiscreteLabel> Conf;

        ExportInferenecAPI(const std::string & modelName, const std::string & inferenceName)
        :   modelName_(modelName),
            inferenceName_(inferenceName){

        }
        friend class bp::def_visitor_access;

        template <class classT>
        void visit(classT& c) const
        {   
           
            c
                //add_property("nVariables", &Model::nVariables)
                //.add_property("nFactors", &Model::nFactors)
            ;

            std::string facFuncName = inferenceName_;
            facFuncName[0] = std::tolower(facFuncName[0]);
            //const std::string infClsName = inferenceName_ + modelName_;
            const std::string optFuncName = facFuncName + std::string("Options");

            // export factory
            bp::def(optFuncName.c_str(), &getDefaultOptions);
            bp::def(facFuncName.c_str(), &inferenceFactory<Inference>,
                (
                    bp::arg("model"),
                    bp::arg("options") = InferenceOptions(),
                    bp::arg("checkOptions") = true
                ),
                CustWardPost< 0,1 ,RetValPolNewObj>()  
            );

        }

        static InferenceOptions getDefaultOptions(const Model & model){
            InferenceOptions  options;
            Inference::defaultOptions(options);
            return options;
        }

    private:
        std::string modelName_;
        std::string inferenceName_;
    };

    /*
    template<class INF>
    void exportInference(
        const std::string & modelName,
        const std::string & inferenceName
    ){
        std::string lcsModelName = modelName;
        lcsModelName[0] = std::tolower(std:L:lcsModelName[0]);
        typedef INF Inference;
        const std::string infClsName = inferenceName + modelName;
        const std::string optFuncName = lcsModelName + std::string("Options");
        const std::string facFuncName = lcsModelName;

        bp::class_< Inference,bp::bases<BaseInf>,boost::noncopyable >(infClsName.c_str(),bp::no_init)
        ; 
        // export factory
        bp::def(optFuncName.c_str(), &getDefaultOptions<Inference, MODEL>);
        bp::def(facFuncName.c_str(), & inferenceFactory<Inference>,
            (
                bp::arg("model"),
                bp::arg("options") = InferenceOptions(),
                bp::arg("checkOptions") = true
            ),
            CustWardPost< 0,1 ,RetValPolNewObj>()  
        );
    }
    */

} // end namespace inferno::inference::export_helper
} // end namespace inferno::inference  
} // end namespace inferno

#endif /* INFERNO_SRC_PYTHON_INFERENCE_EXPORT_INFERENCE_HXX */
