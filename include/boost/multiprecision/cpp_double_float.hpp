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

#include <boost/multiprecision/number.hpp>
#include <boost/assert.hpp>

namespace boost { namespace multiprecision {

namespace backends {

template<typename FloatingPointType>
class cpp_double_float;

} // namespace backends

template<typename FloatingPointType>
struct number_category<backends::cpp_double_float<FloatingPointType>>
  : public std::integral_constant<int, number_kind_floating_point>
{
};

namespace backends {

/*
* A cpp_double_float is represented by an unevaluated sum of two floating-point
* units (say a0 and a1), which satisfy |a1| <= (1 / 2) * ulp(a0)
*/
template <typename FloatingPointType>
class cpp_double_float
{
 public:
   using float_type = FloatingPointType;
   using rep_type = std::pair<float_type, float_type>;

   // Constructors
   cpp_double_float() {}
   cpp_double_float(const float_type& a) : data(std::make_pair(a, (float_type)0)) {}
   cpp_double_float(const float_type& a, const float_type& b) : data(std::make_pair(a, b)) {}
   cpp_double_float(const std::pair<float_type, float_type>& p) : data(p) {}
   cpp_double_float(const cpp_double_float& a) : data(a.data) {}

   // Casts
   operator signed char() const { return (signed char)data.first; }
   operator signed short() const { return (signed short)data.first; }
   operator signed int() const { return (signed int)data.first + (signed int)data.second; }
   operator signed long() const { return (signed long)data.first + (signed long)data.second; }
   operator signed long long() const { return (signed long long)data.first + (signed long long)data.second; }
   operator unsigned char() const { return (unsigned char)data.first; }
   operator unsigned short() const { return (unsigned short)data.first; }
   operator unsigned int() const { return (unsigned int)data.first + (unsigned int)data.second; }
   operator unsigned long() const { return (unsigned long)data.first + (unsigned long)data.second; }
   operator unsigned long long() const { return (unsigned long long)data.first + (unsigned long long)data.second; }
   operator float() const { return (float)data.first + (float)data.second; }
   operator double() const { return (double)data.first + (double)data.second; }
   operator long double() const { return (long double)data.first + (long double)data.second; }

   // Methods
   cpp_double_float<float_type> negative() const { return cpp_double_float<float_type>(-data.first, -data.second); }

   std::string get_str(int precision) const;
   void set_str(const std::string& str);

   // Getters/Setters
   const float_type& first() const { return data.first; }
   const float_type& second() const { return data.second; }

   // Helper functions
   static std::pair<float_type, float_type> fast_exact_sum(const float_type& a, const float_type& b);
   static std::pair<float_type, float_type> exact_sum(const float_type& a, const float_type& b);
   static std::pair<float_type, float_type> exact_difference(const float_type& a, const float_type& b);
   static std::pair<float_type, float_type> exact_product(const float_type& a, const float_type& b);

   static std::pair<float_type, float_type> split(const float_type& a);

   static void normalize_pair(std::pair<float_type, float_type>& p, bool fast = true);

   // Operators
   cpp_double_float& operator=(const cpp_double_float& a)
   {
      data = a.data;
      return *this;
   }
   cpp_double_float& operator=(const std::string& str);

   cpp_double_float& operator+=(const cpp_double_float& a);
   cpp_double_float& operator-=(const cpp_double_float& a);
   cpp_double_float& operator*=(const cpp_double_float& a);
   cpp_double_float& operator/=(const cpp_double_float& a);
   cpp_double_float  operator++(int);
   cpp_double_float  operator--(int);
   cpp_double_float& operator++() { return *this += cpp_double_float<float_type>((double)1.); }
   cpp_double_float& operator--() { return *this -= cpp_double_float<float_type>(1.); }
   cpp_double_float  operator-() const { return negative(); }

