SET(MOD_SRC 
    value_tables.cxx 
    base_discrete_value_table.cxx
)

SET(MOD_EXTRA_LIBS inferno )



INFERNO_ADD_NUMPY_MODULE(inferno/value_tables/value_tables SOURCES ${MOD_SRC} LIBRARIES ${MOD_EXTRA_LIBS} )
