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

   constexpr cpp_double_float(const float_type& a) : data(std::make_pair(a, (float_type)0)) {}
   constexpr cpp_double_float(const float_type& a, const float_type& b) : data(std::make_pair(a, b)) {}
   constexpr cpp_double_float(const std::pair<float_type, float_type>& p) : data(p) {}
   constexpr cpp_double_float(const cpp_double_float& a) : data(a.data) {}

   template <typename UnsignedIntegralType,
             typename std::enable_if<(   (std::is_integral<UnsignedIntegralType>::value == true)
                                      && (std::is_unsigned<UnsignedIntegralType>::value == true)
                                      && (std::numeric_limits<UnsignedIntegralType>::digits <= std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_float(UnsignedIntegralType u) : data(std::make_pair((float_type) u, (float_type) 0.0L)) {}

   template <typename SignedIntegralType,
             typename std::enable_if<(   (std::is_integral<SignedIntegralType>::value == true)
                                      && (std::is_signed  <SignedIntegralType>::value == true)
                                      && (std::numeric_limits<SignedIntegralType>::digits + 1 <= std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_float(SignedIntegralType n)
      : data(n >= 0 ? std::make_pair( (float_type)  n, (float_type) 0.0L)
                    : std::make_pair(-(float_type) -n, (float_type) 0.0L)) {}

   template <typename UnsignedIntegralType,
             typename std::enable_if<((std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true) && (std::numeric_limits<UnsignedIntegralType>::digits > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   cpp_double_float(UnsignedIntegralType u);

   template <typename SignedIntegralType,
             typename std::enable_if<(   (std::is_integral<SignedIntegralType>::value == true)
                                      && (std::is_signed  <SignedIntegralType>::value == true)
                                      && (std::numeric_limits<SignedIntegralType>::digits + 1 > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   cpp_double_float(SignedIntegralType n) : cpp_double_float(static_cast<std::make_unsigned<SignedIntegralType>::type>(std::abs(n)))
   {
      if (n < 0)
         *this = -*this;
   }

   cpp_double_float(const std::string str)
   {
      set_str(str);
   }

   ~cpp_double_float() = default;

   // Casts
   template <typename cast_type>
   cast_type cast() const { return (cast_type) (static_cast<std::make_signed<cast_type>::type>(data.first) + data.second); }
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
   cpp_double_float<float_type> negative() const { return cpp_double_float<float_type>(-data.first, -data.second); }

   std::string get_str(int precision) const;
   void set_str(const std::string& str);

   // Getters/Setters
   const float_type& first () const { return data.first; }
   const float_type& second() const { return data.second; }

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
   cpp_double_float& operator=(const cpp_double_float& a)
   {
      if(this != &a)
      {
        data = a.data;
      }

      return *this;
   }

   cpp_double_float& operator=(cpp_double_float&& a)
   {
      data = a.data;

      return *this;
   }

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
 private:
   rep_type data;

   static cpp_double_float<float_type> pow10(int x);
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
      UnsignedIntegralType hi = u >> std::max(bit_index - MantissaBits, 0);
      u &= ~(hi << std::max(bit_index - MantissaBits, 0));

      *this += static_cast<FloatingPointType>(hi);  // sum

      bit_index -= MantissaBits;

      if (bit_index < 0)
         break;
      else  // shift
         *this *= static_cast<FloatingPointType>(UnsignedIntegralType(1) << std::min(MantissaBits, bit_index));
   }
}
// --

// -- Arithmetic backends
// Exact addition of two floating point numbers, given |a| > |b|
template <typename FloatingPointType>
std::pair<FloatingPointType, FloatingPointType>
cpp_double_float<FloatingPointType>::fast_exact_sum(const float_type& a, const float_type& b)
{
   BOOST_ASSERT(std::abs(a) >= std::abs(b) || a == 0.0);

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
   constexpr int MantissaBits = std::numeric_limits<FloatingPointType>::digits;
   constexpr int               SplitBits = MantissaBits / 2 + 1;
   constexpr FloatingPointType SplitThreshold =
       std::numeric_limits<FloatingPointType>::max() / FloatingPointType(1ULL << (SplitBits + 1));
   constexpr FloatingPointType Splitter = FloatingPointType((1ULL << SplitBits) + 1);

   FloatingPointType                               temp, hi, lo;
   std::pair<FloatingPointType, FloatingPointType> out;

   // Handle if multiplication with the splitter would cause overflow
   if (a > SplitThreshold || a < -SplitThreshold)
   {
      FloatingPointType a_ = a / FloatingPointType(1ULL << (SplitBits + 1));
      temp = Splitter * a;
      hi   = temp - (temp - a_);
      lo   = a_ - hi;
      hi *= FloatingPointType(1ULL << (SplitBits + 1));
      lo *= FloatingPointType(1ULL << (SplitBits + 1));
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

   auto d_prime(pos < 0 ? d : d / pow10(pos));

   auto print_next_digit = [&]() {
      int digit = (int)(d_prime.first());
      BOOST_ASSERT(digit >= 0 && digit <= 9);

      ss << digit;

      d_prime -= FloatingPointType(digit);
      d_prime *= (FloatingPointType)10.0;
   };

   do
   {
      print_next_digit();
      pos--, digits_printed++;
   } while (pos >= 0);

   ss << '.';

   // Print the decimal number part
   do
   {
      print_next_digit();
      digits_printed++;
   } while (digits_printed <= precision);

   return ss.str();
}

template <typename FloatingPointType>
inline void cpp_double_float<FloatingPointType>::set_str(const std::string& str)
{
   *this = 0;

   std::string::size_type pos = 0;
   while (!std::isdigit(str[pos]) && pos < str.size())
      if (str[pos] == '.')
         break;
      else pos++;

   if (pos == str.size())
      return;

   // Set the whole number part
   while (std::isdigit(str[pos]))
      *this = *this * FloatingPointType(10) + FloatingPointType(str[pos++] - '0');

   BOOST_ASSERT(str[pos] == '.');

   std::string::size_type decimal_idx = pos;
   pos++;

   // Set the decimal number part
   while (std::isdigit(str[pos]) && pos < str.size())
   {
      *this += (str[pos] - '0') / pow10(pos - decimal_idx);
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

// -- Misc helper functions
template <typename FloatingPointType>
inline cpp_double_float<FloatingPointType> cpp_double_float<FloatingPointType>::pow10(int x)
{
   BOOST_ASSERT(x >= 0);

   cpp_double_float<FloatingPointType> b(1.0);
   while (x-- > 0)
      b *= (FloatingPointType)10.;
   return b;
}
// --

// -- DEBUGGING
template <typename FloatingPointType>
inline std::string cpp_double_float<FloatingPointType>::get_raw_str() const
{
   std::stringstream ss;
   ss.precision(34);
   ss /*<< std::hexfloat*/ << data.first << " + " /*<< std::hexfloat*/ << data.second;
   return ss.str();
}
// --

} } } // namespace boost::multiprecision::backends

#endif // BOOST_MP_CPP_DOUBLE_FLOAT_2021_06_05_HPP
