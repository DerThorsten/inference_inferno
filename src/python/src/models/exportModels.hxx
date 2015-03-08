#ifndef  INFERNO_SRC_PYTHON_MODELS_EXPORT_MODELS_HXX
#define INFERNO_SRC_PYTHON_MODELS_EXPORT_MODELS_HXX

#include <boost/python/def_visitor.hpp> 

#include "inferno/inferno.hxx"

namespace inferno{
namespace models{
namespace export_helper{

    namespace bp = boost::python;

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
            // conf map
            std::string confClsName = std::string("ConfMap") + clsName_;
            bp::class_<Conf>(confClsName.c_str(), bp::init<const Model & >())
            ;

            c
                .add_property("nVariables", &Model::nVariables)
                .add_property("nFactors", &Model::nFactors)
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
