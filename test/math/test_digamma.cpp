///////////////////////////////////////////////////////////////
//  Copyright 2011 - 2025 John Maddock.
//  Copyright Christopher Kormanyos 2021 - 2025.
//  Distributed under the Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt
//

#include "setup.hpp"
#include "table_type.hpp"
#define TEST_UDT

#include <boost/math/special_functions/math_fwd.hpp>
#include "test_digamma.hpp"

void expected_results()
{
   //
   // Define the max and mean errors expected for
   // various compilers and platforms.
   //
   add_expected_result(
       ".*",                         // compiler
       ".*",                         // stdlib
       ".*",                         // platform
       ".*mpfr_float_backend<18>.*", // test type(s)
       ".*Negative.*",               // test data group
       ".*", 20000, 2000);           // test function
   add_expected_result(
       ".*",                     // compiler
       ".*",                     // stdlib
       ".*",                     // platform
       ".*cpp_double_double.*",  // test type(s)
       ".*Negative.*",           // test data group
       ".*", 650, 50);           // test function
   add_expected_result(
       ".*",           // compiler
       ".*",           // stdlib
       ".*",           // platform
       ".*",           // test type(s)
       ".*Negative.*", // test data group
       ".*", 550, 40); // test function
   add_expected_result(
       ".*",                         // compiler
       ".*",                         // stdlib
       ".*",                         // platform
       ".*cpp_dec_float.*",          // test type(s)
       ".*Near the Positive Root.*", // test data group
       ".*", 2500, 200);             // test function
   add_expected_result(
       ".*",                         // compiler
       ".*",                         // stdlib
       ".*",                         // platform
       ".*mpfr_float_backend<0>.*",  // test type(s)
       ".*Near the Positive Root.*", // test data group
       ".*", 30000, 2000);           // test function
   add_expected_result(
       ".*",                         // compiler
       ".*",                         // stdlib
       ".*",                         // platform
       ".*",                         // test type(s)
       ".*Near the Positive Root.*", // test data group
       ".*", 6000, 1000);            // test function
   add_expected_result(
       ".*",          // compiler
       ".*",          // stdlib
       ".*",          // platform
       ".*",          // test type(s)
       ".*",          // test data group
       ".*", 80, 30); // test function
   //
   // Finish off by printing out the compiler/stdlib/platform names,
   // we do this to make it easier to mark up expected error rates.
   //
   std::cout << "Tests run with " << BOOST_COMPILER << ", "
             << BOOST_STDLIB << ", " << BOOST_PLATFORM << std::endl;
}

template <class T>
void test(T t, const char* p)
{
   test_digamma(t, p);
}

BOOST_AUTO_TEST_CASE(test_main)
{
   using namespace boost::multiprecision;
   expected_results();
   //
   // Test at:
   // 18 decimal digits: tests 80-bit long double approximations
   // 30 decimal digits: tests 128-bit long double approximations
   // 35 decimal digits: tests arbitrary precision code
   //
   ALL_TESTS
}
