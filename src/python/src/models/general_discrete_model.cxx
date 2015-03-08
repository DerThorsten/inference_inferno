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
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/explicit.hxx"

namespace inferno{
namespace models{

    namespace bp = boost::python;

    template<class T>
    vigra::TinyVector<int, 2> 
    addPottsValueTables(GeneralDiscreteGraphicalModel & model,  
                        const DiscreteLabel nl,
                        const vigra::NumpyArray<1, T>  & betas
    ){
        Vti start = model.nValueTables();
        for(size_t i=0; i<betas.shape(0); ++i){
            model.addValueTable(new value_tables::PottsValueTable(nl, betas(i)));
        }
        return vigra::TinyVector<int, 2>(start, model.nValueTables());
    }

    template<class T>
    vigra::TinyVector<int, 2> 
    addL1ValueTables(GeneralDiscreteGraphicalModel & model,  
                        const DiscreteLabel nl,
                        const vigra::NumpyArray<1, T>  & betas
    ){
        Vti start = model.nValueTables();
        for(size_t i=0; i<betas.shape(0); ++i){
            model.addValueTable(new value_tables::L1ValueTable(nl, betas(i)));
        }
        return vigra::TinyVector<int, 2>(start, model.nValueTables());
    }

    template<class T>
    vigra::TinyVector<int, 2> 
    addTruncatedL1ValueTables(GeneralDiscreteGraphicalModel & model,  
                        const DiscreteLabel nl,
                        const vigra::NumpyArray<1, T>  & betas,
                        const vigra::NumpyArray<1, T>  & truncateAts
    ){
        Vti start = model.nValueTables();
        for(size_t i=0; i<betas.shape(0); ++i){
            model.addValueTable(new value_tables::TruncatedL1ValueTable(nl, betas(i), truncateAts(i)));
        }
        return vigra::TinyVector<int, 2>(start, model.nValueTables());
    }


    template<class T, unsigned int A>
    vigra::TinyVector<int, 2> 
    addExplicitValueTables(GeneralDiscreteGraphicalModel & model,  
                          const vigra::NumpyArray<A, T>  & values  
    ){
        Vti start = model.nValueTables();

        const auto firstF = values.bindInner(0);
        ValueMarray val(firstF.shape().begin(), firstF.shape().end());

        for(size_t i=0; i<values.shape(0); ++i){
            const auto f = values.bindInner(i);
            std::copy(f.begin(), f.end(), val.begin());
            model.addValueTable(new value_tables::Explicit(val));
        }
        return vigra::TinyVector<int, 2>(start, model.nValueTables());
    }



    template<class T>
    bp::tuple
    addFactors(GeneralDiscreteGraphicalModel & model, 
               const vigra::TinyVector<int, 2> & vtiRange,
               const vigra::NumpyArray<2, T> & vis
    ){
        INFERNO_CHECK_OP(vis.shape(0), == ,vtiRange[1]-vtiRange[0],"vis shape does not match vtiRange");
        Vti start = model.nFactors();
        uint64_t i = 0;
        for(Vti vti = vtiRange[0]; vti < vtiRange[1]; ++vti,++i){
            const auto fVis = vis.bindInner(i);
            model.addFactor(vti,fVis.begin(), fVis.end());
        }
        return bp::make_tuple(start, model.nFactors());
    }

    template<class T>
    bp::tuple
    addUnaryFactors(GeneralDiscreteGraphicalModel & model, 
               const vigra::TinyVector<int, 2> & vtiRange,
               const vigra::NumpyArray<1, T> & vis
    ){
        INFERNO_CHECK_OP(vis.shape(0), == ,vtiRange[1]-vtiRange[0],"vis shape does not match vtiRange");
        Vti start = model.nFactors();
        uint64_t i = 0;
        for(Vti vti = vtiRange[0]; vti < vtiRange[1]; ++vti,++i){
            const Vi vi = vis(i);
            model.addFactor(vti,&vi, &vi+1);
        }
        return bp::make_tuple(start, model.nFactors());
    }




    void exportGeneralDiscreteGraphicalModel(){
        typedef GeneralDiscreteGraphicalModel Model;
        bp::class_<Model>("GeneralDiscreteGraphicalModel",bp::init<Vi,DiscreteLabel>())
            .def(export_helper::ExportModelAPI<Model>("GeneralDiscreteGraphicalModel"))

            // batch add potts value tableS
            .def("addPottsValueTables",vigra::registerConverters(&addPottsValueTables<float>))
            .def("addPottsValueTables",vigra::registerConverters(&addPottsValueTables<double>))

            .def("addL1ValueTables",vigra::registerConverters(&addL1ValueTables<float>))
            .def("addL1ValueTables",vigra::registerConverters(&addL1ValueTables<double>))

            .def("addTruncatedL1ValueTables",vigra::registerConverters(&addTruncatedL1ValueTables<float>))
            .def("addTruncatedL1ValueTables",vigra::registerConverters(&addTruncatedL1ValueTables<double>))

            .def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<float ,2>)) // 1-order
            .def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<double,2>))
            .def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<float ,3>)) // 2-order
            .def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<double,3>))
            .def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<float ,4>)) // 3-order
            .def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<double,4>))
            .def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<float ,5>)) // 4-order
            .def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<double,5>))
            .def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<float ,6>)) // 5-order
            .def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<double,6>))
            .def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<float ,7>)) // 6-order
            .def("addExplicitValueTables",vigra::registerConverters(&addExplicitValueTables<double,7>))

            // batch add factorS
            .def("addFactors",vigra::registerConverters(&addUnaryFactors<int64_t>))
            .def("addFactors",vigra::registerConverters(&addUnaryFactors<int32_t>))
            .def("addFactors",vigra::registerConverters(&addUnaryFactors<uint64_t>))
            .def("addFactors",vigra::registerConverters(&addUnaryFactors<uint32_t>))

            .def("addFactors",vigra::registerConverters(&addFactors<int64_t>))
            .def("addFactors",vigra::registerConverters(&addFactors<int32_t>))
            .def("addFactors",vigra::registerConverters(&addFactors<uint64_t>))
            .def("addFactors",vigra::registerConverters(&addFactors<uint32_t>))
        ;
    }
} // end namespace inferno::models  
} // end namespace inferno


