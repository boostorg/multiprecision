///////////////////////////////////////////////////////////////
//  Copyright 2011 - 2025 John Maddock.
//  Copyright Christopher Kormanyos 2002 - 2025.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt
//

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include <test.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>

#if !defined(TEST_MPF_50) && !defined(TEST_CPP_DEC_FLOAT) && !defined(TEST_CPP_BIN_FLOAT) && !defined(TEST_MPFR_50) && !defined(TEST_MPFI_50) && !defined(TEST_FLOAT128) && !defined(TEST_CPP_DOUBLE_FLOAT)
#define TEST_MPF_50
#define TEST_CPP_DEC_FLOAT
#define TEST_CPP_BIN_FLOAT
#define TEST_MPFR_50
#define TEST_MPFI_50
#define TEST_FLOAT128
#define TEST_CPP_DOUBLE_FLOAT

#ifdef _MSC_VER
#pragma message("CAUTION!!: No backend type specified so testing everything.... this will take some time!!")
#endif
#ifdef __GNUC__
#pragma warning "CAUTION!!: No backend type specified so testing everything.... this will take some time!!"
#endif

#endif

#include <test_traits.hpp> // Note: include this AFTER the test-backends are defined

#if defined(TEST_MPF_50)
#include <boost/multiprecision/gmp.hpp>
#endif
#if defined(TEST_MPFR_50)
#include <boost/multiprecision/mpfr.hpp>
#endif
#if defined(TEST_MPFI_50)
#include <boost/multiprecision/mpfi.hpp>
#endif
#ifdef TEST_CPP_DEC_FLOAT
#include <boost/multiprecision/cpp_dec_float.hpp>
#endif
#ifdef TEST_CPP_BIN_FLOAT
#include <boost/multiprecision/cpp_bin_float.hpp>
#endif
#ifdef TEST_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#ifdef TEST_CPP_DOUBLE_FLOAT
#include <boost/multiprecision/cpp_double_fp.hpp>
#endif

#include <array>
#include <iostream>
#include <iomanip>

#ifdef BOOST_MSVC
#pragma warning(disable : 4127)
#endif

#if defined(TEST_CPP_DOUBLE_FLOAT)
template <class FloatingPointType, boost::multiprecision::expression_template_option ET>
bool has_bad_bankers_rounding(const boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ET>&)
{
   return true;
}
#endif
#if defined(TEST_MPF_50)
template <unsigned N, boost::multiprecision::expression_template_option ET>
bool has_bad_bankers_rounding(const boost::multiprecision::number<boost::multiprecision::gmp_float<N>, ET>&)
{
   return true;
}
#endif
#if defined(TEST_FLOAT128) && defined(BOOST_INTEL)
bool has_bad_bankers_rounding(const boost::multiprecision::float128&)
{
   return true;
}
#endif
template <class T>
bool has_bad_bankers_rounding(const T&)
{
   return false;
}

bool is_bankers_rounding_error(const std::string& s, const char* expect)
{
   // This check isn't foolproof: that would require *much* more sophisticated code!!!
   std::string::size_type l = std::strlen(expect);
   if (l != s.size())
      return false;
   std::string::size_type len = s.find('e');
   if (len == std::string::npos)
      len = l - 1;
   else
      --len;
   if (s.compare(0, len, expect, len))
      return false;
   if (s[len] != expect[len] + 1)
      return false;
   return true;
}

void print_flags(std::ios_base::fmtflags f)
{
   std::cout << "Formatting flags were: ";
   if (f & std::ios_base::scientific)
      std::cout << "scientific ";
   if (f & std::ios_base::fixed)
      std::cout << "fixed ";
   if (f & std::ios_base::showpoint)
      std::cout << "showpoint ";
   if (f & std::ios_base::showpos)
      std::cout << "showpos ";
   std::cout << std::endl;
}

