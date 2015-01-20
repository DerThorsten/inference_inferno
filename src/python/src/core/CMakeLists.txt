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



SET(MOD_SRC core.cxx factor.cxx)
SET(MOD_EXTRA_LIBS inferno )#${VIGRA_NUMPY_IMPEX_LIBRARY})

macro(add_python_module module_name module_sources module_extra_libs relative_target_dir)
    SET(_module_name _${module_name})

    MESSAGE(STATUS "MODULE NAME DIR: ${_module_name}")


    #add_library(${_module_name} SHARED module_sources ) 
    add_library(${_module_name} SHARED ${${module_sources}} )

    target_link_libraries( ${_module_name}
        ${Boost_PYTHON_LIBRARIES}
        ${VIGRA_IMPEX_LIBRARY}
        ${VIGRA_NUMPY_CORE_LIBRARY}
        ${VIGRA_NUMPY_IMPEX_LIBRARY}
        ${${module_extra_libs}}
    )
    set_target_properties(${_module_name} PROPERTIES PREFIX "")
    MESSAGE(STATUS "TARGET DIR: ${CMAKE_SOURCE_DIR}/src/python/module/${relative_target_dir}")
    get_property(location TARGET ${_module_name} PROPERTY LOCATION)
    add_custom_command(TARGET ${_module_name}
        POST_BUILD        
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  
            ${location}
            ${CMAKE_SOURCE_DIR}/src/python/module/${relative_target_dir}
    )
endmacro()


add_python_module(core MOD_SRC MOD_EXTRA_LIBS "core")