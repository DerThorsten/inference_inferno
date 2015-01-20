#--------------------------------------------------------------
# Add MODULES  
#--------------------------------------------------------------
#add_library( _core SHARED  core.cxx)




# #--------------------------------------------------------------
# # Link libraries
# #--------------------------------------------------------------
# find_library(RT rt)
# target_link_libraries( _core
#     ${Boost_PYTHON_LIBRARIES}
#     ${VIGRA_IMPEX_LIBRARY}
#     ${VIGRA_NUMPY_CORE_LIBRARY}
#     ${VIGRA_NUMPY_IMPEX_LIBRARY}
# )
# 
# #--------------------------------------------------------------
# # RENAME LIBRARIES
# #--------------------------------------------------------------
# set_target_properties(_core PROPERTIES PREFIX "")
# 
# #--------------------------------------------------------------
# # copy from build to src
# #--------------------------------------------------------------
# get_property(location TARGET _core PROPERTY LOCATION)
# add_custom_command(TARGET _core
#     POST_BUILD        
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different  
#         ${location}
#         ${CMAKE_SOURCE_DIR}/python/python_module/skneuro/utilities
# )
# 
# #--------------------------------------------------------------
# # install
# #--------------------------------------------------------------
# install(TARGETS _core LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES}/skneuro/_core)
# install(FILES ${CMAKE_SOURCE_DIR}/python/python_module/skneuro/utilities/__init__.py DESTINATION ${PYTHON_SITE_PACKAGES}/skneuro/utilities)



SET(MOD_SRC 
    core.cxx 
    factor.cxx 
    small_vector.cxx
)
SET(MOD_EXTRA_LIBS inferno )



INFERNO_ADD_NUMPY_MODULE(inferno/core SOURCES ${MOD_SRC} LIBRARIES ${MOD_EXTRA_LIBS} )