template <class T>
void test()
{
   typedef T mp_t;

   std::cout << "Testing type of test: " << typeid(mp_t).name() << std::endl;

   std::array<std::ios_base::fmtflags, 9> f =
       {{std::ios_base::fmtflags(0), std::ios_base::showpoint, std::ios_base::showpos, std::ios_base::scientific, std::ios_base::scientific | std::ios_base::showpos,
         std::ios_base::scientific | std::ios_base::showpoint, std::ios_base::fixed, std::ios_base::fixed | std::ios_base::showpoint,
         std::ios_base::fixed | std::ios_base::showpos}};

   std::array<std::array<const char*, 13 * 9>, 40> string_data = {{
#include "string_data.ipp"
   }};

   double num   = 123456789.0;
   double denom = 1;
   double val   = num;
   for (unsigned j = 0; j < 40; ++j)
   {
      unsigned col = 0;
      for (unsigned prec = 1; prec < 14; ++prec)
      {
         for (unsigned i = 0; i < f.size(); ++i, ++col)
         {
            std::stringstream ss;
            ss.precision(prec);
            ss.flags(f[i]);
            ss << mp_t(val);
            const char* expect = string_data[j][col];
            if (ss.str() != expect)
            {
               if (has_bad_bankers_rounding(mp_t()) && is_bankers_rounding_error(ss.str(), expect))
               {
                  std::cout << "Ignoring bankers-rounding error with " << typeid(T).name() << ".\n";
               }
               else
               {
                  std::cout << std::setprecision(20) << "Testing value " << val << std::endl;
                  print_flags(f[i]);
                  std::cout << "Precision is: " << prec << std::endl;
                  std::cout << "Got:      " << ss.str() << std::endl;
                  std::cout << "Expected: " << expect << std::endl;
                  ++boost::detail::test_errors();
                  mp_t(val).str(prec, f[i]); // for debugging
               }
            }
         }
      }
      num = -num;
      if (j & 1)
         denom *= 8;
      val = num / denom;
   }

   std::array<const char*, 13 * 9> zeros =
       {{"0", "0.", "+0", "0.0e+00", "+0.0e+00", "0.0e+00", "0.0", "0.0", "+0.0", "0", "0.0", "+0", "0.00e+00", "+0.00e+00", "0.00e+00", "0.00", "0.00", "+0.00", "0", "0.00", "+0", "0.000e+00", "+0.000e+00", "0.000e+00", "0.000", "0.000", "+0.000", "0", "0.000", "+0", "0.0000e+00", "+0.0000e+00", "0.0000e+00", "0.0000", "0.0000", "+0.0000", "0", "0.0000", "+0", "0.00000e+00", "+0.00000e+00", "0.00000e+00", "0.00000", "0.00000", "+0.00000", "0", "0.00000", "+0", "0.000000e+00", "+0.000000e+00", "0.000000e+00", "0.000000", "0.000000", "+0.000000", "0", "0.000000", "+0", "0.0000000e+00", "+0.0000000e+00", "0.0000000e+00", "0.0000000", "0.0000000", "+0.0000000", "0", "0.0000000", "+0", "0.00000000e+00", "+0.00000000e+00", "0.00000000e+00", "0.00000000", "0.00000000", "+0.00000000", "0", "0.00000000", "+0", "0.000000000e+00", "+0.000000000e+00", "0.000000000e+00", "0.000000000", "0.000000000", "+0.000000000", "0", "0.000000000", "+0", "0.0000000000e+00", "+0.0000000000e+00", "0.0000000000e+00", "0.0000000000", "0.0000000000", "+0.0000000000", "0", "0.0000000000", "+0", "0.00000000000e+00", "+0.00000000000e+00", "0.00000000000e+00", "0.00000000000", "0.00000000000", "+0.00000000000", "0", "0.00000000000", "+0", "0.000000000000e+00", "+0.000000000000e+00", "0.000000000000e+00", "0.000000000000", "0.000000000000", "+0.000000000000", "0", "0.000000000000", "+0", "0.0000000000000e+00", "+0.0000000000000e+00", "0.0000000000000e+00", "0.0000000000000", "0.0000000000000", "+0.0000000000000"}};

   unsigned col = 0;
   val          = 0;
   for (unsigned prec = 1; prec < 14; ++prec)
   {
      for (unsigned i = 0; i < f.size(); ++i, ++col)
      {
         std::stringstream ss;
         ss.precision(prec);
         ss.flags(f[i]);
         ss << mp_t(val);
         const char* expect = zeros[col];
         if (ss.str() != expect)
         {
            std::cout << std::setprecision(20) << "Testing value " << val << std::endl;
            print_flags(f[i]);
            std::cout << "Precision is: " << prec << std::endl;
            std::cout << "Got:      " << ss.str() << std::endl;
            std::cout << "Expected: " << expect << std::endl;
            ++boost::detail::test_errors();
            mp_t(val).str(prec, f[i]); // for debugging
         }
      }
   }

   if (std::numeric_limits<mp_t>::has_infinity)
   {
      T val = std::numeric_limits<T>::infinity();
      BOOST_CHECK_EQUAL(val.str(), "inf");
      BOOST_CHECK_EQUAL(val.str(0, std::ios_base::showpos), "+inf");
      val = -val;
      BOOST_CHECK_EQUAL(val.str(), "-inf");
      BOOST_CHECK_EQUAL(val.str(0, std::ios_base::showpos), "-inf");

      val = static_cast<T>("inf");
      BOOST_CHECK_EQUAL(val, std::numeric_limits<T>::infinity());
      val = static_cast<T>("+inf");
      BOOST_CHECK_EQUAL(val, std::numeric_limits<T>::infinity());
      val = static_cast<T>("-inf");
      BOOST_CHECK_EQUAL(val, -std::numeric_limits<T>::infinity());
   }
   if (std::numeric_limits<mp_t>::has_quiet_NaN)
   {
      T val = std::numeric_limits<T>::quiet_NaN();
      BOOST_CHECK_EQUAL(val.str(), "nan");
      val = static_cast<T>("nan");
      BOOST_CHECK((boost::math::isnan)(val));
   }
   //
   // Bug cases:
   //
   // https://github.com/boostorg/multiprecision/issues/113
   //
   {
      T val("99.9809");
      BOOST_CHECK_EQUAL(val.str(3, std::ios_base::fixed), "99.981");
   }
}

