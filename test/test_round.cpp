//  (C) Copyright John Maddock 2007 - 2025.
//  (C) Copyright Christopher Kormanyos 2023 - 2025.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include <test.hpp>

#include <boost/detail/lightweight_test.hpp>
#include <boost/math/special_functions/modf.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/math/special_functions/sign.hpp>
#include <boost/math/special_functions/trunc.hpp>
#include <boost/random/mersenne_twister.hpp>

#if !defined(TEST_MPF_50) && !defined(TEST_MPF) && !defined(TEST_BACKEND) && !defined(TEST_CPP_DEC_FLOAT) && !defined(TEST_MPFR) && !defined(TEST_MPFR_50) && !defined(TEST_MPFI_50) && !defined(TEST_FLOAT128) && !defined(TEST_CPP_BIN_FLOAT) && !defined(TEST_CPP_DOUBLE_FLOAT)
#define TEST_MPF_50
#define TEST_MPFR_50
#define TEST_MPFI_50
#define TEST_BACKEND
#define TEST_CPP_DEC_FLOAT
#define TEST_FLOAT128
#define TEST_CPP_BIN_FLOAT
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
#ifdef TEST_MPFR_50
#include <boost/multiprecision/mpfr.hpp>
#endif
#ifdef TEST_MPFI_50
#include <boost/multiprecision/mpfi.hpp>
#endif
#ifdef TEST_BACKEND
#include <boost/multiprecision/concepts/mp_number_archetypes.hpp>
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

#ifdef BOOST_MSVC
#pragma warning(disable : 4127)
#endif

boost::mt19937 rng;

template <class T>
T get_random()
{
   //
   // Fill all the bits in T with random values,
   // likewise set the exponent to a random value
   // that will still fit inside a T, and always
   // have a remainder as well as an integer part.
   //
   int bits     = boost::math::tools::digits<T>();
   int shift    = 0;
   int exponent = rng() % (bits - 4);
   T   result   = 0;
   while (bits > 0)
   {
      result += ldexp(static_cast<T>(rng()), shift);
      shift += std::numeric_limits<int>::digits;
      bits -= std::numeric_limits<int>::digits;
   }
   return rng() & 1u ? T(-ldexp(frexp(result, &bits), exponent)) : T(ldexp(frexp(result, &bits), exponent));
}

template <class T, class U>
typename std::enable_if<!boost::multiprecision::is_interval_number<T>::value>::type check_within_half(T a, U u)
{
   BOOST_MATH_STD_USING

   if (fabs(a - u) > 0.5f)
   {
      BOOST_ERROR("Rounded result differed by more than 0.5 from the original");
      std::cerr << "Values were: " << std::setprecision(35) << std::setw(40)
                << std::left << a << u << std::endl;
   }
   if ((fabs(a - u) == 0.5f) && (fabs(static_cast<T>(u)) < fabs(a)))
   {
      BOOST_ERROR("Rounded result was towards zero with boost::round");
      std::cerr << "Values were: " << std::setprecision(35) << std::setw(40)
                << std::left << a << u << std::endl;
   }
}
template <class T, class U>
typename std::enable_if<boost::multiprecision::is_interval_number<T>::value>::type check_within_half(T a, U u)
{
   BOOST_MATH_STD_USING
   if (upper(T(fabs(a - u))) > 0.5f)
   {
      BOOST_ERROR("Rounded result differed by more than 0.5 from the original");
      std::cerr << "Values were: " << std::setprecision(35) << std::setw(40)
                << std::left << a << u << std::endl;
   }
   if ((upper(T(fabs(a - u))) == 0.5f) && (fabs(static_cast<T>(u)) < fabs(a)))
   {
      BOOST_ERROR("Rounded result was towards zero with boost::round");
      std::cerr << "Values were: " << std::setprecision(35) << std::setw(40)
                << std::left << a << u << std::endl;
   }
}

//
// We may not have an abs overload for long long so provide a fall back:
//
inline unsigned safe_abs(int const& v)
{
   return v < 0 ? static_cast<unsigned>(1u) + static_cast<unsigned>(-(v + 1)) : v;
}
inline unsigned long safe_abs(long const& v)
{
   return v < 0 ? static_cast<unsigned long>(1u) + static_cast<unsigned long>(-(v + 1)) : v;
}
inline unsigned long long safe_abs(long long const& v)
{
   return v < 0 ? static_cast<unsigned long long>(1u) + static_cast<unsigned long long>(-(v + 1)) : v;
}
template <class T>
inline typename std::enable_if<!boost::multiprecision::detail::is_integral<T>::value, T>::type safe_abs(T const& v)
{
   return v < 0 ? -v : v;
}

