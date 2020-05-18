///////////////////////////////////////////////////////////////
//  Copyright 2020 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_MP_INTEL_INTRINSICS_HPP
#define BOOST_MP_INTEL_INTRINSICS_HPP
//
// Select which actual implementation header to use:
//
#ifdef __has_include
#if __has_include(<immintrin.h>)
#define BOOST_MP_HAS_IMMINTRIN_H
#endif
#endif
//
// If this is GCC/clang, then check that the actual intrinsic exists:
//
#if defined(__has_builtin) && defined(__GNUC__)
#if !__has_builtin(__builtin_ia32_addcarryx_u64) && defined(BOOST_MP_HAS_IMMINTRIN_H)
#undef BOOST_MP_HAS_IMMINTRIN_H
#endif
#elif defined(BOOST_MP_HAS_IMMINTRIN_H) && defined(__GNUC__) && !(defined(BOOST_GCC) && (__GNUC__ >= 9))
#undef BOOST_MP_HAS_IMMINTRIN_H
#endif

#if defined(__clang__) && (__clang__ < 9)
// We appear to crash the compiler if we try to use these intrinsics?
#undef BOOST_MP_HAS_IMMINTRIN_H
#endif

#ifdef BOOST_MP_HAS_IMMINTRIN_H

#include <immintrin.h>

#if defined(BOOST_HAS_INT128)

namespace boost { namespace multiprecision { namespace detail {

BOOST_MP_FORCEINLINE unsigned char addcarry_limb(unsigned char carry, limb_type a, limb_type b, limb_type* p_result)
{
#ifdef BOOST_INTEL
   typedef unsigned __int64 cast_type;
#else
   typedef unsigned long long cast_type;
#endif
   return _addcarry_u64(carry, a, b, reinterpret_cast<cast_type*>(p_result));
}

BOOST_MP_FORCEINLINE unsigned char subborrow_limb(unsigned char carry, limb_type a, limb_type b, limb_type* p_result)
{
#ifdef BOOST_INTEL
   typedef unsigned __int64 cast_type;
#else
   typedef unsigned long long cast_type;
#endif
   return _subborrow_u64(carry, a, b, reinterpret_cast<cast_type*>(p_result));
}

}}} // namespace boost::multiprecision::detail

#else

namespace boost { namespace multiprecision { namespace detail {

BOOST_MP_FORCEINLINE unsigned char addcarry_limb(unsigned char carry, limb_type a, limb_type b, limb_type* p_result)
{
   return _addcarry_u32(carry, a, b, reinterpret_cast<unsigned int*>(p_result));
}

BOOST_MP_FORCEINLINE unsigned char subborrow_limb(unsigned char carry, limb_type a, limb_type b, limb_type* p_result)
{
   return _subborrow_u32(carry, a, b, reinterpret_cast<unsigned int*>(p_result));
}

}}} // namespace boost::multiprecision::detail

#endif

#endif

#endif
