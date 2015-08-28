SET(MOD_SRC 

    stochastic_gradient_general_discrete_model.cxx
    stochastic_gradient_parametrized_multicut_model.cxx
    subgradient_general_discrete_model.cxx
    subgradient_parametrized_multicut_model.cxx
    learners.cxx
)

SET(MOD_EXTRA_LIBS "" )



INFERNO_ADD_NUMPY_MODULE(inferno/learning/learners/learners 
                         SOURCES ${MOD_SRC} 
                         LIBRARIES ${MOD_EXTRA_LIBS} )
