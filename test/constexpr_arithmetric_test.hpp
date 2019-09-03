//  (C) Copyright John Maddock 2019.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config.hpp>
#include <boost/multiprecision/number.hpp>
#include <limits>

// clang-format off

template <class T, class U>
BOOST_CXX14_CONSTEXPR T do_test_constexpr_add_subtract(T a, U b)
{
   a = +b;
   if constexpr(std::numeric_limits<U>::is_signed && std::numeric_limits<T>::is_signed)
      b = -b;
   a += b;
   a += a;
   a -= b;
   a -= a;
   ++a;
   --a;
   a += b++;
   a += b--;
   a = a + b;
   a += a - b;
   if constexpr(std::numeric_limits<U>::is_signed)
      a -= b - -a;
   a += b + a;
   if constexpr(std::numeric_limits<T>::is_signed)
   {
      a = -a;
      if constexpr(std::numeric_limits<U>::is_signed)
         a -= b;
   }

   return a;
}

template <class T>
BOOST_CXX14_CONSTEXPR T test_constexpr_add_subtract(T a)
{
   a += do_test_constexpr_add_subtract(a, a);
   a += do_test_constexpr_add_subtract(a, static_cast<unsigned char>(2));
   a += do_test_constexpr_add_subtract(a, static_cast<signed char>(2));
   a += do_test_constexpr_add_subtract(a, static_cast<char>(2));
   a += do_test_constexpr_add_subtract(a, static_cast<short>(2));
   a += do_test_constexpr_add_subtract(a, static_cast<unsigned short>(2));
   a += do_test_constexpr_add_subtract(a, static_cast<int>(2));
   a += do_test_constexpr_add_subtract(a, static_cast<unsigned int>(2));
   a += do_test_constexpr_add_subtract(a, static_cast<long>(2));
   a += do_test_constexpr_add_subtract(a, static_cast<unsigned long>(2));
   a += do_test_constexpr_add_subtract(a, static_cast<long long>(2));
   a += do_test_constexpr_add_subtract(a, static_cast<unsigned long long>(2));

   if constexpr (boost::multiprecision::number_category<T>::value == boost::multiprecision::number_kind_floating_point)
   {
      a += do_test_constexpr_add_subtract(a, static_cast<float>(2));
      a += do_test_constexpr_add_subtract(a, static_cast<double>(2));
      a += do_test_constexpr_add_subtract(a, static_cast<long double>(2));
#ifdef BOOST_HAS_FLOAT128
      a += do_test_constexpr_add_subtract(a, static_cast<__float128>(2));
#endif
   }

   return a;
}

template <class T, class U>
BOOST_CXX14_CONSTEXPR T do_test_constexpr_mul_divide(T a, U b)
{
   a *= b;
   a = a * b;
   if constexpr(std::numeric_limits<T>::is_signed && std::numeric_limits<U>::is_signed)
   {
      a *= -b;
      a = a * -b;
   }
   a /= b;
   a = a / b;
   if constexpr(std::numeric_limits<T>::is_signed && std::numeric_limits<U>::is_signed)
   {
      a /= -b;
      a = a / -b;
   }
   if constexpr (boost::multiprecision::number_category<T>::value == boost::multiprecision::number_kind_integer && boost::multiprecision::number_category<U>::value == boost::multiprecision::number_kind_integer)
   {
      a %= b;
      a = a % b;
   }
   return a;
}

template <class T>
BOOST_CXX14_CONSTEXPR T test_constexpr_mul_divide(T a)
{
   a += do_test_constexpr_mul_divide(a, a);
   a += do_test_constexpr_mul_divide(a, static_cast<unsigned char>(2));
   a += do_test_constexpr_mul_divide(a, static_cast<signed char>(2));
   a += do_test_constexpr_mul_divide(a, static_cast<char>(2));
   a += do_test_constexpr_mul_divide(a, static_cast<short>(2));
   a += do_test_constexpr_mul_divide(a, static_cast<unsigned short>(2));
   a += do_test_constexpr_mul_divide(a, static_cast<int>(2));
   a += do_test_constexpr_mul_divide(a, static_cast<unsigned int>(2));
   a += do_test_constexpr_mul_divide(a, static_cast<long>(2));
   a += do_test_constexpr_mul_divide(a, static_cast<unsigned long>(2));
   a += do_test_constexpr_mul_divide(a, static_cast<long long>(2));
   a += do_test_constexpr_mul_divide(a, static_cast<unsigned long long>(2));

   if constexpr (boost::multiprecision::number_category<T>::value == boost::multiprecision::number_kind_floating_point)
   {
      a += do_test_constexpr_mul_divide(a, static_cast<float>(2));
      a += do_test_constexpr_mul_divide(a, static_cast<double>(2));
      a += do_test_constexpr_mul_divide(a, static_cast<long double>(2));
#ifdef BOOST_HAS_FLOAT128
      a += do_test_constexpr_mul_divide(a, static_cast<__float128>(2));
#endif
    }
   return a;
}

