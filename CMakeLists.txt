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
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)


include(INFERNO_ADD_NUMPY_MODULE)

project(inferos)
set (INFERNO_VERSION_MAJOR 0)
set (INFERNO_VERSION_MINOR 1)
set (INFERNO_VERSION_PATCH 0)

#-------------------------------------------------------------------------------------------------------------------
# c++ 11
#-------------------------------------------------------------------------------------------------------------------
ADD_DEFINITIONS(-std=c++11)

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

file(GLOB_RECURSE headers ${PROJECT_BINARY_DIR}/tmp_install/include/*.hxx)
include_directories(${PROJECT_BINARY_DIR}/tmp_install/include)

file(GLOB_RECURSE headers ${PROJECT_BINARY_DIR}/tmp_install/include/*.h)
include_directories(${PROJECT_BINARY_DIR}/tmp_install/include)

LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${PROJECT_BINARY_DIR}/tmp_install/lib)

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
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++98")
   endif()
   # PEDANTIC -> a lot of warnings
   if(WARNINGLEVEL STREQUAL "2" )
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++98 -pedantic -Wno-long-long ")
   endif() 
   # VERY PEDANTIC -> very lot of warnings
   if(WARNINGLEVEL STREQUAL "3" )
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++98 -pedantic -Wno-long-long -Wextra")
   endif()
   # SADISTIC -> all warnings become errors
   if(WARNINGLEVEL STREQUAL "4" )
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++98 -pedantic -Wno-long-long -Wextra -Werror")
   endif()
elseif(MSVC)
   add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()




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
# source directory
#-------------------------------------------------------------------------------------------------------------------
add_subdirectory(src)

