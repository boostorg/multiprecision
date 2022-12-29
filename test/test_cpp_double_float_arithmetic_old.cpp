///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Test for correctness of arithmetic operators of cpp_double_fp_backend<>

// cd /mnt/c/Users/User/Documents/Ks/PC_Software/Test
// g++ -O3 -Wall -march=native -std=c++11 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 test.cpp -o test_double_float.exe

// TBD: Handle interaction with Boost's wrap of libquadmath __float128.
// g++ -O3 -Wall -march=native -std=gnu++11 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 -DBOOST_MATH_USE_FLOAT128 test.cpp -o -lquadmath test_double_float.exe

#include <boost/config.hpp>
#include <boost/multiprecision/number.hpp>
#ifdef BOOST_MATH_USE_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/cpp_double_fp_backend.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/core/demangle.hpp>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace test_arithmetic_cpp_double_float {
using control_float_type = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<100>, boost::multiprecision::et_off>;
constexpr int TestCases  = 1U << 14;

namespace detail {
#if defined(DISABLE_RNG_SEEDING)
static std::mt19937       rng;
#else
static std::random_device rd;
static std::mt19937       rng(rd());
#endif

template <typename T>
constexpr T(max)(T a, T b)
{
   return ((a > b) ? a : b);
}

template <typename T>
constexpr T(min)(T a, T b)
{
   return ((a < b) ? a : b);
}

template <typename UnsignedIntegralType>
constexpr typename std::enable_if<(std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true), UnsignedIntegralType>::type
negate(UnsignedIntegralType u)
{
   return (UnsignedIntegralType)(((UnsignedIntegralType)~u) + 1U);
}

template <typename SignedIntegralType>
constexpr typename std::enable_if<(std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true), SignedIntegralType>::type
negate(SignedIntegralType n)
{
   return (SignedIntegralType)detail::negate((unsigned long long)n);
}

} // namespace detail

template <typename NumericType,
          typename std::enable_if<std::is_integral<NumericType>::value, bool>::type = true>
NumericType get_rand(control_float_type mn = 0, control_float_type mx = 0)
{
   NumericType max_n = (std::numeric_limits<NumericType>::max)();
   if (mx != 0)
      max_n = (detail::min)(NumericType(mx), max_n);

   NumericType min_n = detail::negate(max_n);
   if (mn != 0)
      min_n = (detail::max)(NumericType(mn), min_n);

   static boost::random::uniform_int_distribution<NumericType> dis(min_n, max_n);

   return dis(detail::rng);
}

template <typename FloatingPointType,
          typename std::enable_if<boost::multiprecision::backends::detail::is_floating_point_or_float128<FloatingPointType>::value, bool>::type = true>
FloatingPointType get_rand(control_float_type mn = 0, control_float_type mx = 0)
{
   FloatingPointType max_n = (std::numeric_limits<FloatingPointType>::max)();
   if (mx != 0)
      max_n = (detail::min)(FloatingPointType(mx), max_n);

   FloatingPointType min_n = (std::numeric_limits<FloatingPointType>::lowest)();
   if (mn != 0)
      min_n = (detail::max)(FloatingPointType(mn), min_n);

   static boost::random::uniform_real_distribution<FloatingPointType> dis(min_n, max_n);

   return dis(detail::rng);
}

template <typename DoubleFloatType>
boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<typename DoubleFloatType::backend_type::float_type> >
get_rand(control_float_type mn = 0, control_float_type mx = 0)
{
   using float_type = typename DoubleFloatType::backend_type::float_type;

   int max_exp, min_exp;

   boost::multiprecision::frexp(mx, &max_exp);
   boost::multiprecision::frexp(mn, &min_exp);

   max_exp = (detail::min)(std::numeric_limits<DoubleFloatType>::max_exponent, max_exp);
   min_exp = (detail::max)(std::numeric_limits<DoubleFloatType>::min_exponent, min_exp);

   static boost::random::uniform_real_distribution<float_type> dis(-1.0F, 1.0F);
   static boost::random::uniform_int_distribution<int>         exponent_dis(min_exp, max_exp);

   auto shifted_rand = [&](int i) {
      using std::ldexp;
#ifdef BOOST_MATH_USE_FLOAT128
      using boost::multiprecision::ldexp;
#endif

      return ldexp(dis(detail::rng), -i * std::numeric_limits<float_type>::digits);
   };

   DoubleFloatType out;
   out.backend() = typename DoubleFloatType::backend_type(shifted_rand(0), shifted_rand(1));

   return boost::multiprecision::ldexp(out, exponent_dis(detail::rng));
}

