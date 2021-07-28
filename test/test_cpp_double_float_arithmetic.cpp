///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Test for correctness of arithmetic operators of cpp_double_float<>



#include <boost/config.hpp>
#include <boost/multiprecision/cpp_double_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#ifdef BOOST_MATH_USE_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/core/demangle.hpp>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace test_arithmetic_cpp_double_float {
// FIXME: this looks like a duplicate from test_cpp_double_float_comparision.cpp file.
template <typename FloatingPointType>
struct is_floating_point
{
   static constexpr bool value = std::is_floating_point<FloatingPointType>::value
#ifdef BOOST_MATH_USE_FLOAT128
                                 or std::is_same<FloatingPointType, boost::multiprecision::float128>::value
#endif
       ;
};

template <typename FloatingPointType,
          typename std::enable_if<is_floating_point<FloatingPointType>::value, bool>::type = true>
FloatingPointType uniform_real()
{
   //static std::random_device                                rd;
   static std::mt19937                                      gen /*(rd())*/;
   static boost::random::uniform_real_distribution<FloatingPointType> dis(0.0, 1.0);

   return dis(gen);
}

int rand_in_range(int a, int b)
{
   return a + int(float(b - a) * uniform_real<float>());
}

template <typename FloatingPointType,
          typename std::enable_if<is_floating_point<FloatingPointType>::value, bool>::type = true>
FloatingPointType uniform_rand()
{
   return uniform_real<FloatingPointType>();
}

template <typename FloatingPointType>
boost::multiprecision::backends::cpp_double_float<typename FloatingPointType::float_type> uniform_rand()
{
   using float_type = typename FloatingPointType::float_type;
   return boost::multiprecision::backends::cpp_double_float<float_type>(uniform_rand<float_type>()) * boost::multiprecision::backends::cpp_double_float<float_type>(uniform_rand<float_type>());
}

template <typename FloatingPointType, typename std::enable_if<is_floating_point<FloatingPointType>::value>::type const* = nullptr>
FloatingPointType log_rand()
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

template <typename ConstructionType, typename ArithmeticType, typename std::enable_if<std::is_arithmetic<ArithmeticType>::value>::type const* = nullptr>
ConstructionType construct_from(ArithmeticType f)
{
   return ConstructionType(f);
}
template <typename ConstructionType, typename DoubleFloatType, typename std::enable_if<!std::is_arithmetic<DoubleFloatType>::value>::type const* = nullptr>
ConstructionType construct_from(DoubleFloatType f)
{
   return construct_from<ConstructionType, DoubleFloatType::float_type>(f.first()) + construct_from<ConstructionType, DoubleFloatType::float_type>(f.second());
}


template <typename FloatingPointType>
int test_op(char op, const unsigned count = 10000U)
{
   using naked_double_float_type = FloatingPointType;
   using control_float_type      = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<naked_double_float_type>::digits10 * 2 + 1>, boost::multiprecision::et_off>;

   const control_float_type MaxError = boost::multiprecision::ldexp(control_float_type(1), -std::numeric_limits<naked_double_float_type>::digits);
   std::cout << "testing operator" << op << " (accuracy = " << std::numeric_limits<naked_double_float_type>::digits << " bits)...";

   int tests_passed = 0;
   for (unsigned i = 0U; i < count; ++i)
   {
      naked_double_float_type  df_a   = log_rand<naked_double_float_type>();
      naked_double_float_type  df_b   = log_rand<naked_double_float_type>();
      const control_float_type ctrl_a = construct_from<control_float_type, naked_double_float_type>(df_a);
      const control_float_type ctrl_b = construct_from<control_float_type, naked_double_float_type>(df_b);

      naked_double_float_type df_c;
      control_float_type      ctrl_c;

      auto is_out_of_range = [&]() {
         // if exponent of result is out of range
         int exp2;
         boost::multiprecision::frexp(ctrl_c, &exp2);
         if (exp2 > std::numeric_limits<naked_double_float_type>::max_exponent || exp2 < std::numeric_limits<naked_double_float_type>::min_exponent)
            return true;
         return false;
      };
      switch (op)
      {
      case '+':
         ctrl_c = ctrl_a + ctrl_b;
         if (is_out_of_range())
           continue;
         df_c   = df_a + df_b;
         break;
      case '-':
         ctrl_c = ctrl_a - ctrl_b;
         if (is_out_of_range())
            continue;
         df_c = df_a - df_b;
         break;
      case '*':
         ctrl_c = ctrl_a * ctrl_b;
         if (is_out_of_range())
            continue;
         df_c = df_a * df_b;
         break;
      case '/':
         if (df_b == naked_double_float_type(0))
            continue;
         ctrl_c = ctrl_a / ctrl_b;
         if (is_out_of_range())
            continue;
         df_c = df_a / df_b;
         break;
      default:
         std::cerr << " internal error (unknown operator: " << op << ")" << std::endl;
         return -1;
      }

      control_float_type ctrl_df_c = construct_from<control_float_type, naked_double_float_type>(df_c);

      const auto delta = fabs(1 - fabs(ctrl_c / ctrl_df_c));

      if (delta > MaxError)
      {
         std::cerr << std::setprecision(std::numeric_limits<naked_double_float_type>::digits10 + 2);
         std::cerr << " [FAILED] while performing '" /*<< std::setprecision(100000)*/ << ctrl_a << "' " << op << " '" << ctrl_b << std::endl;

         // uncomment for more debugging information (only for cpp_double_float<> type)
         std::cerr << "a       : " << df_a.get_raw_str() << std::endl;

         std::cerr << "b       : " << df_b.get_raw_str() << std::endl;
         std::cerr << "expected: " << ctrl_c << std::endl;
         std::cerr << "actual  : " << ctrl_df_c << " (" << df_c.get_raw_str() << ")" << std::endl;
         std::cerr << "error   : " << delta << std::endl;
         std::cerr << "MaxError: " << MaxError << std::endl;

         return -1;
      }

      tests_passed++;
   }

  std::cout << " ok [" << tests_passed << " tests passed]" << std::endl;
  return 0;
}

template <typename T>
bool test_arithmetic()
{
   std::cout << "Testing correctness of arithmetic operators for " << boost::core::demangle(typeid(T).name()) << std::endl;

   int e = 0;
   e += test_op<T>('+');
   e += test_op<T>('-');
   e += test_op<T>('*');
   e += test_op<T>('/');

   std::cout << std::endl;

   return e;
}

} // namespace test_arithmetic_cpp_double_float


int main()
{
   int e = 0;
   // uncomment to check if tests themselves are correct
//   e += test_arithmetic_cpp_double_float::test_arithmetic<float>();
//   e += test_arithmetic_cpp_double_float::test_arithmetic<double>();
//   e += test_arithmetic_cpp_double_float::test_arithmetic<long double>();
//#ifdef BOOST_MATH_USE_FLOAT128
//   e += test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::float128>();
//#endif

   //e += test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::backends::cpp_double_float<float> >();
   //e += test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::backends::cpp_double_float<double> >();
   // e += test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::backends::cpp_double_float<long double> >();
#ifdef BOOST_MATH_USE_FLOAT128
   e += test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::backends::cpp_double_float<boost::multiprecision::float128> >();
#endif
   e += test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::backends::cpp_double_float<boost::multiprecision::backends::cpp_double_float<double> > >();
   e += test_arithmetic_cpp_double_float::test_arithmetic<boost::multiprecision::backends::cpp_double_float<boost::multiprecision::backends::cpp_double_float<float> > >();
   return e;
}
