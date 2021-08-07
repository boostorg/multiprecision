///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Test for correctness of arithmetic operators of cpp_double_float<>

// cd /mnt/c/Users/User/Documents/Ks/PC_Software/Test
// g++ -O3 -Wall -march=native -std=c++11 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 test.cpp -o test_double_float.exe

// Handle interaction with Boost's wrap of libquadmath __float128.
// g++ -O3 -Wall -march=native -std=gnu++11 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 -DBOOST_MATH_USE_FLOAT128 test.cpp -lquadmath -o test_double_float.exe

#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <boost/config.hpp>
#include <boost/multiprecision/number.hpp>
#ifdef BOOST_MATH_USE_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/cpp_quad_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/multiprecision/traits/max_digits10.hpp>
#include <boost/core/demangle.hpp>

#if defined(__clang__)
  #if defined __has_feature && (__has_feature(thread_sanitizer) || __has_feature(address_sanitizer))
  #define CPP_DOUBLE_FLOAT_REDUCE_TEST_DEPTH
  #endif
#elif defined(__GNUC__)
  #if defined(__SANITIZE_THREAD__) || defined(__SANITIZE_ADDRESS__)
  #define CPP_DOUBLE_FLOAT_REDUCE_TEST_DEPTH
  #endif
#endif

namespace local
{
  std::mt19937       engine_man;
  std::ranlux24_base engine_sgn;

  template<typename FloatingPointConstituentType>
  struct control
  {
    using float_type = FloatingPointConstituentType;

    static constexpr int digits       = 4 * std::numeric_limits<float_type>::digits;
    static constexpr int digits10     = boost::multiprecision::detail::calc_digits10<digits>::value;
    static constexpr int max_digits10 = boost::multiprecision::detail::calc_max_digits10<digits>::value;

    static unsigned seed_prescaler;

    using double_float_type  = boost::multiprecision::number<boost::multiprecision::backends::cpp_quad_float<float_type>, boost::multiprecision::et_off>;
    using control_float_type = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<(2 * std::numeric_limits<double_float_type>::digits10) + 1>, boost::multiprecision::et_off>;

    static_assert( digits       == std::numeric_limits<double_float_type>::digits       , "" );
    static_assert( digits10     == std::numeric_limits<double_float_type>::digits10     , "" );
    static_assert( max_digits10 == std::numeric_limits<double_float_type>::max_digits10 , "" );

    template<const std::size_t DigitsToGet = digits10>
    static void get_random_fixed_string(std::string& str, const bool is_unsigned = false)
    {
      // This string generator creates strings of the form
      // 0.458279387.... E+5
      // -0.7182937534953.... E-126
      // etc., where the string can be either positive only
      // (positive only via setting is_unsigned to true)
      // or mixed positive/negative.

      // Re-seed the random engine each approx. 16k calls
      // of this string generator.

      if((seed_prescaler % 0x4000U) == 0U)
      {
        const std::clock_t seed_time_stamp = std::clock();

        engine_man.seed(static_cast<typename std::mt19937::result_type>      (seed_time_stamp));
        engine_sgn.seed(static_cast<typename std::ranlux24_base::result_type>(seed_time_stamp));
      }

      ++seed_prescaler;

      static std::uniform_int_distribution<unsigned>
      dist_sgn
      (
        0,
        1
      );

      static std::uniform_int_distribution<unsigned>
      dist_first
      (
        1,
        9
      );

      static std::uniform_int_distribution<unsigned>
      dist_following
      (
        0,
        9
      );

      const bool is_neg = ((is_unsigned == false) && (dist_sgn(engine_sgn) != 0));

      // Use DigitsToGet + 2, where +2 represents the lenth of "0.".
      std::string::size_type len = static_cast<std::string::size_type>(DigitsToGet + 2);

      std::string::size_type pos = 0U;

      if(is_neg)
      {
        ++len;

        str.resize(len);

        str.at(pos) = char('-');

        ++pos;
      }
      else
      {
        str.resize(len);
      }

      str.at(pos) = static_cast<char>('0');
      ++pos;

      str.at(pos) = static_cast<char>('.');
      ++pos;

      str.at(pos) = static_cast<char>(dist_first(engine_man) + 0x30U);
      ++pos;

      while(pos < str.length())
      {
          str.at(pos) = static_cast<char>(dist_following(engine_man) + 0x30U);
          ++pos;
      }

      const bool exp_is_neg = (dist_sgn(engine_sgn) != 0);

      // TBD: Use even more extreme base-10 exponents if desired/possible
      // and base these on the actual range of the exponent10 member of limits.
      // The use of the digits member here is a strange workaround that
      // still needs to be investigated on GCC's 10-bit x86 long double.
      using local_exp10_float_type =
         typename std::conditional<(std::is_same<float_type, long double>::value == true), double, float_type>::type;

      static std::uniform_int_distribution<unsigned>
      dist_exp
      (
        0,
          ((std::numeric_limits<local_exp10_float_type>::max_exponent10 > 1000) ? 1183
        : ((std::numeric_limits<local_exp10_float_type>::max_exponent10 >  200) ?   83
        : ((std::numeric_limits<local_exp10_float_type>::max_exponent10 >   20) ?    7 : 1)))
      );

      std::string str_exp = ((exp_is_neg == false) ? "E+" :  "E-");

      {
        std::stringstream strm;

        strm << dist_exp(engine_man);

        str_exp += strm.str();
      }

      str += str_exp;
    }

