SET(MOD_SRC 
    inference.cxx 
    base_discrete_inference.cxx
)

SET(MOD_EXTRA_LIBS inferno )



INFERNO_ADD_NUMPY_MODULE(inferno/inference/inference SOURCES ${MOD_SRC} LIBRARIES ${MOD_EXTRA_LIBS} )
