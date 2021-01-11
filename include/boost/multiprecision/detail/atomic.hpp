///////////////////////////////////////////////////////////////////////////////
//  Copyright 2017 John Maddock
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MT_ATOMIC_DETAIL_HPP
#define BOOST_MT_ATOMIC_DETAIL_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_THREADS

#  include <atomic>
#  define BOOST_MATH_ATOMIC_NS std
namespace boost {
   namespace multiprecision {
      namespace detail {
#if ATOMIC_INT_LOCK_FREE == 2
         typedef std::atomic<int> atomic_counter_type;
         typedef std::atomic<unsigned> atomic_unsigned_type;
         typedef int atomic_integer_type;
         typedef unsigned atomic_unsigned_integer_type;
#elif ATOMIC_SHORT_LOCK_FREE == 2
         typedef std::atomic<short> atomic_counter_type;
         typedef std::atomic<unsigned short> atomic_unsigned_type;
         typedef short atomic_integer_type;
         typedef unsigned short atomic_unsigned_integer_type;
#elif ATOMIC_LONG_LOCK_FREE == 2
         typedef std::atomic<long> atomic_unsigned_integer_type;
         typedef std::atomic<unsigned long> atomic_unsigned_type;
         typedef unsigned long atomic_unsigned_integer_type;
         typedef long atomic_integer_type;
#elif ATOMIC_LLONG_LOCK_FREE == 2
         typedef std::atomic<long long> atomic_unsigned_integer_type;
         typedef std::atomic<unsigned long long> atomic_unsigned_type;
         typedef long long atomic_integer_type;
         typedef unsigned long long atomic_unsigned_integer_type;
#else

#define BOOST_MT_NO_ATOMIC_INT

#endif
      }
   }}
#else // BOOST_HAS_THREADS

#define BOOST_MT_NO_ATOMIC_INT

#endif // BOOST_HAS_THREADS

namespace boost { namespace multiprecision { namespace detail {

#ifdef BOOST_MT_NO_ATOMIC_INT
typedef unsigned precision_type;
#else
typedef atomic_unsigned_type precision_type;
#endif

} } }

#endif // BOOST_MATH_ATOMIC_DETAIL_HPP
