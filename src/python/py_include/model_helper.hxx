#ifndef INFERNO_PY_MODEL_HELPER
#define INFERNO_PY_MODEL_HELPER



// boost python related
#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/def_visitor.hpp>

// vigra numpy array converters
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>
#include <boost/python/exception_translator.hpp>

// standart c++ headers (whatever you need (string and vector are just examples))
#include <string>
#include <vector>

// inferno relatex
#include "inferno/inferno.hxx"
#include "inferno/model.hxx"




namespace inferno{


   namespace bp = boost::python;


    class ModelDefVisitor : public bp::def_visitor<ModelDefVisitor>
    {
        friend class bp::def_visitor_access;
        template <class classT>
        void visit(classT& c) const
        {
            c
                .def("variablesInfo",&variablesInfo,"returns the tuple (minVarId, maxVarId, nVar, isDense)")
                .def("factorsInfo",&factorsInfo,    "returns the tuple (minVarId, maxVarId, nVar, isDense)")
            ;
        } //

        
        static bp::tuple variablesInfo(const Model * model){
            const VariablesInfo info = model->variablesInfo();
            return bp::make_tuple(info.minVariableId(),
                                  info.maxVariableId(),
                                  info.minVariableId(),
                                  info.isDense());
        }
        static bp::tuple factorsInfo(const Model * model){
            const FactorsInfo info = model->factorsInfo();
            return bp::make_tuple(info.minFactorId(),
                                  info.maxFactorId(),
                                  info.minFactorId(),
                                  info.isDense());
        }
    };

    struct ModelWrap : Model, bp::wrapper<Model>
    {
        FactorSharedPtr getFactor(const int64_t fi) const{
            return this->get_override("getFactor")(fi);
        }//

        VariablesInfo variablesInfo()const{
            return this->get_override("variablesInfo")();
        }

        FactorsInfo factorsInfo()const{
            return this->get_override("factorsInfo")();
        }

        bool isVariableId(const int64_t id) const{
            return this->get_override("isVariableId")(id);
        }

        bool isFactorId(const int64_t id) const{
            return this->get_override("isFactorId")(id);
        }

        void varBounds(const int64_t vi, MixedLabelBounds & bounds)const {
            this->get_override("varBounds")(vi, bounds);
        }
        void varBounds(const int64_t vi, ContinousLabelBounds & bounds)const {
            this->get_override("varBounds")(vi, bounds);
        }
        void varBounds(const int64_t vi, DiscreteLabelBounds & bounds)const {
            this->get_override("varBounds")(vi, bounds);
        }




        DiscreteFactorSharedPtr  getFactor(const int64_t fi, const DiscreteTag tag) const{
            if (bp::override f = this->get_override("getFactor"))
                return f(fi, tag); // *note*
            return Model::getFactor(fi, tag);
        }
        ContinousFactorSharedPtr getFactor(const int64_t fi, const ContinousTag tag) const{
            if (bp::override f = this->get_override("getFactor"))
                return f(fi, tag); // *note*
            return Model::getFactor(fi, tag);
        }
        MixedFactorSharedPtr     getFactor(const int64_t fi, const MixedTag tag) const{
            if (bp::override f = this->get_override("getFactor"))
                return f(fi, tag); // *note*
            return Model::getFactor(fi, tag);
        }


        // default impls
        DiscreteFactorSharedPtr  defaultGetFactor(const size_t fi, const DiscreteTag tag) const{
            return this->Model::getFactor(fi, tag);
        }
        ContinousFactorSharedPtr defaultGetFactor(const size_t fi, const ContinousTag tag) const{
             return this-> Model::getFactor(fi, tag);
        }
        MixedFactorSharedPtr     defaultGetFactor(const size_t fi, const MixedTag tag) const{
             return this-> Model::getFactor(fi, tag);
        }
    };

    struct DiscreteModelWrap : DiscreteModel, bp::wrapper<DiscreteModel>
    {
        FactorSharedPtr getFactor(const int64_t fi) const{
            return this->get_override("getFactor")(fi);
        }//

        VariablesInfo variablesInfo()const{
            return this->get_override("variablesInfo")();
        }

        FactorsInfo factorsInfo()const{
            return this->get_override("factorsInfo")();
        }

        bool isVariableId(const int64_t id) const{
            return this->get_override("isVariableId")(id);
        }

        bool isFactorId(const int64_t id) const{
            return this->get_override("isFactorId")(id);
        }

       
        void varBounds(const int64_t vi, DiscreteLabelBounds & bounds)const {
            this->get_override("varBounds")(vi, bounds);
        }


        void varBounds(const int64_t vi, MixedLabelBounds & bounds)const {
            if (bp::override f = this->get_override("varBounds"))
                f(vi, bounds); // *note*
            DiscreteModel::varBounds(vi, bounds);
        }
        void varBounds(const int64_t vi, ContinousLabelBounds & bounds)const {
            if (bp::override f = this->get_override("varBounds"))
                f(vi, bounds); // *note*
            DiscreteModel::varBounds(vi, bounds);
        }

        DiscreteFactorSharedPtr  getFactor(const int64_t fi, const DiscreteTag tag) const{
            if (bp::override f = this->get_override("getFactor"))
                return f(); // *note*
            return DiscreteModel::getFactor(fi, tag);
        }
        ContinousFactorSharedPtr getFactor(const int64_t fi, const ContinousTag tag) const{
            if (bp::override f = this->get_override("getFactor"))
                return f(); // *note*
            return DiscreteModel::getFactor(fi, tag);
        }
        MixedFactorSharedPtr     getFactor(const int64_t fi, const MixedTag tag) const{
            if (bp::override f = this->get_override("getFactor"))
                return f(); // *note*
            return DiscreteModel::getFactor(fi, tag);
        }


        // default impls

        void defaultVarBounds(const int64_t id, DiscreteLabelBounds & bounds)const {
            return this->DiscreteModel::varBounds(id, bounds);
        }

        DiscreteFactorSharedPtr  defaultGetFactor(const size_t fi, const DiscreteTag tag) const{
            return this->DiscreteModel::getFactor(fi, tag);
        }
        ContinousFactorSharedPtr defaultGetFactor(const size_t fi, const ContinousTag tag) const{
             return this->DiscreteModel::getFactor(fi, tag);
        }
        MixedFactorSharedPtr     defaultGetFactor(const size_t fi, const MixedTag tag) const{
             return this->DiscreteModel::getFactor(fi, tag);
        }
    };


} // end namespace inferno

#endif