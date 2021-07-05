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
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <iostream>
#include <cstdlib>
#include <random>
#include <numeric>

namespace test_cpp_double_constructors {

template <typename FloatingPointType,
          typename std::enable_if<std::is_floating_point<FloatingPointType>::value, bool>::type = true>
FloatingPointType uniform_real()
{
   static std::random_device                                rd;
   static std::mt19937                                      gen(rd());
   static std::uniform_real_distribution<FloatingPointType> dis(0.0, 1.0);

   return dis(gen);
}

template <typename NumericType,
          typename std::enable_if<std::is_integral<NumericType>::value, bool>::type = true>
NumericType uniform_integral_number() {
   NumericType out = 0;
   
   for (int i = 0; i < sizeof(NumericType); ++i)
      out = (out << 8) + static_cast<NumericType>(std::round(256.0 * uniform_real<float>()));

   return out;
}

template <typename FloatingPointType, typename NumericType>
bool test_constructor()
{
   bool result_is_ok = true;

   constexpr int Trials = 50000;
   std::cout << "Testing constructor for ";
   std::cout.width(30);
   std::cout << typeid(NumericType).name() << "... ";
   using double_float_t = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;

   for (int i = 0; i < Trials; ++i)
   {
      NumericType n = uniform_integral_number<NumericType>();

      double_float_t d(n);

      typename double_float_t::rep_type rep(d.rep());
      double_float_t::normalize_pair(rep);

      // Check if representation of the cpp_double_float is not normalized
      if (rep != d.rep())
      {
         std::cerr << "[FAILED]\n" << typeid(NumericType).name() << " = " << n
           << " (cpp_double_float<" << typeid(FloatingPointType).name() << "> = " << d.get_raw_str() << ")" << std::endl;

         result_is_ok = false;

         break;
      }

      // Check if value is accurately represented and correctly rounded
      constexpr int NumericTypeBits      = std::numeric_limits<NumericType>::digits;
      constexpr int MaxRepresentableBits = std::numeric_limits<FloatingPointType>::digits * 2;
      const NumericType RoundedBitsMask      = (NumericType(1) << (NumericTypeBits - MaxRepresentableBits)) - 1;
      const NumericType RoundedMargin        = (RoundedBitsMask + 1) >> 1;

      NumericType   n_prime(n);

      n_prime >>= NumericTypeBits - MaxRepresentableBits;
      // Round correctly
      if ((n & RoundedBitsMask) > RoundedMargin)
         n_prime |= 1;
      if ((n & RoundedBitsMask) < RoundedMargin)
         n_prime &= ~NumericType(1);

      n_prime <<= NumericTypeBits - MaxRepresentableBits;

      if (std::abs(signed(n_prime - n)) < std::abs(signed(static_cast<NumericType>(d) - n)))
      {
         std::cerr << "[FAILED]\nn = 0x" << std::hex << n << " | cpp_double_float<" << typeid(FloatingPointType).name()
                   << "> = 0x" << std::hex << static_cast<NumericType>(d)
                   << " (expected 0x" << std::hex << n_prime << ")"
                   << std::endl;

         result_is_ok = false;

         break;
      }
   }

   std::cout << "[PASSED] (" << Trials << " cases tested)" << std::endl;

   return result_is_ok;
}

// Test compilation, constructors, basic operatory
template <typename FloatingPointType>
bool test_constructors()
{
   using double_float_t = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;
   double_float_t a, b;

   std::cout << "Testing cpp_double_float< " << typeid(FloatingPointType).name() << " >...\n==="
             << std::endl;

   bool result_is_ok = true;

   result_is_ok &= test_constructor<FloatingPointType, long long int>         (); std::cout << "test_constructor<FloatingPointType, long long int>         (): " << std::boolalpha << result_is_ok << std::endl;
   result_is_ok &= test_constructor<FloatingPointType, unsigned long long int>(); std::cout << "test_constructor<FloatingPointType, unsigned long long int>(): " << std::boolalpha << result_is_ok << std::endl;
   result_is_ok &= test_constructor<FloatingPointType, long int>              (); std::cout << "test_constructor<FloatingPointType, long int>              (): " << std::boolalpha << result_is_ok << std::endl;
   result_is_ok &= test_constructor<FloatingPointType, unsigned long int>     (); std::cout << "test_constructor<FloatingPointType, unsigned long int>     (): " << std::boolalpha << result_is_ok << std::endl;
   result_is_ok &= test_constructor<FloatingPointType, short int>             (); std::cout << "test_constructor<FloatingPointType, short int>             (): " << std::boolalpha << result_is_ok << std::endl;
   result_is_ok &= test_constructor<FloatingPointType, unsigned short int>    (); std::cout << "test_constructor<FloatingPointType, unsigned short int>    (): " << std::boolalpha << result_is_ok << std::endl;
   result_is_ok &= test_constructor<FloatingPointType, signed char>           (); std::cout << "test_constructor<FloatingPointType, signed char>           (): " << std::boolalpha << result_is_ok << std::endl;
   result_is_ok &= test_constructor<FloatingPointType, unsigned char>         (); std::cout << "test_constructor<FloatingPointType, unsigned char>         (): " << std::boolalpha << result_is_ok << std::endl;

   if (result_is_ok)
      std::cout << "PASSED ALL TESTS";
   else
      std::cout << "FAILED at least one of the TESTS";
   std::cout << std::endl << std::endl;

   return result_is_ok;
}
} // namespace test_cpp_double_constructors

int main()
{
   const bool result_float_is_ok  = test_cpp_double_constructors::test_constructors<float>();
   const bool result_double_is_ok = test_cpp_double_constructors::test_constructors<double>();

   const bool result_is_ok = (result_float_is_ok && result_double_is_ok);

   return (result_is_ok ? 0 : -1);
}
