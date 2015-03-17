#ifndef  INFERNO_SRC_PYTHON_INFERENCE_EXPORT_INFERENCE_HXX
#define INFERNO_SRC_PYTHON_INFERENCE_EXPORT_INFERENCE_HXX

#include <cctype>

#include <boost/python/def_visitor.hpp> 

#include "inferno/inferno.hxx"
#include "inferno/inferno_python.hxx"
#include "inferno/inference/base_discrete_inference.hxx"
#include "inferno/inference/base_discrete_inference_factory.hxx"


#define INFERNO_EXPORT_INFERENCE(MODEL, modelName, INF, infName) \
    boost::python::class_< INF,boost::python::bases< DiscreteInferenceBase<MODEL>   >,boost::noncopyable > \
    (( std::string(infName) + modelName ).c_str(),boost::python::no_init) \
        .def(ExportInferenecAPI<INF>(modelName, infName)) 



namespace inferno{
namespace inference{

    inline std::string lowerFirst(const std::string & in){
        std::string out = in;
        out[0] = std::tolower(out[0]);
        return out;
    }

    namespace bp = boost::python;


    template<class INF>
    struct ExportInferenceFactory{
        typedef INF Inference;
        typedef typename Inference::Model Model;
        typedef DiscreteInferenceBase<Model> BaseInf;
        typedef BaseDiscreteInferenceFactory<Model> BaseInfFactory;
        typedef DiscreteInferenceFactory<INF> InfFactory;

        static void op(const std::string  & modelName, const std::string & infName){


            namespace bp = boost::python;

            const std::string clsName =  infName + std::string("Factory") + modelName;
            std::string fName = clsName;
            fName[0] = std::tolower(fName[0]);

            bp::class_<InfFactory, bp::bases<BaseInfFactory>,  boost::noncopyable> 
            (
                clsName.c_str(),bp::no_init
            );

            bp::def(fName.c_str(), &pyFactoryFactory );

        }

        static std::shared_ptr<BaseInfFactory>  pyFactoryFactory(
        ){
            return  std::make_shared<InfFactory>();
        } 
    };


    template<class MODEL>
    struct BaseInfFactoryWrap : 
        public BaseDiscreteInferenceFactory<MODEL>, 
        bp::wrapper<BaseDiscreteInferenceFactory<MODEL> >
    {
        typedef MODEL Model;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef BaseDiscreteInferenceFactory<MODEL> BaseInfFactory;

        std::shared_ptr<BaseInf> create(const Model & model){
            return this->get_override("create")(model);
        }

    };


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

        
        // VIRTUAL FUNCTIONS WITH DEFAULT IMPL

        #if 0 
        // get results
        ValueType lowerBound(){
            return this->get_override("lowerBound")();
        }
        ValueType upperBound(){
            return this->get_override("upperBound")();
        }
        ValueType energy(){
            return this->get_override("energy")();
        }

        // partial optimality
        bool isPartialOptimal(const Vi vi){
            return this->get_override("isPartialOptimal")(vi);
        }
        # endif
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


    // this export function must
    // be called only once for each type of model
    template<class MODEL>
    void exportDiscreteInferenceBase(const std::string modelName){

        // register the shared ptr
        bp::register_ptr_to_python< std::shared_ptr<  DiscreteInferenceBase<MODEL> > >();
        bp::register_ptr_to_python< std::shared_ptr<  BaseDiscreteInferenceFactory<MODEL> > >();
        // the  base inference factory class
        const std::string baseClsFactoryName = std::string("BaseDiscreteInferenceFactory") + modelName;
        typedef BaseDiscreteInferenceFactory<MODEL> BaseInfFactory;
        bp::class_<BaseInfFactoryWrap<MODEL>, boost::noncopyable>(baseClsFactoryName.c_str())
            // pure virtual functions
            .def("create", bp::pure_virtual( &BaseInfFactory::create),CustWardPostEnd< 0,1 >()  )
        ;

        // the actual base inference class
        const std::string baseClsName = std::string("DiscreteInferenceBase") + modelName;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        bp::class_<BaseInfWrap<MODEL>, boost::noncopyable>(baseClsName.c_str())
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
    }

    // this export "visitor" is called for
    // each template instantiation of a particular solver
    template<class INF>
    class ExportInferenecAPI  : public boost::python::def_visitor<ExportInferenecAPI<INF> >
    {
    public:
        typedef INF Inference;
        typedef typename INF::Options Options;
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

            //// export factory
            ExportInferenceFactory<INF>::op(modelName_, inferenceName_);

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

} // end namespace inferno::inference  
} // end namespace inferno

#endif /* INFERNO_SRC_PYTHON_INFERENCE_EXPORT_INFERENCE_HXX */
