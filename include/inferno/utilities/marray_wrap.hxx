/** \file marray_warp.hxx never include marray.hxx directly.
    Include this header instead.

    marray_wrap.hxx will enable c++11 features
    in marray via defines.
*/
#ifndef INFERNO_UTILITIES_MARRAY_WRAP_HXX
#define INFERNO_UTILITIES_MARRAY_WRAP_HXX

#define HAVE_CPP11_VARIADIC_TEMPLATES
#define HAVE_CPP11_INITIALIZER_LISTS
#define HAVE_CPP11_TEMPLATE_ALIASES
#include "marray.hxx"

#endif /* INFERNO_UTILITIES_MARRAY_WRAP_HXX */
