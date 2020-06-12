///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_MATH_MAX_DIGITS10_HPP
#define BOOST_MATH_MAX_DIGITS10_HPP

namespace boost {
namespace multiprecision {
namespace detail {

   template <unsigned digits>
   struct calc_max_digits10
   {
#ifndef BOOST_NO_CXX11_CONSTEXPR
      static constexpr unsigned constexpr_ceil(double d)
      {
         return static_cast<unsigned>(d) == d ? static_cast<unsigned>(d) : static_cast<unsigned>(d) + 1;
      }

      static constexpr unsigned max_digits_10(unsigned d)
      {
         return constexpr_ceil(0.301029995663981195213738894724493026768189881462108541310 * d + 1);
      }
      static const unsigned value = max_digits_10(digits);
#else
      //
      // This version works up to about 15K binary digits, then starts sporadically failing
      // and giving values that are 1 too small.
      //
      BOOST_STATIC_CONSTANT(unsigned, value = 2 + ((static_cast<boost::uint64_t>(digits) * static_cast<boost::uint64_t>(1292913986)) >> 32));
#endif
   };


}}} // namespace boost::multiprecision::detail

#endif
