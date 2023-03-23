//  (C) Copyright John Maddock 2006.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "setup.hpp"
#include "libs/math/test/test_2F0.hpp"

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

void expected_results()
{
   //
   // Define the max and mean errors expected for
   // various compilers and platforms.
   //
   const char* largest_type;

   add_expected_result(
      ".*",                         // compiler
      ".*",                         // stdlib
      ".*",                         // platform
      ".*",                         // test type(s)
      "Random non-integer a2.*",    // test data group
      ".*", 9000, 3000);            // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
       ".*",                         // test type(s)
      "Integer.*",                   // test data group
      ".*", 300, 100);               // test function
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
       ".*",                         // test type(s)
      "a1 = a2 \\+ 0\\.5",           // test data group
      ".*", 2000, 200);              // test function

   //
   // Finish off by printing out the compiler/stdlib/platform names,
   // we do this to make it easier to mark up expected error rates.
   //
   std::cout << "Tests run with " << BOOST_COMPILER << ", "
      << BOOST_STDLIB << ", " << BOOST_PLATFORM << std::endl;
}

template <class T>
void test(T t, const char* s)
{
   test_spots(t, s);
}

BOOST_AUTO_TEST_CASE( test_main )
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
