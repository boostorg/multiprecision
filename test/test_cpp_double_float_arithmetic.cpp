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

// TBD: Handle interaction with Boost's wrap of libquadmath __float128.
// g++ -O3 -Wall -march=native -std=gnu++11 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 -DBOOST_MATH_USE_FLOAT128 test.cpp -o -lquadmath test_double_float.exe

#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <boost/config.hpp>
#include <boost/multiprecision/cpp_double_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#ifdef BOOST_MATH_USE_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/core/demangle.hpp>

namespace test_arithmetic_cpp_double_float {

template <typename FloatingPointType,
          typename std::enable_if<(  std::is_floating_point<FloatingPointType>::value                       == true
#ifdef BOOST_MATH_USE_FLOAT128
                                   ||std::is_same<FloatingPointType,boost::multiprecision::float128>::value == true
#endif
                                  ), bool>::type = true>
FloatingPointType uniform_real()
{
   using distribution_type = boost::random::uniform_real_distribution<FloatingPointType>;

   static unsigned long seed_scaler = 0U;

   static std::random_device rd;
   static std::mt19937       gen(rd());
   static distribution_type  dis(0.0, 1.0);

   if((seed_scaler % 0x200000UL) == 0U)
   {
     gen.seed(static_cast<typename std::mt19937::result_type>(std::clock()));
   }

   return dis(gen);
}

int rand_in_range(int a, int b)
{
   return a + int(float(b - a) * uniform_real<float>());
}

template <typename FloatingPointType,
          typename std::enable_if<(  std::is_floating_point<FloatingPointType>::value                       == true
#ifdef BOOST_MATH_USE_FLOAT128
                                   ||std::is_same<FloatingPointType,boost::multiprecision::float128>::value == true
#endif
                                  ), bool>::type = true>
FloatingPointType uniform_rand()
{
   return uniform_real<FloatingPointType>();
}

template <typename FloatingPointType>
boost::multiprecision::backends::cpp_double_float<typename FloatingPointType::float_type> uniform_rand()
{
   using float_type = typename FloatingPointType::float_type;
   return boost::multiprecision::backends::cpp_double_float<float_type>(uniform_real<float_type>()) * boost::multiprecision::backends::cpp_double_float<float_type>(uniform_real<float_type>());
}


template <typename FloatingPointType>
typename std::enable_if<(  std::is_floating_point<FloatingPointType>::value                       == true
#ifdef BOOST_MATH_USE_FLOAT128
                         ||std::is_same<FloatingPointType,boost::multiprecision::float128>::value == true
#endif
                        ), FloatingPointType>::type
log_rand()
{
   if (uniform_real<float>() < (1. / 100.))
      return 0; // throw in a few zeroes
   using std::ldexp;
   FloatingPointType ret = ldexp(uniform_real<FloatingPointType>(), rand_in_range(std::numeric_limits<FloatingPointType>::min_exponent, std::numeric_limits<FloatingPointType>::max_exponent));
   using std::fmax;
   return fmax(ret, std::numeric_limits<FloatingPointType>::epsilon());
}

template <typename FloatingPointType>
boost::multiprecision::backends::cpp_double_float<typename FloatingPointType::float_type> log_rand()
{
   boost::multiprecision::backends::cpp_double_float<typename FloatingPointType::float_type> a(uniform_rand<boost::multiprecision::backends::cpp_double_float<typename FloatingPointType::float_type> >() + typename FloatingPointType::float_type(1));
   a *= log_rand<typename FloatingPointType::float_type>();
   return a;
}

template <typename ConstructionType, typename FloatingPointType, typename std::enable_if<std::numeric_limits<FloatingPointType>::is_iec559>::type const* = nullptr>
ConstructionType construct_from(FloatingPointType f) {
   return ConstructionType(f);
}

template <typename ConstructionType, typename FloatingPointType, typename std::enable_if<!std::numeric_limits<FloatingPointType>::is_iec559>::type const* = nullptr>
ConstructionType construct_from(FloatingPointType f)
{
   return ConstructionType(f.first()) + ConstructionType(f.second());
}

template <typename FloatingPointType>
bool test_op(char op, const unsigned count = 0x20000U)
{
   using naked_double_float_type = FloatingPointType;
   using control_float_type      = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<naked_double_float_type>::digits10 * 2 + 1>, boost::multiprecision::et_off>;

   const control_float_type MaxError = boost::multiprecision::ldexp(control_float_type(1), -std::numeric_limits<naked_double_float_type>::digits + 0);
   std::cout << "testing operator" << op << " (accuracy = " << std::numeric_limits<naked_double_float_type>::digits << " bits)...";

   for (unsigned i = 0U; i < count; ++i)
   {
      naked_double_float_type  df_a   = log_rand<naked_double_float_type>();
      naked_double_float_type  df_b   = log_rand<naked_double_float_type>();
      const control_float_type ctrl_a = construct_from<control_float_type, naked_double_float_type>(df_a);
      const control_float_type ctrl_b = construct_from<control_float_type, naked_double_float_type>(df_b);

      naked_double_float_type df_c;
      control_float_type      ctrl_c;

      switch (op)
      {
      case '+':
         df_c   = df_a + df_b;
         ctrl_c = ctrl_a + ctrl_b;
         break;
      case '-':
         df_c   = df_a - df_b;
         ctrl_c = ctrl_a - ctrl_b;
         break;
      case '*':
         df_c   = df_a * df_b;
         ctrl_c = ctrl_a * ctrl_b;
         break;
      case '/':
         if (df_b == naked_double_float_type(0))
            continue;
         df_c   = df_a / df_b;
         ctrl_c = ctrl_a / ctrl_b;
         break;
      default:
         std::cerr << " internal error (unknown operator: " << op << ")" << std::endl;
         return false;
      }

      // if exponent of result is out of range, continue
      int exp2;
      boost::multiprecision::frexp(ctrl_c, &exp2);
      if (exp2 > std::numeric_limits<naked_double_float_type>::max_exponent || exp2 < std::numeric_limits<naked_double_float_type>::min_exponent)
         continue;

      control_float_type ctrl_df_c = construct_from<control_float_type, naked_double_float_type>(df_c);

      const auto delta = fabs(1 - fabs(ctrl_c / ctrl_df_c));

      if (delta > MaxError)
      {
         std::cerr << std::setprecision(std::numeric_limits<naked_double_float_type>::digits10 + 2);
         std::cerr << " [FAILED] while performing '" << std::setprecision(100000) << ctrl_a << "' " << op << " '" << ctrl_b << "', got incorrect result: " << (df_c) << std::endl;

         // uncomment for more debugging information (only for cpp_double_float<> type)
         //std::cerr << "(df_a = " << df_a.get_raw_str() << ", df_b = " << df_b.get_raw_str() << ")" << std::endl;
         //std::cerr << "expected: " << ctrl_c << std::endl;
         //std::cerr << "actual  : " << ctrl_df_c << " (" << df_c.get_raw_str() << ")" << std::endl;
         //std::cerr << "error   : " << delta << std::endl;

         return false;
      }
   }

  std::cout << " ok [" << count << " tests passed]" << std::endl;

  return true;
}

template <typename T>
bool test_arithmetic()
{
   std::cout << "Testing correctness of arithmetic operators for " << boost::core::demangle(typeid(T).name()) << std::endl;

   bool result_is_ok = true;

   result_is_ok &= test_op<T>('+');
   result_is_ok &= test_op<T>('-');
   result_is_ok &= test_op<T>('*');
   result_is_ok &= test_op<T>('/');

   std::cout << std::endl;

   return result_is_ok;
}

} // namespace test_arithmetic_cpp_double_float

int main()
{
   bool result_is_ok = true;

   // uncomment to check if tests themselves are correct
   //result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<float>();
   //result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<double>();
   //result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<long double>();
#ifdef BOOST_MATH_USE_FLOAT128
   //result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::float128>();
#endif

   result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::backends::cpp_double_float<float> >();
   result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::backends::cpp_double_float<double> >();
   result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::backends::cpp_double_float<long double> >();
#ifdef BOOST_MATH_USE_FLOAT128
   result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::backends::cpp_double_float<boost::multiprecision::float128> >();
#endif

   return (result_is_ok ? 0 : -1);
}