template <class T, class U>
BOOST_CXX14_CONSTEXPR T do_test_constexpr_bitwise(T a, U b)
{
   a |= b;
   a &= b;
   a ^= b;
   a = a | b;
   a = a & b;
   a = a ^ b;
   if constexpr (std::numeric_limits<T>::is_signed == false)
      return ~a;
   return a;
} 

template <class T>
BOOST_CXX14_CONSTEXPR T test_constexpr_bitwise(T a)
{
   a += do_test_constexpr_bitwise(a, a);
   a += do_test_constexpr_bitwise(a, static_cast<unsigned char>(2));
   a += do_test_constexpr_bitwise(a, static_cast<signed char>(2));
   a += do_test_constexpr_bitwise(a, static_cast<char>(2));
   a += do_test_constexpr_bitwise(a, static_cast<short>(2));
   a += do_test_constexpr_bitwise(a, static_cast<unsigned short>(2));
   a += do_test_constexpr_bitwise(a, static_cast<int>(2));
   a += do_test_constexpr_bitwise(a, static_cast<unsigned int>(2));
   a += do_test_constexpr_bitwise(a, static_cast<long>(2));
   a += do_test_constexpr_bitwise(a, static_cast<unsigned long>(2));
   a += do_test_constexpr_bitwise(a, static_cast<long long>(2));
   a += do_test_constexpr_bitwise(a, static_cast<unsigned long long>(2));

   return a;
}

template <class T, class U>
BOOST_CXX14_CONSTEXPR T do_test_constexpr_logical(T a, U b)
{
   bool result = a || b;
   result &= a && b;
   if(result)
      return a;
   return T(b);
} 

template <class T>
BOOST_CXX14_CONSTEXPR T test_constexpr_logical(T a)
{
   a += do_test_constexpr_logical(a, a);
   a += do_test_constexpr_logical(a, static_cast<unsigned char>(2));
   a += do_test_constexpr_logical(a, static_cast<signed char>(2));
   a += do_test_constexpr_logical(a, static_cast<char>(2));
   a += do_test_constexpr_logical(a, static_cast<short>(2));
   a += do_test_constexpr_logical(a, static_cast<unsigned short>(2));
   a += do_test_constexpr_logical(a, static_cast<int>(2));
   a += do_test_constexpr_logical(a, static_cast<unsigned int>(2));
   a += do_test_constexpr_logical(a, static_cast<long>(2));
   a += do_test_constexpr_logical(a, static_cast<unsigned long>(2));
   a += do_test_constexpr_logical(a, static_cast<long long>(2));
   a += do_test_constexpr_logical(a, static_cast<unsigned long long>(2));

   return a;
}

template <class T, class U>
BOOST_CXX14_CONSTEXPR T do_test_constexpr_compare(T a, U b)
{
   bool result = (a == b) || (a != b) || (a < b) || (a > b) || (a <= b) || (a >= b);
   if(result)
      return a;
   return T(b);
} 

template <class T>
BOOST_CXX14_CONSTEXPR T test_constexpr_compare(T a)
{
   a += do_test_constexpr_compare(a, a);
   a += do_test_constexpr_compare(a, static_cast<unsigned char>(2));
   a += do_test_constexpr_compare(a, static_cast<signed char>(2));
   a += do_test_constexpr_compare(a, static_cast<char>(2));
   a += do_test_constexpr_compare(a, static_cast<short>(2));
   a += do_test_constexpr_compare(a, static_cast<unsigned short>(2));
   a += do_test_constexpr_compare(a, static_cast<int>(2));
   a += do_test_constexpr_compare(a, static_cast<unsigned int>(2));
   a += do_test_constexpr_compare(a, static_cast<long>(2));
   a += do_test_constexpr_compare(a, static_cast<unsigned long>(2));
   a += do_test_constexpr_compare(a, static_cast<long long>(2));
   a += do_test_constexpr_compare(a, static_cast<unsigned long long>(2));

   if constexpr (boost::multiprecision::number_category<T>::value == boost::multiprecision::number_kind_floating_point)
   {
      a += do_test_constexpr_compare(a, static_cast<float>(2));
      a += do_test_constexpr_compare(a, static_cast<double>(2));
      a += do_test_constexpr_compare(a, static_cast<long double>(2));
#ifdef BOOST_HAS_FLOAT128
      a += do_test_constexpr_compare(a, static_cast<__float128>(2));
#endif
    }
   return a;
}

// clang-format on