template <class T, class U>
void check_trunc_result(T a, U u)
{
   BOOST_MATH_STD_USING

   if (fabs(a - u) >= 1)
   {
      BOOST_ERROR("Rounded result differed by more than 1 from the original");
      std::cerr << "Values were: " << std::setprecision(35) << std::setw(40)
                << std::left << a << u << std::endl;
   }

   if (abs(a) < safe_abs(u))
   {
      BOOST_ERROR("Truncated result had larger absolute value than the original");
      std::cerr << "Values were: " << std::setprecision(35) << std::setw(40)
                << std::left << abs(a) << safe_abs(u) << std::endl;
   }

   if (fabs(static_cast<T>(u)) > fabs(a))
   {
      BOOST_ERROR("Rounded result was away from zero with boost::trunc");
      std::cerr << "Values were: " << std::setprecision(35) << std::setw(40)
                << std::left << a << u << std::endl;
   }
}

namespace local {

template <class T>
static inline auto modf_fail_gate(const T& my_sum, const T& my_a) noexcept -> typename std::enable_if<has_poor_exp_range_or_precision_support<T>::value, bool>::type
{
   const T ratio { my_sum / my_a };
   const T delta { fabs(1 - ratio) };

   return (delta > std::numeric_limits<T>::epsilon());
}

template <class T>
static inline auto modf_fail_gate(const T& my_sum, const T& my_a) noexcept -> typename std::enable_if<(!has_poor_exp_range_or_precision_support<T>::value), bool>::type
{
   return (my_sum != my_a);
}

} // namespace local

template <class T, class U>
void check_modf_result(T a, T fract, U ipart)
{
   BOOST_MATH_STD_USING

   const T sum { fract + ipart };

   if (local::modf_fail_gate<T>(sum, a))
   {
      BOOST_ERROR("Fractional and integer results do not add up to the original value");
      std::cerr << "Values were: " << std::setprecision(35) << " "
                << std::left << a << ipart << " " << fract << std::endl;
   }

   if ((boost::math::sign(a) != boost::math::sign(fract)) && boost::math::sign(fract))
   {
      BOOST_ERROR("Original and fractional parts have differing signs");
      std::cerr << "Values were: " << std::setprecision(35) << " "
                << std::left << a << ipart << " " << fract << std::endl;
   }
   if ((boost::math::sign(a) != boost::math::sign(ipart)) && boost::math::sign(ipart))
   {
      BOOST_ERROR("Original and integer parts have differing signs");
      std::cerr << "Values were: " << std::setprecision(35) << " "
                << std::left << a << ipart << " " << ipart << std::endl;
   }
   if (fabs(a - ipart) >= 1)
   {
      BOOST_ERROR("Rounded result differed by more than 1 from the original");
      std::cerr << "Values were: " << std::setprecision(35) << std::setw(40)
                << std::left << a << ipart << std::endl;
   }
}

