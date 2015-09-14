
#-------------------------------------------------------------------------------------------------------------------
# unittest
#-------------------------------------------------------------------------------------------------------------------
# add_executable(test_movemaker test_movemaker.cxx )
# target_link_libraries(test_movemaker ${TEST_LIBS} )
# add_test(test_movemaker test_movemaker)







IF(WITH_CPLEX AND WITH_GUROBI)
    add_executable(test_lp_solver test_lp_solver.cxx )
    target_link_libraries(test_lp_solver ${TEST_LIBS} ${CPLEX_AND_AUX_LIBRARIES} ${GUROBI_LIBRARIES})
    add_test(test_lp_solver test_lp_solver)
ENDIF(WITH_CPLEX AND WITH_GUROBI)