template <class T>
T generate_random()
{
   typedef typename T::backend_type::exponent_type e_type;
   static boost::random::mt19937                   gen;
   T                                               val      = gen();
   T                                               prev_val = -1;
   while (val != prev_val)
   {
      val *= (gen.max)();
      prev_val = val;
      val += gen();
   }
   e_type e;
   val = frexp(val, &e);

   constexpr auto exp_range =
     (::has_poor_exp_range_or_precision_support<T>::value)
       ? static_cast<int>
         (
            static_cast<float>
            (
                 static_cast<float>(std::numeric_limits<T>::max_exponent10)
               - static_cast<float>(static_cast<float>(std::numeric_limits<T>::max_digits10) * 1.1F)
            )
            / 0.301F
         )
       : std::numeric_limits<T>::max_exponent - 10
       ;

   static boost::random::uniform_int_distribution<e_type> ui(0, exp_range);

   return ldexp(val, ui(gen));
}

template <class T>
struct max_digits10_proxy
{
   static const unsigned value = std::numeric_limits<T>::digits10 + 5;
};
#ifdef TEST_CPP_DEC_FLOAT
template <unsigned D, boost::multiprecision::expression_template_option ET>
struct max_digits10_proxy<boost::multiprecision::number<boost::multiprecision::cpp_dec_float<D>, ET> >
{
   static const unsigned value = std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_dec_float<D>, ET> >::max_digits10;
};
#endif
#ifdef TEST_MPF_50
template <unsigned D, boost::multiprecision::expression_template_option ET>
struct max_digits10_proxy<boost::multiprecision::number<boost::multiprecision::gmp_float<D>, ET> >
{
   static const unsigned value = std::numeric_limits<boost::multiprecision::number<boost::multiprecision::gmp_float<D>, ET> >::max_digits10;
};
#endif
#ifdef TEST_MPFR_50
template <unsigned D, boost::multiprecision::expression_template_option ET>
struct max_digits10_proxy<boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<D>, ET> >
{
   static const unsigned value = std::numeric_limits<boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<D>, ET> >::max_digits10;
};
#endif

template <class T>
void do_round_trip(const T& val, std::ios_base::fmtflags f)
{
   std::stringstream ss;
   ss << std::setprecision(std::numeric_limits<T>::max_digits10);
   ss.flags(f);
   ss << val;
   T new_val = static_cast<T>(ss.str());
   //BOOST_CHECK_EQUAL(new_val, val);
   BOOST_CHECK_CLOSE_FRACTION(new_val, val, std::numeric_limits<T>::epsilon() * 100000);
   new_val = static_cast<T>(val.str(f & std::ios_base::fixed ? std::numeric_limits<T>::max_digits10 : 0, f));
   //BOOST_CHECK_EQUAL(new_val, val);
   BOOST_CHECK_CLOSE_FRACTION(new_val, val, std::numeric_limits<T>::epsilon() * 100000);
}

template <class T>
void do_round_trip(const T& val)
{
   do_round_trip(val, std::ios_base::fmtflags(0));
   do_round_trip(val, std::ios_base::fmtflags(std::ios_base::scientific));
   if ((fabs(val) > 1) && (fabs(val) < 1e100))
      do_round_trip(val, std::ios_base::fmtflags(std::ios_base::fixed));
}

template <class T>
void test_round_trip()
{
   std::cout << "Testing type of test_round_trip: " << typeid(T).name() << std::endl;

   for (unsigned i = 0; i < 1000; ++i)
   {
      T val = generate_random<T>();
      do_round_trip(val);
      do_round_trip(T(-val));
      do_round_trip(T(1 / val));
      do_round_trip(T(-1 / val));
   }
}

