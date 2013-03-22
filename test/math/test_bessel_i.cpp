///////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2002 - 2011.
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_
//
// This work is based on an earlier work:
// "Algorithm 910: A Portable C++ Multiple-Precision System for Special-Function Calculations",
// in ACM TOMS, {VOL 37, ISSUE 4, (February 2011)} (C) ACM, 2011. http://doi.acm.org/10.1145/1916461.1916469

#ifdef _MSC_VER
#  define _SCL_SECURE_NO_WARNINGS
#endif

#define BOOST_MATH_OVERFLOW_ERROR_POLICY ignore_error

#if !defined(TEST_MPF_50) && !defined(TEST_BACKEND) && !defined(TEST_CPP_DEC_FLOAT) && !defined(TEST_MPFR_50)
#  define TEST_MPF_50
#  define TEST_MPFR_50
#  define TEST_CPP_DEC_FLOAT

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
#include <boost/multiprecision/concepts/mp_number_archetypes.hpp>
#endif
#ifdef TEST_CPP_DEC_FLOAT
#include <boost/multiprecision/cpp_dec_float.hpp>
#endif

#define SC_(x) static_cast<typename table_type<T>::type>(BOOST_STRINGIZE(x))

#include <boost/math/special_functions/bessel.hpp>
#include "libs/math/test/test_bessel_i.hpp"

void expected_results()
{
   //
   // Define the max and mean errors expected for
   // various compilers and platforms.
   //

   //
   // Catch all cases come last:
   //
   add_expected_result(
      ".*",                          // compiler
      ".*",                          // stdlib
      ".*",                          // platform
      ".*",                          // test type(s)
      ".*",                          // test data group
      ".*",                          // test function
      500,                           // Max Peek error
      200);                          // Max mean error

   //
   // Finish off by printing out the compiler/stdlib/platform names,
   // we do this to make it easier to mark up expected error rates.
   //
   std::cout << "Tests run with " << BOOST_COMPILER << ", "
      << BOOST_STDLIB << ", " << BOOST_PLATFORM << std::endl;
}


BOOST_AUTO_TEST_CASE( test_main )
{
   using namespace boost::multiprecision;
   expected_results();
   //
   // Test at:
   //  9 decimal digits: tests the least wide arbitrary precision code
   // 18 decimal digits: tests 80-bit long double approximations
   // 30 decimal digits: tests 128-bit long double approximations
   // 35 decimal digits: tests arbitrary precision code
   //
#ifdef TEST_MPF_50
   // We have accuracy issues with gmp_float<18> - maybe in std::exp??  Disabled for now.
   //test_bessel(number<gmp_float<18> >(), "number<gmp_float<18> >");
   test_bessel(number<gmp_float<30> >(), "number<gmp_float<30> >");
   test_bessel(number<gmp_float<35> >(), "number<gmp_float<35> >");
   // there should be at least one test with expression templates off:
   test_bessel(number<gmp_float<35>, et_off>(), "number<gmp_float<35>, et_off>");
#endif
#ifdef TEST_MPFR_50
   test_bessel(number<mpfr_float_backend<18> >(), "number<mpfr_float_backend<18> >");
   test_bessel(number<mpfr_float_backend<30> >(), "number<mpfr_float_backend<30> >");
   test_bessel(number<mpfr_float_backend<35> >(), "number<mpfr_float_backend<35> >");
#endif
#ifdef TEST_CPP_DEC_FLOAT
   test_bessel(number<cpp_dec_float<8> >(),  "number<cpp_dec_float<8> >");
   test_bessel(number<cpp_dec_float<18> >(), "number<cpp_dec_float<18> >");
   test_bessel(number<cpp_dec_float<30> >(), "number<cpp_dec_float<30> >");
   test_bessel(number<cpp_dec_float<35, long long, std::allocator<void> > >(), "number<cpp_dec_float<35, long long, std::allocator<void> > >");
#endif
}



