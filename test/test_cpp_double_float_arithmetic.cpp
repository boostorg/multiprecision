#if 1

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
#include <boost/multiprecision/cpp_double_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/random/uniform_real_distribution.hpp>
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

namespace local {

template<typename ConstituentFloatType>
struct control
{
   static constexpr int digits         = (2 * std::numeric_limits<ConstituentFloatType>::digits) - 2;
   static constexpr int digits10       = int(float(digits - 1) * 0.301F);
   static constexpr int max_digits10   = int(float(digits) * 0.301F) + 2;
   static constexpr int max_exponent10 = std::numeric_limits<ConstituentFloatType>::max_exponent10;

   using double_float_type  = boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<ConstituentFloatType>, boost::multiprecision::et_off>;
   using control_float_type = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<(2 * std::numeric_limits<double_float_type>::digits10) + 1>, boost::multiprecision::et_off>;

   using random_engine_type = std::linear_congruential_engine<std::uint32_t, 48271, 0, 2147483647>;

   static std::mt19937       engine_man;
   static std::ranlux24_base engine_sgn;
   static random_engine_type engine_dec_pt;

   template<const std::size_t DigitsToGet = digits10>
   static void get_random_fixed_string(std::string& str, const bool is_unsigned = false)
   {
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

      static std::uniform_int_distribution<unsigned>
      dist_exp
      (
         0,
         unsigned(float(max_exponent10) * 0.15F)
      );

      std::string str_exp = ((exp_is_neg == false) ? "E+" :  "E-");

      {
         std::stringstream strm;

         strm << dist_exp(engine_man);

         str_exp += strm.str();
      }

      str += str_exp;
   }
};

template<typename ConstituentFloatType> constexpr int control<ConstituentFloatType>::digits;
template<typename ConstituentFloatType> constexpr int control<ConstituentFloatType>::digits10;
template<typename ConstituentFloatType> constexpr int control<ConstituentFloatType>::max_digits10;
template<typename ConstituentFloatType> constexpr int control<ConstituentFloatType>::max_exponent10;

template<typename ConstituentFloatType> std::mt19937       control<ConstituentFloatType>::engine_man(static_cast<typename std::mt19937::result_type>(std::clock()));
template<typename ConstituentFloatType> std::ranlux24_base control<ConstituentFloatType>::engine_sgn(static_cast<typename std::ranlux24_base::result_type>(std::clock()));
template<typename ConstituentFloatType> typename control<ConstituentFloatType>::random_engine_type control<ConstituentFloatType>::engine_dec_pt(static_cast<typename random_engine_type::result_type>(std::clock()));

template <typename ConstituentFloatType>
bool test_op(char op, const unsigned count = 10000U)
{
   using float_type         = ConstituentFloatType;
   using double_float_type  = typename control<float_type>::double_float_type;
   using control_float_type = typename control<float_type>::control_float_type;

   const control_float_type MaxError = boost::multiprecision::ldexp(control_float_type(1), -std::numeric_limits<double_float_type>::digits + 2);
   std::cout << "testing operator" << op << " (accuracy = " << std::numeric_limits<double_float_type>::digits << " bits)...";

   for (unsigned i = 0U; i < count; ++i)
   {
      std::string str_a;
      std::string str_b;

      control<float_type>::get_random_fixed_string(str_a);
      control<float_type>::get_random_fixed_string(str_b);

      const double_float_type  df_a(str_a);
      const double_float_type  df_b(str_b);

      #if 0
      const control_float_type ctrl_a =   control_float_type(double_float_type::canonical_value(df_a).crep().first.crep().first)
                                        + control_float_type(double_float_type::canonical_value(df_a).crep().first.crep().second)
                                        + control_float_type(double_float_type::canonical_value(df_a).crep().second.crep().first)
                                        + control_float_type(double_float_type::canonical_value(df_a).crep().second.crep().second)
                                        ;
      const control_float_type ctrl_b =   control_float_type(double_float_type::canonical_value(df_b).crep().first.crep().first)
                                        + control_float_type(double_float_type::canonical_value(df_b).crep().first.crep().second)
                                        + control_float_type(double_float_type::canonical_value(df_b).crep().second.crep().first)
                                        + control_float_type(double_float_type::canonical_value(df_b).crep().second.crep().second)
                                        ;
      #else
      const control_float_type ctrl_a =   control_float_type(double_float_type::canonical_value(df_a).crep().first)
                                        + control_float_type(double_float_type::canonical_value(df_a).crep().second)
                                        ;
      const control_float_type ctrl_b =   control_float_type(double_float_type::canonical_value(df_b).crep().first)
                                        + control_float_type(double_float_type::canonical_value(df_b).crep().second)
                                        ;
      #endif

      double_float_type  df_c;
      control_float_type ctrl_c;

      switch (op)
      {
      default:
      case '+':
         df_c   = df_a   + df_b;
         ctrl_c = ctrl_a + ctrl_b;
         break;
      case '-':
         df_c   = df_a   - df_b;
         ctrl_c = ctrl_a - ctrl_b;
         break;
      case '*':
         df_c   = df_a   * df_b;
         ctrl_c = ctrl_a * ctrl_b;
         break;
      case '/':
         if (df_b != double_float_type(0))
         {
            df_c   = df_a   / df_b;
            ctrl_c = ctrl_a / ctrl_b;
         }
         else
         {
            continue;
         }
         break;
      }

      #if 0
      const control_float_type ctrl_df_c =   control_float_type(double_float_type::canonical_value(df_c).crep().first.crep().first)
                                           + control_float_type(double_float_type::canonical_value(df_c).crep().first.crep().second)
                                           + control_float_type(double_float_type::canonical_value(df_c).crep().second.crep().first)
                                           + control_float_type(double_float_type::canonical_value(df_c).crep().second.crep().second)
                                           ;
      #else
      const control_float_type ctrl_df_c =   control_float_type(double_float_type::canonical_value(df_c).crep().first)
                                           + control_float_type(double_float_type::canonical_value(df_c).crep().second)
                                           ;
      #endif

      const control_float_type delta = fabs(1 - fabs(ctrl_c / ctrl_df_c));

      if (delta > MaxError)
      {
         std::cerr << std::setprecision(std::numeric_limits<double_float_type>::digits10 + 2);
         std::cerr << " [FAILED] while performing '" << std::setprecision(100000) << ctrl_a << "' " << op << " '" << ctrl_b << "', got incorrect result: " << (df_c) << std::endl;

         return false;
      }
   }

  std::cout << " ok [" << count << " tests passed]" << std::endl;

  return true;
}

template <typename ConstituentFloatType>
bool test_sqrt(const unsigned count = 10000U)
{
   using float_type         = ConstituentFloatType;
   using double_float_type  = typename control<float_type>::double_float_type;
   using control_float_type = typename control<float_type>::control_float_type;

   const control_float_type MaxError = boost::multiprecision::ldexp(control_float_type(1), -std::numeric_limits<double_float_type>::digits + 2);
   std::cout << "testing func sqrt (accuracy = " << std::numeric_limits<double_float_type>::digits << " bits)...";

   for (unsigned i = 0U; i < count; ++i)
   {
      std::string str_a;

      control<float_type>::get_random_fixed_string(str_a, true);

      const double_float_type  df_a(str_a);

      #if 0
      const control_float_type ctrl_a =   control_float_type(double_float_type::canonical_value(df_a).crep().first.crep().first)
                                        + control_float_type(double_float_type::canonical_value(df_a).crep().first.crep().second)
                                        + control_float_type(double_float_type::canonical_value(df_a).crep().second.crep().first)
                                        + control_float_type(double_float_type::canonical_value(df_a).crep().second.crep().second)
                                        ;
      #else
      const control_float_type ctrl_a =   control_float_type(double_float_type::canonical_value(df_a).crep().first)
                                        + control_float_type(double_float_type::canonical_value(df_a).crep().second)
                                        ;
      #endif

      double_float_type  df_c;
      control_float_type ctrl_c;

      df_c   = sqrt(df_a);
      ctrl_c = sqrt(ctrl_a);

      #if 0
      const control_float_type ctrl_df_c =   control_float_type(double_float_type::canonical_value(df_c).crep().first.crep().first)
                                           + control_float_type(double_float_type::canonical_value(df_c).crep().first.crep().second)
                                           + control_float_type(double_float_type::canonical_value(df_c).crep().second.crep().first)
                                           + control_float_type(double_float_type::canonical_value(df_c).crep().second.crep().second)
                                           ;
      #else
      const control_float_type ctrl_df_c =   control_float_type(double_float_type::canonical_value(df_c).crep().first)
                                           + control_float_type(double_float_type::canonical_value(df_c).crep().second)
                                           ;
      #endif

      const control_float_type delta = fabs(1 - fabs(ctrl_c / ctrl_df_c));

      if (delta > MaxError)
      {
         std::cerr << std::setprecision(std::numeric_limits<double_float_type>::digits10 + 2);
         std::cerr << " [FAILED] while performing '" << std::setprecision(100000) << ctrl_a << "' sqrt', got incorrect result: " << (df_c) << std::endl;

         return false;
      }
   }

  std::cout << " ok [" << count << " tests passed]" << std::endl;

  return true;
}

template <typename T>
bool test_arithmetic(const unsigned count = 10000U)
{
   std::cout << "Testing correctness of arithmetic operators for " << boost::core::demangle(typeid(T).name()) << std::endl;

   bool result_is_ok = true;

   result_is_ok &= test_op  <T>('+', count);
   result_is_ok &= test_op  <T>('-', count);
   result_is_ok &= test_op  <T>('*', count);
   result_is_ok &= test_op  <T>('/', count);
   result_is_ok &= test_sqrt<T>(count);

   std::cout << std::endl;

   return result_is_ok;
}

} // namespace local

