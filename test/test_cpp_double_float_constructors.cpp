///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 - 2023 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Constructor tests for cpp_double_fp_backend<>

#include <cstdlib>
#include <iostream>
#include <numeric>
#include <random>

#include <boost/config.hpp>
#include <boost/core/demangle.hpp>
#include <boost/multiprecision/number.hpp>
#ifdef BOOST_HAS_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/cpp_double_fp.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace test_cpp_double_float_constructors {
using control_float_type = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<100>, boost::multiprecision::et_off>;
constexpr int TestCases  = 1U << 14;

namespace detail {

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

   static std::random_device                                   rd;
   static std::mt19937                                         gen(rd());
   static boost::random::uniform_int_distribution<NumericType> dis(min_n, max_n);

   return dis(gen);
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

   static std::random_device                                          rd;
   static std::mt19937                                                gen(rd());
   static boost::random::uniform_real_distribution<FloatingPointType> dis(min_n, max_n);

   return dis(gen);
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

   static std::random_device                                   rd;
   static std::mt19937                                         gen(rd());
   static boost::random::uniform_real_distribution<float_type> dis(-1.0F, 1.0F);
   static boost::random::uniform_int_distribution<int>         exponent_dis(min_exp, max_exp);

   auto shifted_rand = [&](int i) {
      using std::ldexp;
#ifdef BOOST_HAS_FLOAT128
      using boost::multiprecision::ldexp;
#endif

      return ldexp(dis(gen), -i * std::numeric_limits<float_type>::digits);
   };

   DoubleFloatType out;
   out.backend() = typename DoubleFloatType::backend_type(shifted_rand(0), shifted_rand(1));

   return boost::multiprecision::ldexp(out, exponent_dis(gen));
}

template <typename ConstructionType, typename ArithmeticType, typename std::enable_if<(std::is_arithmetic<ArithmeticType>::value || boost::multiprecision::backends::detail::is_floating_point_or_float128<ArithmeticType>::value)>::type const* = nullptr>
ConstructionType construct_from(const ArithmeticType& f)
{
   return ConstructionType(f);
}

template <typename ConstructionType, typename DoubleFloatType>
ConstructionType construct_from(const boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<typename DoubleFloatType::backend_type::float_type> >& f)
{
   return ConstructionType(f.backend().my_first()) + ConstructionType(f.backend().my_second());
}

template <typename FloatingPointType, typename NumericType>
int test_constructor()
{
   using double_float_backend_t = boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>;
   using double_float_t         = boost::multiprecision::number<double_float_backend_t>;
   using double_limits          = std::numeric_limits<double_float_t>;

   static_assert(double_limits::digits10 + 5 < std::numeric_limits<control_float_type>::digits10, "Insufficient precision of control_float_type");

   control_float_type double_min = construct_from<control_float_type, double_float_t>(double_limits::lowest());
   control_float_type double_max = construct_from<control_float_type, double_float_t>(double_limits::max());

   std::cout << "Testing constructor for ";
   std::cout.width(30);
   std::cout << boost::core::demangle(typeid(NumericType).name()) << "... ";

   int i;
   for (i = 0; i < TestCases; ++i)
   {
      NumericType n = get_rand<NumericType>(double_min, double_max);

      double_float_t d(n);

      typename double_float_backend_t::rep_type rep(d.backend().crep());
      double_float_backend_t::arithmetic::normalize(rep);

      // Check if representation of the cpp_double_fp_backend is not normalized
      if (rep != d.backend().crep())
      {
         std::cerr << "[FAILED]\nabnormal representation for " << boost::core::demangle(typeid(NumericType).name()) << " = " << n
                   << " (cpp_double_fp_backend<" << boost::core::demangle(typeid(FloatingPointType).name()) << "> = " << d.backend().raw_str() << ")" << std::endl;
         return -1;
      }

      const control_float_type MaxError = boost::multiprecision::ldexp(control_float_type(1), -std::numeric_limits<double_float_t>::digits);
      control_float_type       n_prime  = construct_from<control_float_type, NumericType>(n);
      control_float_type       d_prime  = construct_from<control_float_type, double_float_t>(d);

      using boost::multiprecision::fabs;

      if (fabs(1 - fabs(n_prime / d_prime)) > MaxError)
      {
         std::cerr << "[FAILED] exceeded acceptable error (n = " << n << ")" << std::endl;
         std::cerr << "error    : " << fabs(1 - fabs(n_prime / d_prime)) << std::endl
                   << "tolerance: " << MaxError << std::endl;
         return -1;
      }
   }

   std::cout << "ok" << std::endl;

   return 0;
}

// Test compilation, constructors, basic operatory
template <typename FloatingPointType>
int test_constructors()
{
   using double_float_t = boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>;
   double_float_t a, b;

   std::cout << "Testing cpp_double_fp_backend< " << boost::core::demangle(typeid(FloatingPointType).name()) << " >";
   std::cout << " [" << TestCases << " cases per test]" << "...\n=== " << std::endl;

   int e = 0;

   e += test_constructor<FloatingPointType, long long int>();
   e += test_constructor<FloatingPointType, unsigned long long int>();
   e += test_constructor<FloatingPointType, long int>();
   e += test_constructor<FloatingPointType, unsigned long int>();
   e += test_constructor<FloatingPointType, short int>();
   e += test_constructor<FloatingPointType, unsigned short int>();
   e += test_constructor<FloatingPointType, signed char>();
   e += test_constructor<FloatingPointType, unsigned char>();
   e += test_constructor<FloatingPointType, float>();
   e += test_constructor<FloatingPointType, double>();
   e += test_constructor<FloatingPointType, long double>();
#ifdef BOOST_HAS_FLOAT128
   e += test_constructor<FloatingPointType, boost::multiprecision::float128>();
#endif

   e += test_constructor<FloatingPointType, boost::multiprecision::cpp_double_float>();
   e += test_constructor<FloatingPointType, boost::multiprecision::cpp_double_double>();
   e += test_constructor<FloatingPointType, boost::multiprecision::cpp_double_long_double>();
#ifdef BOOST_HAS_FLOAT128
   e += test_constructor<FloatingPointType, boost::multiprecision::cpp_double_float128>();
#endif

   if (e == 0)
      std::cout << "PASSED all tests";
   else
      std::cout << "FAILED some test(s)";

   std::cout << std::endl
             << std::endl;

   return e;
}
} // namespace test_cpp_double_float_constructors

int main()
{
   int e = 0;
   
   e += test_cpp_double_float_constructors::test_constructors<float>();
   e += test_cpp_double_float_constructors::test_constructors<double>();
   e += test_cpp_double_float_constructors::test_constructors<long double>();
#ifdef BOOST_HAS_FLOAT128
   e += test_cpp_double_float_constructors::test_constructors<boost::multiprecision::float128>();
#endif

   if (e == 0)
      std::cout << "TOTAL: PASSED all tests";
   else
      std::cout << "TOTAL: FAILED some test(s)";

   std::cout << std::endl
             << std::endl;

   return ((e == 0) ? 0 : -1);
}
