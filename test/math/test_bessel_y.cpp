///////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2002 - 2025.
//  Copyright 2011 - 2025 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt
//
// This work is based on an earlier work:
// "Algorithm 910: A Portable C++ Multiple-Precision System for Special-Function Calculations",
// in ACM TOMS, {VOL 37, ISSUE 4, (February 2011)} (C) ACM, 2011. http://doi.acm.org/10.1145/1916461.1916469

#include "setup.hpp"
#include "table_type.hpp"

#include <boost/math/special_functions/math_fwd.hpp>
#include "test_bessel_y.hpp"

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
       ".*(Y[nv]|y).*Random.*",      // test data group
       ".*", 200000, 10000);         // test function
   add_expected_result(
       ".*",                    // compiler
       ".*",                    // stdlib
       ".*",                    // platform
       ".*cpp_bin_float.*",     // test type(s)
       ".*(Y[nv]|y).*Random.*", // test data group
       ".*", 2000000, 1000000); // test function
   add_expected_result(
       ".*",                        // compiler
       ".*",                        // stdlib
       ".*",                        // platform
       ".*mpfr_float_backend<0>.*", // test type(s)
       ".*(Y[nv]|y).*Random.*",     // test data group
       ".*", 150000, 15000);        // test function
   add_expected_result(
       ".*",                    // compiler
       ".*",                    // stdlib
       ".*",                    // platform
       ".*cpp_double_double.*", // test type(s)
       ".*",                    // test data group
       ".*", 800000, 60000);    // test function
   add_expected_result(
       ".*",                    // compiler
       ".*",                    // stdlib
       ".*",                    // platform
       ".*",                    // test type(s)
       ".*(Y[nv]|y).*Random.*", // test data group
       ".*", 70000, 4000);      // test function
   add_expected_result(
       ".*",                         // compiler
       ".*",                         // stdlib
       ".*",                         // platform
       ".*mpfr_float_backend<18>.*", // test type(s)
       ".*Y0.*",                     // test data group
       ".*", 3000, 2000);            // test function
   add_expected_result(
       ".*",                // compiler
       ".*",                // stdlib
       ".*",                // platform
       ".*cpp_bin_float.*", // test type(s)
       ".*Y0.*",            // test data group
       ".*", 40000, 20000); // test function
   add_expected_result(
       ".*",            // compiler
       ".*",            // stdlib
       ".*",            // platform
       ".*",            // test type(s)
       ".*Y0.*",        // test data group
       ".*", 800, 400); // test function
   add_expected_result(
       ".*",                         // compiler
       ".*",                         // stdlib
       ".*",                         // platform
       ".*mpfr_float_backend<18>.*", // test type(s)
       ".*Yn.*",                     // test data group
       ".*", 400000, 70000);         // test function
   add_expected_result(
       ".*",                  // compiler
       ".*",                  // stdlib
       ".*",                  // platform
       ".*cpp_bin_float.*",   // test type(s)
       ".*Yn.*",              // test data group
       ".*", 400000, 200000); // test function
   add_expected_result(
       ".*",             // compiler
       ".*",             // stdlib
       ".*",             // platform
       ".*",             // test type(s)
       ".*Yn.*",         // test data group
       ".*", 1700, 600); // test function
   add_expected_result(
       ".*",                         // compiler
       ".*",                         // stdlib
       ".*",                         // platform
       ".*mpfr_float_backend<18>.*", // test type(s)
       ".*",                         // test data group
       ".*", 15000, 4000);           // test function
   add_expected_result(
       ".*",                // compiler
       ".*",                // stdlib
       ".*",                // platform
       ".*cpp_bin_float.*", // test type(s)
       ".*",                // test data group
       ".*", 50000, 20000); // test function
   add_expected_result(
       ".*",                // compiler
       ".*",                // stdlib
       ".*",                // platform
       ".*",                // test type(s)
       "Extra.*",           // test data group
       ".*", 6000, 3000);   // test function
   add_expected_result(
       ".*",           // compiler
       ".*",           // stdlib
       ".*",           // platform
       ".*",           // test type(s)
       ".*",           // test data group
       ".*", 150, 60); // test function
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
   test_bessel(t, p);
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
   ALL_SMALL_TESTS
}
