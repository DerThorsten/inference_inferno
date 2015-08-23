SET(MOD_SRC 
    loss_functions.cxx
    variation_of_information_general_discrete_model.cxx
    variation_of_information_parametrized_multicut_model.cxx
)

SET(MOD_EXTRA_LIBS "" )



INFERNO_ADD_NUMPY_MODULE(inferno/learning/loss_functions/loss_functions 
                         SOURCES ${MOD_SRC} 
                         LIBRARIES ${MOD_EXTRA_LIBS} )
