///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MP_CPP_INT_CORE_HPP
#define BOOST_MP_CPP_INT_CORE_HPP

namespace boost{ namespace multiprecision{

typedef boost::uint32_t limb_type;
typedef boost::int32_t signed_limb_type;
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

