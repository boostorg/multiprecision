///////////////////////////////////////////////////////////////
//  Copyright 2010 - 2021 Douglas Gregor
//  Copyright 2021 Matt Borland.
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt
//
//  Used to support configuration options depending on standalone context
//  by providing either required support or disabling functionality  

#ifndef BOOST_MP_STANDALONE_CONFIG_HPP
#define BOOST_MP_STANDALONE_CONFIG_HPP

// If any of the most frequently used boost headers are missing assume that standalone mode is supposed to be used
#ifdef __has_include
#if !__has_include(<boost/config.hpp>) || !__has_include(<boost/assert.hpp>) || !__has_include(<boost/lexical_cast.hpp>) || \
    !__has_include(<boost/throw_exception.hpp>) || !__has_include(<boost/predef/other/endian.h>)
#   ifndef BOOST_MP_STANDALONE
#       define BOOST_MP_STANDALONE
#   endif
#endif
#endif

#ifndef BOOST_MP_STANDALONE

#include <boost/config.hpp>
#include <boost/integer.hpp>
#include <boost/integer_traits.hpp>

#else // Standalone mode

// Prevent Macro sub
#ifndef BOOST_PREVENT_MACRO_SUBSTITUTION
#  define BOOST_PREVENT_MACRO_SUBSTITUTION
#endif

// int/float 128 types

#ifdef (__SIZEOF_INT128__)
#  define BOOST_HAS_INT128
#endif

#ifdef (__SIZEOF_FLOAT128__)
#  define BOOST_HAS_FLOAT128
#endif

#if defined(BOOST_HAS_INT128) && defined(__cplusplus)
namespace boost{
#  ifdef __GNUC__
   __extension__ typedef __int128 int128_type;
   __extension__ typedef unsigned __int128 uint128_type;
#  else
   typedef __int128 int128_type;
   typedef unsigned __int128 uint128_type;
#  endif
}
#endif
// same again for __float128:
#if defined(BOOST_HAS_FLOAT128) && defined(__cplusplus)
namespace boost {
#  ifdef __GNUC__
   __extension__ typedef __float128 float128_type;
#  else
   typedef __float128 float128_type;
#  endif
}

#endif

#endif // BOOST_MP_STANDALONE

#endif // BOOST_MP_STANDALONE_CONFIG_HPP
