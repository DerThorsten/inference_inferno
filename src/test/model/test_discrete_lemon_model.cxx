#define BOOST_TEST_MODULE DiscreteLemonModelTest
#include <boost/test/unit_test.hpp>
#include "inferno_test/test.hxx"

#include <random>
#include <iostream>

#include <vigra/multi_gridgraph.hxx>

#include "inferno/model/discrete_lemon_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/new_unary.hxx"




BOOST_AUTO_TEST_CASE(TestCompile_1)
{
   using namespace inferno;

   typedef vigra::GridGraph<2> GridGraph2;

   typedef typename GridGraph2:: template NodeMap<float> UnaryMap;
   typedef typename GridGraph2:: template EdgeMap<float> FactorMap;
   
   typedef models::DiscreteLemonModel<GridGraph2, UnaryMap, FactorMap> Model;


}