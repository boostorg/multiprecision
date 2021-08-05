///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_QUAD_FLOAT_2021_07_29_HPP
#define BOOST_MP_CPP_QUAD_FLOAT_2021_07_29_HPP

#include <boost/config.hpp>

#include <type_traits>
#include <string>
#include <utility>
#include <limits>
#include <sstream>
#include <tuple>
#include <vector>
#include <array>

#include <boost/assert.hpp>
#include <boost/multiprecision/number.hpp>
#include <boost/multiprecision/detail/float_string_cvt.hpp>
#include <boost/multiprecision/detail/hash.hpp>
#include <boost/multiprecision/cpp_double_float.hpp>
#include <boost/type_traits/common_type.hpp>

namespace boost { namespace multiprecision { namespace backends {

template <typename FloatingPointType>
class cpp_quad_float;

template <typename FloatingPointType>
inline cpp_quad_float<FloatingPointType> operator+(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b);
template <typename FloatingPointType>
inline cpp_quad_float<FloatingPointType> operator-(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b);
template <typename FloatingPointType>
inline cpp_quad_float<FloatingPointType> operator*(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b);
template <typename FloatingPointType>
inline cpp_quad_float<FloatingPointType> operator/(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b);

template <typename FloatingPointType>
inline cpp_quad_float<FloatingPointType> operator+(const cpp_quad_float<FloatingPointType>& a, const FloatingPointType& b);
template <typename FloatingPointType>
inline cpp_quad_float<FloatingPointType> operator-(const cpp_quad_float<FloatingPointType>& a, const FloatingPointType& b);
template <typename FloatingPointType>
inline cpp_quad_float<FloatingPointType> operator*(const cpp_quad_float<FloatingPointType>& a, const FloatingPointType& b);
template <typename FloatingPointType>
inline cpp_quad_float<FloatingPointType> operator/(const cpp_quad_float<FloatingPointType>& a, const FloatingPointType& b);

template <typename FloatingPointType>
inline bool operator<(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b);
template <typename FloatingPointType>
inline bool operator<=(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b);
template <typename FloatingPointType>
inline bool operator==(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b);
template <typename FloatingPointType>
inline bool operator!=(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b);
template <typename FloatingPointType>
inline bool operator>=(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b);
template <typename FloatingPointType>
inline bool operator>(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b);

template <typename FloatingPointType>
void eval_add(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& x);
template <typename FloatingPointType>
void eval_subtract(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& x);
template <typename FloatingPointType>
void eval_multiply(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& x);
template <typename FloatingPointType>
void eval_divide(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& x);

template <typename FloatingPointType>
void eval_frexp(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& a, int* v);
template <typename FloatingPointType>
void eval_ldexp(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& a, int v);
template <typename FloatingPointType>
void eval_floor(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& x);
template <typename FloatingPointType>
void eval_ceil(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& x);
template <typename FloatingPointType>
void eval_sqrt(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& o);
template <typename FloatingPointType>
int eval_fpclassify(const cpp_quad_float<FloatingPointType>& o);

template <typename FloatingPointType,
          typename R>
typename std::enable_if<std::is_integral<R>::value == true>::type eval_convert_to(R* result, const cpp_quad_float<FloatingPointType>& backend);

template <typename FloatingPointType,
          typename R>
typename std::enable_if<std::is_integral<R>::value == false>::type eval_convert_to(R* result, const cpp_quad_float<FloatingPointType>& backend);

template <typename FloatingPointType,
          typename char_type,
          typename traits_type>
std::basic_ostream<char_type, traits_type>& operator<<(std::basic_ostream<char_type, traits_type>& os,
                                                       const cpp_quad_float<FloatingPointType>&  f);

template <typename FloatingPointType>
std::size_t hash_value(const cpp_quad_float<FloatingPointType>& a);

}}} // namespace boost::multiprecision::backends

namespace boost { namespace math {

template <typename FloatingPointType>
int fpclassify(const boost::multiprecision::backends::cpp_quad_float<FloatingPointType>& o);

}} // namespace boost::math

