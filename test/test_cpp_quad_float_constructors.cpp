///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Constructor tests for cpp_quad_fp_backend<>

#include <boost/config.hpp>
#include <boost/multiprecision/number.hpp>
#ifdef BOOST_MATH_USE_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/cpp_quad_float.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <iostream>
#include <cstdlib>
#include <random>
#include <numeric>

namespace test_cpp_quad_float_constructors {

namespace detail {

template <typename T>
constexpr T max(T a, T b)
{
   return ((a > b) ? a : b);
}

} // namespace detail

template <typename FloatingPointType,
          typename std::enable_if<boost::multiprecision::backends::detail::is_floating_point_or_float128<FloatingPointType>::value, bool>::type = true>
FloatingPointType uniform_real()
{
   static std::random_device                                          rd;
   static std::mt19937                                                gen(rd());
   static boost::random::uniform_real_distribution<FloatingPointType> dis(0.0, 1.0);

   return dis(gen);
}

template <typename NumericType,
          typename std::enable_if<std::is_integral<NumericType>::value, bool>::type = true>
NumericType uniform_integral_number()
{
   NumericType out = 0;

   for (int i = 0; i < int(sizeof(NumericType)); ++i)
      out = (out << 8) + static_cast<NumericType>(std::round(256.0 * uniform_real<float>()));

   return out;
}

template <typename NumericType,
          typename std::enable_if<std::is_integral<NumericType>::value, bool>::type = true>
NumericType get_rand()
{
   return uniform_integral_number<NumericType>();
}

template <typename FloatingPointType,
          typename std::enable_if<boost::multiprecision::backends::detail::is_floating_point_or_float128<FloatingPointType>::value, bool>::type = true>
FloatingPointType get_rand()
{
   return uniform_real<FloatingPointType>();
}

template <typename FloatingPointType>
boost::multiprecision::backends::cpp_quad_fp_backend<typename FloatingPointType::float_type> get_rand()
{
   using float_type = typename FloatingPointType::float_type;
   return boost::multiprecision::backends::cpp_quad_fp_backend<float_type>(uniform_real<float_type>())
        * boost::multiprecision::backends::cpp_quad_fp_backend<float_type>(uniform_real<float_type>())
        * boost::multiprecision::backends::cpp_quad_fp_backend<float_type>(uniform_real<float_type>())
        * boost::multiprecision::backends::cpp_quad_fp_backend<float_type>(uniform_real<float_type>());
}

template <typename ConstructionType, typename ArithmeticType, typename std::enable_if<std::is_arithmetic<ArithmeticType>::value || boost::multiprecision::backends::detail::is_floating_point_or_float128<ArithmeticType>::value>::type const* = nullptr>
ConstructionType construct_from(ArithmeticType f)
{
   return ConstructionType(f);
}

template <typename ConstructionType, typename QuadFloatType, typename std::enable_if<!(std::is_arithmetic<QuadFloatType>::value || boost::multiprecision::backends::detail::is_floating_point_or_float128<QuadFloatType>::value)>::type const* = nullptr>
ConstructionType construct_from(QuadFloatType f)
{
   static_assert(std::is_same<boost::multiprecision::backends::cpp_quad_fp_backend<typename QuadFloatType::float_type>, typename std::decay<QuadFloatType>::type>::value, "Only quad float should come here");
   return ConstructionType(std::get<0>(f.rep())) + ConstructionType(std::get<1>(f.rep())) + ConstructionType(std::get<2>(f.rep())) + ConstructionType(std::get<3>(f.rep()));
}

template <typename FloatingPointType, typename NumericType>
int test_constructor()
{
   using quad_float_t     = boost::multiprecision::backends::cpp_quad_fp_backend<FloatingPointType>;
   using control_float_type = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<(detail::max)(std::numeric_limits<quad_float_t>::digits10, std::numeric_limits<NumericType>::digits10) * 2 + 1>, boost::multiprecision::et_off>;

   std::cout << "Testing constructor for ";
   std::cout.width(30);
   std::cout << typeid(NumericType).name() << "... ";

   int i;
   for (i = 0; i < 10000; ++i)
   {
      NumericType n = get_rand<NumericType>();

      quad_float_t d(n);

      typename quad_float_t::rep_type rep(d.rep());
      quad_float_t::arithmetic::normalize(rep);

      // Check if representation of the cpp_quad_fp_backend is not normalized
      if (rep != d.rep())
      {
         std::cerr << "[FAILED]\nabnormal representation for " << typeid(NumericType).name() << " = " << n
                   << " (cpp_quad_fp_backend<" << typeid(FloatingPointType).name() << "> = " << d.raw_str() << ")" << std::endl;
         return -1;
      }

      const control_float_type MaxError = boost::multiprecision::ldexp(control_float_type(1), 1-std::numeric_limits<quad_float_t>::digits);
      control_float_type       n_prime  = construct_from<control_float_type, NumericType>(n);
      control_float_type       d_prime  = construct_from<control_float_type, quad_float_t>(d);

      using boost::multiprecision::fabs;

      if (fabs(1 - fabs(n_prime / d_prime)) > MaxError)
      {
         std::cerr << "[FAILED] exceeded acceptable error (n = " << n << ")" << std::endl;
         std::cerr << "error    : " << fabs(1 - fabs(n_prime / d_prime)) << std::endl
                   << "tolerance: " << MaxError << std::endl;
         return -1;
      }
   }

   std::cout << "ok (" << i << " cases tested)" << std::endl;

   return 0;
}

// Test compilation, constructors, basic operatory
template <typename FloatingPointType>
int test_constructors()
{
   using quad_float_t = boost::multiprecision::backends::cpp_quad_fp_backend<FloatingPointType>;
   quad_float_t a, b;

   std::cout << "Testing cpp_quad_fp_backend< " << typeid(FloatingPointType).name() << " >...\n==="
             << std::endl;

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
#ifdef BOOST_MATH_USE_FLOAT128
   e += test_constructor<FloatingPointType, boost::multiprecision::float128>();
#endif
   e += test_constructor<FloatingPointType, boost::multiprecision::backends::cpp_quad_fp_backend<float> >();
   e += test_constructor<FloatingPointType, boost::multiprecision::backends::cpp_quad_fp_backend<double> >();
   e += test_constructor<FloatingPointType, boost::multiprecision::backends::cpp_quad_fp_backend<long double> >();
#ifdef BOOST_MATH_USE_FLOAT128
   e += test_constructor<FloatingPointType, boost::multiprecision::backends::cpp_quad_fp_backend<boost::multiprecision::float128> >();
#endif

   if (e == 0)
      std::cout << "PASSED all tests";
   else
      std::cout << "FAILED some test(s)";

   std::cout << std::endl
             << std::endl;

   return e;
}
} // namespace test_cpp_quad_constructors

int main()
{
   int e = 0;

   e += test_cpp_quad_float_constructors::test_constructors<float>();
   e += test_cpp_quad_float_constructors::test_constructors<double>();
   //e += test_cpp_quad_constructors::test_constructors<long double>();
#ifdef BOOST_MATH_USE_FLOAT128
   e += test_cpp_quad_constructors::test_constructors<boost::multiprecision::float128>();
#endif

   return e;
}