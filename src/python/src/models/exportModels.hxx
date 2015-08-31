#ifndef  INFERNO_SRC_PYTHON_MODELS_EXPORT_MODELS_HXX
#define INFERNO_SRC_PYTHON_MODELS_EXPORT_MODELS_HXX

#include <boost/python/def_visitor.hpp> 

#include "inferno/inferno.hxx"
#include "inferno/inferno_python.hxx"
#include "inferno/python/export_non_copyable_vector.hxx"
#include "inferno/python/export_factor.hxx"
#include "inferno/python/export_non_copyable_vector.hxx"
#include "inferno/python/export_model_maps.hxx"

namespace inferno{
namespace models{
namespace export_helper{

    namespace bp = boost::python;


   




  


    template<class MODEL>
    void exportVectorOfModels(const std::string & modelClsName){
        const std::string clsName = modelClsName + std::string("Vector");
        typedef std::vector<MODEL> ModelVector;

        bp::class_<ModelVector,boost::noncopyable >(clsName.c_str(), bp::init<>())
            .def(bp::init<const size_t >())
            .def(python::NonCopyableVectorVisitor<ModelVector>())
        ;
    }

    template<class MODEL>
    void exportVectorOfConfs(const std::string & modelClsName){
        const std::string clsName = modelClsName + std::string("ConfMapVector"); 
        typedef typename MODEL:: template VariableMap<DiscreteLabel> ConfMap;
        typedef std::vector<ConfMap> ConfVector;

        bp::class_<ConfVector,boost::noncopyable >(clsName.c_str(), bp::init<>())
            .def(bp::init<const size_t >())
            .def(python::NonCopyableVectorVisitor<ConfVector>())
        ;
    }

    

    template<class MODEL>
    class ExportModelAPI  : public boost::python::def_visitor<ExportModelAPI<MODEL> >
    {
    public:
        typedef MODEL Model;
        typedef typename Model:: template VariableMap<inferno::DiscreteLabel> Conf;
        typedef typename Model::FactorDescriptor FactorDescriptor;

        typedef typename std::result_of<
            decltype(&Model::factor)(Model, const FactorDescriptor )
        >::type ReturnedFactorType;





        ExportModelAPI(const std::string clsName)
        : clsName_(clsName){

        }
        friend class bp::def_visitor_access;

        template <class classT>
        void visit(classT& c) const
        {   

            // export all var maps
            exportVectorOfModels<Model>(clsName_);
            exportVectorOfConfs<Model>(clsName_);
            python::exportFactor<Model>(clsName_);

            python::ModelMapExporter<Model>::exportModelMaps(clsName_);

            c
                // properties
                .add_property("nVariables", &Model::nVariables)
                .add_property("nFactors", &Model::nFactors)
                .add_property("nUnaries", &Model::nUnaries)

                // functions
                .def("nLabels",&eval)
                .def("eval",&eval)
                .def("consTerm",&Model::constTerm)
                .def("maxArity",&maxArity)
                .def("hasSimpleLabelSpace",&hasSimpleLabelSpace)
                .def("minMaxNLabels",&minMaxNLabels)
                
                // id's and and descriptors
                .def("factorId",&Model::factorId)
                .def("variableId",&Model::variableId)
                .def("factorDescriptor",&Model::factorDescriptor)
                .def("variableDescriptor",&Model::variableDescriptor)

                .def("denseVariableIds",&Model::denseVariableIds)

                .def("minVarId",&Model::minVarId)
                .def("maxVarId",&Model::maxVarId)
                .def("minFactorId",&Model::minFactorId)
                .def("maxFactorId",&Model::maxFactorId)

                //  (high level) - model info
                .def("hasUnaries",&Model::hasUnaries)
                .def("nPairwiseFactors",&Model::nPairwiseFactors)
                .def("nPairwiseFactors",&Model::nPairwiseFactors)
                .def("isSecondOrderMulticutModel",&Model::isSecondOrderMulticutModel)
                .def("isMulticutModel",&Model::isMulticutModel)
            ;
        }
    private:

        static ValueType eval(const Model & m, const Conf & conf){
            ValueType v=0;
            {
                ScopedGILRelease allowThreads;
                v =  m.eval(conf);
            }
            return v;
        }
        static ArityType maxArity(const Model & m){
            ArityType a=0;
            {
                ScopedGILRelease allowThreads;
                a =  m.maxArity();
            }
            return a;
        }
        static bp::tuple hasSimpleLabelSpace(const Model & m){
            DiscreteLabel l = 0;
            bool isSimple;
            {
                ScopedGILRelease allowThreads;
                isSimple =  m.hasSimpleLabelSpace(l);
            }
            return bp::make_tuple(isSimple, l);
        }
        static bp::tuple minMaxNLabels(const Model & m){
            DiscreteLabel lmin = 0;
            DiscreteLabel lmax = 0;
            {
                ScopedGILRelease allowThreads;
                m.minMaxNLabels(lmin, lmax);
            }
            return bp::make_tuple(lmin, lmax);
        }


        std::string clsName_;
        //static void foo(MODEL& self);
        //static void bar(MODEL& self);
    };
} // end namespace inferno::models::export_helper
} // end namespace inferno::models  
} // end namespace inferno

#endif /* INFERNO_SRC_PYTHON_MODELS_EXPORT_MODELS_HXX */
