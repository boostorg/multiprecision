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
      BOOST_STATIC_CONSTANT(unsigned long long, base = static_cast<unsigned long long>(digits) * 301 + 1000);
      BOOST_STATIC_CONSTANT(unsigned, value = static_cast<unsigned>(base / 1000 + 1));
   };


}}} // namespace boost::multiprecision::detail

#endif
