SET(MOD_SRC 
    dataset.cxx 
    vector_dataset_general_discrete_model.cxx
    vector_dataset_parametrized_multicut_model.cxx
    default_dataset_parametrized_multicut_model.cxx
)

SET(MOD_EXTRA_LIBS "" )



INFERNO_ADD_NUMPY_MODULE(inferno/learning/dataset/dataset 
                         SOURCES ${MOD_SRC} 
                         LIBRARIES ${MOD_EXTRA_LIBS} )
