
#-------------------------------------------------------------------------------------------------------------------
# unittest
#-------------------------------------------------------------------------------------------------------------------
add_executable(test_mixed_label test_mixed_label.cxx )
target_link_libraries(test_mixed_label ${TEST_LIBS})
add_test(test_mixed_label test_mixed_label)

add_executable(test_shape_walker test_shape_walker.cxx )
target_link_libraries(test_shape_walker ${TEST_LIBS})
add_test(test_shape_walker test_shape_walker)