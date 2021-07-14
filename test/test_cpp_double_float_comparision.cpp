///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Test comparision operators of cpp_double_float<>
// Note: This series of tests depend on the correctness of constructor
//       so please run test_cpp_double_float_constructors.cpp before this

#include <boost/multiprecision/cpp_double_float.hpp>
#include <iostream>
#include <cstdlib>
#include <random>
#include <numeric>
#include <iomanip>

namespace test_cpp_double_comparision {
template <typename FloatingPointType,
          typename std::enable_if<std::is_floating_point<FloatingPointType>::value, bool>::type = true>
FloatingPointType uniform_real()
{
   //static std::random_device                                rd;
   static std::mt19937                                      gen/*(rd())*/;
   static std::uniform_real_distribution<FloatingPointType> dis(0.0, 1.0);

   return dis(gen);
}

template <typename NumericType,
          typename std::enable_if<std::is_integral<NumericType>::value && !std::is_floating_point<NumericType>::value, bool>::type = true>
NumericType uniform_integral_number()
{
   NumericType out = 0;

   for (int i = 0; i < sizeof(NumericType); ++i)
      out = (out << 8) + static_cast<NumericType>(std::round(256.0 * uniform_real<float>()));

   return out;
}

int rand_in_range(int a, int b)
{
   return a + int(float(b - a) * uniform_real<float>());
}

template <typename NumericType,
          typename std::enable_if<std::is_integral<NumericType>::value && !std::is_floating_point<NumericType>::value, bool>::type = true>
NumericType uniform_rand()
{
   return uniform_integral_number<NumericType>();
}

template <typename FloatingPointType,
          typename std::enable_if<std::is_floating_point<FloatingPointType>::value, bool>::type = true>
FloatingPointType uniform_rand()
{
   return uniform_real<FloatingPointType>();
}

template <typename FloatingPointType>
boost::multiprecision::backends::cpp_double_float<typename FloatingPointType::float_type> uniform_rand()
{
  using float_type = FloatingPointType::float_type;
   return boost::multiprecision::backends::cpp_double_float<float_type>(uniform_real<float_type>())
        * boost::multiprecision::backends::cpp_double_float<float_type>(uniform_real<float_type>());
}

template <typename NumericType, typename std::enable_if<std::is_integral<NumericType>::value>::type const* = nullptr>
NumericType log_rand()
{
   return uniform_integral_number<NumericType>() >> int(uniform_real<float>() * float(std::numeric_limits<NumericType>::digits+1));
}

template <typename FloatingPointType, typename std::enable_if<std::is_floating_point<FloatingPointType>::value>::type const* = nullptr>
FloatingPointType log_rand()
{
   if (uniform_real<float>() < (1. / 100.))
      return 0; // throw in a few zeroes
   return std::ldexp(uniform_real<FloatingPointType>(), rand_in_range(std::numeric_limits<FloatingPointType>::min_exponent, std::numeric_limits<FloatingPointType>::max_exponent));
}

template <typename FloatingPointType>
boost::multiprecision::backends::cpp_double_float<typename FloatingPointType::float_type> log_rand()
{
   boost::multiprecision::backends::cpp_double_float<FloatingPointType::float_type> a(uniform_rand<boost::multiprecision::backends::cpp_double_float<FloatingPointType::float_type> >());
   a *= log_rand<FloatingPointType::float_type>();
   return a;
}

template <typename FloatingPointType, typename ComparisionType>
int test()
{
   using double_float_t = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;
   using largest_type   = boost::multiprecision::backends::cpp_double_float<double>;

   std::string type_name = typeid(ComparisionType).name();
   size_t      idx;
   if ((idx = type_name.rfind(":")) != std::string::npos)
      type_name = type_name.substr(idx+1, type_name.size());

   std::cout << "* Testing comparision operators vs ";
   std::cout.width(30);
   std::cout.setf(std::ios::left, std::ios::adjustfield);
   std::cout.fill(' ');
   std::cout << type_name << "...";

   bool failed = false;
   int  i;
   for (i = 0; i < 10000; ++i)
   {
      ComparisionType r1 = uniform_rand<ComparisionType>();
      ComparisionType r2 = log_rand<ComparisionType>();
      r2 = (uniform_real<float>() < 0.5) ? r1 + r2 : r1 - r2;

      double_float_t  a = double_float_t(r1);
      ComparisionType b = r2;

      // operator>
      if (a > b)
      {
         if (static_cast<largest_type>(a) <= static_cast<largest_type>(b))
            failed = true;
         if (b > a || b >= a || b == a || a == b)
            failed = true;
      }
      else
      {
         if (!(a <= b))
          failed = true;
         if (!(b >= a))
            failed = true;
      }
      if (failed)
      {
         std::cerr << " [FAILED] operator> failed" << std::endl;
         return -1;
      }
      // operator<
      if (a < b)
      {
         if (static_cast<largest_type>(a) >= static_cast<largest_type>(b))
            failed = true;
         if (b < a || b <= a || b == a || a == b)
            failed = true;
      }
      else
      {
         if (!(a >= b))
            failed = true;
         if (!(b <= a))
            failed = true;
      }

      if (failed)
      {
         std::cerr << " [FAILED] operator< failed" << std::endl;
         return -1;
      }

      // operator<=
      if (a <= b)
      {
         if (b < a)
            failed = true;
         if (static_cast<largest_type>(a) > static_cast<largest_type>(b))
            failed = true;
      }
      else
      {
         if (!(a > b))
            failed = true;
         if (!(b < a))
            failed = true;
      }

      if (failed)
      {
         std::cerr << " [FAILED] operator<= failed" << std::endl;
         return -1;
      }

      // operator>=
      if (a >= b)
      {
         if (b > a)
            failed = true;
         if (static_cast<largest_type>(a) < static_cast<largest_type>(b))
            failed = true;
      }
      else
      {
         if (!(a < b))
            failed = true;
         if (!(b > a))
            failed = true;
      }

      if (failed)
      {
         std::cerr << " [FAILED] operator>= failed" << std::endl;
         return -1;
      }

      // operator==
      if (a == b)
      {
         if (b != a)
            failed = true;
         if (static_cast<largest_type>(a) != static_cast<largest_type>(b))
            failed = true;
      }
      else
      {
         if (!(a != b))
            failed = true;
         if (!(b != a))
            failed = true;
      }

      if (failed)
      {
         std::cerr << " [FAILED] operator== failed" << std::endl;
         return -1;
      }

      // operator!=
      if (a != b)
      {
         if (b == a)
            failed = true;
         if (static_cast<largest_type>(a) == static_cast<largest_type>(b))
            failed = true;
      }
      else
      {
         if (!(a == b))
            failed = true;
         if (!(b == a))
            failed = true;
      }

      if (failed)
      {
         std::cerr << " [FAILED] operator!= failed" << std::endl;
         return -1;
      }
   }

   std::cout << " ok [" << i << " tests]" << std::endl;

   return 0;
}

template <typename FloatingPointType>
int test_basic() {
   using double_float_t = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;

   std::cout << "Performing basic comparision tests for cpp_double_float<" << typeid(FloatingPointType).name() << ">... ";

   int i;
   for (i = 0; i < 10000; ++i)
   {
      double_float_t a(uniform_rand<double_float_t>()), a_prime;

      a_prime = a + std::numeric_limits<double_float_t>::epsilon();
      if (a > a_prime)
      {
         std::cerr << "operator> failed" << std::endl;
         return -1;
      }
      if (a >= a_prime)
      {
         std::cerr << "operator>= failed" << std::endl;
         return -1;
      }
      if (a_prime < a)
      {
         std::cerr << "operator< failed" << std::endl;
         return -1;
      }
      if (a_prime <= a)
      {
         std::cerr << "operator<= failed" << std::endl;
         return -1;
      }
      if (a == a_prime || a_prime == a)
      {
         std::cerr << "operator== failed" << std::endl;
         return -1;
      }
      if (!(a != a_prime) || !(a_prime != a))
      {
         std::cerr << "operator!= failed" << std::endl;
         return -1;
      }

      a_prime = a - std::numeric_limits<double_float_t>::epsilon();
      if (a < a_prime)
      {
         std::cerr << "operator< failed" << std::endl;
         return -1;
      }
      if (a <= a_prime)
      {
         std::cerr << "operator<= failed" << std::endl;
         return -1;
      }
      if (a_prime > a)
      {
         std::cerr << "operator> failed" << std::endl;
         return -1;
      }
      if (a_prime >= a)
      {
         std::cerr << "operator>= failed" << std::endl;
         return -1;
      }
      if (a == a_prime || a_prime == a)
      {
         std::cerr << "operator== failed" << std::endl;
         return -1;
      }
      if (!(a != a_prime) || !(a_prime != a))
      {
         std::cerr << "operator!= failed" << std::endl;
         return -1;
      }
   }

   std::cerr << "ok [" << i << " tests]" << std::endl;
   return 0;
}
} // namespace test_cpp_double_comparision

