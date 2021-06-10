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
   cpp_double_float(const float_type& a) : data(std::make_pair(a, 0)) {}
   cpp_double_float(const float_type& a, const float_type& b) : data(std::make_pair(a, b)) {}
   cpp_double_float(const std::pair<float_type, float_type>& p) : data(p) {}
   cpp_double_float(const cpp_double_float& a) : data(a.data) {}

   // Casts
   operator signed char() const { return (signed char)data.first; }
   operator signed short() const { return (signed short)data.first; }
   operator signed int() const { return (signed int)data.first; }
   operator signed long() const { return (signed long)data.first; }
   operator signed long long() const { return (signed long long)data.first; }
   operator unsigned char() const { return (unsigned char)data.first; }
   operator unsigned short() const { return (unsigned short)data.first; }
   operator unsigned int() const { return (unsigned int)data.first; }
   operator unsigned long() const { return (unsigned long)data.first; }
   operator unsigned long long() const { return (unsigned long long)data.first + (unsigned long long)data.second; } // FIXME
   operator float() const { return (float)data.first; }
   operator double() const { return (double)data.first + (double)data.second; }                // FIXME
   operator long double() const { return (long double)data.first + (long double)data.second; } // FIXME

   // Methods
   cpp_double_float<float_type> negative() const { return cpp_double_float<float_type>(-data.first, -data.second); }
   std::string                  get_str(int precision) const;

   // Getters/Setters
   const float_type& first() const { return data.first; }
   const float_type& second() const { return data.second; }

   // Helper functions
   static std::pair<float_type, float_type> fast_exact_sum(const float_type& a, const float_type& b);
   static std::pair<float_type, float_type> exact_sum(const float_type& a, const float_type& b);
   static std::pair<float_type, float_type> exact_product(const float_type& a, const float_type& b);
   static std::pair<float_type, float_type> split(const float_type& a);

   static void normalize_pair(std::pair<float_type, float_type>& p, bool fast = true);

   // Operators
   cpp_double_float& operator=(const cpp_double_float& a)
   {
      data = a.data;
      return *this;
   }

   cpp_double_float& operator+=(const cpp_double_float& a);
   cpp_double_float& operator-=(const cpp_double_float& a);
   cpp_double_float& operator*=(const cpp_double_float& a);
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
   return cpp_double_float<FloatingPointType>(a + (-b));
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
   return cpp_double_float<FloatingPointType>(a + b.negative());
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
// --

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

   while (precision-- >= 0 || d.first() > 1.)
   {
      using namespace std;

      if (d.first() > 1.)
      {
         int d_exp = (int)std::log10(d.first());
         int digit = std::floor(d.first() / std::pow(10, d_exp));

         ss << digit;
         d -= float_type(digit * std::pow(10, d_exp));
      }
      else
      {
         if (ss.str().find('.') == std::string::npos)
            ss << '.';

         d *= (float_type)10.;

         int digit = int(d.first());
         ss << digit;
         d = d - (FloatingPointType)digit;
      }
   }

   return ss.str();
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
std::basic_ostream<char_type, traits_type>& operator<<
(std::basic_ostream<char_type, traits_type>& os, const cpp_double_float<FloatingPointType>& f)
{
   os << f.get_str(os.precision());
   return os;
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
