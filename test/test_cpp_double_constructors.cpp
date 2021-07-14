///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Constructor tests for cpp_double_float<>

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
NumericType uniform_integral_number()
{
   NumericType out = 0;

   for (int i = 0; i < sizeof(NumericType); ++i)
      out = (out << 8) + static_cast<NumericType>(std::round(256.0 * uniform_real<float>()));

   return out;
}


template <typename NumericType,
          typename std::enable_if<std::is_integral<NumericType>::value && !std::is_floating_point<NumericType>::value, bool>::type = true>
NumericType get_rand()
{
   return uniform_integral_number<NumericType>();
}

template <typename FloatingPointType,
          typename std::enable_if<std::is_floating_point<FloatingPointType>::value, bool>::type = true>
FloatingPointType get_rand()
{
   return uniform_real<FloatingPointType>();
}

template <typename FloatingPointType>
boost::multiprecision::backends::cpp_double_float<typename FloatingPointType::float_type> get_rand()
{
   using float_type = FloatingPointType::float_type;
   return boost::multiprecision::backends::cpp_double_float<float_type>(uniform_real<float_type>()) * boost::multiprecision::backends::cpp_double_float<float_type>(uniform_real<float_type>());
}

template <typename FloatingPointType, typename NumericType, typename std::enable_if<std::is_integral<NumericType>::value>::type const* = nullptr>
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
      NumericType n = get_rand<NumericType>();

      double_float_t d(n);

      typename double_float_t::rep_type rep(d.rep());
      double_float_t::normalize_pair(rep);

      // Check if representation of the cpp_double_float is not normalized
      if (rep != d.rep())
      {
         std::cerr << "[FAILED]\nabnormal representation for " << typeid(NumericType).name() << " = " << n
                   << " (cpp_double_float<" << typeid(FloatingPointType).name() << "> = " << d.get_raw_str() << ")" << std::endl;

         result_is_ok = false;

         break;
      }

      NumericType n_prime(n);

      // Check if value is accurately represented
      constexpr int NumericTypeBits      = std::numeric_limits<NumericType>::digits;
      constexpr int MaxRepresentableBits = std::numeric_limits<double_float_t>::digits;

      // Round correctly if the integral type has more precision than what can be represented
      if (NumericTypeBits > MaxRepresentableBits)
      {
         const NumericType RoundedBitsMask = (NumericType(1) << (NumericTypeBits - MaxRepresentableBits)) - 1;
         const NumericType RoundedMargin   = (RoundedBitsMask + 1) >> 1;

         n_prime >>= NumericTypeBits - MaxRepresentableBits;
         if ((n & RoundedBitsMask) > RoundedMargin)
            n_prime |= 1;
         else if ((n & RoundedBitsMask) < RoundedMargin)
            n_prime &= ~NumericType(1);

         n_prime <<= NumericTypeBits - MaxRepresentableBits;
      }

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

   std::cout << "ok (" << Trials << " cases tested)" << std::endl;

   return result_is_ok;
}

template <typename FloatingPointType, typename OtherFloatType, typename std::enable_if<!std::is_integral<OtherFloatType>::value>::type const* = nullptr>
bool test_constructor()
{
   bool result_is_ok = true;

   constexpr int Trials = 50000;

   std::string   type_name = typeid(OtherFloatType).name();
   size_t        idx;
   if ((idx = type_name.rfind(":")) != std::string::npos)
      type_name = type_name.substr(idx + 1, type_name.size());

   std::cout << "Testing constructor for ";
   std::cout.width(30);
   std::cout << type_name << "... ";
   using double_float_t = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;

   for (int i = 0; i < Trials; ++i)
   {
      OtherFloatType n = get_rand<OtherFloatType>();

      double_float_t d(n);

      typename double_float_t::rep_type rep(d.rep());
      double_float_t::normalize_pair(rep);

      // Check if representation of the cpp_double_float is not normalized
      if (rep != d.rep())
      {
         std::cerr << "[FAILED]\nabnormal representation for " << typeid(OtherFloatType).name() << " = " << n
                   << " (cpp_double_float<" << typeid(FloatingPointType).name() << "> = " << d.get_raw_str() << ")" << std::endl;

         result_is_ok = false;

         break;
      }

      // Check if the binary digits match (work in progress...)
      /*const int      DigitsToMatch  = std::min(std::numeric_limits<double_float_t>::digits, std::numeric_limits<OtherFloatType>::digits);
      int digits_matched = 0;

      OtherFloatType n_prime(n);
      double_float_t d_prime(d);

      while (n_prime > 1)
      {
         n_prime /= 2;
         d_prime /= 2;
      }*/
   }

   std::cout << "ok (" << Trials << " cases tested)" << std::endl;

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

   result_is_ok &= test_constructor<FloatingPointType, long long int>();
   result_is_ok &= test_constructor<FloatingPointType, unsigned long long int>();
   result_is_ok &= test_constructor<FloatingPointType, long int>();
   result_is_ok &= test_constructor<FloatingPointType, unsigned long int>();
   result_is_ok &= test_constructor<FloatingPointType, short int>();
   result_is_ok &= test_constructor<FloatingPointType, unsigned short int>();
   result_is_ok &= test_constructor<FloatingPointType, signed char>();
   result_is_ok &= test_constructor<FloatingPointType, unsigned char>();
   result_is_ok &= test_constructor<FloatingPointType, float>();
   result_is_ok &= test_constructor<FloatingPointType, double>();
   result_is_ok &= test_constructor<FloatingPointType, float>();
   result_is_ok &= test_constructor<FloatingPointType, boost::multiprecision::backends::cpp_double_float<float>>();
   result_is_ok &= test_constructor<FloatingPointType, boost::multiprecision::backends::cpp_double_float<double>>();

   if (result_is_ok)
      std::cout << "PASSED all tests";
   else
      std::cout << "FAILED some test(s)";
   std::cout << std::endl
             << std::endl;

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