int main()
{
   int e = 0;
   e += test_cpp_double_comparision::test_basic<float>();
   e += test_cpp_double_comparision::test_basic<double>();

   std::cout << "\nTesting comparision operators for cpp_double_float<float>" << std::endl;
   e += test_cpp_double_comparision::test<float, unsigned long long>();
   e += test_cpp_double_comparision::test<float, signed long long>();
   e += test_cpp_double_comparision::test<float, unsigned long>();
   e += test_cpp_double_comparision::test<float, signed long>();
   e += test_cpp_double_comparision::test<float, unsigned short>();
   e += test_cpp_double_comparision::test<float, signed short>();
   e += test_cpp_double_comparision::test<float, unsigned char>();
   e += test_cpp_double_comparision::test<float, signed char>();
   e += test_cpp_double_comparision::test<float, float>();
   e += test_cpp_double_comparision::test<float, double>();
   e += test_cpp_double_comparision::test<float, boost::multiprecision::backends::cpp_double_float<float> >();
   e += test_cpp_double_comparision::test<float, boost::multiprecision::backends::cpp_double_float<double> >();
   std::cout << std::endl;

   std::cout << "\nTesting comparision operators for cpp_double_float<double>" << std::endl;
   e += test_cpp_double_comparision::test<double, unsigned long long>();
   e += test_cpp_double_comparision::test<double, signed long long>();
   e += test_cpp_double_comparision::test<double, unsigned long>();
   e += test_cpp_double_comparision::test<double, signed long>();
   e += test_cpp_double_comparision::test<double, unsigned short>();
   e += test_cpp_double_comparision::test<double, signed short>();
   e += test_cpp_double_comparision::test<double, unsigned char>();
   e += test_cpp_double_comparision::test<double, signed char>();
   e += test_cpp_double_comparision::test<double, float>();
   e += test_cpp_double_comparision::test<double, double>();
   e += test_cpp_double_comparision::test<double, boost::multiprecision::backends::cpp_double_float<float> >();
   e += test_cpp_double_comparision::test<double, boost::multiprecision::backends::cpp_double_float<double> >();

   std::cout << (e == 0 ? "PASSED all tests" : "FAILED some test(s)") << std::endl;
   return int(e == 0) - 1;
}