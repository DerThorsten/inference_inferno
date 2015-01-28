
#-------------------------------------------------------------------------------------------------------------------
# unittest
#-------------------------------------------------------------------------------------------------------------------
add_executable(test_movemaker test_movemaker.cxx )
target_link_libraries(test_movemaker ${TEST_LIBS} )
add_test(test_movemaker test_movemaker)


