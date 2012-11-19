//  Copyright John Maddock 2008.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <gmp.h>

#ifdef __GNUC__
#pragma message #__GNU_MP_VERSION
#pragma message #__GNU_MP_VERSION_MINOR
#endif 

#if (__GNU_MP_VERSION < 4) || ((__GNU_MP_VERSION == 4) && (__GNU_MP_VERSION_MINOR < 2))
#error "Incompatible GMP version"
#endif