int main()
{
   bool result_is_ok = true;

   #if defined(CPP_DOUBLE_FLOAT_REDUCE_TEST_DEPTH)
   constexpr unsigned int test_cases_built_in = 10000U;
   #else
   constexpr unsigned int test_cases_built_in = 1000000U;
   #endif

   #if defined(CPP_DOUBLE_FLOAT_REDUCE_TEST_DEPTH)
   constexpr unsigned int test_cases_float128 = 1000U;
   #else
   constexpr unsigned int test_cases_float128 = 20000U;
   #endif

   result_is_ok &= local::test_arithmetic<float>(test_cases_built_in);
   result_is_ok &= local::test_arithmetic<double>(test_cases_built_in);
   //result_is_ok &= local::test_arithmetic<long double>(test_cases_built_in);
#ifdef BOOST_MATH_USE_FLOAT128
   result_is_ok &= local::test_arithmetic<boost::multiprecision::float128>(test_cases_float128);
#else
   (void) test_cases_float128;
#endif

   return (result_is_ok ? 0 : -1);
}

#else

///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include <boost/multiprecision/cpp_double_float.hpp>

#include "test_arithmetic.hpp"

// cd /mnt/c/Users/User/Documents/Ks/PC_Software/Test
// g++ -O3 -Wall -march=native -std=c++11 -I/mnt/c/boost/modular_boost/boost/libs/multiprecision/test -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 test.cpp -o test_arithmetic.exe

int main()
{
   using double_float_type = boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<double>, boost::multiprecision::et_off>;

   test<double_float_type>();
   return boost::report_errors();
}

#endif
