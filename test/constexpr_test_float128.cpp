//  (C) Copyright John Maddock 2019.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "constexpr_arithmetric_test.hpp"
#include <boost/multiprecision/float128.hpp>
#include <iostream>

int main()
{
   using boost::multiprecision::float128;

   {
      constexpr float128 a(22);
      constexpr float128 b = test_constexpr_add_subtract(a);

      constexpr __float128 f128 = (__float128)b;

      static_assert(f128 == -108.0f);
   }
   {
      constexpr float128 a(22);
      constexpr float128 b = test_constexpr_mul_divide(a);
      static_assert((__float128)b == 90112);
   }
   std::cout << "Done!" << std::endl;
}