template <typename ConstructionType, typename ArithmeticType, typename std::enable_if<(std::is_arithmetic<ArithmeticType>::value || boost::multiprecision::backends::detail::is_floating_point_or_float128<ArithmeticType>::value)>::type const* = nullptr>
ConstructionType construct_from(const ArithmeticType& f)
{
   return ConstructionType(f);
}

template <typename ConstructionType, typename DoubleFloatType>
ConstructionType construct_from(const boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<typename DoubleFloatType::backend_type::float_type> >& f)
{
   return ConstructionType(f.backend().first()) + ConstructionType(f.backend().second());
}

template <typename FloatingPointType>
bool test_op(char op, const unsigned count = TestCases)
{
   using double_float_type   = FloatingPointType;
   using double_float_limits = std::numeric_limits<double_float_type>;

   const control_float_type MaxError = boost::multiprecision::ldexp(control_float_type(1), 3-double_float_limits::digits);
   std::cout << "testing operator" << op << " (accuracy = " << double_float_limits::digits-3 << " bits)...";

   for (unsigned i = 0U; i < count; ++i)
   {
      const double_float_type  df_a   = get_rand<double_float_type>();
      const double_float_type  df_b   = get_rand<double_float_type>();
      const control_float_type ctrl_a = construct_from<control_float_type, double_float_type>(df_a);
      const control_float_type ctrl_b = construct_from<control_float_type, double_float_type>(df_b);

      double_float_type  df_c;
      control_float_type ctrl_c;

      auto out_of_range = [](const control_float_type& c) {
        return c > construct_from<control_float_type, double_float_type>(double_float_limits::max())
          ||   c < construct_from<control_float_type, double_float_type>(double_float_limits::lowest());
      };

      switch (op)
      {
      case '+':
         ctrl_c = ctrl_a + ctrl_b;

         if (out_of_range(ctrl_c)) continue;
         
         df_c   = df_a + df_b;
         break;
      case '-':
         ctrl_c = ctrl_a - ctrl_b;
         
         if (out_of_range(ctrl_c)) continue;
         
         df_c   = df_a - df_b;
         break;
      case '*':
         ctrl_c = ctrl_a * ctrl_b;
         
         if (out_of_range(ctrl_c)) continue;
         
         df_c = df_a * df_b;
         break;
      case '/':
         if (df_b == double_float_type(0))
            continue;
         
         ctrl_c = ctrl_a / ctrl_b;
         
         if (out_of_range(ctrl_c)) continue;
         
         df_c   = df_a / df_b;
         break;
      default:
         std::cerr << " internal error (unknown operator: " << op << ")" << std::endl;
         return false;
      }

      control_float_type ctrl_df_c = construct_from<control_float_type, double_float_type>(df_c);

      const auto delta = fabs(1 - fabs(ctrl_c / ctrl_df_c));

      if (delta > MaxError)
      {
         std::cerr << std::setprecision(std::numeric_limits<double_float_type>::digits10 + 3);
         std::cerr << std::showpos;
         std::cerr << " [FAILED]" << std::endl;

         // uncomment for more debugging information (only for cpp_double_fp_backend<> type)
         std::cerr << "a     = "             << df_a.backend().raw_str() << " (" << ctrl_a    << ")" << std::endl;
         std::cerr << "b     = "             << df_b.backend().raw_str() << " (" << ctrl_b    << ")" << std::endl;
         std::cerr << "a " << op << " b = "  << df_c.backend().raw_str() << " (" << ctrl_df_c << ")" << std::endl;
         std::cerr << "expected                                                " << ctrl_c           << std::endl;
         int mag;
         boost::multiprecision::frexp(delta, &mag);
         std::cerr << "magnitude of error: 2^" << mag << std::endl;

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

   result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::cpp_double_float>();
   result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::cpp_double_double>();
   result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::cpp_double_long_double> ();
#ifdef BOOST_MATH_USE_FLOAT128
   result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::cpp_double_float128> > ();
#endif

   return (result_is_ok ? 0 : -1);
}