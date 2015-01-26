cmake_minimum_required(VERSION 2.8)



SET(MAXFLOW_IBFS_SRC_FILES
    ibfs.cpp
)

#include_directories( ad3 )
#include_directories( Eigen )


ADD_LIBRARY(inferno-external-maxflow-ibfs   ${MAXFLOW_IBFS_SRC_FILES})
ADD_LIBRARY(inferno-external-maxflow-ibfs-shared   SHARED ${MAXFLOW_IBFS_SRC_FILES} )

install (TARGETS inferno-external-maxflow-ibfs DESTINATION lib/inferno/externals/)
install (TARGETS inferno-external-maxflow-ibfs-shared DESTINATION lib/inferno/externals/)


install(DIRECTORY ibfs
    DESTINATION include/inferno/externals/ibfs
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN ".svn" EXCLUDE
)
