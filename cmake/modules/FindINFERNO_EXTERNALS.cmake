


SET(TO_FIND "")
set(INFERNO_EXTERNALS_ROOT_DIR "" CACHE PATH "INFERNO EXTERNALS root dir (install path of externals).")

# include path of externals
FIND_PATH(INFERNO_EXTERNALS_INCLUDE_DIR
  inferno_externals/inferno_externals_path.hxx
  HINTS ${INFERNO_EXTERNALS_ROOT_DIR}/include
        /home/tbeier/inst/include
        ${HOME}/include
  PATHS ENV C_INCLUDE_PATH
        ENV C_PLUS_INCLUDE_PATH
        ENV INCLUDE_PATH
)


IF(WITH_QPBO)
    FIND_LIBRARY(INFERNO_EXTERNAL_LIB_QPBO_SHARED
        NAMES 
             inferno-external-qpbo-shared libinferno-external-qpbo-shared 
        HINTS 
            ${INFERNO_EXTERNALS_ROOT_DIR}/lib/inferno_externals
            /home/tbeier/inst/lib/inferno_externals        
        PATHS 
            ENV LIBRARY_PATH                       #unix
            ENV LIBRARY_PATH/inferno_externals     #unix
            ENV LD_LIBRARY_PATH                    #unix
            ENV LD_LIBRARY_PATH/inferno_externals  #unix
    )
    SET(TO_FIND ${TO_FIND} INFERNO_EXTERNAL_LIB_QPBO_SHARED)
ENDIF(WITH_QPBO)




INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(INFERNO_EXTERNALS DEFAULT_MSG  INFERNO_EXTERNALS_INCLUDE_DIR  ${TO_FIND})

MARK_AS_ADVANCED( INFERNO_EXTERNALS_INCLUDE_DIR  ${TO_FIND})
