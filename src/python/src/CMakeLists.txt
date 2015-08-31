#-------------------------------------------------------------------------------------------------------------------
# inferno python  core module
#-------------------------------------------------------------------------------------------------------------------
add_subdirectory(core)

#-------------------------------------------------------------------------------------------------------------------
# inferno python  models module
#-------------------------------------------------------------------------------------------------------------------
add_subdirectory(models)

#-------------------------------------------------------------------------------------------------------------------
# inferno python  value_tables module
#-------------------------------------------------------------------------------------------------------------------
add_subdirectory(value_tables)


#-------------------------------------------------------------------------------------------------------------------
# inferno python  inference module
#-------------------------------------------------------------------------------------------------------------------
add_subdirectory(inference)


#-------------------------------------------------------------------------------------------------------------------
# inferno python  bechmark module
#-------------------------------------------------------------------------------------------------------------------
#add_subdirectory(benchmark)


#-------------------------------------------------------------------------------------------------------------------
# inferno python  learning module
#-------------------------------------------------------------------------------------------------------------------
add_subdirectory(learning)



add_custom_target(inferno-python)
add_dependencies(inferno-python inferno_python_core )
add_dependencies(inferno-python inferno_python_inference )
add_dependencies(inferno-python inferno_python_models )
add_dependencies(inferno-python inferno_python_learning )
add_dependencies(inferno-python inferno_python_dataset )
add_dependencies(inferno-python inferno_python_learners )
add_dependencies(inferno-python inferno_python_loss_functions )
