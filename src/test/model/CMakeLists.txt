
#-------------------------------------------------------------------------------------------------------------------
# unittest
#-------------------------------------------------------------------------------------------------------------------
add_executable(test_factor test_factor.cxx )
target_link_libraries(test_factor ${TEST_LIBS})
add_test(test_factor test_factor)


add_executable(test_model test_model.cxx )
target_link_libraries(test_model ${TEST_LIBS})
add_test(test_model test_model)


add_executable(test_sparse_model test_sparse_model.cxx )
target_link_libraries(test_sparse_model ${TEST_LIBS})
add_test(test_sparse_model test_sparse_model)





add_executable(test_view_submodel test_view_submodel.cxx )
target_link_libraries(test_view_submodel ${TEST_LIBS})
add_test(test_view_submodel test_view_submodel)