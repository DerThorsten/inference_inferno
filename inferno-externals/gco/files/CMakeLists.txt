cmake_minimum_required(VERSION 2.8)

SET(GCO_SRC_FILES
  GCoptimization.cpp
  graph.cpp
  LinkedBlockList.cpp
  maxflow.cpp
  instances.inc
)



ADD_LIBRARY(inferno-external-gco   ${GCO_SRC_FILES})
ADD_LIBRARY(inferno-external-gco-shared   SHARED ${GCO_SRC_FILES} )

install (TARGETS inferno-external-gco DESTINATION lib/inferno/externals/)
install (TARGETS inferno-external-gco-shared DESTINATION lib/inferno/externals/)


install(DIRECTORY .
    DESTINATION include/inferno/externals/gco
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN ".svn" EXCLUDE
)


