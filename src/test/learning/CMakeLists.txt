add_executable(test_learning test_learning.cxx )
target_link_libraries(test_learning ${TEST_LIBS})
add_test(test_learning test_learning)


IF(WITH_QPBO AND WITH_CPLEX)
    add_executable(test_multicut_learning test_multicut_learning.cxx )
    target_link_libraries(test_multicut_learning 
                          ${TEST_LIBS} 
                          ${INFERNO_EXTERNAL_LIB_QPBO_SHARED} 
                          ${CPLEX_AND_AUX_LIBRARIES})
    add_test(test_multicut_learning test_multicut_learning)
ENDIF()

