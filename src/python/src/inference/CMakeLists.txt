SET(MOD_SRC 
    inference_options.cxx
    inference.cxx 
    visitors.cxx
    general_discrete_model_inference.cxx
    parametrzied_multicut_model_inference.cxx
)
SET(MOD_EXTRA_LIBS inferno )

IF(WITH_QPBO)
    SET(MOD_EXTRA_LIBS ${MOD_EXTRA_LIBS} ${INFERNO_EXTERNAL_LIB_QPBO_SHARED} )
ENDIF()
IF(WITH_CPLEX)
    SET(MOD_EXTRA_LIBS ${MOD_EXTRA_LIBS} ${CPLEX_AND_AUX_LIBRARIES} )
ENDIF()


INFERNO_ADD_NUMPY_MODULE(inferno/inference/inference SOURCES ${MOD_SRC} LIBRARIES ${MOD_EXTRA_LIBS} )
