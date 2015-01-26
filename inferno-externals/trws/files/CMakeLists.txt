cmake_minimum_required(VERSION 2.8)


SET(TRWS_SRC_FILES
  minimize.cpp
  MRFEnergy.cpp
  ordering.cpp
  treeProbabilities.cpp
)


#include_directories( ad3 )
#include_directories( Eigen )


ADD_LIBRARY(inferno-external-trws   ${TRWS_SRC_FILES})
ADD_LIBRARY(inferno-external-trws-shared   SHARED ${TRWS_SRC_FILES} )

install (TARGETS inferno-external-trws DESTINATION lib/inferno/externals/)
install (TARGETS inferno-external-trws-shared DESTINATION lib/inferno/externals/)


install(DIRECTORY "."
    DESTINATION include/inferno/externals/trws
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN ".svn" EXCLUDE
)
#
#install(DIRECTORY Eigen
#    DESTINATION include/inferno/externals
#    PATTERN ".svn" EXCLUDE
#)

