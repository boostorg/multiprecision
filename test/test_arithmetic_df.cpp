///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock.
//  Copyright 2021 - 2025 Christopher Kormanyos.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#if defined(_MSC_VER)
#define _SCL_SECURE_NO_WARNINGS
#endif

#include <boost/multiprecision/cpp_double_fp.hpp>

#include "test_arithmetic.hpp"

void test_constexpr_ness()
{
   using boost::multiprecision::cpp_double_double;

   constexpr cpp_double_double one { 1 };

   static_assert(static_cast<int>(one) == 1, "Error: constexpr cpp_double_double construction failed");

   BOOST_CHECK(static_cast<int>(one) == 1);

   constexpr cpp_double_double dd_a { 1.23 };
   constexpr cpp_double_double dd_b { 4.56 };
   constexpr cpp_double_double dd_c { dd_a * dd_b };

   constexpr cpp_double_double dd_s { sqrt(dd_a) };
   constexpr cpp_double_double sqrt_hundred { sqrt(cpp_double_double(100)) };
   constexpr cpp_double_double log_hundred { log(cpp_double_double(100)) };

   static_assert(dd_c > 5, "Error in constexpr multiplication");
   static_assert(dd_s > 1, "Error in constexpr square root");
   static_assert(sqrt_hundred == 10, "Error in constexpr square root");
   static_assert(log_hundred > 4.605, "Error in constexpr logarithm");

   BOOST_CHECK(dd_c > 5);
   BOOST_CHECK(dd_s > 1);
   BOOST_CHECK(sqrt_hundred == 10);
   BOOST_CHECK(log_hundred > 4.605);
}

int main()
{
   ::test_constexpr_ness();

   using boost::multiprecision::cpp_double_float;
   using boost::multiprecision::cpp_double_double;
   using boost::multiprecision::cpp_double_long_double;
#ifdef BOOST_HAS_FLOAT128
   using boost::multiprecision::cpp_double_float128;
#endif

   test<cpp_double_float>();
   test<cpp_double_double>();
   test<cpp_double_long_double>();
#ifdef BOOST_HAS_FLOAT128
   test<cpp_double_float128>();
#endif
   return boost::report_errors();
}
