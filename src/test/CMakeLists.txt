
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

include_directories(test_include)

#-------------------------------------------------------------------------------------------------------------------
# test inference
#-------------------------------------------------------------------------------------------------------------------
add_subdirectory(model)
add_subdirectory(utilities)
#add_subdirectory(inference)
add_subdirectory(value_tables)
add_subdirectory(learning)
