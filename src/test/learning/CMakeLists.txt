add_executable(test_learning test_learning.cxx )
target_link_libraries(test_learning ${TEST_LIBS})
add_test(test_learning test_learning)
