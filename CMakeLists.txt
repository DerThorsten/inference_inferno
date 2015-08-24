#-------------------------------------------------------------------------------------------------------------------
# default build-type (release)
# (the next lines must be called bevore project(opengm2))
#-------------------------------------------------------------------------------------------------------------------
IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE()
   SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF()

#-------------------------------------------------------------------------------------------------------------------
# Inferno
#-------------------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)
if(CMAKE_MAJOR_VERSION STREQUAL "3")
  cmake_policy(SET CMP0026 OLD)
endif()
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)


include(INFERNO_ADD_NUMPY_MODULE)

project(inferos)
set (INFERNO_VERSION_MAJOR 0)
set (INFERNO_VERSION_MINOR 2)
set (INFERNO_VERSION_PATCH 1)

#-------------------------------------------------------------------------------------------------------------------
# c++ 11
#-------------------------------------------------------------------------------------------------------------------
ADD_DEFINITIONS(-std=c++11)



OPTION(BUILD_BENCHMARK "Build Benchmark" OFF)
OPTION(BUILD_PYTHON_PACKAGE "Build Benchmark" OFF)

#-------------------------------------------------------------------------------------------------------------------
# enable_testing
#-------------------------------------------------------------------------------------------------------------------
enable_testing()



#-------------------------------------------------------------------------------------------------------------------
# global headers
#-------------------------------------------------------------------------------------------------------------------
file(GLOB_RECURSE headers include/*.hxx)
file(GLOB_RECURSE headers ${CMAKE_INSTALL_PREFIX}/include/*.hxx)
include_directories(include)


#-------------------------------------------------------------------------------------------------------------------
# debug info
#-------------------------------------------------------------------------------------------------------------------
#add_definitions(-DTRWS_DEBUG_OUTPUT)

#-------------------------------------------------------------------------------------------------------------------
# warning level
#------------------------------------------------------------------------------------------------------------------- 
SET(WARNINGLEVEL "0" CACHE STRING "selected level for compiler warning from  0 (sloppy) to 4 (sadistic)") 
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
   # no warnings
   if(WARNINGLEVEL STREQUAL "0" )
     message(STATUS "--------------------------------------------------------------------------")
     message(STATUS "WARNING: Compiler warnings are very sloppy -> increase CMake-WARNINGLEVEL")
     message(STATUS "--------------------------------------------------------------------------")
   endif()
   # with warning
   if(WARNINGLEVEL STREQUAL "1" )
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
   endif()
   # PEDANTIC -> a lot of warnings
   if(WARNINGLEVEL STREQUAL "2" )
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wno-long-long ")
   endif() 
   # VERY PEDANTIC -> very lot of warnings
   if(WARNINGLEVEL STREQUAL "3" )
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wno-long-long -Wextra")
   endif()
   # SADISTIC -> all warnings become errors
   if(WARNINGLEVEL STREQUAL "4" )
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wno-long-long -Wextra -Werror")
   endif()
elseif(MSVC)
   add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()

#-------------------------------------------------------------------------------------------------------------------
# Find Externals
#-------------------------------------------------------------------------------------------------------------------

OPTION(WITH_QPBO "Include wrapper for QPBO code" OFF)
OPTION(WITH_TRWS "Include wrapper for TRWS code" OFF)

IF(WITH_QPBO)
    add_definitions(-DWITH_QPBO)
ENDIF(WITH_QPBO)


OPTION(WITH_OPENMP "Use OpenMP " OFF)

FIND_PACKAGE(Threads REQUIRED)  # includes pthread
FIND_PACKAGE(INFERNO_EXTERNALS REQUIRED)

#-------------------------------------------------------------------------------------------------------------------
# Options as (WITH_CPLEX)
#-------------------------------------------------------------------------------------------------------------------
OPTION(WITH_CPLEX "Include CPLEX" OFF)
if(WITH_CPLEX)
    message(STATUS "build with CPLEX interface")
    find_package(CPLEX REQUIRED)
    add_definitions(-DWITH_CPLEX)
    include_directories(${CPLEX_INCLUDE_DIRS})
    # add_definitions(-DILOUSESTL) redundant according to http://www-01.ibm.com/support/docview.wss?uid=swg21399983
    add_definitions(-DIL_STD)

    if(WIN32)
        SET(CPLEX_AND_AUX_LIBRARIES ${CPLEX_LIBRARIES} )
    else()
        SET(CPLEX_AND_AUX_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} ${CPLEX_LIBRARIES} )
    endif()


else()
   message(STATUS "build without CPLEX interface")
endif()

file(GLOB_RECURSE headers ${INFERNO_EXTERNALS_INCLUDE_DIR})
include_directories(${headers})
include_directories(${INFERNO_EXTERNALS_INCLUDE_DIR})
link_directories(${INFERNO_EXTERNALS_LIBRARIES_DIR})

MESSAGE(STATUS "INFERNO_EXTERNALS_LIBRARIES_DIR" ${INFERNO_EXTERNALS_LIBRARIES_DIR})

#INFERNO_EXTERNALS_INCLUDE_DIR

#-------------------------------------------------------------------------------------------------------------------
# OpenMP
#-------------------------------------------------------------------------------------------------------------------
if(WITH_OPENMP)
  FIND_PACKAGE(OpenMP REQUIRED)
  message(STATUS "build with OpenMP") 
  #SET(OPENMP_INCLUDE_DIR "" CACHE STRING "OpenMP include dir")
  #include_directories(${OPENMP_INCLUDE_DIR})
  add_definitions(-DWITH_OPENMP)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
   message(STATUS "build without openMP -> multithreaded options disabled")
endif(WITH_OPENMP)


#-------------------------------------------------------------------------------------------------------------------
# thread libs
#-------------------------------------------------------------------------------------------------------------------
find_package(Threads)
find_package(OpenMP)


#-------------------------------------------------------------------------------------------------------------------
# rt lib
#-------------------------------------------------------------------------------------------------------------------
if(UNIX AND NOT APPLE)
  find_library(RT rt) 
  set(LINK_RT true) 
  message(STATUS "Linking to RT is enabled")
else()
  set(LINK_RT false) 
  message(STATUS "Linking to RT is diabled")
endif()

#-------------------------------------------------------------------------------------------------------------------
# documentation
#-------------------------------------------------------------------------------------------------------------------

option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" OFF)
IF(BUILD_DOCUMENTATION)
 
file(GLOB_RECURSE all_include_hxx_headers include/*.hxx)

  FIND_PACKAGE(Doxygen)
  IF(NOT DOXYGEN_FOUND)
    MESSAGE(FATAL_ERROR
      "Doxygen is needed to build the documentation.")
  ENDIF()
 
  SET( doxyfile_in          ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in     )
  SET( doxyfile             ${PROJECT_BINARY_DIR}/Doxyfile              )
  SET( doxy_html_index_file ${CMAKE_CURRENT_BINARY_DIR}/html/index.html )
  SET( doxy_output_root     ${CMAKE_CURRENT_BINARY_DIR}                 ) # Pasted into Doxyfile.in
  SET( doxy_input           ${PROJECT_SOURCE_DIR}/include               ) # Pasted into Doxyfile.in
  SET( doxy_extra_files     ${CMAKE_CURRENT_SOURCE_DIR}/mainpage.dox    ) # Pasted into Doxyfile.in
 
  CONFIGURE_FILE( ${doxyfile_in} ${doxyfile} @ONLY )
 
  ADD_CUSTOM_COMMAND( OUTPUT ${doxy_html_index_file}
                      COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                      # The following should be ${doxyfile} only but it
                      # will break the dependency.
                      # The optimal solution would be creating a
                      # custom_command for ${doxyfile} generation
                      # but I still have to figure out how...
                      MAIN_DEPENDENCY ${doxyfile} ${doxyfile_in}
                      DEPENDS ${all_include_hxx_headers} ${doxy_extra_files}
                      DEPENDS  ${doxy_extra_files}
                      COMMENT "Generating HTML documentation")
  MESSAGE(STATUS ${headers})
  ADD_CUSTOM_TARGET( doc  DEPENDS ${doxy_html_index_file} ${all_include_hxx_headers})
 
  INSTALL( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc )
ENDIF()



#-------------------------------------------------------------------------------------------------------------------
# source directory
#-------------------------------------------------------------------------------------------------------------------
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TEST "Build test" ON)
add_subdirectory(src)

