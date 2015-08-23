SET(MOD_SRC 
    learning.cxx 
    weights.cxx
)

SET(MOD_EXTRA_LIBS "" )
INFERNO_ADD_NUMPY_MODULE(inferno/learning/learning
                         SOURCES ${MOD_SRC} 
                         LIBRARIES ${MOD_EXTRA_LIBS} )


add_subdirectory(dataset)
add_subdirectory(loss_functions)
add_subdirectory(learners)
