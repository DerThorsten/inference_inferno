#ifndef  INFERNO_SRC_PYTHON_MODELS_EXPORT_MODELS_HXX
#define INFERNO_SRC_PYTHON_MODELS_EXPORT_MODELS_HXX

#include <boost/python/def_visitor.hpp> 

#include "inferno/inferno.hxx"
#include "inferno/inferno_python.hxx"

namespace inferno{
namespace models{
namespace export_helper{

    namespace bp = boost::python;


    template<class MODEL, class VAR_MAP, class T>
    vigra::NumpyAnyArray asIdIndexableArray(
        const VAR_MAP & varMap,
        const MODEL & model,
        vigra::NumpyArray<1, T> out
    ){  
        uint64_t size = model.nVariables()==0 ? 0 : uint64_t(model.maxVarId()+1);
        out.reshapeIfEmpty(typename vigra::NumpyArray<1, T>::difference_type(size));
        {
            ScopedGILRelease allowThreads; 
            for(auto var: model.variableDescriptors()){
                const auto varId = model.variableId(var);
                out(varId) = varMap[var];  
            }
        }
        return out;
    }

    template<class MODEL, class VAR_MAP, class T>
    void fromIdIndexableArray(
        VAR_MAP & varMap,
        const MODEL & model,
        const vigra::NumpyArray<1, T> & in
    ){  
        vigra::MultiArrayView<1, T> view(in);
        {
            ScopedGILRelease allowThreads; 
            for(auto var: model.variableDescriptors()){
                const auto varId = model.variableId(var);
                varMap[var] = view(varId);  
            }
        }
    }

    template<class MODEL, class VAR_MAP, class T>
    T readAtId(
        const VAR_MAP & varMap,
        const MODEL & model,
        const Vi id
    ){ 
        return varMap[model.variableDescriptor(id)];
    }

    template<class MODEL, class VAR_MAP, class T>
    vigra::NumpyAnyArray readAtIds(
        const VAR_MAP & varMap,
        const MODEL & model,
        const vigra::NumpyArray<1, Vi> viArray,
        vigra::NumpyArray<1, T> out
    ){ 
        uint64_t size = viArray.shape(0);
        out.reshapeIfEmpty(typename vigra::NumpyArray<1, T>::difference_type(size));
        vigra::MultiArrayView<1, T> view(out);
        {
            ScopedGILRelease allowThreads; 
            auto i=0;
            for(const auto & vi : viArray){
                out[i] = varMap[model.variableDescriptor(vi)];
                ++i;
            }
        }
        return out;
    }

    template<class MODEL, class VAR_MAP, class T>
    void writeAtIds(
        VAR_MAP & varMap,
        const MODEL & model,
        const vigra::NumpyArray<1, Vi> viArray,
        const vigra::NumpyArray<1, T> & in
    ){ 
        vigra::MultiArrayView<1, T> view(in);
        {
            ScopedGILRelease allowThreads; 
            auto i=0;
            for(const auto & vi : viArray){
                varMap[model.variableDescriptor(vi)] = in[i];
                ++i;
            }
        }
    }


    template<class MODEL, class VAR_MAP, class T>
    void writeAtId(
        VAR_MAP & varMap,
        const MODEL & model,
        const Vi id,
        const T & value
    ){ 
        varMap[model.variableDescriptor(id)] = value;
    }


    template<class MODEL, class VAR_MAP, class T>
    void assignWithVal(
        VAR_MAP & varMap,
        const MODEL & model,
        const T & val
    ){
        varMap.assign(model, val);
    }
    template<class MODEL, class VAR_MAP, class T>
    void assign(
        VAR_MAP & varMap,
        const MODEL & model
    ){
        varMap.assign(model);
    }


