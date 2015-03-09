SET(MOD_SRC 
    models.cxx 
    general_discrete_model.cxx
    general_discrete_tl_model.cxx
    modified_multiwaycut_model.cxx
)

SET(MOD_EXTRA_LIBS inferno )


INFERNO_ADD_NUMPY_MODULE(inferno/models/models 
    SOURCES ${MOD_SRC} 
    LIBRARIES ${MOD_EXTRA_LIBS} )