   // -- DEBUGGING
   std::string to_string_raw() const;
   // --
 private:
   rep_type data;
};

// -- Arithmetic backends
// Exact addition of two floating point numbers, given |a| > |b|
template <typename FloatingPointType>
std::pair<FloatingPointType, FloatingPointType>
cpp_double_float<FloatingPointType>::fast_exact_sum(const float_type& a, const float_type& b)
{
   assert(std::abs(a) >= std::abs(b));

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
   out.second   = (a - (out.first - v)) + (b + v);

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

   constexpr int MantissaBits = std::numeric_limits<FloatingPointType>::digits;
   constexpr int               SplitBits = MantissaBits / 2 + 1;
   constexpr FloatingPointType SplitThreshold =
       std::numeric_limits<FloatingPointType>::max() / FloatingPointType(1 << (SplitBits + 1));
   constexpr FloatingPointType Splitter = FloatingPointType((1 << SplitBits) + 1);

   FloatingPointType                               temp, hi, lo;
   std::pair<FloatingPointType, FloatingPointType> out;

   // Handle if multiplication with the splitter would cause overflow
   if (a > SplitThreshold || a < -SplitThreshold)
   {
      FloatingPointType a_ = a / FloatingPointType(1 << (SplitBits + 1));
      temp = Splitter * a;
      hi   = temp - (temp - a_);
      lo   = a_ - hi;
      hi *= FloatingPointType(1 << (SplitBits + 1));
      lo *= FloatingPointType(1 << (SplitBits + 1));
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

// double_float<> - double_float<>
template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>
operator-(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b)
{
   using double_float_t = cpp_double_float<FloatingPointType>;

   double_float_t::rep_type s, t;

   s = double_float_t::exact_difference(a.first(), b.first());
   t = double_float_t::exact_difference(a.second(), b.second());

   s.second += t.first;
   double_float_t::normalize_pair(s, false);

   s.second += t.second;
   double_float_t::normalize_pair(s, false);

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

   r = double_float_t(p) + p_prime;

   return r;
}
// --

// -- String Conversions
// [FIXME] Basic string conversion
template <typename FloatingPointType>
inline std::string cpp_double_float<FloatingPointType>::get_str(int precision) const
{
   std::ostringstream ss;

   cpp_double_float<FloatingPointType> d(*this);

   if (d.first() < 0)
   {
      d = -d;
      ss << "-";
   }
   else if (d.first() == 0.0)
   {
     // FIXME
      return "0";
   }

   int pos = (int)std::floor(std::log10(d.first())), digits_printed = 0;

   // Print the whole number part
   if (pos >= 0)
   {
      while (pos >= 0)
      {
         int digit = (int)std::fmod(double(d / (FloatingPointType)std::pow(10.0, pos)), 10.0);
         ss << digit;

         d -= FloatingPointType(digit * std::pow(10, pos));

         pos--, digits_printed++;
      }
   }
   else ss << "0";

   ss << '.';
   // Print the decimal number part
   do
   {
      d *= 10;
      int digit = (int)std::trunc(d.first());
      ss << digit;

      d -= (FloatingPointType)digit;

      digits_printed++;
   } while (digits_printed <= precision);

   return ss.str();
}

template <typename FloatingPointType>
inline void cpp_double_float<FloatingPointType>::set_str(const std::string& str)
{
   *this = 0.0;
   
   size_t pos = 0;
   while (!std::isdigit(str[pos]))
      if (str[pos] == '.')
         break;
      else pos++;

   // Set the whole number part
   while (std::isdigit(str[pos]))
      *this = *this * FloatingPointType(10) + FloatingPointType(str[pos++] - '0');

   BOOST_ASSERT(str[pos] == '.');

   size_t decimal_idx = pos;
   pos++;

   // Set the decimal number part
   while (std::isdigit(str[pos]) && pos < str.size())
   {
      auto inv_pow10 = [](int exp) {
         cpp_double_float<FloatingPointType> x(1.0);
         while (exp--) x /= 10.0;
         return x;
      };

      *this += inv_pow10(pos - decimal_idx) * (FloatingPointType)(str[pos] - '0');
      pos++;
   }

   // Get the sign
   for (char c : str) {
      if (c == '-')
         *this = -*this;
      if (c <= '9' || c >= '0' || c == '.')
         break;
   }
}

template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType>&
boost::multiprecision::backends::cpp_double_float<FloatingPointType>::operator=(const std::string& str)
{
   int digit_idx = 0;
   cpp_double_float<FloatingPointType> d(0.0);

   for (char c : str)
   {
      if (c == '.')
         digit_idx = -1, continue;

      if (digit_idx >= 0)
         d = d * 10 + (FloatingPointType)(c - '0')
      else
         d += (FloatingPointType)((c - '0') / std::pow(10.0, digit_idx--));
   }

   *this = d;
}
// --

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

template <typename FloatingPointType, typename char_type, typename traits_type>
std::basic_ostream<char_type, traits_type>&
operator<<(std::basic_ostream<char_type, traits_type>& os, const cpp_double_float<FloatingPointType>& f)
{
   os << f.get_str((int)os.precision());
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

// -- DEBUGGING
template <typename FloatingPointType>
inline std::string cpp_double_float<FloatingPointType>::to_string_raw() const
{
   std::stringstream ss;
   ss << std::hexfloat << data.first << " + " << std::hexfloat << data.second;
   return ss.str();
}
// --

} } } // namespace boost::multiprecision::backends

#endif // BOOST_MP_CPP_DOUBLE_FLOAT_2021_06_05_HPP