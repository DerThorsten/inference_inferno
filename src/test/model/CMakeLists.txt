
#-------------------------------------------------------------------------------------------------------------------
# unittest
#-------------------------------------------------------------------------------------------------------------------
add_executable(test_factor test_factor.cxx )
target_link_libraries(test_factor ${TEST_LIBS})
add_test(test_factor test_factor)


add_executable(test_model test_model.cxx )
target_link_libraries(test_model ${TEST_LIBS})
add_test(test_model test_model)

add_executable(test_discrete_lemon_model test_discrete_lemon_model.cxx )
target_link_libraries(test_discrete_lemon_model ${TEST_LIBS})
add_test(test_discrete_lemon_model test_discrete_lemon_model)


add_executable(test_parametrized_multicut_model test_parametrized_multicut_model.cxx )
target_link_libraries(test_parametrized_multicut_model ${TEST_LIBS})
add_test(test_parametrized_multicut_model test_parametrized_multicut_model)

#add_executable(test_mmwc test_mmwc.cxx )
#target_link_libraries(test_mmwc ${TEST_LIBS})
#add_test(test_mmwc test_mmwc)


#add_executable(test_sparse_model test_sparse_model.cxx )
#target_link_libraries(test_sparse_model ${TEST_LIBS})
#add_test(test_sparse_model test_sparse_model)

#add_executable(test_view_submodel test_view_submodel.cxx )
#target_link_libraries(test_view_submodel ${TEST_LIBS})
#add_test(test_view_submodel test_view_submodel)

#add_executable(test_tl_model test_tl_model.cxx )
#target_link_libraries(test_tl_model ${TEST_LIBS})
#add_test(test_tl_model test_tl_model)

#add_executable(test_tl_model2 test_tl_model2.cxx )
#target_link_libraries(test_tl_model2 ${TEST_LIBS})
#add_test(test_tl_model2 test_tl_model2)
