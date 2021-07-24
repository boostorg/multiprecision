///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DOUBLE_FLOAT_2021_06_05_HPP
#define BOOST_MP_CPP_DOUBLE_FLOAT_2021_06_05_HPP

#include <boost/config.hpp>

#include <type_traits>
#include <string>
#include <utility>
#include <limits>
#include <sstream>
#include <vector>

#include <boost/multiprecision/number.hpp>
#include <boost/assert.hpp>

namespace boost { namespace multiprecision { namespace backends {
template <typename FloatingPointType>
class cpp_double_float;
}}}

// Foward decleration for std::numeric_limits
template <typename FloatingPointType>
class std::numeric_limits<boost::multiprecision::backends::cpp_double_float<FloatingPointType> >;

namespace boost { namespace multiprecision {
template<typename FloatingPointType>
struct number_category<backends::cpp_double_float<FloatingPointType>>
  : public std::integral_constant<int, number_kind_floating_point>
{
};

namespace backends {

/*
* A cpp_double_float is represented by an unevaluated sum of two floating-point
* units (say a0 and a1) which satisfy |a1| <= (1 / 2) * ulp(a0)
*/
template <typename FloatingPointType>
class cpp_double_float
{
 public:
   using float_type = FloatingPointType;
   using rep_type   = std::pair<float_type, float_type>;

   // Constructors
   cpp_double_float() { }
   
   // default constructor
   constexpr cpp_double_float(const cpp_double_float& a) : data(a.data) {}

   // Constructors from other floating-point types
   template <typename FloatType,
             typename std::enable_if<(std::is_floating_point<FloatType>::value == true)
             && (std::numeric_limits<FloatType>::digits <= std::numeric_limits<float_type>::digits)>::type const* = nullptr>
   constexpr cpp_double_float(const FloatType& f) : data(std::make_pair(f, (float_type)0)) {}
   template <typename FloatType,
             typename std::enable_if<(std::numeric_limits<FloatType>::is_iec559 == true)
             && (std::numeric_limits<FloatType>::digits > std::numeric_limits<float_type>::digits)>::type const* = nullptr>
   constexpr cpp_double_float(const FloatType& f)
       : data(std::make_pair(static_cast<float_type>(f),
                             static_cast<float_type>(f - (FloatType) static_cast<float_type>(f)))) {}

   // Constructor from other cpp_double_float<> objects
   template <typename OtherFloatType, typename std::enable_if<!std::is_same<FloatingPointType, OtherFloatType>::value>::type const* = nullptr>
   cpp_double_float(const cpp_double_float<OtherFloatType>& a)
       : cpp_double_float(a.first())
   {
     // TODO Remove cast by overloading operator +=
      *this += cpp_double_float(a.second());
   }

   // Constructors from integers
   template <typename IntegralType,
             typename std::enable_if<(std::is_integral<IntegralType>::value == true) && (std::numeric_limits<IntegralType>::digits <= std::numeric_limits<FloatingPointType>::digits)>::type const* = nullptr>
   constexpr cpp_double_float(const IntegralType& f) : data(std::make_pair(static_cast<float_type>(f), (float_type)0)) {}

