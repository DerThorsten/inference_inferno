#define BOOST_TEST_MODULE ViewSubModelTest

#include <boost/test/unit_test.hpp>
#include "inferno_test/test.hxx"

#include "inferno/inferno.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/model/factors_of_variables.hxx"
#include "inferno/model/view_submodel.hxx"





BOOST_AUTO_TEST_CASE(TestViewSubmodel)
{
    using namespace inferno;

    // create the base model
    const auto shape = {3, 3};
    const auto nl = 5;
    const auto beta = 0.1;
    const auto submodular = false;

    // create the base model
    typedef GeneralDiscreteGraphicalModel Model;
    typedef FactorsOfVariables<Model> FacOfVars;
    const Model model = test_models::grid2d(shape, nl, beta, submodular);
    const FacOfVars facOfVars(model);

    // create the view submodel
    typedef models::ViewSubmodel<Model, FacOfVars> ViewSubmodelType;
    BOOST_TEST_CHECKPOINT("submodel constructor");
    ViewSubmodelType submodel(model, facOfVars);

    BOOST_TEST_CHECKPOINT("check nVariables");
    BOOST_CHECK_EQUAL(submodel.nVariables(),0);
    //BOOST_CHECK_EQUAL(submodel.nFactors(),0);
    // 0 | 1 | 2 
    // -- --- --
    // 3 | 4 | 5
    // -- --  --
    // 6 | 7 | 8

    BOOST_TEST_CHECKPOINT("get labels");
    // set the states of the base model
    auto & labels = submodel.baseModelLabelMap();

    BOOST_TEST_CHECKPOINT("fill labels");
    for(const auto vi : model.variableIds())
        labels[vi] = 0;

    //   | 1 |   
    // -- --- --
    //   | 4 | 5
    // -- --  --
    //   |   |  
    BOOST_TEST_CHECKPOINT("setSubmodelVariables");
    submodel.setSubmodelVariables({1, 4, 5});
    BOOST_CHECK_EQUAL(submodel.nVariables(),3);


}
