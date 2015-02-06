#define BOOST_TEST_MODULE ViewSubModelTest

#include <boost/test/unit_test.hpp>
#include "inferno_test/test.hxx"

#include "inferno/inferno.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/model/view_submodel.hxx"





BOOST_AUTO_TEST_CASE(TestViewSubmodel)
{
    using namespace inferno;

    // create the base model
    const auto shape = {20, 20};
    const auto nl = 5;
    const auto beta = 0.1;
    const auto submodular = false;
    const auto baseModel = test_models::grid2d(shape, nl, beta, submodular);

    // create the view submodel
    typedef models::ViewSubmodel<GeneralDiscreteGraphicalModel> ViewSubmodelType;
    ViewSubmodelType submodel(baseModel);


}
