
#-------------------------------------------------------------------------------------------------------------------
# unittest
#-------------------------------------------------------------------------------------------------------------------
add_executable(test_movemaker test_movemaker.cxx )
target_link_libraries(test_movemaker ${TEST_LIBS} )
add_test(test_movemaker test_movemaker)


add_executable(test_ideas test_ideas.cxx )
target_link_libraries(test_ideas ${TEST_LIBS} )
add_test(test_ideas test_ideas)
