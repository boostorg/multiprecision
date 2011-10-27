///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#include <boost/detail/lightweight_test.hpp>

#if !defined(TEST_MPF_50) && !defined(TEST_MPF) && !defined(TEST_BACKEND) && !defined(TEST_MPZ) && !defined(TEST_MP_FLOAT) && !defined(TEST_MPFR) && !defined(TEST_MPFR_50) && !defined(TEST_MPQ)
#  define TEST_MPF_50
#  define TEST_MPF
#  define TEST_BACKEND
#  define TEST_MPZ
#  define TEST_MPFR
#  define TEST_MPFR_50
#  define TEST_MP_FLOAT
#  define TEST_MPQ

#ifdef _MSC_VER
#pragma message("CAUTION!!: No backend type specified so testing everything.... this will take some time!!")
#endif
#ifdef __GNUC__
#pragma warning "CAUTION!!: No backend type specified so testing everything.... this will take some time!!"
#endif

#endif

#if defined(TEST_MPF_50) || defined(TEST_MPF) || defined(TEST_MPZ) || defined(TEST_MPQ)
#include <boost/multiprecision/gmp.hpp>
#endif
#ifdef TEST_BACKEND
#include <boost/multiprecision/concepts/mp_number_architypes.hpp>
#endif
#ifdef TEST_MP_FLOAT
#include <boost/multiprecision/mp_float.hpp>
#endif
#if defined(TEST_MPFR) || defined(TEST_MPFR_50)
#include <boost/multiprecision/mpfr.hpp>
#endif

#define PRINT(x)\
   std::cout << BOOST_STRINGIZE(x) << " = " << std::numeric_limits<Number>::x << std::endl;

template <class Number>
void test()
{
   //
   // Note really a test just yet, but we can at least print out all the values:
   //
   std::cout << "numeric_limits values for type " << typeid(Number).name() << std::endl;

   PRINT(is_specialized);
   PRINT(min());
   PRINT(max());
#ifndef BOOST_NO_NUMERIC_LIMITS_LOWEST
   PRINT(lowest());
#endif
   PRINT(digits);
   PRINT(digits10);
#ifndef BOOST_NO_NUMERIC_LIMITS_LOWEST
   PRINT(max_digits10);
#endif
   PRINT(is_signed);
   PRINT(is_integer);
   PRINT(is_exact);
   PRINT(radix);
   PRINT(epsilon());
   PRINT(round_error());
   PRINT(min_exponent);
   PRINT(min_exponent10);
   PRINT(max_exponent);
   PRINT(max_exponent10);
   PRINT(has_infinity);
   PRINT(has_quiet_NaN);
   PRINT(has_signaling_NaN);
   PRINT(has_denorm);
   PRINT(has_denorm_loss);
   PRINT(infinity());
   PRINT(quiet_NaN());
   PRINT(signaling_NaN());
   PRINT(denorm_min());
   PRINT(is_iec559);
   PRINT(is_bounded);
   PRINT(is_modulo);
   PRINT(traps);
   PRINT(tinyness_before);
   PRINT(round_style);

   if(std::numeric_limits<Number>::is_specialized)
   {
      if(std::numeric_limits<Number>::has_quiet_NaN)
      {
         BOOST_TEST((boost::math::isnan)(std::numeric_limits<Number>::quiet_NaN()));
         BOOST_TEST(FP_NAN == (boost::math::fpclassify)(std::numeric_limits<Number>::quiet_NaN()));
         BOOST_TEST(!(boost::math::isfinite)(std::numeric_limits<Number>::quiet_NaN()));
         BOOST_TEST(!(boost::math::isnormal)(std::numeric_limits<Number>::quiet_NaN()));
         BOOST_TEST(!(boost::math::isinf)(std::numeric_limits<Number>::quiet_NaN()));
      }
      if(std::numeric_limits<Number>::has_signaling_NaN)
      {
         BOOST_TEST((boost::math::isnan)(std::numeric_limits<Number>::signaling_NaN()));
         BOOST_TEST(FP_NAN == (boost::math::fpclassify)(std::numeric_limits<Number>::signaling_NaN()));
         BOOST_TEST(!(boost::math::isfinite)(std::numeric_limits<Number>::signaling_NaN()));
         BOOST_TEST(!(boost::math::isnormal)(std::numeric_limits<Number>::signaling_NaN()));
         BOOST_TEST(!(boost::math::isinf)(std::numeric_limits<Number>::signaling_NaN()));
      }
      if(std::numeric_limits<Number>::has_infinity)
      {
         BOOST_TEST((boost::math::isinf)(std::numeric_limits<Number>::infinity()));
         BOOST_TEST(FP_INFINITE == (boost::math::fpclassify)(std::numeric_limits<Number>::infinity()));
         BOOST_TEST(!(boost::math::isfinite)(std::numeric_limits<Number>::infinity()));
         BOOST_TEST(!(boost::math::isnormal)(std::numeric_limits<Number>::infinity()));
         BOOST_TEST(!(boost::math::isnan)(std::numeric_limits<Number>::infinity()));
      }
      if(std::numeric_limits<Number>::has_denorm)
      {
         BOOST_TEST(FP_SUBNORMAL == (boost::math::fpclassify)(std::numeric_limits<Number>::denorm_min()));
         BOOST_TEST((boost::math::isfinite)(std::numeric_limits<Number>::denorm_min()));
         BOOST_TEST(!(boost::math::isnormal)(std::numeric_limits<Number>::denorm_min()));
         BOOST_TEST(!(boost::math::isinf)(std::numeric_limits<Number>::denorm_min()));
         BOOST_TEST(!(boost::math::isnan)(std::numeric_limits<Number>::denorm_min()));
      }
   }
   Number n = 0;
   BOOST_TEST((boost::math::fpclassify)(n) == FP_ZERO);
   BOOST_TEST((boost::math::isfinite)(n));
   BOOST_TEST(!(boost::math::isnormal)(n));
   BOOST_TEST(!(boost::math::isinf)(n));
   BOOST_TEST(!(boost::math::isnan)(n));
   n = 2;
   BOOST_TEST((boost::math::fpclassify)(n) == FP_NORMAL);
   BOOST_TEST((boost::math::isfinite)(n));
   BOOST_TEST((boost::math::isnormal)(n));
   BOOST_TEST(!(boost::math::isinf)(n));
   BOOST_TEST(!(boost::math::isnan)(n));
}


int main()
{
#ifdef TEST_BACKEND
   test<boost::multiprecision::mp_number<boost::multiprecision::concepts::mp_number_backend_float_architype> >();
#endif
#ifdef TEST_MPF_50
   test<boost::multiprecision::mpf_float_50>();
#endif
#ifdef TEST_MPF
   boost::multiprecision::mpf_float::default_precision(1000);
   /*
   boost::multiprecision::mpf_float r;
   r.precision(50);
   BOOST_TEST(r.precision() >= 50);
   */
   BOOST_TEST(boost::multiprecision::mpf_float::default_precision() == 1000);
   test<boost::multiprecision::mpf_float>();
#endif
#ifdef TEST_MPZ
   test<boost::multiprecision::mpz_int>();
#endif
#ifdef TEST_MPQ
   test<boost::multiprecision::mpq_rational>();
#endif
#ifdef TEST_MP_FLOAT
   test<boost::multiprecision::mp_float_50>();
   test<boost::multiprecision::mp_float_100>();
#endif
#ifdef TEST_MPFR
   test<boost::multiprecision::mpfr_float>();
#endif
#ifdef TEST_MPFR_50
   test<boost::multiprecision::mpfr_float_50>();
#endif
   return boost::report_errors();
}