   // Constructors from integers which hold more information than *this can contain
   template <typename UnsignedIntegralType,
             typename std::enable_if<((std::is_integral<UnsignedIntegralType>::value == true)
               && (std::is_unsigned<UnsignedIntegralType>::value == true)
               && (std::numeric_limits<UnsignedIntegralType>::digits > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   cpp_double_float(UnsignedIntegralType u);

   template <typename SignedIntegralType,
             typename std::enable_if<((std::is_integral<SignedIntegralType>::value == true)
               && (std::is_signed<SignedIntegralType>::value == true)
               && (std::numeric_limits<SignedIntegralType>::digits + 1 > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   cpp_double_float(SignedIntegralType n) : cpp_double_float(static_cast<typename std::make_unsigned<SignedIntegralType>::type>(std::abs(n)))
   {
      if (n < 0)
         *this = -*this;
   }

   constexpr cpp_double_float(const float_type& a, const float_type& b) : data(std::make_pair(a, b)) {}
   constexpr cpp_double_float(const std::pair<float_type, float_type>& p) : data(p) {}

   cpp_double_float(const std::string& str)
   {
      set_str(str);
   }

   constexpr cpp_double_float(cpp_double_float&&) = default;

   ~cpp_double_float() = default;

   // Casts
   operator signed char() const { return (signed char)data.first; }
   operator signed short() const { return (signed short)data.first; }
   operator signed int() const { return (signed int)data.first + (signed int)data.second; }
   operator signed long() const { return (signed long)data.first + (signed long)data.second; }
   operator signed long long() const { return (signed long long)data.first + (signed long long)data.second; }
   operator unsigned char() const { return (unsigned char)data.first; }
   operator unsigned short() const { return (unsigned short)data.first; }
   operator unsigned int() const { return (unsigned int)((unsigned int)data.first + (signed int)data.second); }
   operator unsigned long() const { return (unsigned long)((unsigned long)data.first + (signed long)data.second); }
   operator unsigned long long() const { return (unsigned long long)((unsigned long long)data.first + (signed long long)data.second); }
   operator float() const { return (float)data.first + (float)data.second; }
   operator double() const { return (double)data.first + (double)data.second; }
   operator long double() const { return (long double)data.first + (long double)data.second; }

   // Methods
   constexpr cpp_double_float<float_type> negative()    const { return cpp_double_float<float_type>(-data.first, -data.second); }
   constexpr bool                         is_negative() const { return data.first < 0; }

   // FIXME Merge set_str() to operator<<
   void set_str(std::string str);

   // Getters/Setters
   constexpr const float_type& first () const { return data.first; }
   constexpr const float_type& second() const { return data.second; }

         rep_type&  rep()       { return data; }
   const rep_type&  rep() const { return data; }
   const rep_type& crep() const { return data; }

   // Helper functions
   static std::pair<float_type, float_type> fast_exact_sum(const float_type& a, const float_type& b);
   static std::pair<float_type, float_type> exact_sum(const float_type& a, const float_type& b);
   static std::pair<float_type, float_type> exact_difference(const float_type& a, const float_type& b);
   static std::pair<float_type, float_type> exact_product(const float_type& a, const float_type& b);

   static std::pair<float_type, float_type> split(const float_type& a);

   static void normalize_pair(std::pair<float_type, float_type>& p, bool fast = true);

   // Operators
   cpp_double_float& operator=(const cpp_double_float&) = default;

   cpp_double_float& operator=(cpp_double_float&&) = default;

   cpp_double_float& operator+=(const cpp_double_float& a);
   cpp_double_float& operator-=(const cpp_double_float& a);
   cpp_double_float& operator*=(const cpp_double_float& a);
   cpp_double_float& operator/=(const cpp_double_float& a);
   cpp_double_float& operator+=(const float_type& a);
   cpp_double_float& operator-=(const float_type& a);
   cpp_double_float& operator*=(const float_type& a);
   cpp_double_float& operator/=(const float_type& a);
   cpp_double_float  operator++(int);
   cpp_double_float  operator--(int);
   cpp_double_float& operator++() { return *this += cpp_double_float<float_type>((double)1.); }
   cpp_double_float& operator--() { return *this -= cpp_double_float<float_type>(1.); }
   cpp_double_float  operator-() const { return negative(); }

   // -- DEBUGGING
   std::string get_raw_str() const;
   // --
   static cpp_double_float<float_type> pow10(int x);
 private:
   rep_type data;
};

// -- Special Constructors
template <typename FloatingPointType>
template <typename UnsignedIntegralType,
          typename std::enable_if<((std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true) && (std::numeric_limits<UnsignedIntegralType>::digits > std::numeric_limits<FloatingPointType>::digits))>::type const*>
inline cpp_double_float<FloatingPointType>::cpp_double_float(UnsignedIntegralType u)
{
   constexpr int MantissaBits = std::numeric_limits<FloatingPointType>::digits - 1;

   int bit_index = sizeof(UnsignedIntegralType) * 8;

   for (;;)
   {
      // Mask the maximum number of bits that can be stored without
      // precision loss in a single FloatingPointType, then sum and shift
      UnsignedIntegralType hi = u >> (std::max)(bit_index - MantissaBits, 0);
      u &= ~(hi << (std::max)(bit_index - MantissaBits, 0));

      *this += static_cast<FloatingPointType>(hi);  // sum

      bit_index -= MantissaBits;

      if (bit_index < 0)
         break;
      else
      {  // shift
         // FIXME replace with a single ldexp function once you implement it
         data.first  = std::ldexp(data.first,  (std::min)(MantissaBits, bit_index));
         data.second = std::ldexp(data.second, (std::min)(MantissaBits, bit_index));
      }
   }
}
// --

// -- Arithmetic backends
// Exact addition of two floating point numbers, given |a| > |b|
template <typename FloatingPointType>
std::pair<FloatingPointType, FloatingPointType>
cpp_double_float<FloatingPointType>::fast_exact_sum(const float_type& a, const float_type& b)
{
   using std::fabs;
   using std::isnormal;
   BOOST_ASSERT(fabs(a) >= fabs(b) || a == 0.0 || !isnormal(a));

   std::pair<float_type, float_type> out;
   out.first  = a + b;
   out.second = b - (out.first - a);

   return out;
}

// Exact addition of two floating point numbers
template <typename FloatingPointType>
std::pair<FloatingPointType, FloatingPointType>
cpp_double_float<FloatingPointType>::exact_sum(const float_type& a, const float_type& b)
{
   std::pair<float_type, float_type> out;

   out.first    = a + b;
   float_type v = out.first - a;
   out.second   = (a - (out.first - v)) + (b - v);

   return out;
}

// Exact subtraction of two floating point numbers
template <typename FloatingPointType>
std::pair<FloatingPointType, FloatingPointType>
cpp_double_float<FloatingPointType>::exact_difference(const float_type& a, const float_type& b)
{
   std::pair<float_type, float_type> out;

   out.first    = a - b;
   float_type v = out.first - a;
   out.second   = (a - (out.first - v)) - (b + v);

   return out;
}
   // Convert a pair of floats to standard form
template <typename FloatingPointType>
inline void
cpp_double_float<FloatingPointType>::normalize_pair(std::pair<float_type, float_type>& p, bool fast)
{
   p = (fast ? fast_exact_sum(p.first, p.second) : exact_sum(p.first, p.second));
}

// Split a floating point number in two (high and low) parts approximating the
// upper-half and lower-half bits of the float
template <typename FloatingPointType>
std::pair<FloatingPointType, FloatingPointType> inline cpp_double_float<FloatingPointType>::split(const FloatingPointType& a)
{
   static_assert(std::numeric_limits<FloatingPointType>::is_iec559,
                 "double_float<> invoked with non-native floating-point unit");

   // TODO Replace bit shifts with constexpr funcs for better compaitibility
   constexpr int               MantissaBits = std::numeric_limits<FloatingPointType>::digits;
   constexpr int               SplitBits    = MantissaBits / 2 + 1;
   constexpr FloatingPointType Splitter     = FloatingPointType((1ULL << SplitBits) + 1);
   auto                        SplitThreshold =
       (std::numeric_limits<FloatingPointType>::max)() / (Splitter*2);

   FloatingPointType temp, hi, lo;

   // Handle if multiplication with the splitter would cause overflow
   if (a > SplitThreshold || a < -SplitThreshold)
   {
      constexpr FloatingPointType Normalizer = FloatingPointType(1ULL << (SplitBits + 1));

      FloatingPointType a_ = a / Normalizer;

      temp = Splitter * a_;
      hi   = temp - (temp - a_);
      lo   = a_ - hi;

      hi *= Normalizer;
      lo *= Normalizer;
   }
   else
   {
      temp = Splitter * a;
      hi   = temp - (temp - a);
      lo   = a - hi;
   }

   return std::make_pair(hi, lo);
}

// Exact product of two floating point numbers
template <typename FloatingPointType>
std::pair<FloatingPointType, FloatingPointType>
cpp_double_float<FloatingPointType>::exact_product(const float_type& a, const float_type& b)
{
   std::pair<float_type, float_type> a_split = split(a);
   std::pair<float_type, float_type> b_split = split(b);
   std::pair<float_type, float_type> p;

   p.first  = a * b;
   p.second = ((a_split.first * b_split.first - p.first) + a_split.first * b_split.second + a_split.second * b_split.first) + a_split.second * b_split.second;

   return p;
}

// --

// -- Double-float arithmetic
// operator+ and operator+=
// double_float<> + native-float
template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>
operator+(const cpp_double_float<FloatingPointType>& a, const FloatingPointType& b)
{
   using double_float_t = cpp_double_float<FloatingPointType>;

   auto s = double_float_t::exact_sum(a.first(), b);

   s.second += a.second();
   double_float_t::normalize_pair(s);

   return double_float_t(s);
}

// double_float<> + double_float<>
// Satisfies IEEE-754 bounds
template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>
operator+(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b)
{
   using double_float_t = cpp_double_float<FloatingPointType>;

   std::pair<FloatingPointType, FloatingPointType> s, t;

   s = double_float_t::exact_sum(a.first(), b.first());
   t = double_float_t::exact_sum(a.second(), b.second());

   s.second += t.first;
   double_float_t::normalize_pair(s);
   s.second += t.second;
   double_float_t::normalize_pair(s);

   return double_float_t(s);
}


// double_float<> - native-float
template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>
operator-(const cpp_double_float<FloatingPointType>& a, const FloatingPointType& b)
{
   using double_float_t = cpp_double_float<FloatingPointType>;

   auto s = double_float_t::exact_difference(a.first(), b);

   s.second += a.second();
   double_float_t::normalize_pair(s);

   return double_float_t(s);
}

// double_float<> - double_float<>
template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>
operator-(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b)
{
   using double_float_t = cpp_double_float<FloatingPointType>;

   typename double_float_t::rep_type s, t;

   s = double_float_t::exact_difference(a.first(), b.first());
   t = double_float_t::exact_difference(a.second(), b.second());

   s.second += t.first;
   double_float_t::normalize_pair(s);

   s.second += t.second;
   double_float_t::normalize_pair(s);

   return double_float_t(s);
}

// double_float<> * native-float
template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>
operator*(const cpp_double_float<FloatingPointType>& a, const FloatingPointType& b)
{
   using double_float_t = cpp_double_float<FloatingPointType>;

   auto p = double_float_t::exact_product(a.first(), b);
   p.second += a.second() * b;

   double_float_t::normalize_pair(p);

   return double_float_t(p);
}

// double_float<> * double_float<>
template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>
operator*(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b)
{
   using double_float_t = cpp_double_float<FloatingPointType>;

   auto p = double_float_t::exact_product(a.first(), b.first());
   p.second += a.first() * b.second() + a.second() * b.first();

   double_float_t::normalize_pair(p);

   return double_float_t(p);
}

// double_float<> / native-float
template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>
operator/(const cpp_double_float<FloatingPointType>& a, const FloatingPointType& b)
{
   using double_float_t = cpp_double_float<FloatingPointType>;
   
   std::pair<FloatingPointType, FloatingPointType> p, q, s;

   p.first = a.first() / b;

   q = double_float_t::exact_product(p.first, b);
   s = double_float_t::exact_difference(a.first(), q.first);
   s.second += a.second();
   s.second -= q.second;

   p.second = (s.first + s.second) / b;

   double_float_t::normalize_pair(p);

   return double_float_t(p);
}

template <typename NumericType, typename FloatingPointType>
inline cpp_double_float<FloatingPointType>
operator/(const NumericType& a, const cpp_double_float<FloatingPointType>& b)
{
   return cpp_double_float<FloatingPointType>(a) / b;
}

// double_float<> / double_float<>
template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>
operator/(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b)
{
   using double_float_t = cpp_double_float<FloatingPointType>;

   std::pair<FloatingPointType, FloatingPointType> p;
   FloatingPointType p_prime;
   double_float_t r;

   p.first = a.first() / b.first(); // First approximation
   r       = a - b * p.first;

   p.second = r.first() / b.first();
   r -= b * p.second;

   p_prime = r.first() / b.first();

   double_float_t::normalize_pair(p);

   return double_float_t(p) + p_prime;
}
// --

// -- String Conversions
// FIXME Merge set_str() to operator<<
template <typename FloatingPointType>
inline void cpp_double_float<FloatingPointType>::set_str(std::string str)
{
   *this = 0;

   int final_exponent = 0;
   std::string::size_type pos;
   if ((pos = str.find('e')) != std::string::npos || (pos = str.find('E')) != std::string::npos)
   {
      std::stringstream ss;
      ss << str.data() + pos + 1 << std::endl;
      ss >> final_exponent;
      str = str.substr(0, pos);
   }

   pos = 0;
   while (!std::isdigit(str[pos]) && pos < str.size())
      if (str[pos] == '.')
         break;
      else pos++;

   if (pos == str.size())
      return;

   // Set the whole number part
   while (std::isdigit(str[pos]))
      *this = *this * FloatingPointType(10) + FloatingPointType(str[pos++] - '0');

   
   // Set the decimal number part
   if (str[pos] == '.')
   {
      std::string::size_type decimal_idx = pos;
      pos++;

      while (std::isdigit(str[pos]) && pos < str.size())
      {
         *this += (str[pos] - '0') / pow10(pos - decimal_idx);
         pos++;
      }
   }

   // Get the sign
   for (char c : str) {
      if (c == '-')
         *this = -*this;
      if ((c <= '9' && c >= '0') || c == '.')
         break;
   }

   *this *= pow10(final_exponent);
}

// -- Overloaded operators
template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>&
cpp_double_float<FloatingPointType>::operator+=(const cpp_double_float<FloatingPointType>& a)
{
   *this = *this + a;
   return *this;
}

template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>&
cpp_double_float<FloatingPointType>::operator-=(const cpp_double_float<FloatingPointType>& a)
{
   *this = *this - a;
   return *this;
}

template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>&
cpp_double_float<FloatingPointType>::operator*=(const cpp_double_float<FloatingPointType>& a)
{
   *this = *this * a;
   return *this;
}

template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>&
cpp_double_float<FloatingPointType>::operator/=(const cpp_double_float<FloatingPointType>& a)
{
   *this = *this / a;
   return *this;
}

template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>&
cpp_double_float<FloatingPointType>::operator+=(const FloatingPointType& a)
{
   *this = *this + a;
   return *this;
}

template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>&
cpp_double_float<FloatingPointType>::operator-=(const FloatingPointType& a)
{
   *this = *this - a;
   return *this;
}

template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>&
cpp_double_float<FloatingPointType>::operator*=(const FloatingPointType& a)
{
   *this = *this * a;
   return *this;
}

template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>&
cpp_double_float<FloatingPointType>::operator/=(const FloatingPointType& a)
{
   *this = *this / a;
   return *this;
}

template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>
cpp_double_float<FloatingPointType>::operator++(int)
{
   auto t(*this);
   ++*this;
   return t;
}

template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>
cpp_double_float<FloatingPointType>::operator--(int)
{
   auto t(*this);
   --*this;
   return t;
}

// -- Comparision Operators
// Comparison operators work by determining the type containing more detail at
// compile time, and then promoting the type with less detail to the type with
// more detail, and then comparing. Some minor complications arise while
// comparing an unsigned type to cpp_double_float<> that are handled as well

// operator>
template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<std::is_arithmetic<ComparisionType>::value, bool>::type
operator>(const cpp_double_float<FloatingPointType>& a, const ComparisionType& b)
{
   using first_type  = typename std::remove_reference<decltype(a)>::type;
   using second_type = typename std::remove_reference<decltype(b)>::type;
   using larger_type = typename std::conditional<std::numeric_limits<first_type>::digits >= std::numeric_limits<second_type>::digits, first_type, second_type>::type;

   return (   (std::is_unsigned<ComparisionType>::value && std::is_same<larger_type, ComparisionType>::value)
           && (a.is_negative())) ? false // Check for negative values
                                 : larger_type(a) > larger_type(b);
}

template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<std::is_same<FloatingPointType, ComparisionType>::value, bool>::type
operator>(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<ComparisionType>& b)
{
   return a.first() > b.first() ? true : a.first() == b.first() ? (a.second() > b.second() ? true : false) : false;
}

template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<!std::is_same<FloatingPointType, ComparisionType>::value, bool>::type
operator>(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<ComparisionType>& b)
{
   using first_type  = typename std::remove_reference<decltype(a)>::type;
   using second_type = typename std::remove_reference<decltype(b)>::type;
   using larger_type = typename std::conditional<std::numeric_limits<first_type>::digits >= std::numeric_limits<second_type>::digits, first_type, second_type>::type;
   
   return static_cast<larger_type>(a) > static_cast<larger_type>(b);
}

template <typename ComparisionType, typename FloatingPointType>
inline constexpr typename std::enable_if<std::is_arithmetic<ComparisionType>::value, bool>::type
operator>(const ComparisionType& a, const cpp_double_float<FloatingPointType>& b)
{
   return b < a;
}

// operator<
template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<std::is_arithmetic<ComparisionType>::value, bool>::type
operator<(const cpp_double_float<FloatingPointType>& a, const ComparisionType& b)
{
   using first_type  = typename std::remove_reference<decltype(a)>::type;
   using second_type = typename std::remove_reference<decltype(b)>::type;
   using larger_type = typename std::conditional<std::numeric_limits<first_type>::digits >= std::numeric_limits<second_type>::digits, first_type, second_type>::type;

   return (   (std::is_unsigned<ComparisionType>::value && std::is_same<larger_type, ComparisionType>::value)
           && (a.is_negative())) ? true // Check for negative values
                                 : larger_type(a) < larger_type(b);
}

template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<std::is_same<FloatingPointType, ComparisionType>::value, bool>::type
operator<(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<ComparisionType>& b)
{
   return a.first() < b.first() ? true : (a.first() == b.first()) ? (a.second() < b.second() ? true : false) : false;
}

template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<!std::is_same<FloatingPointType, ComparisionType>::value, bool>::type
operator<(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<ComparisionType>& b)
{
   using first_type  = typename std::remove_reference<decltype(a)>::type;
   using second_type = typename std::remove_reference<decltype(b)>::type;
   using larger_type = typename std::conditional<std::numeric_limits<first_type>::digits >= std::numeric_limits<second_type>::digits, first_type, second_type>::type;
   
   return static_cast<larger_type>(a) < static_cast<larger_type>(b);
}

template <typename ComparisionType, typename FloatingPointType>
inline constexpr typename std::enable_if<std::is_arithmetic<ComparisionType>::value, bool>::type
operator<(const ComparisionType& a, const cpp_double_float<FloatingPointType>& b)
{
   return b > a;
}

// operator>=
template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<std::is_arithmetic<ComparisionType>::value, bool>::type
operator>=(const cpp_double_float<FloatingPointType>& a, const ComparisionType& b)
{
   using first_type  = typename std::remove_reference<decltype(a)>::type;
   using second_type = typename std::remove_reference<decltype(b)>::type;
   using larger_type = typename std::conditional<std::numeric_limits<first_type>::digits >= std::numeric_limits<second_type>::digits, first_type, second_type>::type;

   return (   (std::is_unsigned<ComparisionType>::value && std::is_same<larger_type, ComparisionType>::value)
           && (a.is_negative())) ? false // Check for negative values
                                 : larger_type(a) >= larger_type(b);
}

template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<std::is_same<FloatingPointType, ComparisionType>::value, bool>::type
operator>=(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<ComparisionType>& b)
{
   return a.first() > b.first() ? true : (a.first() == b.first()) ? (a.second() >= b.second() ? true : false) : false;
}

template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<!std::is_same<FloatingPointType, ComparisionType>::value, bool>::type
operator>=(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<ComparisionType>& b)
{
   using first_type  = typename std::remove_reference<decltype(a)>::type;
   using second_type = typename std::remove_reference<decltype(b)>::type;
   using larger_type = typename std::conditional<std::numeric_limits<first_type>::digits >= std::numeric_limits<second_type>::digits, first_type, second_type>::type;

   return static_cast<larger_type>(a) >= static_cast<larger_type>(b);
}

template <typename ComparisionType, typename FloatingPointType>
inline constexpr typename std::enable_if<std::is_arithmetic<ComparisionType>::value, bool>::type
operator>=(const ComparisionType& a, const cpp_double_float<FloatingPointType>& b)
{
   return b <= a;
}

// operator <=
template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<std::is_arithmetic<ComparisionType>::value, bool>::type
operator<=(const cpp_double_float<FloatingPointType>& a, const ComparisionType& b)
{
   using first_type  = typename std::remove_reference<decltype(a)>::type;
   using second_type = typename std::remove_reference<decltype(b)>::type;
   using larger_type = typename std::conditional<std::numeric_limits<first_type>::digits >= std::numeric_limits<second_type>::digits, first_type, second_type>::type;

   return (   (std::is_unsigned<ComparisionType>::value && std::is_same<larger_type, ComparisionType>::value)
           && (a.is_negative())) ? true // Check for negative values
                                 : larger_type(a) <= larger_type(b);
}

template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<std::is_same<FloatingPointType, ComparisionType>::value, bool>::type
operator<=(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<ComparisionType>& b)
{
   return a.first() < b.first() ? true : (a.first() == b.first()) ? (a.second() <= b.second() ? true : false) : false;
}

template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<!std::is_same<FloatingPointType, ComparisionType>::value, bool>::type
operator<=(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<ComparisionType>& b)
{
   using first_type  = typename std::remove_reference<decltype(a)>::type;
   using second_type = typename std::remove_reference<decltype(b)>::type;
   using larger_type = typename std::conditional<std::numeric_limits<first_type>::digits >= std::numeric_limits<second_type>::digits, first_type, second_type>::type;

   return static_cast<larger_type>(a) <= static_cast<larger_type>(b);
}

template <typename ComparisionType, typename FloatingPointType>
inline constexpr typename std::enable_if<std::is_arithmetic<ComparisionType>::value, bool>::type
operator<=(const ComparisionType& a, const cpp_double_float<FloatingPointType>& b)
{
   return b >= a;
}

// operator ==
template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<std::is_arithmetic<ComparisionType>::value, bool>::type
operator==(const cpp_double_float<FloatingPointType>& a, const ComparisionType& b)
{
   using first_type  = typename std::remove_reference<decltype(a)>::type;
   using second_type = typename std::remove_reference<decltype(b)>::type;
   using larger_type = typename std::conditional<std::numeric_limits<first_type>::digits >= std::numeric_limits<second_type>::digits, first_type, second_type>::type;

   return (   (std::is_unsigned<ComparisionType>::value && std::is_same<larger_type, ComparisionType>::value)
           && (a.is_negative())) ? false // Check for negative values
                                 : larger_type(a) == larger_type(b);
}

template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<std::is_same<FloatingPointType, ComparisionType>::value, bool>::type
operator==(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<ComparisionType>& b)
{
   return a.first() == b.first() ? (a.second() == b.second() ? true : false) : false;
}

template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<!std::is_same<FloatingPointType, ComparisionType>::value, bool>::type
operator==(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<ComparisionType>& b)
{
   using first_type  = typename std::remove_reference<decltype(a)>::type;
   using second_type = typename std::remove_reference<decltype(b)>::type;
   using larger_type = typename std::conditional<std::numeric_limits<first_type>::digits >= std::numeric_limits<second_type>::digits, first_type, second_type>::type;

   return static_cast<larger_type>(a) == static_cast<larger_type>(b);
}

template <typename ComparisionType, typename FloatingPointType>
inline constexpr typename std::enable_if<std::is_arithmetic<ComparisionType>::value, bool>::type
operator==(const ComparisionType& a, const cpp_double_float<FloatingPointType>& b)
{
   return b == a;
}

// operator !=
template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<std::is_arithmetic<ComparisionType>::value, bool>::type
operator!=(const cpp_double_float<FloatingPointType>& a, const ComparisionType& b)
{
   using first_type  = typename std::remove_reference<decltype(a)>::type;
   using second_type = typename std::remove_reference<decltype(b)>::type;
   using larger_type = typename std::conditional<std::numeric_limits<first_type>::digits >= std::numeric_limits<second_type>::digits, first_type, second_type>::type;

   return (   (std::is_unsigned<ComparisionType>::value && std::is_same<larger_type, ComparisionType>::value)
           && (a.is_negative())) ? true // Check for negative values
                                 : larger_type(a) != larger_type(b);
}

template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<std::is_same<FloatingPointType, ComparisionType>::value, bool>::type
operator!=(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<ComparisionType>& b)
{
   return a.first() != b.first() ? true : (a.second() != b.second() ? true : false);
}

template <typename FloatingPointType, typename ComparisionType>
inline constexpr typename std::enable_if<!std::is_same<FloatingPointType, ComparisionType>::value, bool>::type
operator!=(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<ComparisionType>& b)
{
   using first_type  = typename std::remove_reference<decltype(a)>::type;
   using second_type = typename std::remove_reference<decltype(b)>::type;
   using larger_type = typename std::conditional<std::numeric_limits<first_type>::digits >= std::numeric_limits<second_type>::digits, first_type, second_type>::type;

   return static_cast<larger_type>(a) != static_cast<larger_type>(b);
}

template <typename ComparisionType, typename FloatingPointType>
inline constexpr typename std::enable_if<std::is_arithmetic<ComparisionType>::value, bool>::type
operator!=(const ComparisionType& a, const cpp_double_float<FloatingPointType>& b)
{
   return b != a;
}
// --

// -- Input/Output Streaming
template <typename FloatingPointType, typename char_type, typename traits_type>
std::basic_ostream<char_type, traits_type>&
operator<<(std::basic_ostream<char_type, traits_type>& os, const cpp_double_float<FloatingPointType>& f)
{
  using std::fabs;
   auto is_set = [&](std::ios::fmtflags flg) {
      return os.flags() & flg;
   };

   using std::isinf;
   using std::floor;
   using std::log10;
   if (isinf(f.first()))
   {
      os << f.first();
      return os;
   }

  if (f < cpp_double_float<FloatingPointType>(0) || os.flags() & std::ios::showpos)
      os << (f < cpp_double_float<FloatingPointType>(0) ? "-" : "+");

   int exp10 = 0;

   if (f != cpp_double_float<FloatingPointType>(0))
      exp10 = (int)floor(log10(fabs(f.first())));
   else
      exp10 = 0;

   auto f_prime = (f > cpp_double_float<FloatingPointType>(0) ? f : -f);
   f_prime /= cpp_double_float<FloatingPointType>::pow10(exp10);
   
   // TODO Handle subnormal numbers

   if (f_prime < cpp_double_float<FloatingPointType>(1) && f_prime > cpp_double_float<FloatingPointType>(0))
   {
      f_prime *= FloatingPointType(10);
      exp10++;
   }
   else if (f_prime >= 10)
   {
      f_prime /= FloatingPointType(10);
      exp10--;
   }

   // Collect all the required digits to print (plus one digit for rounding)
   std::vector<int> digits;

   int p = (int)os.precision();
   if (is_set(std::ios::fixed))
      p += exp10 + 1;
   else if (is_set(std::ios::scientific))
      p = (std::max)(1, p + 1);
   else
      p = (std::max)(p, 1);

   // TODO Maybe switch to fmod() based digit extraction for correct rounding?
   while (p-- > 0)
   {
      // FIXME Replace with std::floor function
      int digit = static_cast<int>(f_prime.first());

      if (f_prime.second() < 0 && (f_prime.first() - (FloatingPointType)digit < -f_prime.second()))
         digit -= 1;

      BOOST_ASSERT(digit >= 0 && digit <= 9);

      digits.push_back(digit);

      f_prime -= static_cast<FloatingPointType>(digit);
      f_prime *= static_cast<FloatingPointType>(10);
   }

   auto round_up = [&]() {
      int i = digits.size() - 1;
      if (i > -1)
      {
         do
         {
            if (digits[i] == 9)
               digits[i--] = 0;
            else
            {
               digits[i--] += 1;
               break;
            }
         } while (i >= 0);

         // Special case in which all of the collected digits are incorrectly
         // rounded (e.g. 9.999 rounded to three significant figures = 10.0)
         if (i == -1 && digits[0] == 0)
         {
            digits = {1};
            exp10++;
         }
      }
      else
      {
         digits.insert(digits.begin(), 1);
         exp10++;
      }
   };

   // Perform rounding (rounding mode = round-to-nearest, ties-to-even)
   // Three possible cases: the remaining part of the number is
   // (1) greater than 0.5 (round-up)
   // (2) less than 0.5 (round-down)
   // (3) equal to 0.5 (round-to-even)
   if (f_prime > 5)
      round_up();
   else if (f_prime < 5)  // do nothing. already correctly rounded
   {
      // TODO add some kind of an option for configurable rounding mode
   }
   else if (digits.back() % 2 != 0)
      // remaining part is exactly 0.5, so round-to-even
      round_up();

   // Remove trailing zeroes
   if (!is_set(std::ios::fixed) && !is_set(std::ios::scientific) && !is_set(std::ios::showpoint))
      while (digits.back() == 0 && (std::ptrdiff_t(digits.size()) > std::ptrdiff_t(1 + exp10) || exp10 < 0))
         digits.pop_back();

   auto fill_zeroes = [](std::string& s, size_t pos, int n) {
      for (int i = 0; i < n; ++i)
         s.insert(pos, "0");
   };

   // Print the required numbers to a string
   std::string str = "";
   size_t      str_size;

   for (auto d : digits)
      str.push_back(static_cast<char>(d + '0'));

   // Fixed-point style
   if (is_set(std::ios::fixed) || (exp10 >= -4 && (exp10 < os.precision()) && !is_set(std::ios::scientific)))
   {
      if (exp10 + 1 <= 0) // Number < 1
      {
         str_size = (size_t)os.precision() + 2;

         if (!is_set(std::ios::fixed) && os.precision() == 0)
            str_size++;

         str.insert(0, "0.");

         fill_zeroes(str, 2, -(exp10 + 1));
         if (!is_set(std::ios::fixed))
            str_size += -(exp10 + 1);
      }
      else  // Number >= 1
      {
         str_size = std::size_t(1 + os.precision());
         if (is_set(std::ios::fixed))
            str_size += exp10 + 1;

         fill_zeroes(str, str.size(), str_size - str.size() - 1);

         BOOST_ASSERT(std::ptrdiff_t(exp10 + 1) <= std::ptrdiff_t(str.size()));
         str.insert(exp10 + 1, ".");
      }
 
      while (str.size() > str_size)
         str.pop_back();
      while (str.size() < str_size)
         str.push_back('0');
      while (!is_set(std::ios::showpoint) && !is_set(std::ios::fixed) && str.back() == '0')
         str.pop_back();

      if (str.back() == '.' && !is_set(std::ios::showpoint))
         str.pop_back();
   }
   // Scientific style
   else if (is_set(std::ios::scientific) || (exp10 < -4 || (exp10 + 1 > (std::max)((int)os.precision(), 1))))
   {
      str_size = (size_t)os.precision() + 1;
      if (os.precision() == 0 || is_set(std::ios::scientific))
         str_size++;

      str.insert(1, ".");
      // Pad with zeroes
      fill_zeroes(str, str.size(), str_size - str.size());

      // Remove trailing zeroes
      while (str.size() > str_size)
         str.pop_back();
      while (!is_set(std::ios::scientific) && !is_set(std::ios::showpoint) && str.back() == '0')
         str.pop_back();

      // Remove unnecessary point
      if (str.back() == '.' && !is_set(std::ios::showpoint))
         str.pop_back();

      std::stringstream ss;

      ss << str;
      ss << (os.flags() & std::ios::uppercase ? "E" : "e");
      ss << (exp10 < 0 ? "-" : "+");
      using std::log10;
      ss.width((std::max)(1 + (std::streamsize)log10(exp10), (std::streamsize)2));
      ss.fill('0');
      ss << fabs(exp10);

      str = ss.str();
   }
   else if (exp10 == os.precision())
   {
      if (os.flags() & std::ios::showpoint)
         str.push_back('.');
   }

   os << str;
   return os;
}

template <typename FloatingPointType, typename char_type, typename traits_type>
std::basic_istream<char_type, traits_type>&
operator>>(std::basic_istream<char_type, traits_type>& is, cpp_double_float<FloatingPointType>& f)
{
   std::string str;
   is >> str;
   f.set_str(str);
   return is;
}
// --

// -- Misc helper functions
template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType> cpp_double_float<FloatingPointType>::pow10(int p)
{
   using local_float_type = cpp_double_float<FloatingPointType>;

   local_float_type result;

   if (p < 0) result = local_float_type(1U) / pow10(-p);
   else if (p == 0) result = local_float_type(1U);
   else if (p == 1) result = local_float_type(10U);
   else if (p == 2) result = local_float_type(100U);
   else if (p == 3) result = local_float_type(1000U);
   else if (p == 4) result = local_float_type(10000U);
   else
   {
      result = local_float_type(1U);

      local_float_type y(10U);

      std::uint32_t p_local = (std::uint32_t)p;

      for (;;)
      {
         if (std::uint_fast8_t(p_local & 1U) != 0U) result *= y;

         p_local >>= 1U;

         if (p_local == 0U) break;
         else y *= y;
      }
   }

   return result;
}
// --

// -- DEBUGGING
template <typename FloatingPointType>
inline std::string cpp_double_float<FloatingPointType>::get_raw_str() const
{
   std::stringstream ss;
   ss << std::hexfloat << data.first << " + " << std::hexfloat << data.second;
   return ss.str();
}
// --

} } } // namespace boost::multiprecision::backends

// Specialization of numeric_limits for cpp_double_float<>
template <typename FloatingPointType>
class std::numeric_limits<boost::multiprecision::backends::cpp_double_float<FloatingPointType> > : public std::numeric_limits<FloatingPointType>
{
 public:
   static constexpr bool is_iec559 = false;

