
#-------------------------------------------------------------------------------------------------------------------
# unittest
#-------------------------------------------------------------------------------------------------------------------
add_executable(test_factor test_factor.cxx )
target_link_libraries(test_factor ${TEST_LIBS})
add_test(test_factor test_factor)


add_executable(test_model test_model.cxx )
target_link_libraries(test_model ${TEST_LIBS})
add_test(test_model test_model)
