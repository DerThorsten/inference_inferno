
#-------------------------------------------------------------------------------------------------------------------
# unittest
#-------------------------------------------------------------------------------------------------------------------

find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 

SET(TEST_LIBS
    inferno
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)




add_executable(test_mixed_label test_mixed_label.cxx )
target_link_libraries(test_mixed_label ${TEST_LIBS})
add_test(test_mixed_label test_mixed_label)



add_executable(test_factor test_factor.cxx )
target_link_libraries(test_factor ${TEST_LIBS})
add_test(test_factor test_factor)


add_executable(test_model test_model.cxx )
target_link_libraries(test_model ${TEST_LIBS})
add_test(test_model test_model)