   static constexpr int digits       = 2 * std::numeric_limits<FloatingPointType>::digits - 2;
   static constexpr int digits10     =  int(float(digits - 1) * 0.301F);
   static constexpr int max_digits10 =  int(float(digits)     * 0.301F) + 2;

   static constexpr int max_exponent = std::numeric_limits<FloatingPointType>::max_exponent - std::numeric_limits<FloatingPointType>::digits;
   static constexpr int min_exponent = std::numeric_limits<FloatingPointType>::min_exponent + std::numeric_limits<FloatingPointType>::digits;

   static constexpr boost::multiprecision::backends::cpp_double_float<FloatingPointType>(min)() noexcept { return (std::numeric_limits<FloatingPointType>::min)(); }
   static constexpr boost::multiprecision::backends::cpp_double_float<FloatingPointType>(max)() noexcept { return (std::numeric_limits<FloatingPointType>::max)(); }
   static constexpr boost::multiprecision::backends::cpp_double_float<FloatingPointType> lowest() noexcept { return std::numeric_limits<FloatingPointType>::lowest(); }
   static constexpr boost::multiprecision::backends::cpp_double_float<FloatingPointType> epsilon() noexcept {return std::numeric_limits<FloatingPointType>::epsilon(); } // NOTE: doesn't construct from float128
   static constexpr boost::multiprecision::backends::cpp_double_float<FloatingPointType> round_error() noexcept { return std::numeric_limits<FloatingPointType>::round_error(); }
   static constexpr boost::multiprecision::backends::cpp_double_float<FloatingPointType> denorm_min() noexcept { return std::numeric_limits<FloatingPointType>::denorm_min(); }
   static constexpr boost::multiprecision::backends::cpp_double_float<FloatingPointType> infinity() noexcept { return std::numeric_limits<FloatingPointType>::infinity(); }
   static constexpr boost::multiprecision::backends::cpp_double_float<FloatingPointType> quiet_NaN() noexcept { return std::numeric_limits<FloatingPointType>::quiet_NaN(); }
   static constexpr boost::multiprecision::backends::cpp_double_float<FloatingPointType> signaling_NaN() noexcept { return std::numeric_limits<FloatingPointType>::signaling_NaN(); }
};
// TODO have explicit specializations for cpp_double_float< float/double >

#endif // BOOST_MP_CPP_DOUBLE_FLOAT_2021_06_05_HPP