template<typename T>
void test_to_string()
{
   std::cout << "Testing type of test_to_string: " << typeid(T).name() << std::endl;

   using std::to_string;
   T x0{"23.43"};
   BOOST_CHECK_EQUAL(to_string(x0), "23.430000");

   T x1{"1e-9"};
   BOOST_CHECK_EQUAL(to_string(x1), "0.000000");

   T x2{"123456789"};
   BOOST_CHECK_EQUAL(to_string(x2), "123456789.000000");
}

#ifdef TEST_FLOAT128
void test_hexadecimal_floating_point()
{
   using boost::multiprecision::float128;

   float128 x = 0x1p+0Q;

   std::string s = x.str(0, std::ios_base::fmtflags(std::ios_base::fixed | std::ios_base::scientific));
   BOOST_CHECK_EQUAL(s, "0x1p+0");

   s = x.str(0, std::ios_base::fmtflags(std::ios_base::floatfield));
   BOOST_CHECK_EQUAL(s, "0x1p+0");

   x = -1;
   s = x.str(0, std::ios_base::fmtflags(std::ios_base::floatfield));
   BOOST_CHECK_EQUAL(s, "-0x1p+0");

   // hexadecimal representation of pi; test a round trip:
   float128 pi1 = 0x1.921fb54442d18469898cc51701b8p+1Q;
   s            = pi1.str(0, std::ios_base::fmtflags(std::ios_base::floatfield));
   float128 pi2(s);
   BOOST_CHECK_EQUAL(pi1, pi2);
}
#endif

int main()
{
#ifdef TEST_MPFR_50
   test<boost::multiprecision::mpfr_float_50>();
   test<boost::multiprecision::mpfr_float_100>();

   test_round_trip<boost::multiprecision::mpfr_float_50>();
   test_round_trip<boost::multiprecision::mpfr_float_100>();

   test_to_string<boost::multiprecision::mpfr_float_50>();
   test_to_string<boost::multiprecision::mpfr_float_100>();

#endif
#ifdef TEST_MPFI_50
   test<boost::multiprecision::mpfr_float_50>();
   test<boost::multiprecision::mpfr_float_100>();

   test_round_trip<boost::multiprecision::mpfr_float_50>();
   test_round_trip<boost::multiprecision::mpfr_float_100>();
#endif
#ifdef TEST_CPP_DEC_FLOAT
   test<boost::multiprecision::cpp_dec_float_50>();
   test<boost::multiprecision::cpp_dec_float_100>();

   // cpp_dec_float has extra guard digits that messes this up:
   test_round_trip<boost::multiprecision::cpp_dec_float_50>();
   test_round_trip<boost::multiprecision::cpp_dec_float_100>();

   test_to_string<boost::multiprecision::cpp_dec_float_50>();
   test_to_string<boost::multiprecision::cpp_dec_float_100>();

#endif
#ifdef TEST_CPP_BIN_FLOAT
   test<boost::multiprecision::cpp_bin_float_50>();
   test<boost::multiprecision::cpp_bin_float_100>();

   test_round_trip<boost::multiprecision::cpp_bin_float_50>();
   test_round_trip<boost::multiprecision::cpp_bin_float_100>();

   test_to_string<boost::multiprecision::cpp_bin_float_50>();
   test_to_string<boost::multiprecision::cpp_bin_float_100>();

#endif
#ifdef TEST_MPF_50
   test<boost::multiprecision::mpf_float_50>();
   test<boost::multiprecision::mpf_float_100>();
   /*
   // I can't get this to work with mpf_t - mpf_str appears
   // not to actually print enough decimal digits:
   test_round_trip<boost::multiprecision::mpf_float_50>();
   test_round_trip<boost::multiprecision::mpf_float_100>();
   */
#endif
#ifdef TEST_FLOAT128
   test<boost::multiprecision::float128>();
   test_hexadecimal_floating_point();
   test_to_string<boost::multiprecision::float128>();
#ifndef BOOST_INTEL
   test_round_trip<boost::multiprecision::float128>();
#endif
#endif
#ifdef TEST_CPP_DOUBLE_FLOAT
   test<boost::multiprecision::cpp_double_double>();
   test_to_string<boost::multiprecision::cpp_double_double>();
   test_round_trip<boost::multiprecision::cpp_double_double>();

   test<boost::multiprecision::cpp_double_long_double>();
   test_to_string<boost::multiprecision::cpp_double_long_double>();
   test_round_trip<boost::multiprecision::cpp_double_long_double>();

   #if defined(BOOST_MP_CPP_DOUBLE_FP_HAS_FLOAT128)
   test<boost::multiprecision::cpp_double_float128>();
   test_to_string<boost::multiprecision::cpp_double_float128>();
   test_round_trip<boost::multiprecision::cpp_double_float128>();
   #endif
#endif
   return boost::report_errors();
}