template <class T>
void test()
{
   std::cout << "Testing type: " << typeid(T).name() << std::endl;

   BOOST_MATH_STD_USING

   for (int index = 0; index < 1000; ++index)
   {
      static_cast<void>(index);

      const T arg { get_random<T>() };

      T r   = round(arg);
      check_within_half(arg, r);
      BOOST_TEST(r == round(arg + 0));
      r = trunc(arg);
      check_trunc_result(arg, r);
      BOOST_TEST(r == trunc(arg + 0));

      T frac = modf(arg, &r);
      check_modf_result(arg, frac, r);

      if (abs(r) < (std::numeric_limits<int>::max)())
      {
         int irnd = iround(arg);
         check_within_half(arg, irnd);
         BOOST_TEST(irnd == iround(arg + 0));
         irnd = itrunc(arg);
         check_trunc_result(arg, irnd);
         BOOST_TEST(irnd == itrunc(arg + 0));
         r = modf(arg, &irnd);
         check_modf_result(arg, r, irnd);
      }

      if (abs(r) < (std::numeric_limits<long>::max)())
      {
         long l = lround(arg);
         check_within_half(arg, l);
         BOOST_TEST(l == lround(arg + 0));
         l = ltrunc(arg);
         check_trunc_result(arg, l);
         BOOST_TEST(l == ltrunc(arg + 0));
         r = modf(arg, &l);
         check_modf_result(arg, r, l);
      }

#if defined(BOOST_HAS_LONG_LONG)
      if (abs(r) < (std::numeric_limits<long long>::max)())
      {
         long long ll = llround(arg);
         check_within_half(arg, ll);
         BOOST_TEST(ll == llround(arg + 0));
         ll = lltrunc(arg);
         check_trunc_result(arg, ll);
         BOOST_TEST(ll == lltrunc(arg + 0));
         r = modf(arg, &ll);
         check_modf_result(arg, r, ll);
      }
#endif
   }
   //
   // Test boundary cases:
   //
   if (std::numeric_limits<T>::digits >= std::numeric_limits<int>::digits)
   {
      int si = iround(static_cast<T>((std::numeric_limits<int>::max)()));
      check_within_half(static_cast<T>((std::numeric_limits<int>::max)()), si);
      BOOST_TEST(si == iround(static_cast<T>((std::numeric_limits<int>::max)()) + 0));
      si = iround(static_cast<T>((std::numeric_limits<int>::min)()));
      check_within_half(static_cast<T>((std::numeric_limits<int>::min)()), si);
      BOOST_TEST(si == iround(static_cast<T>((std::numeric_limits<int>::min)()) + 0));
      si = itrunc(static_cast<T>((std::numeric_limits<int>::max)()));
      check_trunc_result(static_cast<T>((std::numeric_limits<int>::max)()), si);
      BOOST_TEST(si == itrunc(static_cast<T>((std::numeric_limits<int>::max)()) + 0));
      si = itrunc(static_cast<T>((std::numeric_limits<int>::min)()));
      check_trunc_result(static_cast<T>((std::numeric_limits<int>::min)()), si);
      BOOST_TEST(si == itrunc(static_cast<T>((std::numeric_limits<int>::min)()) + 0));

      si = iround(static_cast<T>((std::numeric_limits<int>::max)() - 1));
      check_within_half(static_cast<T>((std::numeric_limits<int>::max)() - 1), si);
      si = iround(static_cast<T>((std::numeric_limits<int>::min)() + 1));
      check_within_half(static_cast<T>((std::numeric_limits<int>::min)() + 1), si);
      si = itrunc(static_cast<T>((std::numeric_limits<int>::max)() - 1));
      check_trunc_result(static_cast<T>((std::numeric_limits<int>::max)() - 1), si);
      si = itrunc(static_cast<T>((std::numeric_limits<int>::min)() + 1));
      check_trunc_result(static_cast<T>((std::numeric_limits<int>::min)() + 1), si);
   }

   if (std::numeric_limits<T>::digits >= std::numeric_limits<long>::digits)
   {
      long k = lround(static_cast<T>((std::numeric_limits<long>::max)()));
      check_within_half(static_cast<T>((std::numeric_limits<long>::max)()), k);
      BOOST_TEST(k == lround(static_cast<T>((std::numeric_limits<long>::max)()) + 0));
      k = lround(static_cast<T>((std::numeric_limits<long>::min)()));
      check_within_half(static_cast<T>((std::numeric_limits<long>::min)()), k);
      BOOST_TEST(k == lround(static_cast<T>((std::numeric_limits<long>::min)()) + 0));
      k = ltrunc(static_cast<T>((std::numeric_limits<long>::max)()));
      check_trunc_result(static_cast<T>((std::numeric_limits<long>::max)()), k);
      BOOST_TEST(k == ltrunc(static_cast<T>((std::numeric_limits<long>::max)()) + 0));
      k = ltrunc(static_cast<T>((std::numeric_limits<long>::min)()));
      check_trunc_result(static_cast<T>((std::numeric_limits<long>::min)()), k);
      BOOST_TEST(k == ltrunc(static_cast<T>((std::numeric_limits<long>::min)()) + 0));

      k = lround(static_cast<T>((std::numeric_limits<long>::max)() - 1));
      check_within_half(static_cast<T>((std::numeric_limits<long>::max)() - 1), k);
      k = lround(static_cast<T>((std::numeric_limits<long>::min)() + 1));
      check_within_half(static_cast<T>((std::numeric_limits<long>::min)() + 1), k);
      k = ltrunc(static_cast<T>((std::numeric_limits<long>::max)() - 1));
      check_trunc_result(static_cast<T>((std::numeric_limits<long>::max)() - 1), k);
      k = ltrunc(static_cast<T>((std::numeric_limits<long>::min)() + 1));
      check_trunc_result(static_cast<T>((std::numeric_limits<long>::min)() + 1), k);
   }

#if !defined(BOOST_NO_LONG_LONG)
   if (std::numeric_limits<T>::digits >= std::numeric_limits<long long>::digits)
   {
      long long j = llround(static_cast<T>((std::numeric_limits<long long>::max)()));
      check_within_half(static_cast<T>((std::numeric_limits<long long>::max)()), j);
      BOOST_TEST(j == llround(static_cast<T>((std::numeric_limits<long long>::max)()) + 0));
      j = llround(static_cast<T>((std::numeric_limits<long long>::min)()));
      check_within_half(static_cast<T>((std::numeric_limits<long long>::min)()), j);
      BOOST_TEST(j == llround(static_cast<T>((std::numeric_limits<long long>::min)()) + 0));
      j = lltrunc(static_cast<T>((std::numeric_limits<long long>::max)()));
      check_trunc_result(static_cast<T>((std::numeric_limits<long long>::max)()), j);
      BOOST_TEST(j == lltrunc(static_cast<T>((std::numeric_limits<long long>::max)()) + 0));
      j = lltrunc(static_cast<T>((std::numeric_limits<long long>::min)()));
      check_trunc_result(static_cast<T>((std::numeric_limits<long long>::min)()), j);
      BOOST_TEST(j == lltrunc(static_cast<T>((std::numeric_limits<long long>::min)()) + 0));

      j = llround(static_cast<T>((std::numeric_limits<long long>::max)() - 1));
      check_within_half(static_cast<T>((std::numeric_limits<long long>::max)() - 1), j);
      j = llround(static_cast<T>((std::numeric_limits<long long>::min)() + 1));
      check_within_half(static_cast<T>((std::numeric_limits<long long>::min)() + 1), j);
      j = lltrunc(static_cast<T>((std::numeric_limits<long long>::max)() - 1));
      check_trunc_result(static_cast<T>((std::numeric_limits<long long>::max)() - 1), j);
      j = lltrunc(static_cast<T>((std::numeric_limits<long long>::min)() + 1));
      check_trunc_result(static_cast<T>((std::numeric_limits<long long>::min)() + 1), j);
   }
#endif
   //
   // Finish off by testing the error handlers:
   //
   T result;
#ifndef BOOST_NO_EXCEPTIONS
   BOOST_CHECK_THROW(result = static_cast<T>(iround(static_cast<T>(1e20))), boost::math::rounding_error);
   BOOST_CHECK_THROW(result = static_cast<T>(iround(static_cast<T>(-1e20))), boost::math::rounding_error);
   BOOST_CHECK_THROW(result = static_cast<T>(lround(static_cast<T>(1e20))), boost::math::rounding_error);
   BOOST_CHECK_THROW(result = static_cast<T>(lround(static_cast<T>(-1e20))), boost::math::rounding_error);
#ifdef BOOST_HAS_LONG_LONG
   BOOST_CHECK_THROW(result = static_cast<T>(llround(static_cast<T>(1e20))), boost::math::rounding_error);
   BOOST_CHECK_THROW(result = static_cast<T>(llround(static_cast<T>(-1e20))), boost::math::rounding_error);
#endif
   if (std::numeric_limits<T>::has_infinity)
   {
      BOOST_CHECK_EQUAL(static_cast<T>(round(std::numeric_limits<T>::infinity())), std::numeric_limits<T>::infinity()); // See C99 Annex F.
      BOOST_CHECK_THROW(result = static_cast<T>(iround(std::numeric_limits<T>::infinity())), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(iround(-std::numeric_limits<T>::infinity())), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(lround(std::numeric_limits<T>::infinity())), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(lround(-std::numeric_limits<T>::infinity())), boost::math::rounding_error);
#ifdef BOOST_HAS_LONG_LONG
      BOOST_CHECK_THROW(result = static_cast<T>(llround(std::numeric_limits<T>::infinity())), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(llround(-std::numeric_limits<T>::infinity())), boost::math::rounding_error);
#endif
   }
   if (std::numeric_limits<T>::has_quiet_NaN)
   {
      BOOST_CHECK((boost::multiprecision::isnan)(round(std::numeric_limits<T>::quiet_NaN())));
      BOOST_CHECK_THROW(result = static_cast<T>(iround(std::numeric_limits<T>::quiet_NaN())), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(lround(std::numeric_limits<T>::quiet_NaN())), boost::math::rounding_error);
#ifdef BOOST_HAS_LONG_LONG
      BOOST_CHECK_THROW(result = static_cast<T>(llround(std::numeric_limits<T>::quiet_NaN())), boost::math::rounding_error);
#endif
   }
   BOOST_CHECK_THROW(result = static_cast<T>(itrunc(static_cast<T>(1e20))), boost::math::rounding_error);
   BOOST_CHECK_THROW(result = static_cast<T>(itrunc(static_cast<T>(-1e20))), boost::math::rounding_error);
   BOOST_CHECK_THROW(result = static_cast<T>(ltrunc(static_cast<T>(1e20))), boost::math::rounding_error);
   BOOST_CHECK_THROW(result = static_cast<T>(ltrunc(static_cast<T>(-1e20))), boost::math::rounding_error);
#ifdef BOOST_HAS_LONG_LONG
   BOOST_CHECK_THROW(result = static_cast<T>(lltrunc(static_cast<T>(1e20))), boost::math::rounding_error);
   BOOST_CHECK_THROW(result = static_cast<T>(lltrunc(static_cast<T>(-1e20))), boost::math::rounding_error);
#endif
   if (std::numeric_limits<T>::has_infinity)
   {
      BOOST_CHECK_EQUAL(static_cast<T>(trunc(std::numeric_limits<T>::infinity())), std::numeric_limits<T>::infinity());
      BOOST_CHECK_EQUAL(static_cast<T>(trunc(-std::numeric_limits<T>::infinity())), -std::numeric_limits<T>::infinity());
      BOOST_CHECK_THROW(result = static_cast<T>(itrunc(std::numeric_limits<T>::infinity())), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(itrunc(-std::numeric_limits<T>::infinity())), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(ltrunc(std::numeric_limits<T>::infinity())), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(ltrunc(-std::numeric_limits<T>::infinity())), boost::math::rounding_error);
#ifdef BOOST_HAS_LONG_LONG
      BOOST_CHECK_THROW(result = static_cast<T>(lltrunc(std::numeric_limits<T>::infinity())), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(lltrunc(-std::numeric_limits<T>::infinity())), boost::math::rounding_error);
#endif
   }
   if (std::numeric_limits<T>::has_quiet_NaN)
   {
      BOOST_CHECK((boost::multiprecision::isnan)(trunc(std::numeric_limits<T>::quiet_NaN())));
      BOOST_CHECK_THROW(result = static_cast<T>(itrunc(std::numeric_limits<T>::quiet_NaN())), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(ltrunc(std::numeric_limits<T>::quiet_NaN())), boost::math::rounding_error);
#ifdef BOOST_HAS_LONG_LONG
      BOOST_CHECK_THROW(result = static_cast<T>(lltrunc(std::numeric_limits<T>::quiet_NaN())), boost::math::rounding_error);
#endif
   }
   if (std::numeric_limits<T>::digits >= std::numeric_limits<int>::digits)
   {
      BOOST_CHECK_THROW(result = static_cast<T>(itrunc(static_cast<T>((std::numeric_limits<int>::max)()) + 1)), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(itrunc(static_cast<T>((std::numeric_limits<int>::min)()) - 1)), boost::math::rounding_error);
   }
   if (std::numeric_limits<T>::digits >= std::numeric_limits<long>::digits)
   {
      BOOST_CHECK_THROW(result = static_cast<T>(ltrunc(static_cast<T>((std::numeric_limits<long>::max)()) + 1)), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(ltrunc(static_cast<T>((std::numeric_limits<long>::min)()) - 1)), boost::math::rounding_error);
   }
#ifndef BOOST_NO_LONG_LONG
   if (std::numeric_limits<T>::digits >= std::numeric_limits<long long>::digits)
   {
      BOOST_CHECK_THROW(result = static_cast<T>(lltrunc(static_cast<T>((std::numeric_limits<long long>::max)()) + 1)), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(lltrunc(static_cast<T>((std::numeric_limits<long long>::min)()) - 1)), boost::math::rounding_error);
   }
#endif
   if (std::numeric_limits<T>::digits >= std::numeric_limits<int>::digits)
   {
      BOOST_CHECK_THROW(result = static_cast<T>(iround(static_cast<T>((std::numeric_limits<int>::max)()) + 1)), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(iround(static_cast<T>((std::numeric_limits<int>::min)()) - 1)), boost::math::rounding_error);
   }
   if (std::numeric_limits<T>::digits >= std::numeric_limits<long>::digits)
   {
      BOOST_CHECK_THROW(result = static_cast<T>(lround(static_cast<T>((std::numeric_limits<long>::max)()) + 1)), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(lround(static_cast<T>((std::numeric_limits<long>::min)()) - 1)), boost::math::rounding_error);
   }
#ifndef BOOST_NO_LONG_LONG
   if (std::numeric_limits<T>::digits >= std::numeric_limits<long long>::digits)
   {
      BOOST_CHECK_THROW(result = static_cast<T>(llround(static_cast<T>((std::numeric_limits<long long>::max)()) + 1)), boost::math::rounding_error);
      BOOST_CHECK_THROW(result = static_cast<T>(llround(static_cast<T>((std::numeric_limits<long long>::min)()) - 1)), boost::math::rounding_error);
   }
#endif
#endif
}

