
#--------------------------------------------------------------
# vigra
#--------------------------------------------------------------
FIND_PACKAGE(VIGRA REQUIRED)
include_directories(${VIGRA_INCLUDE_DIR})

#--------------------------------------------------------------
# Boost
#--------------------------------------------------------------
find_package(Boost 1.47 REQUIRED)
find_package(Boost COMPONENTS python REQUIRED)

#--------------------------------------------------------------
# Python Lib
#--------------------------------------------------------------
find_package (PythonLibs REQUIRED)
include_directories(
    ${PYTHON_INCLUDE_DIRS}
    ${PYTHON_NUMPY_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${Boost_PYTHON_INCLUDE_DIR}
)





file(GLOB_RECURSE headers py_include/*.hxx)
include_directories(py_include)
include_directories(${headers})


#--------------------------------------------------------------
# Python INSTALL PATH
#--------------------------------------------------------------
FIND_PACKAGE(PythonInterp)
#find default install directory for Python modules (usually PYTHONDIR/Lib/site-packages)
IF(NOT DEFINED INFERNO_PYTHON_MODULE_INSTALL_DIR OR INFERNO_PYTHON_MODULE_INSTALL_DIR MATCHES "^$")
  execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
  "from __future__ import print_function; from distutils.sysconfig import get_python_lib; print(get_python_lib(1))"
  OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
  FILE(TO_CMAKE_PATH ${PYTHON_SITE_PACKAGES} INFERNO_PYTHON_MODULE_INSTALL_DIR)
ENDIF()
SET(INFERNO_PYTHON_MODULE_INSTALL_DIR ${INFERNO_PYTHON_MODULE_INSTALL_DIR}
CACHE PATH "where to install the OpenGM Python package" FORCE)
# this is the install path relative to CMAKE_INSTALL_PREFIX,
# use this in INSTALL() commands to get packaging right
FILE(RELATIVE_PATH INFERNO_PYTHON_MODULE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX} ${INFERNO_PYTHON_MODULE_INSTALL_DIR})


#--------------------------------------------------------------
# Python TEMP Module path
#--------------------------------------------------------------
SET(inferno_python_tmp_dir ${CMAKE_BINARY_DIR}/src/python/module)
SET(inferno_python_src_dir ${CMAKE_SOURCE_DIR}/src/python/module)
file(MAKE_DIRECTORY ${inferno_python_tmp_dir})

#file(COPY ${CMAKE_SOURCE_DIR}/src/python/module DESTINATION ${CMAKE_BINARY_DIR}/src/python/)

add_custom_target(inferno_python ALL)

file(GLOB_RECURSE PYTHON_PY_FILES RELATIVE ${inferno_python_src_dir}/ *.py)




foreach(file ${PYTHON_PY_FILES})
  #... calculate ${i} to get the test name
  #add_test(validate_${i}, "validator", ${file})
  
  get_filename_component(target_folder ${file} DIRECTORY)
  file(MAKE_DIRECTORY ${inferno_python_tmp_dir}/${target_folder})

  MESSAGE("FILE" ${file})
  MESSAGE("PATH" ${inferno_python_src_dir}/${file})
  MESSAGE("COPY TO" ${inferno_python_tmp_dir}/${target_folder})

  ADD_CUSTOM_COMMAND(
   TARGET inferno_python
   POST_BUILD
   COMMAND ${CMAKE_COMMAND} ARGS -E copy ${inferno_python_src_dir}/${file} ${inferno_python_tmp_dir}/${target_folder}
   COMMENT "Copying *.py files to temporary module directory")


endforeach()

#ADD_CUSTOM_COMMAND(
#    TARGET ${TARGET_NAME}
#    POST_BUILD
#    COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${loc} ${target_tmp_path}
#    COMMENT "Copying module to temporary module directory")


#-------------------------------------------------------------------------------------------------------------------
# inferno python src dir
#-------------------------------------------------------------------------------------------------------------------
add_subdirectory(src)
