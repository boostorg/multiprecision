///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Basic compilation, constructors, and operatory tests for cpp_double_float<>

#include <boost/multiprecision/cpp_double_float.hpp>
#include <iostream>
#include <cstdlib>
#include <random>
#include <numeric>

template <typename NumericType,
          typename std::enable_if<std::is_integral<NumericType>::value, bool>::type = true>
NumericType uniform_integral_number()
{
   NumericType    out   = 0;
   unsigned char* bytes = reinterpret_cast<unsigned char*>(&out);

   for (int i = 0; i < sizeof(NumericType); ++i)
      bytes[i] |= (unsigned char)std::round(256.0 * (double(std::rand()) / RAND_MAX));

   return out;
}

template <typename FloatingPointType,
          typename std::enable_if<std::is_floating_point<FloatingPointType>::value, bool>::type = true>
FloatingPointType uniform_real()
{
   static std::random_device             rd;
   static std::mt19937                   gen(rd());
   static std::uniform_real_distribution<FloatingPointType> dis(0.0, 1.0);

   return dis(gen);
}

template <typename FloatingPointType, typename NumericType>
void test_constructor() {
   std::cout << "Testing constructor for " << typeid(NumericType).name() << "... ";
   constexpr int Trials = 500;
   using double_float_t = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;

   int i;
   for (i = 0; i < Trials; ++i)
   {
      NumericType    n = uniform_integral_number<NumericType>();
      double_float_t d(n);

      double_float_t::rep_type rep(d.rep());
      double_float_t::normalize_pair(rep);

      if (rep != d.rep() || static_cast<NumericType>(d) != n)
      {
         std::cout << "Failed for n = " << n << " != " << static_cast<NumericType>(d) << std::endl;
         // std::cout << d.get_raw_str() << std::endl;
         return;
      }
   }

   std::cout << "OK (" << Trials << " cases tested)" << std::endl;
}

// Test compilation, constructors, basic operatory
template <typename FloatingPointType>
void test_basic_operations()
{
   using double_float_t = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;
   double_float_t a, b;

   std::cout << "Testing cpp_double_float< " << typeid(FloatingPointType).name() << " >...\n"
             << std::endl;

   test_constructor<FloatingPointType, long long int>();
   test_constructor<FloatingPointType, unsigned long long int>();
   test_constructor<FloatingPointType, long int>();
   test_constructor<FloatingPointType, unsigned long int>();
   test_constructor<FloatingPointType, short int>();
   test_constructor<FloatingPointType, unsigned short int>();
   test_constructor<FloatingPointType, signed char>();
   test_constructor<FloatingPointType, unsigned char>();

   a = uniform_real<FloatingPointType>();
   b = uniform_real<FloatingPointType>();

   std::cout.precision(std::numeric_limits<FloatingPointType>::digits10 * 2);

   std::cout << "a: " << a << "\tb: " << b << std::endl;
   std::cout << std::endl;

   std::cout << "a + b = " << a + b << std::endl;
   std::cout << "a - b = " << a - b << std::endl;
   std::cout << "a * b = " << a * b << std::endl;
   std::cout << "a / b = " << a / b << std::endl;
   std::cout << std::endl;

   std::cout << "a += b";
   a += b;
   std::cout << " (a = " << a << ")" << std::endl;

   std::cout << "a -= b";
   a -= b;
   std::cout << " (a = " << a << ")" << std::endl;

   std::cout << "a *= b";
   a *= b;
   std::cout << " (a = " << a << ")" << std::endl;

   std::cout << "a /= b";
   a /= b;
   std::cout << " (a = " << a << ")" << std::endl;

   std::cout << std::endl;

   std::cout << "a   = " << a << std::endl;
   std::cout << "++a = " << ++a << " (a = " << a << ")" << std::endl;
   std::cout << "--a = " << --a << " (a = " << a << ")" << std::endl;
   std::cout << "a++ = " << a++ << " (a = " << a << ")" << std::endl;
   std::cout << "a-- = " << a-- << " (a = " << a << ")" << std::endl;
   std::cout << std::endl;

   std::cout << "     a =  " << a << std::endl;
   std::cout << "    -a = " << -a << std::endl;
   std::cout << "a + -a =  " << a + -a << std::endl;

   std::cout << std::endl;

   std::cout << "(a / b) * (b / a) = ";
   std::cout << (a / b) * (b / a) << std::endl;

   std::cout << std::endl;
}

//int main()
//{
//   test_basic_operations<double>();
//   test_basic_operations<float>();
//
//   return 0;
//}