    template<class MODEL, class T>
    void exportVarMapT(const std::string & modelClsName, const std::string & tClsName){
        typedef typename MODEL:: template VariableMap<T> VarMap;
        const std::string clsName = modelClsName + std::string("VariableMap")+tClsName;

        bp::class_<VarMap>(clsName.c_str(), bp::init<const MODEL & >())
            .def(bp::init<const MODEL &, const T &>())
            .def("model",&VarMap::model, bp::return_internal_reference<>())
            .def("assign",assignWithVal<MODEL, VarMap, T>,
                (
                    bp::arg("model"),
                    bp::arg("value")
                )
            )
            .def("assign",assign<MODEL, VarMap, T>,
                (
                    bp::arg("model")
                )
            )

            .def("asIdIndexableArray",
                vigra::registerConverters(&asIdIndexableArray<MODEL, VarMap, T>),
                (
                    bp::arg("model"),
                    bp::arg("out") = bp::object()
                )
            )
            .def("fromIdIndexableArray",
                vigra::registerConverters(&fromIdIndexableArray<MODEL, VarMap, T>),
                (
                    bp::arg("model"),
                    bp::arg("in")
                )
            )
            .def("readAtId",
                &readAtId<MODEL, VarMap, T>,
                (
                    bp::arg("model"),
                    bp::arg("id")
                )
            )
            .def("readAtIds",
                vigra::registerConverters(&readAtIds<MODEL, VarMap, T>),
                (
                    bp::arg("model"),
                    bp::arg("ids"),
                    bp::arg("out") = bp::object()
                )
            )
            .def("writeAtId",
                &writeAtId<MODEL, VarMap, T>,
                (
                    bp::arg("model"),
                    bp::arg("id"),
                    bp::arg("value")
                )
            )
            .def("writeAtIds",
                vigra::registerConverters(&writeAtIds<MODEL, VarMap, T>),
                (
                    bp::arg("model"),
                    bp::arg("ids"),
                    bp::arg("in")
                )
            )
        ;
    }

    template<class MODEL>
    void exportVarMaps(const std::string & modelClsName){
        exportVarMapT<MODEL, bool>(modelClsName,        std::string("Bool"));
        exportVarMapT<MODEL, int32_t>(modelClsName,     std::string("Int32"));
        exportVarMapT<MODEL, int64_t>(modelClsName,     std::string("Int64"));
        exportVarMapT<MODEL, uint32_t>(modelClsName,    std::string("UInt32"));
        exportVarMapT<MODEL, uint64_t>(modelClsName,    std::string("UInt64"));
        exportVarMapT<MODEL, float>(modelClsName,       std::string("Float32"));
        exportVarMapT<MODEL, double>(modelClsName,      std::string("Float64"));
        //exportVarMapT<MODEL, void *>(modelClsName,      std::string("VoidPtr"));
    }


    template<class MODEL>
    void exportVectorOfModels(const std::string & modelClsName){
        const std::string clsName = modelClsName + std::string("Vector");
        typedef std::vector<MODEL> ModelVector;

        bp::class_<ModelVector,boost::noncopyable >(clsName.c_str(), bp::init<>())
            .def(bp::init<const size_t >())
        ;

    }

    template<class MODEL>
    class ExportModelAPI  : public boost::python::def_visitor<ExportModelAPI<MODEL> >
    {
    public:
        typedef MODEL Model;
        typedef typename Model:: template VariableMap<inferno::DiscreteLabel> Conf;

        ExportModelAPI(const std::string clsName)
        : clsName_(clsName){

        }
        friend class bp::def_visitor_access;

        template <class classT>
        void visit(classT& c) const
        {   

            // export all var maps
            exportVarMaps<MODEL>(clsName_);
            exportVectorOfModels<MODEL>(clsName_);
       

            c
                .add_property("nVariables", &Model::nVariables)
                .add_property("nFactors", &Model::nFactors)
                .add_property("nUnaries", &Model::nUnaries)
            ;
        }
    private:
        std::string clsName_;
        //static void foo(MODEL& self);
        //static void bar(MODEL& self);
    };
} // end namespace inferno::models::export_helper
} // end namespace inferno::models  
} // end namespace inferno

#endif /* INFERNO_SRC_PYTHON_MODELS_EXPORT_MODELS_HXX */
