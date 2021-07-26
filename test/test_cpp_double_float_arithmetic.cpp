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

namespace test_arithmetic_cpp_double_float {

template<typename FloatingPointType,
         typename std::enable_if<(boost::multiprecision::backends::detail::is_floating_point_or_float128<FloatingPointType>::value == true), bool>::type = true>
FloatingPointType uniform_real(const FloatingPointType&)
{
   using distribution_type = boost::random::uniform_real_distribution<FloatingPointType>;

   static unsigned long seed_scaler = 0U;

   static std::random_device rd;
   static std::mt19937       gen(rd());
   static distribution_type  dis(FloatingPointType(0.0L), FloatingPointType(1.0L));

   if((seed_scaler % 0x100000UL) == 0U)
   {
     gen.seed(static_cast<typename std::mt19937::result_type>(std::clock()));
   }

   return dis(gen);
}

int rand_in_range(int a, int b)
{
   return a + int(float(b - a) * uniform_real<float>(1.0F));
}

template<typename FloatingPointType,
         typename std::enable_if<(boost::multiprecision::backends::detail::is_floating_point_or_float128<FloatingPointType>::value == true), bool>::type = true>
FloatingPointType uniform_rand(const FloatingPointType&)
{
   return uniform_real<FloatingPointType>(FloatingPointType(1.0F));
}

template <typename CompositeFloatType>
CompositeFloatType uniform_rand(const CompositeFloatType&)
{
   using float_type = typename CompositeFloatType::backend_type::float_type;

   return   CompositeFloatType(uniform_real<float_type>(float_type()))
          * CompositeFloatType(uniform_real<float_type>(float_type()));
}

template<typename FloatingPointType,
         typename std::enable_if<(boost::multiprecision::backends::detail::is_floating_point_or_float128<FloatingPointType>::value == true), bool>::type = true>
FloatingPointType log_rand(const FloatingPointType&)
{
   if (uniform_real<float>(1.0F) < (1.0F / 100.0F))
      return 0; // throw in a few zeroes
   using std::ldexp;
   using boost::multiprecision::ldexp;
   FloatingPointType ret = ldexp(uniform_real<FloatingPointType>(FloatingPointType(1.0F)), rand_in_range(std::numeric_limits<FloatingPointType>::min_exponent, std::numeric_limits<FloatingPointType>::max_exponent));
   using std::fmax;
   return fmax(ret, std::numeric_limits<FloatingPointType>::epsilon());
}

template<typename CompositeFloatType,
         typename std::enable_if<(boost::multiprecision::backends::detail::is_floating_point_or_float128<CompositeFloatType>::value == false), bool>::type = true>
CompositeFloatType log_rand(const CompositeFloatType&)
{
   using float_type = typename CompositeFloatType::backend_type::float_type;

   CompositeFloatType
   a
   (
       uniform_rand<CompositeFloatType>(CompositeFloatType(1.0F))
     + CompositeFloatType(float_type(1.0F))
   );

   a *= CompositeFloatType(log_rand<float_type>(float_type(1.0F)));

   return a;
}

template <typename ConstructionType, typename FloatingPointType, typename std::enable_if<std::numeric_limits<FloatingPointType>::is_iec559>::type const* = nullptr>
ConstructionType construct_from(FloatingPointType f)
{
   return ConstructionType(f);
}

template <typename ConstructionType, typename FloatingPointType, typename std::enable_if<!std::numeric_limits<FloatingPointType>::is_iec559>::type const* = nullptr>
ConstructionType construct_from(FloatingPointType f)
{
   const ConstructionType ct_x(ConstructionType::canonical_value(f).first());
   const ConstructionType ct_y(ConstructionType::canonical_value(f).second());

   const ConstructionType ct = ct_x + ct_y;

   return ct;
}

template <typename FloatingPointType>
bool test_op(char op, const unsigned count = 0x20000U)
{
   using naked_double_float_type = FloatingPointType;

   constexpr int d10 = std::numeric_limits<naked_double_float_type>::digits10 * 2 + 1;

   using control_float_type = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<d10>, boost::multiprecision::et_off>;

   using boost::multiprecision::ldexp;

   const control_float_type MaxError = ldexp(control_float_type(1), -std::numeric_limits<naked_double_float_type>::digits + 1);
   std::cout << "testing operator" << op << " (accuracy = " << std::numeric_limits<naked_double_float_type>::digits << " bits)...";

   for (unsigned i = 0U; i < count; ++i)
   {
      naked_double_float_type  df_a   = log_rand<naked_double_float_type>(naked_double_float_type(1.0F));
      naked_double_float_type  df_b   = log_rand<naked_double_float_type>(naked_double_float_type(1.0F));
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

   result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<float>,       boost::multiprecision::et_off>>();
   result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<double>,      boost::multiprecision::et_off>>();
   result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<long double>, boost::multiprecision::et_off>>();
#ifdef BOOST_MATH_USE_FLOAT128
   //result_is_ok &= test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<boost::multiprecision::float128>, boost::multiprecision::et_off>>();
#endif

   return (result_is_ok ? 0 : -1);
}
