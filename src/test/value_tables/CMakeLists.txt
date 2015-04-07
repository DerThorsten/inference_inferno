
#-------------------------------------------------------------------------------------------------------------------
# unittest
#-------------------------------------------------------------------------------------------------------------------
add_executable(test_value_tables test_value_tables.cxx )
target_link_libraries(test_value_tables ${TEST_LIBS})
add_test(test_value_tables test_value_tables)