namespace std {

// Foward declarations of various specializations of std::numeric_limits

template <typename FloatingPointType>
class numeric_limits<boost::multiprecision::backends::cpp_quad_float<FloatingPointType> >;

template <typename FloatingPointType,
          const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
class numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_quad_float<FloatingPointType>, ExpressionTemplatesOption> >;

} // namespace std

namespace boost { namespace multiprecision {

template <typename FloatingPointType>
struct number_category<backends::cpp_quad_float<FloatingPointType> >
    : public std::integral_constant<int, number_kind_floating_point>
{};

namespace backends {

// A cpp_quad_float is represented by an unevaluated sum of four floating-point
// units (say a[0...n]) which satisfy |a[i+1]| <= (1 / 2) * ulp(a[i]).
// The type of the floating-point constituents should adhere to IEEE754.

template <typename FloatingPointType>
class cpp_quad_float
{
 public:
   using float_type = FloatingPointType;
   using rep_type   = std::tuple<float_type, float_type, float_type, float_type>;
   using float_pair = typename detail::exact_arithmetic<float_type>::float_pair;
   using arithmetic = detail::exact_arithmetic<float_type>;

   using signed_types   = std::tuple<signed char, signed short, signed int, signed long, signed long long, std::intmax_t>;
   using unsigned_types = std::tuple<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long, std::uintmax_t>;
   using float_types    = std::tuple<float, double, long double>;
   using exponent_type  = int;

   // Default constructor.
   cpp_quad_float() {}

   // Copy constructor.
   constexpr cpp_quad_float(const cpp_quad_float&) = default;

   // Constructors from other floating-point types.
   template <typename OtherFloatType,
             typename std::enable_if<(detail::is_floating_point_or_float128<OtherFloatType>::value == true) && (std::numeric_limits<OtherFloatType>::digits <= std::numeric_limits<float_type>::digits)>::type const* = nullptr>
   constexpr cpp_quad_float(const OtherFloatType& f) : data(std::make_tuple(f, (float_type)0, (float_type)0, (float_type)0)) {}

   constexpr cpp_quad_float(const rep_type& r) : data(r) {}

