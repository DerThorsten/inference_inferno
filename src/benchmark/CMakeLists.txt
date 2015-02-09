
#-------------------------------------------------------------------------------------------------------------------
# benchmark
#-------------------------------------------------------------------------------------------------------------------

FIND_PACKAGE(BENCHMARK REQUIRED)
FIND_PACKAGE(Opengm REQUIRED)

MESSAGE("BENCHMARK_INCLUDE_DIR" ${BENCHMARK_INCLUDE_DIR})
include_directories(${BENCHMARK_INCLUDE_DIR})


if(CMAKE_COMPILER_IS_GNUCXX)
    SET(BENCH_LIBS  ${BENCHMARK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
else(CMAKE_COMPILER_IS_GNUCXX)
    SET(BENCH_LIBS  ${BENCHMARK_LIBRARY})
ENDIF(CMAKE_COMPILER_IS_GNUCXX)



add_executable(benchmark benchmark.cxx )
target_link_libraries(benchmark  ${BENCH_LIBS})



add_executable(inferno_vs_opengm inferno_vs_opengm.cxx )
target_link_libraries(inferno_vs_opengm  ${BENCH_LIBS})