#ifndef INFERNO_UTILITIES_PARALLEL_PARALLEL_HXX
#define INFERNO_UTILITIES_PARALLEL_PARALLEL_HXX

#include <cstdint>

namespace inferno{
namespace utilities{

    enum class Concurrency : int64_t{
        OpenMp = 0,
        StdThreads = 1
    };
}
}

#endif /* INFERNO_UTILITIES_PARALLEL_PARALLEL_HXX */