///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MP_CPP_INT_CORE_HPP
#define BOOST_MP_CPP_INT_CORE_HPP

#include <boost/integer.hpp>

namespace boost{ namespace multiprecision{

namespace detail{

//
// These traits calculate the largest type in the list
// [unsigned] ong long, long, int, which has the specified number
// of bits.  Note that intN_t and boost::int_t<N> find the first
// member of the above list, not the last.  We want the last in the 
// list to ensure that mixed arithmetic operations are as efficient 
// as possible.
//
template <unsigned N>
struct largest_signed_type
{
   typedef typename mpl::if_c<
      1 + std::numeric_limits<long long>::digits == N,
      long long,
      typename mpl::if_c<
         1 + std::numeric_limits<long>::digits == N,
         long,
         typename mpl::if_c<
            1 + std::numeric_limits<int>::digits == N,
            int,
            typename boost::int_t<N>::exact
         >::type
      >::type
   >::type type;
};

template <unsigned N>
struct largest_unsigned_type
{
   typedef typename mpl::if_c<
      std::numeric_limits<unsigned long long>::digits == N,
      unsigned long long,
      typename mpl::if_c<
         std::numeric_limits<unsigned long>::digits == N,
         unsigned long,
         typename mpl::if_c<
            std::numeric_limits<unsigned int>::digits == N,
            unsigned int,
            typename boost::uint_t<N>::exact
         >::type
      >::type
   >::type type;
};

}

#if defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))) \
   && !defined(BOOST_INTEL) && defined(__WORDSIZE) && (__WORDSIZE == 64)

typedef detail::largest_unsigned_type<64>::type limb_type;
typedef detail::largest_signed_type<64>::type signed_limb_type;
typedef unsigned __int128 double_limb_type;
typedef __int128 signed_double_limb_type;
static const limb_type max_block_10 = 1000000000000000000uLL;
static const limb_type digits_per_block_10 = 18;

inline limb_type block_multiplier(unsigned count)
{
   static const limb_type values[digits_per_block_10] 
      = { 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000, 1000000000000, 10000000000000, 100000000000000, 1000000000000000, 10000000000000000, 100000000000000000, 1000000000000000000 };
   BOOST_ASSERT(count < digits_per_block_10);
   return values[count];
}

// Can't do formatted IO on an __int128
#define BOOST_MP_NO_DOUBLE_LIMB_TYPE_IO

#else

typedef detail::largest_unsigned_type<32>::type limb_type;
typedef detail::largest_signed_type<32>::type signed_limb_type;
typedef boost::uint64_t double_limb_type;
typedef boost::int64_t signed_double_limb_type;
static const limb_type max_block_10 = 1000000000;
static const limb_type digits_per_block_10 = 9;

inline limb_type block_multiplier(unsigned count)
{
   static const limb_type values[digits_per_block_10] 
      = { 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
   BOOST_ASSERT(count < digits_per_block_10);
   return values[count];
}

#endif

template <class T>
inline void minmax(const T& a, const T& b, T& aa, T& bb)
{
   if(a < b)
   {
      aa = a;
      bb = b;
   }
   else
   {
      aa = b;
      bb = a;
   }
}

}}

#endif // BOOST_MP_CPP_INT_CORE_HPP

