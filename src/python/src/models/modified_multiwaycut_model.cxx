#define PY_ARRAY_UNIQUE_SYMBOL inferno_models_PyArray_API
#define NO_IMPORT_ARRAY


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


// export helper
#include "exportModels.hxx"

// standart c++ headers (whatever you need (string and vector are just examples))
#include <string>
#include <vector>

// inferno relatex
#include "inferno/inferno.hxx"


#include "inferno/inferno.hxx"
#include "inferno/model/modified_multiwaycut_model.hxx"

namespace inferno{
namespace models{

    namespace bp = boost::python;






    template<class VI, class T>
    void addPottsFactors(ModifiedMultiwaycutModel & model, 
               const vigra::NumpyArray<1, T> & betas,
               const vigra::NumpyArray<2, VI> & vis
    ){
        INFERNO_CHECK_OP(betas.shape(0), == ,vis.shape(0),"vis.shape does not match betas.shape");
        INFERNO_CHECK_OP(vis.shape(1), == ,2," wrong vi shape");


        for(uint64_t i=0; i<betas.shape(0); ++i ){
            const Vi u = vis(i,0);
            const Vi v = vis(i,1);
            const ValueType b = betas(i);
            model.addPottsFactor(u, v, b);
        }
    }


    template<class VI, class T>
    void addUnaryFactors(ModifiedMultiwaycutModel & model, 
               const vigra::NumpyArray<2, T> & semanticCosts,
               const vigra::NumpyArray<1, VI> & vis
    ){
        INFERNO_CHECK_OP(semanticCosts.shape(0), == ,vis.shape(0),"vis.shape does not match semanticCosts.shape");
        INFERNO_CHECK_OP(semanticCosts.shape(1), == ,model.nSemanticClasses(),
            "semanticCosts shape does not match "
            " with the number of semantic classes"
        );
        for(uint64_t i=0; i<semanticCosts.shape(0); ++i ){
            const Vi u = vis(i);
            const auto vals = semanticCosts.bindInner(i);
            model.addUnaryFactor(u, vals.begin(), vals.end());
        }
    }



    void exportModifiedMultiwaycutModel(){
        typedef ModifiedMultiwaycutModel Model;
        bp::class_<Model>("ModifiedMultiwaycutModel",bp::init<const Vi,const Vi>())
            .def(export_helper::ExportModelAPI<Model>("ModifiedMultiwaycutModel"))


            .def("",vigra::registerConverters(&addPottsFactors<int64_t, float>))
            .def("addPottsFactors",vigra::registerConverters(&addPottsFactors<int32_t, float>))
            .def("addPottsFactors",vigra::registerConverters(&addPottsFactors<uint64_t, float>))
            .def("addPottsFactors",vigra::registerConverters(&addPottsFactors<uint32_t, float>))
            .def("addPottsFactors",vigra::registerConverters(&addPottsFactors<int64_t, double>))
            .def("addPottsFactors",vigra::registerConverters(&addPottsFactors<int32_t, double>))
            .def("addPottsFactors",vigra::registerConverters(&addPottsFactors<uint64_t, double>))
            .def("addPottsFactors",vigra::registerConverters(&addPottsFactors<uint32_t, double>))

            .def("addUnaryFactors",vigra::registerConverters(&addUnaryFactors<int64_t, float>))
            .def("addUnaryFactors",vigra::registerConverters(&addUnaryFactors<int32_t, float>))
            .def("addUnaryFactors",vigra::registerConverters(&addUnaryFactors<uint64_t, float>))
            .def("addUnaryFactors",vigra::registerConverters(&addUnaryFactors<uint32_t, float>))
            .def("addUnaryFactors",vigra::registerConverters(&addUnaryFactors<int64_t, double>))
            .def("addUnaryFactors",vigra::registerConverters(&addUnaryFactors<int32_t, double>))
            .def("addUnaryFactors",vigra::registerConverters(&addUnaryFactors<uint64_t, double>))
            .def("addUnaryFactors",vigra::registerConverters(&addUnaryFactors<uint32_t, double>))

            // batch add potts value tableS
            //.def("addPottsValueTables",vigra::registerConverters(&addPottsValueTables<float>))
            //.def("addPottsValueTables",vigra::registerConverters(&addPottsValueTables<double>))
            //.def("addL1ValueTables",vigra::registerConverters(&addL1ValueTables<float>))
            //.def("addL1ValueTables",vigra::registerConverters(&addL1ValueTables<double>))
            //.def("addTruncatedL1ValueTables",vigra::registerConverters(&addTruncatedL1ValueTables<float>))
            //.def("addTruncatedL1ValueTables",vigra::registerConverters(&addTruncatedL1ValueTables<double>))
            //.def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<float ,2>)) // 1-order
            //.def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<double,2>))
            //.def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<float ,3>)) // 2-order
            //.def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<double,3>))
            //.def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<float ,4>)) // 3-order
            //.def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<double,4>))
            //.def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<float ,5>)) // 4-order
            //.def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<double,5>))
            //.def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<float ,6>)) // 5-order
            //.def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<double,6>))
            //.def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<float ,7>)) // 6-order
            //.def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<double,7>))
            //// batch add factorS
            //.def("addFactors",vigra::registerConverters(&addUnaryFactors<int64_t>))
            //.def("addFactors",vigra::registerConverters(&addUnaryFactors<int32_t>))
            //.def("addFactors",vigra::registerConverters(&addUnaryFactors<uint64_t>))
            //.def("addFactors",vigra::registerConverters(&addUnaryFactors<uint32_t>))
            //
            //
            //
            //
        ;
    }
} // end namespace inferno::models  
} // end namespace inferno


