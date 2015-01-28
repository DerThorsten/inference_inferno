
#-------------------------------------------------------------------------------------------------------------------
# unittest
#-------------------------------------------------------------------------------------------------------------------
add_subdirectory(utilities)




add_executable(test_externals_qpbo test_externals_qpbo.cxx )
target_link_libraries(test_externals_qpbo ${TEST_LIBS} ${INFERNO_EXTERNAL_LIB_QPBO_SHARED})
add_test(test_externals_qpbo test_externals_qpbo)



IF(WITH_CPLEX)
    add_executable(test_multicut test_multicut.cxx )
    target_link_libraries(test_multicut ${TEST_LIBS} ${CPLEX_AND_AUX_LIBRARIES})
    add_test(test_multicut test_multicut)
ENDIF(WITH_CPLEX)