/** \file inferno.hxx 
    \brief Main header of inferno.
    Any project using inferno should 
    include this header.


    This header has some fundamental typedefs.
    Runtime error classes 
    and useful macros for runtime checks.
    In fact, almost any other header of inferno
    will include this one.
*/
#ifndef INFERNO_INFERNO_KEYWORDS_HXX
#define INFERNO_INFERNO_KEYWORDS_HXX

#include <boost/parameter/name.hpp>
#include <boost/parameter/preprocessor.hpp>


namespace inferno{
namespace keywords{



// graph properties
BOOST_PARAMETER_NAME(planar);

// threads and memory
BOOST_PARAMETER_NAME(saveMemory);
BOOST_PARAMETER_NAME(nThreads);

// epsilon and convergence
BOOST_PARAMETER_NAME(eps);
BOOST_PARAMETER_NAME(convergece);

// norms
BOOST_PARAMETER_NAME(norm);

// heuristics
BOOST_PARAMETER_NAME(heuristic);

//  iterative algorithm parameter
BOOST_PARAMETER_NAME(maxIterations);

// random parameters
BOOST_PARAMETER_NAME(temperature);
BOOST_PARAMETER_NAME(sigma);


// qpbo-ish options
BOOST_PARAMETER_NAME(probing);
BOOST_PARAMETER_NAME(improving);
BOOST_PARAMETER_NAME(strongPersistency);

// message-passing-ish options
BOOST_PARAMETER_NAME(damping);

// cgc
BOOST_PARAMETER_NAME(bookKeeping);

// astar
BOOST_PARAMETER_NAME(heapSize);



}
}

#endif