int main()
{
#ifdef TEST_MPF_50
   test<boost::multiprecision::mpf_float_50>();
   test<boost::multiprecision::mpf_float_100>();
#endif
#ifdef TEST_MPFR_50
   test<boost::multiprecision::mpfr_float_50>();
   test<boost::multiprecision::mpfr_float_100>();
#endif
#ifdef TEST_MPFI_50
   test<boost::multiprecision::mpfi_float_50>();
   test<boost::multiprecision::mpfi_float_100>();
#endif
#ifdef TEST_CPP_DEC_FLOAT
   test<boost::multiprecision::cpp_dec_float_50>();
   test<boost::multiprecision::cpp_dec_float_100>();
#ifndef SLOW_COMPLER
   // Some "peculiar" digit counts which stress our code:
   test<boost::multiprecision::number<boost::multiprecision::cpp_dec_float<65> > >();
   test<boost::multiprecision::number<boost::multiprecision::cpp_dec_float<64> > >();
   test<boost::multiprecision::number<boost::multiprecision::cpp_dec_float<63> > >();
   test<boost::multiprecision::number<boost::multiprecision::cpp_dec_float<62> > >();
   test<boost::multiprecision::number<boost::multiprecision::cpp_dec_float<61, long long> > >();
   test<boost::multiprecision::number<boost::multiprecision::cpp_dec_float<60, long long> > >();
   test<boost::multiprecision::number<boost::multiprecision::cpp_dec_float<59, long long, std::allocator<char> > > >();
   test<boost::multiprecision::number<boost::multiprecision::cpp_dec_float<58, long long, std::allocator<char> > > >();
#endif
#endif
#ifdef TEST_CPP_BIN_FLOAT
   test<boost::multiprecision::cpp_bin_float_50>();
   test<boost::multiprecision::cpp_bin_float_100>();
   test<boost::multiprecision::number<boost::multiprecision::cpp_bin_float<35, boost::multiprecision::digit_base_10, std::allocator<char>, long long> > >();
#endif
#ifdef TEST_BACKEND
   test<boost::multiprecision::number<boost::multiprecision::concepts::number_backend_float_architype> >();
#endif
#ifdef TEST_FLOAT128
   test<boost::multiprecision::float128>();
#endif
#ifdef TEST_CPP_DOUBLE_FLOAT
   test<boost::multiprecision::cpp_double_float>();
   test<boost::multiprecision::cpp_double_double>();
   test<boost::multiprecision::cpp_double_long_double>();
   #if defined(BOOST_MP_CPP_DOUBLE_FP_HAS_FLOAT128)
   test<boost::multiprecision::cpp_double_float128>();
   #endif
#endif

   return boost::report_errors();
}
