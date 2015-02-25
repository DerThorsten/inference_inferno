SET(MOD_SRC 
    benchmarkCore.cxx 
    bench_potts.cxx
)
SET(MOD_EXTRA_LIBS inferno )



INFERNO_ADD_NUMPY_MODULE(inferno/benchmark/benchmarkCore SOURCES ${MOD_SRC} LIBRARIES ${MOD_EXTRA_LIBS} )
