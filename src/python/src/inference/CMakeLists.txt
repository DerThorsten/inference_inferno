SET(MOD_SRC 
    inference.cxx 
    visitors.cxx
    base_discrete_inference.cxx
)
SET(MOD_EXTRA_LIBS inferno )

IF(WITH_QPBO)
    SET(MOD_EXTRA_LIBS ${MOD_EXTRA_LIBS} ${INFERNO_EXTERNAL_LIB_QPBO_SHARED} )
ENDIF()

INFERNO_ADD_NUMPY_MODULE(inferno/inference/inference SOURCES ${MOD_SRC} LIBRARIES ${MOD_EXTRA_LIBS} )