    template<typename ConstructionType>
    static ConstructionType construct_from(const double_float_type& f)
    {
      return   ConstructionType(std::get<0>(double_float_type::canonical_value(f).crep()))
             + ConstructionType(std::get<1>(double_float_type::canonical_value(f).crep()))
             + ConstructionType(std::get<2>(double_float_type::canonical_value(f).crep()))
             + ConstructionType(std::get<3>(double_float_type::canonical_value(f).crep()))
             ;
    }

    static bool test_add__(const std::uint32_t count)
    {
      bool result_is_ok = true;

      const control_float_type MaxError = ldexp(control_float_type(1), 4 - std::numeric_limits<double_float_type>::digits);

      for(std::uint32_t i = 0U; ((i < count) && result_is_ok); ++i)
      {
        std::string str_a;
        std::string str_b;

        control<float_type>::get_random_fixed_string<digits10 + 4>(str_a);
        control<float_type>::get_random_fixed_string<digits10 + 4>(str_b);

        const double_float_type  df_a(str_a);
        const double_float_type  df_b(str_b);

        const control_float_type ctrl_a = construct_from<control_float_type>(df_a);
        const control_float_type ctrl_b = construct_from<control_float_type>(df_b);

        double_float_type  df_c    = df_a   + df_b;
        control_float_type ctrl_c  = ctrl_a + ctrl_b;

        const control_float_type delta = fabs(1 - fabs(ctrl_c / construct_from<control_float_type>(df_c)));

        const bool b_ok = (delta < MaxError);

        result_is_ok &= b_ok;
      }

      return result_is_ok;
    }

    static bool test_sub__(const std::uint32_t count)
    {
      bool result_is_ok = true;

      const control_float_type MaxError = ldexp(control_float_type(1), 4 - std::numeric_limits<double_float_type>::digits);

      for(std::uint32_t i = 0U; ((i < count) && result_is_ok); ++i)
      {
        std::string str_a;
        std::string str_b;

        control<float_type>::get_random_fixed_string<digits10 + 4>(str_a);
        control<float_type>::get_random_fixed_string<digits10 + 4>(str_b);

        const double_float_type  df_a(str_a);
        const double_float_type  df_b(str_b);

        const control_float_type ctrl_a = construct_from<control_float_type>(df_a);
        const control_float_type ctrl_b = construct_from<control_float_type>(df_b);

        double_float_type  df_c   = df_a   - df_b;
        control_float_type ctrl_c = ctrl_a - ctrl_b;

        const control_float_type delta = fabs(1 - fabs(ctrl_c / construct_from<control_float_type>(df_c)));

        const bool b_ok = (delta < MaxError);

        result_is_ok &= b_ok;
      }

      return result_is_ok;
    }

    static bool test_mul__(const std::uint32_t count)
    {
      bool result_is_ok = true;

      const control_float_type MaxError = ldexp(control_float_type(1), 10 - std::numeric_limits<double_float_type>::digits);

      for(std::uint32_t i = 0U; ((i < count) && result_is_ok); ++i)
      {
        std::string str_a;
        std::string str_b;

        control<float_type>::get_random_fixed_string<digits10 + 4>(str_a);
        control<float_type>::get_random_fixed_string<digits10 + 4>(str_b);

        const double_float_type df_a(str_a);
        const double_float_type df_b(str_b);

        const control_float_type ctrl_a = construct_from<control_float_type>(df_a);
        const control_float_type ctrl_b = construct_from<control_float_type>(df_b);

        double_float_type  df_c   = df_a   * df_b;
        control_float_type ctrl_c = ctrl_a * ctrl_b;

        const control_float_type delta = fabs(1 - fabs(ctrl_c / construct_from<control_float_type>(df_c)));

        const bool b_ok = (delta < MaxError);

        result_is_ok &= b_ok;
      }

      return result_is_ok;
    }

    static bool test_div__(const std::uint32_t count)
    {
      bool result_is_ok = true;

      const control_float_type MaxError = ldexp(control_float_type(1), 10 - std::numeric_limits<double_float_type>::digits);

      for(std::uint32_t i = 0U;((i < count) && result_is_ok); ++i)
      {
        std::string str_a;
        std::string str_b;

        control<float_type>::get_random_fixed_string<digits10 + 4>(str_a);
        control<float_type>::get_random_fixed_string<digits10 + 4>(str_b);

        const double_float_type  df_a  (str_a);
        const double_float_type  df_b  (str_b);

        const control_float_type ctrl_a = construct_from<control_float_type>(df_a);
        const control_float_type ctrl_b = construct_from<control_float_type>(df_b);

        const double_float_type  df_c   = df_a   / df_b;
        const control_float_type ctrl_c = ctrl_a / ctrl_b;

        const control_float_type delta = fabs(1 - fabs(ctrl_c / construct_from<control_float_type>(df_c)));

        const bool b_ok = (delta < MaxError);

        result_is_ok &= b_ok;
      }

      return result_is_ok;
    }