   template <typename OtherFloatType,
             typename std::enable_if<((std::numeric_limits<OtherFloatType>::is_iec559 == true) && (std::numeric_limits<OtherFloatType>::digits > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   cpp_quad_float(const OtherFloatType& f)
   {
      using std::get;
      get<0>(data) = static_cast<float_type>(f);
      get<1>(data) = static_cast<float_type>(f - (OtherFloatType)get<0>(data));
      get<2>(data) = static_cast<float_type>(f - (OtherFloatType)get<1>(data) - (OtherFloatType)get<0>(data));
      get<3>(data) = static_cast<float_type>(f - (OtherFloatType)get<2>(data) - (OtherFloatType)get<1>(data) - (OtherFloatType)get<0>(data));
   }

   // Constructor from other cpp_quad_float<> objects.
   template <typename OtherFloatType,
             typename std::enable_if<((std::is_floating_point<OtherFloatType>::value == true) && (std::is_same<FloatingPointType, OtherFloatType>::value == false))>::type const* = nullptr>
   cpp_quad_float(const cpp_quad_float<OtherFloatType>& a)
   {
      using std::get;
      using precise_type =
         typename std::conditional<(std::numeric_limits<OtherFloatType>::digits > std::numeric_limits<float_type>::digits), cpp_quad_float, float_type>::type;

      *this += (precise_type)get<0>(a.rep());
      *this += (precise_type)get<1>(a.rep());
      *this += (precise_type)get<2>(a.rep());
      *this += (precise_type)get<3>(a.rep());
      //data = rep_type((float_type)get<0>(a.rep()), (float_type)get<1>(a.rep()), (float_type)get<2>(a.rep()), (float_type)get<3>(a.rep()));
      //arithmetic::normalize(data);
   }

   // Constructors from integers which can be easily fit into underlying floating-point type
   template <typename IntegralType,
             typename std::enable_if<((std::is_integral<IntegralType>::value == true) && (std::numeric_limits<IntegralType>::digits <= std::numeric_limits<FloatingPointType>::digits))>::type const* = nullptr>
   constexpr cpp_quad_float(const IntegralType& f) : data(std::make_tuple(static_cast<float_type>(f), (float_type)0, (float_type)0, (float_type)0)) {}

   // Constructors from integers which hold more information than *this can contain
   template <typename UnsignedIntegralType,
             typename std::enable_if<((std::is_integral<UnsignedIntegralType>::value == true) && (std::is_unsigned<UnsignedIntegralType>::value == true) && (std::numeric_limits<UnsignedIntegralType>::digits > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   cpp_quad_float(UnsignedIntegralType u)
   {
      constexpr int MantissaBits = std::numeric_limits<FloatingPointType>::digits - 1;

      int bit_index = sizeof(UnsignedIntegralType) * 8;

      for (;;)
      {
         // Mask the maximum number of bits that can be stored without
         // precision loss in a single FloatingPointType, then sum and shift
         UnsignedIntegralType hi = u >> (std::max)(bit_index - MantissaBits, 0);
         u &= ~(hi << (std::max)(bit_index - MantissaBits, 0));

         *this += static_cast<FloatingPointType>(hi); // sum

         bit_index -= MantissaBits;

         if (bit_index < 0)
            break;
         else // shift
            eval_ldexp(*this, *this, (std::min)(MantissaBits, bit_index));
      }
   }

   template <typename SignedIntegralType,
             typename std::enable_if<((std::is_integral<SignedIntegralType>::value == true) && (std::is_signed<SignedIntegralType>::value == true) && (std::numeric_limits<SignedIntegralType>::digits + 1 > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   cpp_quad_float(SignedIntegralType n) : cpp_quad_float(static_cast<typename std::make_unsigned<SignedIntegralType>::type>(std::abs(n)))
   {
      if (n < 0)
         *this = -*this;
   }

   cpp_quad_float(const std::string& str)
   {
      boost::multiprecision::detail::convert_from_string(*this, str.c_str());
   }

   cpp_quad_float(const char* pstr)
   {
      boost::multiprecision::detail::convert_from_string(*this, pstr);
   }

   constexpr cpp_quad_float(cpp_quad_float&&) = default;

   ~cpp_quad_float() = default;

   std::size_t hash() const
   {
      // Here we first convert to scientific string, then
      // hash the charactgers in the scientific string.
      // TBD: Is there a faster or more simple hash method?

      const std::string str_to_hash = str(std::numeric_limits<cpp_quad_float>::digits10, std::ios::scientific);

      std::size_t result = 0;

      for (std::string::size_type i = 0U; i < str_to_hash.length(); ++i)
         boost::multiprecision::detail::hash_combine(result, str_to_hash.at(i));

      return result;
   }

   // Casts
//   operator signed char() const { return (signed char)data.first; }
//   operator signed short() const { return (signed short)data.first; }
//   operator signed int() const { return (signed int)data.first + (signed int)data.second; }
//   operator signed long() const { return (signed long)data.first + (signed long)data.second; }
//   operator signed long long() const { return (signed long long)data.first + (signed long long)data.second; }
//   operator unsigned char() const { return (unsigned char)data.first; }
//   operator unsigned short() const { return (unsigned short)data.first; }
//   operator unsigned int() const { return (unsigned int)((unsigned int)data.first + (signed int)data.second); }
//   operator unsigned long() const { return (unsigned long)((unsigned long)data.first + (signed long)data.second); }
//   operator unsigned long long() const { return (unsigned long long)((unsigned long long)data.first + (signed long long)data.second); }
//   operator float() const { return (float)data.first + (float)data.second; }
//   operator double() const { return (double)data.first + (double)data.second; }
//   operator long double() const { return (long double)data.first + (long double)data.second; }
//#ifdef BOOST_MATH_USE_FLOAT128
//   explicit operator boost::multiprecision::float128() const
//   {
//      return static_cast<boost::multiprecision::float128>(data.first) + static_cast<boost::multiprecision::float128>(data.second);
//   }
//#endif

   // Methods
   constexpr cpp_quad_float<float_type> negative() const
   {
     using std::get;
     return cpp_quad_float<float_type>(std::make_tuple(-get<0>(data), -get<1>(data), -get<2>(data), -get<3>(data)));
   }

   constexpr bool is_negative() const { return data.first < 0; }

   void negate()
   {
      *this = -*this;
   }

   rep_type&       rep() { return data; }
   const rep_type& rep() const { return data; }
   const rep_type& crep() const { return data; }

   // Assignment operators.
   cpp_quad_float& operator=(const cpp_quad_float&) = default;

   cpp_quad_float& operator=(cpp_quad_float&&) = default;

   // Non-member add/sub/mul/div with constituent type.
   // TODO Make specializations for these operators
   friend inline cpp_quad_float operator+(const cpp_quad_float& a, const float_type& b) { return a + cpp_quad_float(b); }
   friend inline cpp_quad_float operator-(const cpp_quad_float& a, const float_type& b) { return a - cpp_quad_float(b); }
   friend inline cpp_quad_float operator*(const cpp_quad_float& a, const float_type& b) { return a * cpp_quad_float(b); }
   friend inline cpp_quad_float operator/(const cpp_quad_float& a, const float_type& b) { return a / cpp_quad_float(b); }

   // Unary add/sub/mul/div with constituent part.
   cpp_quad_float& operator+=(const float_type& a) { *this = *this + a; return *this; }
   cpp_quad_float& operator-=(const float_type& a) { *this = *this - a; return *this; }
   cpp_quad_float& operator*=(const float_type& a) { *this = *this * a; return *this; }
   cpp_quad_float& operator/=(const float_type& a) { *this = *this / a; return *this; }

   // Unary add/sub/mul/div.
   cpp_quad_float& operator+=(const cpp_quad_float& other)
   {
      using std::array;
      using std::fabs;
      using std::get;
      using std::tie;

      float_pair u, v;
      int        i, j, k;

      i = j = k = 0;

      array<float_type, 4> a = {get<0>(this->data), get<1>(this->data), get<2>(this->data), get<3>(this->data)};
      array<float_type, 4> b = {get<0>(other.data), get<1>(other.data), get<2>(other.data), get<3>(other.data)};
      array<float_type, 4> s = {};

      if (fabs(a[i]) > fabs(b[j]))
         v.first = a[i++];
      else
         v.first = b[j++];
      if (fabs(a[i]) > fabs(b[j]))
         v.second = a[i++];
      else
         v.second = b[j++];

      arithmetic::normalize(v);

      while (k < 4)
      {
         if (i >= 4 && j >= 4)
         {
            s[k] = v.first;
            if (k < 3)
               s[++k] = v.second;
            break;
         }

         if (i >= 4)
            u.second = b[j++];
         else if (j >= 4)
            u.second = a[i++];
         else if (fabs(a[i]) > fabs(b[j]))
            u.second = a[i++];
         else
            u.second = b[j++];

         tie(u.first, v.second) = arithmetic::sum(v.second, u.second);

         // TODO try fast_sum
         tie(u.first, v.first) = arithmetic::sum(u.first, v.first);

         // TODO can the conditions be simplified further?
         if (v.first == 0 || v.second == 0)
         {
            if (v.second == 0)
            {
               v.second = v.first;
               v.first  = u.first;
            }
            else
               v.first = u.first;

            u.first = 0;
         }

         if (u.first != 0)
            s[k++] = u.first;
      }

      // Add the rest
      for (k = i; k < 4; k++)
         s[3] += a[k];
      for (k = j; k < 4; k++)
         s[3] += b[k];

      data = std::make_tuple(s[0], s[1], s[2], s[3]);
      arithmetic::normalize(data);

      return *this;
   }

   cpp_quad_float& operator-=(const cpp_quad_float& other)
   {
      *this += -other;
      return *this;
   }

   cpp_quad_float& operator*=(const cpp_quad_float& other)
   {
     using std::get;
     using std::tie;

      std::array<float_pair, 10> p;
      float_pair r, t, s;
      float_type s_;

      p[0] = arithmetic::product(get<0>(this->data), get<0>(other.data));

      p[1] = arithmetic::product(get<0>(this->data), get<1>(other.data));
      p[2] = arithmetic::product(get<1>(this->data), get<0>(other.data));

      p[3] = arithmetic::product(get<0>(this->data), get<2>(other.data));
      p[4] = arithmetic::product(get<1>(this->data), get<1>(other.data));
      p[5] = arithmetic::product(get<2>(this->data), get<0>(other.data));

      arithmetic::three_sum(p[1].first, p[2].first , p[0].second);
      arithmetic::three_sum(p[2].first, p[1].second, p[2].second);
      arithmetic::three_sum(p[3].first, p[4].first , p[5].first );

      tie(s.first , t.first ) = arithmetic::sum(p[2].first , p[3].first);
      tie(s.second, t.second) = arithmetic::sum(p[1].second, p[4].first);

      s_ = p[2].second + p[5].first;

      tie(s.second, t.first) = arithmetic::sum(s.second, t.first);

      s_ += t.first + t.second;

      p[6] = arithmetic::product(get<0>(this->data), get<3>(other.data));
      p[7] = arithmetic::product(get<1>(this->data), get<2>(other.data));
      p[8] = arithmetic::product(get<2>(this->data), get<1>(other.data));
      p[9] = arithmetic::product(get<3>(this->data), get<0>(other.data));

      tie(p[0].second, p[3].second) = arithmetic::sum(p[0].second, p[3].second);
      tie(p[4].second, p[5].second) = arithmetic::sum(p[4].second, p[5].second);
      tie(p[6].first , p[7].first ) = arithmetic::sum(p[6].first , p[7].first);
      tie(p[8].first , p[9].first ) = arithmetic::sum(p[8].first , p[9].first);

      t = arithmetic::sum(p[0].second, p[4].second);
      t.second += p[3].second + p[5].second;

      r = arithmetic::sum(p[6].first, p[8].first);
      r.second += p[7].first + p[9].first;

      tie(p[3].second, p[4].second) = arithmetic::sum(t.first, r.first);
      p[4].second += t.second + r.second;

      t = arithmetic::sum(p[3].second, s.second);
      t.second += p[4].second;

      t.second += get<1>(this->data) * get<3>(other.data);
      t.second += get<2>(this->data) * get<2>(other.data);
      t.second += get<3>(this->data) * get<1>(other.data);
      
      t.second += p[6].second;
      t.second += p[7].second;
      t.second += p[8].second;
      t.second += p[9].second;

      t.second += s_;

      data = std::make_tuple(p[0].first, p[1].first, s.first, t.first);
      arithmetic::normalize(data, t.second);

      return *this;
   }

   cpp_quad_float& operator/=(const cpp_quad_float& other)
   {
      using std::get;

      rep_type       q;
      cpp_quad_float r;

      get<0>(q) = get<0>(this->data) / get<0>(other.data);
      r = *this - (other * get<0>(q));

      get<1>(q) = get<0>(r.data) / get<0>(other.data);
      r -= other * get<1>(q);

      get<2>(q) = get<0>(r.data) / get<0>(other.data);
      r -= other * get<2>(q);

      get<3>(q) = get<0>(r.data) / get<0>(other.data);
      r -= other * get<3>(q);

      arithmetic::normalize(q, get<0>(r.data) / get<0>(other.data));
      data = q;
      return *this;
   }

   cpp_quad_float operator++(int)
   {
      cpp_quad_float t(*this);
      ++*this;
      return t;
   }

   cpp_quad_float operator--(int)
   {
      cpp_quad_float t(*this);
      --*this;
      return t;
   }

   cpp_quad_float& operator++() { return *this += cpp_quad_float<float_type>(float_type(1.0F)); }
   cpp_quad_float& operator--() { return *this -= cpp_quad_float<float_type>(float_type(1.0F)); }

   cpp_quad_float  operator-() const { return negative(); }

   // Helper functions
   static cpp_quad_float<float_type> pow10(int p)
   {
      using local_float_type = cpp_quad_float;

      local_float_type result;

      if (p < 0)
         result = local_float_type(1U) / pow10(-p);
      else if (p == 0)
         result = local_float_type(1U);
      else if (p == 1)
         result = local_float_type(10U);
      else if (p == 2)
         result = local_float_type(100U);
      else if (p == 3)
         result = local_float_type(1000U);
      else if (p == 4)
         result = local_float_type(10000U);
      else
      {
         result = local_float_type(1U);

         local_float_type y(10U);

         std::uint32_t p_local = (std::uint32_t)p;

         for (;;)
         {
            if (std::uint_fast8_t(p_local & 1U) != 0U)
            {
               result *= y;
            }

            p_local >>= 1U;

            if (p_local == 0U)
            {
               break;
            }
            else
            {
               y *= y;
            }
         }
      }

      return result;
   }

   void swap(cpp_quad_float& other)
   {
      rep_type tmp = data;
      data = other.data;
      other.data = tmp;
   }

   // Return 1 for *this > other, -1 for *this < other, 0 for *this = other.
   int compare(const cpp_quad_float& other) const
   {
      using std::get;
      int n_result;

      if ((get<0>(this->data) > get<0>(other.data) ||
           (get<0>(this->data) == get<0>(other.data) && (get<1>(this->data) > get<1>(other.data) ||
                             (get<1>(this->data) == get<1>(other.data) && (get<2>(this->data) > get<2>(other.data) ||
                                               (get<2>(this->data) == get<2>(other.data) && get<3>(this->data) > get<3>(other.data))))))))
      {
         n_result = 1;
      }
      else if ((get<0>(this->data) < get<0>(other.data) ||
                (get<0>(this->data) == get<0>(other.data) && (get<1>(this->data) < get<1>(other.data) ||
                                  (get<1>(this->data) == get<1>(other.data) && (get<2>(this->data) < get<2>(other.data) ||
                                                    (get<2>(this->data) == get<2>(other.data) && get<3>(this->data) < get<3>(other.data))))))))
      {
         n_result = -1;
      }
      else
      {
         n_result = 0;
      }

      return n_result;
   }

   // Debugging
   std::string raw_str() const
   {
      using std::get;

      std::stringstream ss;

      ss << std::hexfloat;
      ss << get<0>(data) << " + " << get<1>(data) << " + " << get<2>(data) << " + " << get<3>(data);
      ss << std::endl;

      return ss.str();
   }

   std::string str(std::streamsize number_of_digits, const std::ios::fmtflags format_flags) const
   {
     // FIXME
      return raw_str();
      //if (number_of_digits == 0)
      //   number_of_digits = std::numeric_limits<cpp_quad_float>::digits10;

      //const std::string my_str = boost::multiprecision::detail::convert_to_string(*this, number_of_digits, format_flags);

      //return my_str;
   }

 private:
   rep_type data;

};

template <typename FloatingPointType>
inline cpp_quad_float<FloatingPointType> operator+(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b) { return cpp_quad_float<FloatingPointType>(a) += b; }
template <typename FloatingPointType>
inline cpp_quad_float<FloatingPointType> operator-(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b) { return cpp_quad_float<FloatingPointType>(a) -= b; }
template <typename FloatingPointType>
inline cpp_quad_float<FloatingPointType> operator*(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b) { return cpp_quad_float<FloatingPointType>(a) *= b; }
template <typename FloatingPointType>
inline cpp_quad_float<FloatingPointType> operator/(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b) { return cpp_quad_float<FloatingPointType>(a) /= b; }

template <typename FloatingPointType>
inline bool operator<(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b) { return (a.compare(b) < 0); }
template <typename FloatingPointType>
inline bool operator<=(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b) { return (a.compare(b) <= 0); }
template <typename FloatingPointType>
inline bool operator==(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b) { return (a.compare(b) == 0); }
template <typename FloatingPointType>
inline bool operator!=(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b) { return (a.compare(b) != 0); }
template <typename FloatingPointType>
inline bool operator>=(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b) { return (a.compare(b) >= 0); }
template <typename FloatingPointType>
inline bool operator>(const cpp_quad_float<FloatingPointType>& a, const cpp_quad_float<FloatingPointType>& b) { return (a.compare(b) > 0); }

// -- Input/Output Streaming
template <typename FloatingPointType, typename char_type, typename traits_type>
std::basic_ostream<char_type, traits_type>&
operator<<(std::basic_ostream<char_type, traits_type>& os, const cpp_quad_float<FloatingPointType>& f)
{
   const std::string str_result = f.str(os.precision(), os.flags());

   return (os << str_result);
}

template <typename FloatingPointType, typename char_type, typename traits_type>
std::basic_istream<char_type, traits_type>&
operator>>(std::basic_istream<char_type, traits_type>& is, cpp_quad_float<FloatingPointType>& f)
{
   std::string str;
   is >> str;
   boost::multiprecision::detail::convert_from_string(f, str.c_str());
   return is;
}

template <typename FloatingPointType>
void eval_add(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& x) { result += x; }
template <typename FloatingPointType>
void eval_subtract(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& x) { result -= x; }
template <typename FloatingPointType>
void eval_multiply(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& x) { result *= x; }
template <typename FloatingPointType>
void eval_divide(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& x) { result /= x; }

template <typename FloatingPointType>
void eval_frexp(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& a, int* v)
{
}

template <typename FloatingPointType>
void eval_ldexp(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& a, int v)
{
   using std::ldexp;
   using std::get;

   typename cpp_quad_float<FloatingPointType>::rep_type z =
       std::make_tuple(
           ldexp(get<0>(a.crep()), v),
           ldexp(get<1>(a.crep()), v),
           ldexp(get<2>(a.crep()), v),
           ldexp(get<3>(a.crep()), v));

   cpp_double_float<FloatingPointType>::arithmetic::normalize(z);

   result.rep() = z;
}

template <typename FloatingPointType>
void eval_floor(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& x)
{
}

template <typename FloatingPointType>
void eval_ceil(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& x)
{
}

template <typename FloatingPointType>
void eval_sqrt(cpp_quad_float<FloatingPointType>& result, const cpp_quad_float<FloatingPointType>& o)
{
}

template <typename FloatingPointType>
int eval_fpclassify(const cpp_quad_float<FloatingPointType>& o)
{
   return (int)(boost::math::fpclassify)(o.crep().first);
}

template <typename FloatingPointType,
          typename R>
typename std::enable_if<std::is_integral<R>::value == true>::type eval_convert_to(R* result, const cpp_quad_float<FloatingPointType>& backend)
{
   // TBD: Does boost::common_type have a C++ 11 replacement?
   using c_type = typename boost::common_type<R, FloatingPointType>::type;

   using std::fabs;

   BOOST_CONSTEXPR const c_type my_max = static_cast<c_type>((std::numeric_limits<R>::max)());
   BOOST_CONSTEXPR const c_type my_min = static_cast<c_type>((std::numeric_limits<R>::min)());
   c_type                       ct     = fabs(backend.crep().first);

   (void)my_min;

   if (ct > my_max)
      if (!std::is_unsigned<R>::value)
         *result = backend.crep().first >= typename cpp_quad_float<FloatingPointType>::float_type(0U) ? (std::numeric_limits<R>::max)() : detail::minus_max<R>();
      else
         *result = (std::numeric_limits<R>::max)();
   else
   {
     // TODO
   }
}

template <typename FloatingPointType,
          typename R>
typename std::enable_if<std::is_integral<R>::value == false>::type eval_convert_to(R* result, const cpp_quad_float<FloatingPointType>& backend)
{
}

template <typename FloatingPointType>
std::size_t hash_value(const cpp_quad_float<FloatingPointType>& a)
{
   return a.hash();
}

} // namespace backends
}} // namespace boost::multiprecision

namespace boost { namespace math {

template <typename FloatingPointType>
int fpclassify(const boost::multiprecision::backends::cpp_quad_float<FloatingPointType>& o)
{
   using std::fpclassify;

   return (int)(fpclassify)(o.crep().first);
}

}} // namespace boost::math

namespace std {

// Specialization of numeric_limits for cpp_quad_float<>
template <typename FloatingPointType>
class numeric_limits<boost::multiprecision::backends::cpp_quad_float<FloatingPointType> >
    : public std::numeric_limits<FloatingPointType>
{
 private:
   using base_class_type = std::numeric_limits<FloatingPointType>;

   using self_type = boost::multiprecision::backends::cpp_quad_float<FloatingPointType>;

 public:
   static constexpr bool is_iec559                     = false;
   static constexpr std::float_denorm_style has_denorm = std::denorm_absent;  // TODO Discuss (verify denormal arithmetic is done correctly) 

   static constexpr int digits       = 4 * (base_class_type::digits);
   static constexpr int digits10     = int(float(digits - 1) * 0.301F);
   static constexpr int max_digits10 = int(float(digits) * 0.301F) + 2;

   static constexpr int max_exponent = std::numeric_limits<FloatingPointType>::max_exponent - 3 * base_class_type::digits;
   static constexpr int min_exponent = std::numeric_limits<FloatingPointType>::min_exponent + 3 * base_class_type::digits;

   // TODO Are these values rigorous?
   static constexpr           self_type(min)() noexcept { return self_type(boost::multiprecision::ldexp(self_type(1), -min_exponent)); }
   static constexpr           self_type(max)() noexcept { return self_type(boost::multiprecision::ldexp(base_class_type::max, -base_class_type::digits)); }
   static constexpr self_type lowest() noexcept { return self_type(-max()); }
   static constexpr self_type epsilon() noexcept { return self_type(boost::multiprecision::ldexp(self_type(1), -digits)); }
   static constexpr self_type round_error() noexcept { return self_type(base_class_type::round_error()); }
   static constexpr self_type denorm_min() noexcept { return self_type(min()); }

   static constexpr self_type infinity() noexcept { return self_type(base_class_type::infinity()); }
   static constexpr self_type quiet_NaN() noexcept { return self_type(base_class_type::quiet_NaN()); }
   static constexpr self_type signaling_NaN() noexcept { return self_type(base_class_type::signaling_NaN()); }
};

// Specialization of numeric_limits for boost::multiprecision::number<cpp_quad_float<>>
template <typename FloatingPointType,
          const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
class numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_quad_float<FloatingPointType>, ExpressionTemplatesOption> >
    : public std::numeric_limits<FloatingPointType>
{
 private:
   using base_class_type = std::numeric_limits<FloatingPointType>;

   using self_type =
       boost::multiprecision::number<boost::multiprecision::backends::cpp_quad_float<FloatingPointType>, ExpressionTemplatesOption>;

 public:
   static constexpr bool is_iec559                     = false;
   static constexpr std::float_denorm_style has_denorm = std::denorm_absent;

   static constexpr int digits       = 4 * (base_class_type::digits);
   static constexpr int digits10     = int(float(digits - 1) * 0.301F);
   static constexpr int max_digits10 = int(float(digits) * 0.301F) + 2;

   static constexpr int max_exponent = std::numeric_limits<FloatingPointType>::max_exponent - 3 * base_class_type::digits;
   static constexpr int min_exponent = std::numeric_limits<FloatingPointType>::min_exponent + 3 * base_class_type::digits;

   static constexpr           self_type(min)() noexcept { return self_type(boost::multiprecision::ldexp(self_type(1), -min_exponent)); }
   static constexpr           self_type(max)() noexcept { return self_type(std::ldexp(base_class_type::max(), -base_class_type::digits)); }
   static constexpr self_type lowest() noexcept { return self_type(-max()); }
   static constexpr self_type epsilon() noexcept { return self_type(boost::multiprecision::ldexp(self_type(1), -digits)); }
   static constexpr self_type round_error() noexcept { return self_type(base_class_type::round_error()); }
   static constexpr self_type denorm_min() noexcept { return self_type(min()); }

   static constexpr self_type infinity() noexcept { return self_type(base_class_type::infinity()); }
   static constexpr self_type quiet_NaN() noexcept { return self_type(base_class_type::quiet_NaN()); }
   static constexpr self_type signaling_NaN() noexcept { return self_type(base_class_type::signaling_NaN()); }
};

} // namespace std

#endif // BOOST_MP_CPP_QUAD_FLOAT_2021_07_29_HPP
