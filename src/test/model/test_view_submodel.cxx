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
    const auto nl = 4;
    const auto beta = 0.1;
    const auto submodular = false;

    // create the base model
    typedef GeneralDiscreteGraphicalModel Model;
    typedef FactorsOfVariables<Model> FacOfVars;
    BOOST_TEST_CHECKPOINT("model constructor");
    const Model model = test_models::grid2d(shape, nl, beta, submodular);
    const FacOfVars facOfVars(model);
    BOOST_CHECK_EQUAL(model.maxVarId(),8);
    BOOST_CHECK_EQUAL(model.nVariables(),9);
    BOOST_CHECK_EQUAL(model.nFactors(),21);

    // create the view submodel
    typedef models::ViewSubmodel<Model, FacOfVars> ViewSubmodelType;
    BOOST_TEST_CHECKPOINT("submodel constructor");
    ViewSubmodelType submodel(model, facOfVars);
    INFERNO_CHECK_EMPTY_MODEL(submodel);
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

    // X | 1 | X  
    // -- --- --
    // X | 4 | 5
    // -- --  --
    //   | X | X
    BOOST_TEST_CHECKPOINT("setSubmodelVariables");
    submodel.setSubmodelVariables({1, 4, 5});
    BOOST_CHECK_EQUAL(submodel.nVariables(),3);

    {
        const auto svi = {0, 1, 2};
        const auto snl = {nl, nl, nl};
        INFERNO_CHECK_MODEL_VIS(ViewSubmodelType, submodel, svi);
    }

    // 2 binary factors left
    // 6 new unaries
    // 3 old unaries

    // => 11 factors
    BOOST_CHECK_EQUAL(submodel.nFactors(),11);
    for(const auto fi : submodel.factorIds()){
        const auto factor = submodel[fi];
        const auto arity = factor->arity();
        BOOST_CHECK(arity<=2);
        if(arity==1)
            BOOST_CHECK_EQUAL(factor->size(),nl);
        else{
            BOOST_CHECK_EQUAL(factor->size(),nl*nl);
        }
        //std::cout<<inferno::io::variableIds(factor)<<"\n";
    }

    std::vector<DiscreteLabel> conf(model.nVariables(), 0);
    std::vector<DiscreteLabel> subConf(submodel.nVariables(), 0);

    const ValueType constTerm = submodel.constTerm();
    for(size_t x1=0; x1<nl; ++x1)
    for(size_t x4=0; x4<nl; ++x4)
    for(size_t x5=0; x5<nl; ++x5){

        conf[1] = x1;
        conf[4] = x4;
        conf[5] = x5;

        subConf[submodel.baseViToVi(1)] = x1;
        subConf[submodel.baseViToVi(4)] = x4;
        subConf[submodel.baseViToVi(5)] = x5;


        const auto modelEval = model.eval(conf);
        const auto submodelEval = submodel.eval(subConf);

        BOOST_CHECK_CLOSE(modelEval, submodelEval + constTerm, TEST_EPS);

        //std::cout<<"model "<<modelEval<< "sub "<<submodelEval<<" d "<<modelEval-submodelEval<<"\n";
    }




}