    static bool test_sqrt_(const std::uint32_t count)
    {
      bool result_is_ok = true;

      const control_float_type MaxError = ldexp(control_float_type(1), 6 - std::numeric_limits<double_float_type>::digits);

      for(std::uint32_t i = 0U; ((i < count) && result_is_ok); ++i)
      {
        std::string str_a;
        std::string str_b;

        control<float_type>::get_random_fixed_string<digits10 + 4>(str_a, true);

        const double_float_type  df_a(str_a);

        const control_float_type ctrl_a = construct_from<control_float_type>(df_a);

        double_float_type  df_c   = sqrt(df_a);
        control_float_type ctrl_c = sqrt(ctrl_a);

        const control_float_type delta = fabs(1 - fabs(ctrl_c / construct_from<control_float_type>(df_c)));

        const bool b_ok = (delta < MaxError);

        result_is_ok &= b_ok;
      }

      return result_is_ok;
    }
  };

  template<typename FloatingPointConstituentType> unsigned control<FloatingPointConstituentType>::seed_prescaler;

  template<typename FloatingPointConstituentType>
  bool test_arithmetic(const std::uint32_t count)
  {
    using float_type = FloatingPointConstituentType;

    std::cout << "Testing " << count << " arithmetic cases for constituent float type = " << typeid(FloatingPointConstituentType).name() << "..." << std::endl;

    const bool result_add___is_ok = control<float_type>::test_add__(count); std::cout << "result_add___is_ok: " << std::boolalpha << result_add___is_ok << std::endl;
    const bool result_sub___is_ok = control<float_type>::test_sub__(count); std::cout << "result_sub___is_ok: " << std::boolalpha << result_sub___is_ok << std::endl;
    const bool result_mul___is_ok = control<float_type>::test_mul__(count); std::cout << "result_mul___is_ok: " << std::boolalpha << result_mul___is_ok << std::endl;
    const bool result_div___is_ok = control<float_type>::test_div__(count); std::cout << "result_div___is_ok: " << std::boolalpha << result_div___is_ok << std::endl;
    const bool result_sqrt__is_ok = control<float_type>::test_sqrt_(count); std::cout << "result_sqrt__is_ok: " << std::boolalpha << result_sqrt__is_ok << std::endl;

    const bool result_all_is_ok = (   result_add___is_ok
                                   && result_sub___is_ok
                                   && result_mul___is_ok
                                   && result_div___is_ok
                                   && result_sqrt__is_ok);

    return result_all_is_ok;
  }
}

int main()
{
  #if !defined(CPP_DOUBLE_FLOAT_REDUCE_TEST_DEPTH)
  constexpr unsigned int test_cases_built_in = (unsigned int) (1ULL << 16U);
  #else
  constexpr unsigned int test_cases_built_in = (unsigned int) (1ULL << 12U);
  #endif

  #if !defined(CPP_DOUBLE_FLOAT_REDUCE_TEST_DEPTH)
  constexpr unsigned int test_cases_float128 = (unsigned int) (1ULL << 10U);
  #else
  constexpr unsigned int test_cases_float128 = (unsigned int) (1ULL <<  6U);
  #endif

  const bool result_flt___is_ok = local::test_arithmetic<float>      (test_cases_built_in); std::cout << "result_flt___is_ok: " << std::boolalpha << result_flt___is_ok << std::endl;
  const bool result_dbl___is_ok = local::test_arithmetic<double>     (test_cases_built_in); std::cout << "result_dbl___is_ok: " << std::boolalpha << result_dbl___is_ok << std::endl;
  const bool result_ldbl__is_ok = local::test_arithmetic<long double>(test_cases_built_in); std::cout << "result_ldbl__is_ok: " << std::boolalpha << result_ldbl__is_ok << std::endl;

#ifdef BOOST_MATH_USE_FLOAT128
  const bool result_f128__is_ok = local::test_arithmetic<boost::multiprecision::float128>(test_cases_float128);
                                                                                            std::cout << "result_f128__is_ok: " << std::boolalpha << result_f128__is_ok << std::endl;
#else
   (void) test_cases_float128;
#endif

  const bool result_is_ok =
  (
      result_flt___is_ok
   && result_dbl___is_ok
   && result_ldbl__is_ok
#ifdef BOOST_MATH_USE_FLOAT128
   && result_f128__is_ok
#endif
  );

  return (result_is_ok ? 0 : -1);
}
