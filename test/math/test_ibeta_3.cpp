///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifdef _MSC_VER
#  define _SCL_SECURE_NO_WARNINGS
#endif

#define BOOST_MATH_OVERFLOW_ERROR_POLICY ignore_error
#define BOOST_MATH_SMALL_CONSTANT(x) x

#if !defined(TEST_MPF_50) && !defined(TEST_BACKEND) && !defined(TEST_CPP_FLOAT) && !defined(TEST_MPFR_50)
#  define TEST_MPF_50
#  define TEST_MPFR_50
#  define TEST_CPP_FLOAT

#ifdef _MSC_VER
#pragma message("CAUTION!!: No backend type specified so testing everything.... this will take some time!!")
#endif
#ifdef __GNUC__
#pragma warning "CAUTION!!: No backend type specified so testing everything.... this will take some time!!"
#endif

#endif

#if defined(TEST_MPF_50)
#include <boost/multiprecision/gmp.hpp>
#endif
#if defined(TEST_MPFR_50)
#include <boost/multiprecision/mpfr.hpp>
#endif
#ifdef TEST_BACKEND
#include <boost/multiprecision/concepts/mp_number_architypes.hpp>
#endif
#ifdef TEST_CPP_FLOAT
#include <boost/multiprecision/cpp_float.hpp>
#endif

#define SC_(x) T(BOOST_STRINGIZE(x))
#define TEST_UDT

#define TEST_DATA 3

#include "libs/math/test/test_ibeta.hpp"

void expected_results()
{
   //
   // Define the max and mean errors expected for
   // various compilers and platforms.
   //
   add_expected_result(
      "[^|]*",                          // compiler
      "[^|]*",                          // stdlib
      "[^|]*",                          // platform
      ".*",                             // test type(s)
      "(?i).*small.*",                  // test data group
      ".*", 90, 25);  // test function
   add_expected_result(
      "[^|]*",                          // compiler
      "[^|]*",                          // stdlib
      "[^|]*",                          // platform
      ".*",                             // test type(s)
      "(?i).*medium.*",                 // test data group
      ".*", 200, 50);  // test function
   add_expected_result(
      "[^|]*",                          // compiler
      "[^|]*",                          // stdlib
      "[^|]*",                          // platform
      ".*",                             // test type(s)
      "(?i).*large.*",                  // test data group
      ".*", 6000000, 500000);           // test function
   //
   // Finish off by printing out the compiler/stdlib/platform names,
   // we do this to make it easier to mark up expected error rates.
   //
   std::cout << "Tests run with " << BOOST_COMPILER << ", " 
      << BOOST_STDLIB << ", " << BOOST_PLATFORM << std::endl;
}


int test_main(int, char* [])
{
   using namespace boost::multiprecision;
   expected_results();
   //
   // Test at:
   // 18 decimal digits: tests 80-bit long double approximations
   // 30 decimal digits: tests 128-bit long double approximations
   // 35 decimal digits: tests arbitrary precision code
   //
#ifdef TEST_MPF_50
   test_beta(mp_number<gmp_float<18> >(), "mp_number<gmp_float<18> >");
   test_beta(mp_number<gmp_float<30> >(), "mp_number<gmp_float<30> >");
   test_beta(mp_number<gmp_float<35> >(), "mp_number<gmp_float<35> >");
#endif
#ifdef TEST_MPFR_50
   test_beta(mp_number<mpfr_float_backend<18> >(), "mp_number<mpfr_float_backend<18> >");
   test_beta(mp_number<mpfr_float_backend<30> >(), "mp_number<mpfr_float_backend<30> >");
   test_beta(mp_number<mpfr_float_backend<35> >(), "mp_number<mpfr_float_backend<35> >");
#endif
#ifdef TEST_CPP_FLOAT
   test_beta(mp_number<cpp_float<18> >(), "mp_number<cpp_float<18> >");
   test_beta(mp_number<cpp_float<30> >(), "mp_number<cpp_float<30> >");
   test_beta(mp_number<cpp_float<35> >(), "mp_number<cpp_float<35> >");
#endif
   return 0;
}

