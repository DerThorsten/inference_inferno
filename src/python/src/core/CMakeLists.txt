SET(MOD_SRC 
    core.cxx 
    factor.cxx 
    model.cxx
    small_vector.cxx
)
SET(MOD_EXTRA_LIBS inferno )



INFERNO_ADD_NUMPY_MODULE(inferno/core SOURCES ${MOD_SRC} LIBRARIES ${MOD_EXTRA_LIBS} )
