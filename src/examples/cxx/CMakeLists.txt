
#-------------------------------------------------------------------------------------------------------------------
# unittest
#-------------------------------------------------------------------------------------------------------------------
add_executable(very_simple_potts_model very_simple_potts_model.cxx )
add_executable(grid_2d_non_shared_non_submodular grid_2d_non_shared_non_submodular.cxx)

IF(WITH_CPLEX)
    add_executable(very_simple_multicut_model very_simple_multicut_model.cxx )
    target_link_libraries(very_simple_multicut_model  ${CPLEX_AND_AUX_LIBRARIES})
ENDIF(WITH_CPLEX)
