///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DOUBLE_FLOAT_2021_06_05_HPP
#define BOOST_MP_CPP_DOUBLE_FLOAT_2021_06_05_HPP

#include <limits>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/multiprecision/detail/float_string_cvt.hpp>
#include <boost/multiprecision/detail/hash.hpp>
#if defined(BOOST_MATH_USE_FLOAT128)
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/number.hpp>
#include <boost/multiprecision/traits/max_digits10.hpp>
#include <boost/type_traits/common_type.hpp>

namespace boost { namespace multiprecision { namespace backends {

template <typename FloatingPointType>
class cpp_double_float;

namespace detail {

template <class T> struct is_arithmetic_or_float128
{
   static constexpr bool value = (   (std::is_arithmetic<T>::value == true)
#if defined(BOOST_MATH_USE_FLOAT128)
                                  || (std::is_same<typename std::decay<T>::type, boost::multiprecision::float128>::value == true)
#endif
                                 );
};

template <class T> struct is_floating_point_or_float128
{
   static constexpr bool value = (   (std::is_floating_point<T>::value == true)
#if defined(BOOST_MATH_USE_FLOAT128)
                                  || (std::is_same<typename std::decay<T>::type, boost::multiprecision::float128>::value == true)
#endif
                                 );
};

}

template<typename FloatingPointType> inline cpp_double_float<FloatingPointType> operator+(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b);
template<typename FloatingPointType> inline cpp_double_float<FloatingPointType> operator-(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b);
template<typename FloatingPointType> inline cpp_double_float<FloatingPointType> operator*(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b);
template<typename FloatingPointType> inline cpp_double_float<FloatingPointType> operator/(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b);

template<typename FloatingPointType> cpp_double_float<FloatingPointType> operator+(const cpp_double_float<FloatingPointType>& a, const FloatingPointType& b);
template<typename FloatingPointType> cpp_double_float<FloatingPointType> operator-(const cpp_double_float<FloatingPointType>& a, const FloatingPointType& b);
template<typename FloatingPointType> cpp_double_float<FloatingPointType> operator*(const cpp_double_float<FloatingPointType>& a, const FloatingPointType& b);
template<typename FloatingPointType> cpp_double_float<FloatingPointType> operator/(const cpp_double_float<FloatingPointType>& a, const FloatingPointType& b);

template<typename FloatingPointType> inline bool operator< (const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b);
template<typename FloatingPointType> inline bool operator<=(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b);
template<typename FloatingPointType> inline bool operator==(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b);
template<typename FloatingPointType> inline bool operator!=(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b);
template<typename FloatingPointType> inline bool operator>=(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b);
template<typename FloatingPointType> inline bool operator> (const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b);

template<typename FloatingPointType> void eval_add     (cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x);
template<typename FloatingPointType> void eval_subtract(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x);
template<typename FloatingPointType> void eval_multiply(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x);
template<typename FloatingPointType> void eval_divide  (cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x);

template<typename FloatingPointType> void eval_fabs      (cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& a);
template<typename FloatingPointType> void eval_frexp     (cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& a, int* v);
template<typename FloatingPointType> void eval_ldexp     (cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& a, int v);
template<typename FloatingPointType> void eval_floor     (cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x);
template<typename FloatingPointType> void eval_ceil      (cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x);
template<typename FloatingPointType> int  eval_fpclassify(const cpp_double_float<FloatingPointType>& o);
template<typename FloatingPointType> void eval_sqrt      (cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& o);

template<typename FloatingPointType,
         typename std::enable_if<(   (detail::is_floating_point_or_float128<FloatingPointType>::value == true)
                                  && (std::numeric_limits<cpp_double_float<FloatingPointType>>::digits10 <= 15))>::type const* = nullptr>
void eval_exp(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x);

template<typename FloatingPointType,
         typename std::enable_if<(   (detail::is_floating_point_or_float128<FloatingPointType>::value == true)
                                  && ((std::numeric_limits<cpp_double_float<FloatingPointType>>::digits10 > 15) && (std::numeric_limits<cpp_double_float<FloatingPointType>>::digits10 <= 37)))>::type const* = nullptr>
void eval_exp(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x);

template<typename FloatingPointType,
         typename std::enable_if<(   (detail::is_floating_point_or_float128<FloatingPointType>::value == true)
                                  && (std::numeric_limits<cpp_double_float<FloatingPointType>>::digits10 > 37))>::type const* = nullptr>
void eval_exp(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x);

template<typename FloatingPointType> void eval_log       (cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x);

template<typename FloatingPointType,
         typename R>
typename std::enable_if<std::is_integral<R>::value == true>::type eval_convert_to(R* result, const cpp_double_float<FloatingPointType>& backend);

template<typename FloatingPointType,
         typename R>
typename std::enable_if<std::is_integral<R>::value == false>::type eval_convert_to(R* result, const cpp_double_float<FloatingPointType>& backend);

template<typename FloatingPointType,
         typename char_type,
         typename traits_type>
std::basic_ostream<char_type, traits_type>& operator<<(std::basic_ostream<char_type, traits_type>& os,
                                                       const cpp_double_float<FloatingPointType>& f);

template<typename FloatingPointType>
std::size_t hash_value(const cpp_double_float<FloatingPointType>& a);

template<typename FloatingPointType>
cpp_double_float<FloatingPointType> fabs(const cpp_double_float<FloatingPointType>& a);

} } } // namespace boost::multiprecision::backends

namespace boost { namespace math {

template<typename FloatingPointType>
int (fpclassify)(const boost::multiprecision::backends::cpp_double_float<FloatingPointType>& o);

} }

namespace std {

// Foward declarations of various specializations of std::numeric_limits

template <typename FloatingPointType>
class numeric_limits<boost::multiprecision::backends::cpp_double_float<FloatingPointType>>;

template <typename FloatingPointType,
          const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
class numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<FloatingPointType>, ExpressionTemplatesOption>>;

}

namespace boost { namespace multiprecision {

template<typename FloatingPointType>
struct number_category<backends::cpp_double_float<FloatingPointType>>
  : public std::integral_constant<int, number_kind_floating_point> { };

namespace backends {

namespace detail {

template<typename R>
typename std::enable_if<boost::is_unsigned<R>::value == false, R>::type minus_max()
{
   return boost::is_signed<R>::value ? (std::numeric_limits<R>::min)() : -(std::numeric_limits<R>::max)();
}

template<typename R>
typename std::enable_if<boost::is_unsigned<R>::value == true, R>::type minus_max()
{
   return 0;
}

// exact_arithmetic<> implements extended precision techniques that are used in
// cpp_double_float and cpp_quad_float
template <typename FloatingPointType>
struct exact_arithmetic
{
   static_assert(detail::is_floating_point_or_float128<FloatingPointType>::value == true, "exact_arithmetic<> invoked with unknown floating-point type");
   using float_type  = FloatingPointType;
   using float_pair  = std::pair<float_type, float_type>;
   using float_tuple = std::tuple<float_type, float_type, float_type, float_type>;

   static float_pair split(const float_type& a)
   {
      // Split a floating point number in two (high and low) parts approximating the
      // upper-half and lower-half bits of the float
      //static_assert(std::numeric_limits<float_type>::is_iec559,
      //              "double_float<> invoked with non-native floating-point unit");

      // TODO Replace bit shifts with constexpr funcs or ldexp for better compaitibility
      constexpr int        MantissaBits   = std::numeric_limits<float_type>::digits;
      constexpr int        SplitBits      = MantissaBits / 2 + 1;
      constexpr float_type Splitter       = FloatingPointType((1ULL << SplitBits) + 1);
      const float_type     SplitThreshold = (std::numeric_limits<float_type>::max)() / (Splitter * 2);

      float_type temp, hi, lo;

      // Handle if multiplication with the splitter would cause overflow
      if (a > SplitThreshold || a < -SplitThreshold)
      {
         constexpr float_type Normalizer = float_type(1ULL << (SplitBits + 1));

         const float_type a_ = a / Normalizer;

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

   static float_pair fast_sum(const float_type& a, const float_type& b)
   {
      // Exact addition of two floating point numbers, given |a| > |b|
      using std::fabs;
      using std::isnormal;

      float_pair out;
      out.first  = a + b;
      out.second = b - (out.first - a);

      return out;
   }

   static float_pair sum(const float_type& a, const float_type& b)
   {
      // Exact addition of two floating point numbers
      float_pair out;

      out.first    = a + b;
      float_type v = out.first - a;
      out.second   = (a - (out.first - v)) + (b - v);

      return out;
   }

   static void three_sum(float_type& a, float_type& b, float_type& c)
   {
      using std::tie;
      using std::get;

      std::tuple<float_type, float_type, float_type> t;

      tie(get<0>(t), get<1>(t)) = sum(a        , b);
      tie(a        , get<2>(t)) = sum(c        , get<0>(t));
      tie(b        , c        ) = sum(get<1>(t), get<2>(t));
   }

   static void sum(float_pair& p, float_type& e)
   {
      using std::tie;

      float_pair t;
      float_type t_;

      t                = sum(p.first, p.second);
      tie(p.first, t_) = sum(e, t.first);
      tie(p.second, e) = sum(t.second, t_);
   }

   static float_pair difference(const float_type& a, const float_type& b)
   {
      // Exact subtraction of two floating point numbers
      float_pair out;

      out.first    = a - b;
      float_type v = out.first - a;
      out.second   = (a - (out.first - v)) - (b + v);

      return out;
   }

   static float_pair product(const float_type& a, const float_type& b)
   {
      // Exact product of two floating point numbers
      const float_pair a_split = split(a);
      const float_pair b_split = split(b);

      const volatile float_type pf = a * b;

      return std::make_pair(
          (const float_type&)pf,
          (
              ((a_split.first * b_split.first) - (const float_type&)pf) + (a_split.first * b_split.second) + (a_split.second * b_split.first)) +
              (a_split.second * b_split.second));
   }

   static void normalize(float_pair& p, bool fast = true)
   {
      // Converts a pair of floats to standard form
      //BOOST_ASSERT(std::isfinite(p.first));
      p = (fast ? fast_sum(p.first, p.second) : sum(p.first, p.second));

      // TODO: discuss ?
      // will it help somewhere else? In some mathematical functions? I'm not sure.
      // Maybe it's only for this special case which I constructed to work exactly with bits. / Janek
      //extra_normalize(p);
   }

   static void extra_normalize(float_pair& p)
   {
      // TODO: discuss ?
      // If exponent of the second component is farther away than digits represented by this type
      // then means that these "dangling" bits should be zero.
      int e1 = 0;
      int e2 = 0;
      using std::frexp;
      frexp(p.first, &e1);
      frexp(p.second, &e2);
      // Interesting: when we set   digits = 2 * <digits of underlying type>
      // then this extra normalize, to work with DecomposedReal guard_bits needs ↓ the - 2 here.
      if((e1 - e2) > std::numeric_limits<cpp_double_float<float_type>>::digits - 2) {
         p.second = 0;
      }
      // ... maybe even better would be to zero all the bits further away than cpp_double_float<float_type>>::digits away
      // not only when entire p.second is too far.
      // FIXME - currently I have no idea how to implement this efficiently. But for debugging maybe even the super slow (with frexp, ldexp) implementation will help in edge cases...
      // best would be doing & operation on a bitmask..... But can we make sure that would work on all architectures?
   }

   static void normalize(float_tuple& t)
   {
      using std::get;
      using std::tie;

      float_tuple s(float_type(0.0F), float_type(0.0F), float_type(0.0F), float_type(0.0F));

      tie(get<0>(s), get<3>(t)) = fast_sum(get<2>(t), get<3>(t));
      tie(get<0>(s), get<2>(t)) = fast_sum(get<1>(t), get<0>(s));
      tie(get<0>(t), get<1>(t)) = fast_sum(get<0>(t), get<0>(s));

      tie(get<0>(s), get<1>(s)) = std::make_tuple(get<0>(t), get<1>(t));

      if (get<1>(s) != 0)
      {
         tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<2>(t));

         if (get<2>(s) != 0)
            tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), get<3>(t));
         else
            tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<3>(t));
      }
      else
      {
         tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), get<2>(t));
         if (get<1>(s) != 0)
            tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<3>(t));
         else
            tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), get<3>(t));
      }

      t = s;
   }

   static void normalize(float_tuple& t, float_type e)
   {
      using std::tie;
      using std::get;

      float_tuple s(float_type(0.0F), float_type(0.0F), float_type(0.0F), float_type(0.0F));

      tie(get<0>(s), e)         = fast_sum(get<3>(t), e);
      tie(get<0>(s), get<3>(t)) = fast_sum(get<2>(t), get<0>(s));
      tie(get<0>(s), get<2>(t)) = fast_sum(get<1>(t), get<0>(s));
      tie(get<0>(t), get<1>(t)) = fast_sum(get<0>(t), get<0>(s));

      tie(get<0>(s), get<1>(s)) = std::make_tuple(get<0>(t), get<1>(t));

      if (get<1>(s) != 0)
      {
         tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<2>(t));
         if (get<2>(s) != 0)
         {
            tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), get<3>(t));
            if (get<3>(s) != 0)
               get<3>(s) += e;
               else tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), e);
         }
         else
         {
            tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<3>(t));
            if (get<2>(s) != 0)
               tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), e);
            else
               tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), e);
         }
      }
      else
      {
         tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), get<2>(t));
         if (get<1>(s) != 0)
         {
            tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<3>(t));
            if (get<2>(s) != 0)
               tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), e);
            else
               tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), e);
         }
         else
         {
            tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), get<3>(t));
            if (get<1>(s) != 0)
               tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), e);
            else
               tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), e);
         }
      }

      t  = s;
   }
};

} // namespace detail

// A cpp_double_float is represented by an unevaluated sum of two floating-point
// units (say a0 and a1) which satisfy |a1| <= (1 / 2) * ulp(a0).
// The type of the floating-point constituents should adhere to IEEE754.

template <typename FloatingPointType>
class cpp_double_float
{
 public:
   using float_type = FloatingPointType;
   using rep_type   = std::pair<float_type, float_type>;
   using arithmetic = detail::exact_arithmetic<float_type>;

  using   signed_types = std::tuple<  signed char,   signed short,   signed int,   signed long,   signed long long, std::intmax_t>;
  using unsigned_types = std::tuple<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long, std::uintmax_t>;
  using float_types    = std::tuple<float, double, long double>;
  using exponent_type  = int;

   // Default constructor.
   cpp_double_float() { }

   // Copy constructor.
   constexpr cpp_double_float(const cpp_double_float& other) : data(other.data) { }

   // Constructors from other floating-point types.
   template <typename FloatType,
             typename std::enable_if<    (detail::is_floating_point_or_float128<FloatType>::value == true)
                                      && (std::numeric_limits<FloatType>::digits <= std::numeric_limits<float_type>::digits)>::type const* = nullptr>
   constexpr cpp_double_float(const FloatType& f) : data(std::make_pair(f, (float_type)0)) {}

   template <typename FloatType,
             typename std::enable_if<(   (detail::is_floating_point_or_float128<FloatType>::value == true)
                                      && (std::numeric_limits<FloatType>::digits > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_float(const FloatType& f)
       : data(std::make_pair(static_cast<float_type>(f),
                             static_cast<float_type>(f - (FloatType) static_cast<float_type>(f)))) {}

   // Constructor from other cpp_double_float<> objects.
   template <typename OtherFloatType,
             typename std::enable_if<(   (detail::is_floating_point_or_float128<OtherFloatType>::value == true)
                                      && (std::is_same<FloatingPointType, OtherFloatType>::value == false))>::type const* = nullptr>
   cpp_double_float(const cpp_double_float<OtherFloatType>& a)
      : cpp_double_float(a.first())
   {
      // TBD: Maybe specialize this constructor for cases either wider or less wide.
      *this += a.second();
   }

   // Constructors from integers
   template <typename IntegralType,
             typename std::enable_if<(   (std::is_integral<IntegralType>::value == true)
                                      && (std::numeric_limits<IntegralType>::digits <= std::numeric_limits<FloatingPointType>::digits))>::type const* = nullptr>
   constexpr cpp_double_float(const IntegralType& f) : data(std::make_pair(static_cast<float_type>(f), (float_type)0)) {}

   // Constructors from integers which hold more information than *this can contain
   template <typename UnsignedIntegralType,
             typename std::enable_if<(    (std::is_integral<UnsignedIntegralType>::value == true)
                                       && (std::is_unsigned<UnsignedIntegralType>::value == true)
                                       && (std::numeric_limits<UnsignedIntegralType>::digits > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   cpp_double_float(UnsignedIntegralType u)
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
         else // shift
            eval_ldexp(*this, *this, (std::min)(MantissaBits, bit_index));
      }
   }

   template <typename SignedIntegralType,
             typename std::enable_if<(   (std::is_integral<SignedIntegralType>::value == true)
                                      && (std::is_signed<SignedIntegralType>::value == true)
                                      && (std::numeric_limits<SignedIntegralType>::digits + 1 > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   cpp_double_float(SignedIntegralType n) : cpp_double_float(static_cast<typename std::make_unsigned<SignedIntegralType>::type>(std::abs(n)))
   {
      if (n < 0)
         *this = -*this;
   }

   constexpr cpp_double_float(const float_type& a, const float_type& b) : data(std::make_pair(a, b)) { }
   constexpr cpp_double_float(const std::pair<float_type, float_type>& p) : data(p) { }

   cpp_double_float(const std::string& str)
   {
      boost::multiprecision::detail::convert_from_string(*this, str.c_str());
   }

   cpp_double_float(const char* pstr)
   {
      boost::multiprecision::detail::convert_from_string(*this, pstr);
   }

   constexpr cpp_double_float(cpp_double_float&& other) : data(other.data) { }

   ~cpp_double_float() { }

   std::size_t hash() const
   {
      // Here we first convert to scientific string, then
      // hash the charactgers in the scientific string.
      // TBD: Is there a faster or more simple hash method?

      const std::string str_to_hash = str(std::numeric_limits<cpp_double_float>::digits10, std::ios::scientific);

      std::size_t result = 0;

      for (std::string::size_type i = 0U; i < str_to_hash.length(); ++i)
         boost::multiprecision::detail::hash_combine(result, str_to_hash.at(i));

      return result;
   }

   // Methods
   constexpr bool is_neg() const { return (data.first < 0); }

   bool is_zero() const { return (compare(cpp_double_float(0U)) == 0); }
   bool is_one () const { return (compare(cpp_double_float(1U)) == 0); }

   void negate()
   {
      data.first  = -data.first;
      data.second = -data.second;

      arithmetic::normalize(data);
   }

   // Getters/Setters
   constexpr const float_type& first () const { return data.first; }
   constexpr const float_type& second() const { return data.second; }

         rep_type&  rep()       { return data; }
   const rep_type&  rep() const { return data; }
   const rep_type& crep() const { return data; }

   // Retrieve debug string.
   std::string get_raw_str() const
   {
      std::stringstream ss;
      ss << std::hexfloat << data.first << " + " << std::hexfloat << data.second;
      return ss.str();
   }

   // Assignment operators.
   cpp_double_float& operator=(const cpp_double_float& other)
   {
      if(this != &other)
      {
         data = other.data;
      }

      return *this;
   }

   cpp_double_float& operator=(cpp_double_float&& other)
   {
     data = other.data;

     return *this;
   }

   // Non-member add/sub/mul/div with constituent type.
   friend cpp_double_float operator+(const cpp_double_float& a, const float_type& b)
   {
      using other_cpp_double_float_type = cpp_double_float<float_type>;

      typename other_cpp_double_float_type::rep_type s = other_cpp_double_float_type::arithmetic::sum(a.first(), b);

      s.second += a.second();
      other_cpp_double_float_type::arithmetic::normalize(s);

      return other_cpp_double_float_type(s);
   }

   friend cpp_double_float operator-(const cpp_double_float& a, const float_type& b)
   {
      using other_cpp_double_float_type = cpp_double_float<float_type>;

      typename other_cpp_double_float_type::rep_type s = other_cpp_double_float_type::arithmetic::difference(a.first(), b);

      s.second += a.second();
      other_cpp_double_float_type::arithmetic::normalize(s);

      return other_cpp_double_float_type(s);
   }

   friend cpp_double_float operator*(const cpp_double_float& a, const float_type& b)
   {
      using other_cpp_double_float_type = cpp_double_float<float_type>;

      typename other_cpp_double_float_type::rep_type p = other_cpp_double_float_type::arithmetic::product(a.first(), b);

      using std::isfinite;
      using boost::multiprecision::isfinite;

      if ((isfinite)(p.first) == false)
         return other_cpp_double_float_type(p);

      p.second += a.second() * b;

      other_cpp_double_float_type::arithmetic::normalize(p);

      return other_cpp_double_float_type(p);
   }

   friend cpp_double_float operator/(const cpp_double_float& a, const float_type& b)
   {
      using other_cpp_double_float_type = cpp_double_float<float_type>;

      typename other_cpp_double_float_type::rep_type p, q, s;

      p.first = a.first() / b;

      q = other_cpp_double_float_type::arithmetic::product(p.first, b);
      s = other_cpp_double_float_type::arithmetic::difference(a.first(), q.first);
      s.second += a.second();
      s.second -= q.second;

      p.second = (s.first + s.second) / b;

      other_cpp_double_float_type::arithmetic::normalize(p);

      return other_cpp_double_float_type(p);
   }

   // Unary add/sub/mul/div with constituent part.
   cpp_double_float& operator+=(const float_type& a) { *this = *this + a; return *this; }
   cpp_double_float& operator-=(const float_type& a) { *this = *this - a; return *this; }
   cpp_double_float& operator*=(const float_type& a) { *this = *this * a; return *this; }
   cpp_double_float& operator/=(const float_type& a) { *this = *this / a; return *this; }

   // Unary add/sub/mul/div.
   cpp_double_float& operator+=(const cpp_double_float& other)
   {
      const rep_type t = arithmetic::sum(second(), other.second());

      data = arithmetic::sum(first(),  other.first());

      using std::isfinite;
      using boost::multiprecision::isfinite;

      if ((isfinite)(first()) == false)
         return *this;

      data.second += t.first;
      arithmetic::normalize(data);
      data.second += t.second;
      arithmetic::normalize(data);

      return *this;
   }

   cpp_double_float& operator-=(const cpp_double_float& other)
   {
      const rep_type t = arithmetic::difference(second(), other.second());
      data = arithmetic::difference(first(), other.first());

      using std::isfinite;
      using boost::multiprecision::isfinite;

      if ((isfinite)(first()) == false)
         return *this;

      data.second += t.first;
      arithmetic::normalize(data);

      data.second += t.second;
      arithmetic::normalize(data);

      return *this;
   }

   cpp_double_float& operator*=(const cpp_double_float& other)
   {
      rep_type tmp = arithmetic::product(data.first, other.data.first);

      tmp.second += (  data.first  * other.data.second
                     + data.second * other.data.first);

      data = tmp;

      return *this;
   }

   cpp_double_float& operator/=(const cpp_double_float& other)
   {
      rep_type p;

      // First approximation
      p.first = first() / other.first();

      using std::isfinite;
      using boost::multiprecision::isfinite;

      if ((isfinite)(p.first) == false)
      {
         data = p;
         return *this;
      }

      cpp_double_float r = *this - (other * p.first);

      p.second = r.first() / other.first();
      r -= other * p.second;

      const FloatingPointType p_prime = r.first() / other.first();

      arithmetic::normalize(p);

      data = p;

      operator+=(p_prime);

      return *this;
   }

   cpp_double_float  operator++(int) { cpp_double_float t(*this); ++*this; return t; }
   cpp_double_float  operator--(int) { cpp_double_float t(*this); --*this; return t; }
   cpp_double_float& operator++() { return *this += cpp_double_float<float_type>(float_type(1.0F)); }
   cpp_double_float& operator--() { return *this -= cpp_double_float<float_type>(float_type(1.0F)); }

   cpp_double_float  operator-() const { cpp_double_float v(*this); v.negate(); return v; }

   // Helper functions
   static cpp_double_float pown(const cpp_double_float& x, int p)
   {
      using local_float_type = cpp_double_float;

      local_float_type result;

      if      (p <  0) result = pown(local_float_type(1U) / x, -p);
      else if (p == 0) result = local_float_type(1U);
      else if (p == 1) result = x;
      else if (p == 2) result = local_float_type(x * x);
      else if (p == 3) result = local_float_type((x * x) * x);
      else
      {
         result = local_float_type(1U);

         local_float_type y(x);

         std::uint32_t p_local = (std::uint32_t) p;

         for (;;)
         {
            if (std::uint_fast8_t(p_local & 1U) != 0U) { result *= y; }

            p_local >>= 1U;

            if (p_local == 0U) { break; }
            else               { y *= y; }
         }
      }

      return result;
   }

   void swap(cpp_double_float& other)
   {
      rep_type tmp = data;

      data = other.data;

      other.data = tmp;
   }

   int compare(const cpp_double_float& other) const
   {
     // Return 1 for *this > other, -1 for *this < other, 0 for *this = other.
     return (first () > other.first ()) ?  1 :
            (first () < other.first ()) ? -1 :
            (second() > other.second()) ?  1 :
            (second() < other.second()) ? -1 : 0;
   }

   std::string str(std::streamsize number_of_digits, const std::ios::fmtflags format_flags) const
   {
      if (number_of_digits == 0)
         number_of_digits = std::numeric_limits<cpp_double_float>::digits10;

      const std::string my_str = boost::multiprecision::detail::convert_to_string(*this, number_of_digits, format_flags);

      return my_str;
   }

   int  order02  () const { using std::frexp; int e2; frexp(first(), &e2); return e2; }
   int  order10  () const { return (int) (float(order02()) * 0.301F); }
   bool small_arg() const { return (order10() < (-std::numeric_limits<cpp_double_float>::digits10 / 6)); }
   bool near_one () const { return cpp_double_float(fabs(cpp_double_float(1U) - *this)).small_arg(); }

 private:
   rep_type data;

   template<typename OtherFloatingPointType,
            typename std::enable_if<(   (detail::is_floating_point_or_float128<OtherFloatingPointType>::value == true)
                                     && (std::numeric_limits<cpp_double_float<OtherFloatingPointType>>::digits10 <= 15))>::type const*>
   friend void eval_exp(cpp_double_float<OtherFloatingPointType>& result, const cpp_double_float<OtherFloatingPointType>& x);

   template<typename OtherFloatingPointType,
            typename std::enable_if<(   (detail::is_floating_point_or_float128<OtherFloatingPointType>::value == true)
                                     && ((std::numeric_limits<cpp_double_float<OtherFloatingPointType>>::digits10 > 15) && (std::numeric_limits<cpp_double_float<OtherFloatingPointType>>::digits10 <= 37)))>::type const*>
   friend void eval_exp(cpp_double_float<OtherFloatingPointType>& result, const cpp_double_float<OtherFloatingPointType>& x);

   template<typename OtherFloatingPointType,
            typename std::enable_if<(   (detail::is_floating_point_or_float128<OtherFloatingPointType>::value == true)
                                     && (std::numeric_limits<cpp_double_float<OtherFloatingPointType>>::digits10 > 37))>::type const*>
   friend void eval_exp(cpp_double_float<OtherFloatingPointType>& result, const cpp_double_float<OtherFloatingPointType>& x);
};

template<typename FloatingPointType> inline cpp_double_float<FloatingPointType> operator+(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b) { return cpp_double_float<FloatingPointType>(a) += b; }
template<typename FloatingPointType> inline cpp_double_float<FloatingPointType> operator-(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b) { return cpp_double_float<FloatingPointType>(a) -= b; }
template<typename FloatingPointType> inline cpp_double_float<FloatingPointType> operator*(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b) { return cpp_double_float<FloatingPointType>(a) *= b; }
template<typename FloatingPointType> inline cpp_double_float<FloatingPointType> operator/(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b) { return cpp_double_float<FloatingPointType>(a) /= b; }

template<typename FloatingPointType> inline bool operator< (const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b) { return (a.compare(b) <  0); }
template<typename FloatingPointType> inline bool operator<=(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b) { return (a.compare(b) <= 0); }
template<typename FloatingPointType> inline bool operator==(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b) { return (a.compare(b) == 0); }
template<typename FloatingPointType> inline bool operator!=(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b) { return (a.compare(b) != 0); }
template<typename FloatingPointType> inline bool operator>=(const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b) { return (a.compare(b) >= 0); }
template<typename FloatingPointType> inline bool operator> (const cpp_double_float<FloatingPointType>& a, const cpp_double_float<FloatingPointType>& b) { return (a.compare(b) >  0); }

// -- Input/Output Streaming
template <typename FloatingPointType, typename char_type, typename traits_type>
std::basic_ostream<char_type, traits_type>&
operator<<(std::basic_ostream<char_type, traits_type>& os, const cpp_double_float<FloatingPointType>& f)
{
   const std::string str_result = f.str(os.precision(), os.flags());

   return (os << str_result);
}

template <typename FloatingPointType, typename char_type, typename traits_type>
std::basic_istream<char_type, traits_type>&
operator>>(std::basic_istream<char_type, traits_type>& is, cpp_double_float<FloatingPointType>& f)
{
   std::string str;
   is >> str;
   boost::multiprecision::detail::convert_from_string(f, str.c_str());
   return is;
}

template<typename FloatingPointType> void eval_add     (cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x) { result += x; }
template<typename FloatingPointType> void eval_subtract(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x) { result -= x; }
template<typename FloatingPointType> void eval_multiply(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x) { result *= x; }
template<typename FloatingPointType> void eval_divide  (cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x) { result /= x; }

template<typename FloatingPointType> void eval_fabs(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& a)
{
   result = a;

   if(a.is_neg())
   {
      result.negate();
   }
}

template<typename FloatingPointType> void eval_frexp(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& a, int* v)
{
   using std::frexp;
   using std::ldexp;

   result.rep().first  = frexp(a.rep().first,    v);
   result.rep().second = ldexp(a.rep().second, -*v);
}

template<typename FloatingPointType>
void eval_ldexp(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& a, int v)
{
   using std::ldexp;

   typename cpp_double_float<FloatingPointType>::rep_type z =
   std::make_pair
   (
      ldexp(a.crep().first,  v),
      ldexp(a.crep().second, v)
   );

   cpp_double_float<FloatingPointType>::arithmetic::normalize(z);

   result.rep() = z;
}

template<typename FloatingPointType>
void eval_floor(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x)
{
   using double_float_type = cpp_double_float<FloatingPointType>;

   using std::floor;

   const typename double_float_type::float_type fhi = floor(x.rep().first);

   if(fhi != x.first())
   {
      result.rep().first  = fhi;
      result.rep().second = static_cast<typename double_float_type::float_type>(0.0F);
   }
   else
   {
      result.rep().first  = fhi;
      result.rep().second = floor(x.rep().second);

      double_float_type::arithmetic::normalize(result.rep());
   }
}

template<typename FloatingPointType>
void eval_ceil(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x)
{
   // Compute -floor(-x);
   eval_floor(result, -x);

   result.negate();
}

template<typename FloatingPointType>
int eval_fpclassify(const cpp_double_float<FloatingPointType>& o)
{
   return (int) (boost::math::fpclassify)(o.crep().first);
}

template<typename FloatingPointType>
void eval_sqrt(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& o)
{
   using local_float_type = typename cpp_double_float<FloatingPointType>::float_type;

   using std::sqrt;

   local_float_type c = sqrt(o.crep().first);

   local_float_type p,q,hx,tx,u,uu,cc;
   local_float_type t1;

   constexpr int              MantissaBits = std::numeric_limits<local_float_type>::digits;
   constexpr int              SplitBits    = MantissaBits / 2 + 1;
   constexpr local_float_type Splitter     = local_float_type((1ULL << SplitBits) + 1);

   p = Splitter * c;
   hx = (c-p);
   hx = hx+p;
   tx = c-hx;
   p = hx*hx;
   q = hx*tx;
   q = q+q;

   u = p+q;
   uu = p-u;
   uu = uu+q;
   t1 = tx*tx;
   uu = uu+t1;

   cc = o.crep().first-u;
   cc = cc-uu;
   cc = cc+o.crep().second;
   t1 = c+c;
   cc = cc/t1;

   hx = c+cc;
   tx = c-hx;
   tx = tx+cc;

   result.rep().first  = hx;
   result.rep().second = tx;
}

template<typename FloatingPointType,
         typename std::enable_if<(   (detail::is_floating_point_or_float128<FloatingPointType>::value == true)
                                  && (std::numeric_limits<cpp_double_float<FloatingPointType>>::digits10 <= 15))>::type const*>
void eval_exp(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x)
{
   const bool x_is_zero = x.is_zero();

   if((eval_fpclassify(x) != (int) FP_NORMAL) && (x_is_zero == false))
   {
      result = x;
   }
   else
   {
      using double_float_type = cpp_double_float<FloatingPointType>;
      using local_float_type  = typename cpp_double_float<FloatingPointType>::float_type;

      // Get a local copy of the argument and force it to be positive.
      const bool b_neg = x.is_neg();

      double_float_type xx;

      eval_fabs(xx, x);

      // Check the range of the input.
      // Will the result of exponentiation overflow/underflow?
      static const local_float_type max_exp_input = []() -> local_float_type { using std::log; const local_float_type e_max = (std::numeric_limits<double_float_type>::max)().crep().first; return log(e_max); }();
      static const local_float_type min_exp_input = []() -> local_float_type { using std::log; const local_float_type e_min = (std::numeric_limits<double_float_type>::min)().crep().first; return log(e_min); }();

      if(x_is_zero)
      {
         result = double_float_type(1U);
      }
      else if(x.crep().first < min_exp_input)
      {
         result = double_float_type(0U);
      }
      else if(xx.crep().first > max_exp_input)
      {
         result = double_float_type(std::numeric_limits<local_float_type>::infinity());
      }
      else if(xx.is_one())
      {
         static const double_float_type constant_e1         (std::string("2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274"));
         static const double_float_type constant_one_over_e1(std::string("0.36787944117144232159552377016146086744581113103176783450783680169746149574489980335714727434591964375"));

         result = ((b_neg == false) ? constant_e1 : constant_one_over_e1);
      }
      else
      {
         // Use an argument reduction algorithm for exp() in classic MPFUN-style.
         static const double_float_type constant_ln2         (std::string("0.69314718055994530941723212145817656807550013436025525412068000949339362196969471560586332699641868754"));
         static const double_float_type constant_one_over_ln2(std::string("1.4426950408889634073599246810018921374266459541529859341354494069311092191811850798855266228935063445"));

         double_float_type nf;

         eval_floor(nf, xx * constant_one_over_ln2);

         // Prepare the scaled variables.
         const bool b_scale = (xx.order02() > -1);

         double_float_type r;

         if(b_scale)
         {
           eval_ldexp(r, xx - (nf * constant_ln2), -2);
         }
         else
         {
           r = xx;
         }

         // PadeApproximant[Exp[x] - 1, {x, 0, {6, 6}}]
         // FullSimplify[%]
         //   (84 x (7920 + 240 x^2 + x^4))
         // / (665280 + x (-332640 + x (75600 + x (-10080 + x (840 + (-42 + x) x)))))

         static const double_float_type n84  (  84);
         static const double_float_type n240 ( 240);
         static const double_float_type n7920(7920);

         static const double_float_type n665280(665280);
         static const double_float_type n332640(332640);
         static const double_float_type n75600 ( 75600);
         static const double_float_type n10080 ( 10080);
         static const double_float_type n840   (   840);
         static const double_float_type n42    (    42);

         const double_float_type r2 = r * r;

         result = double_float_type(1U) +   ((n84 * r) * (n7920 + r2 * (n240 + r2)))
                                          / (n665280 + r * (-n332640 + r * (n75600 + r * (-n10080 + r * (n840 + (-n42 + r) * r)))));

         // Use the small-argument Pade approximation having coefficients shown above.
         const double_float_type top = (n84 * r * (n7920 + (n240 + r2) * r2));
         const double_float_type bot = (n665280 + r * (-n332640 + r * (n75600 + r * (-n10080 + r * (n840 + (-n42 + r) * r)))));

         result  = double_float_type(1U) + (top / bot);

         // Rescale the result.
         if(b_scale)
         {
            result *= result;
            result *= result;

            int n;

            eval_convert_to(&n, nf);

            if(n > 0)
            {
               eval_ldexp(result, double_float_type(result), n);
            }
         }

         if(b_neg)
         {
            result = double_float_type(1U) / result;
         }
      }
   }
}

template<typename FloatingPointType,
         typename std::enable_if<(   (detail::is_floating_point_or_float128<FloatingPointType>::value == true)
                                  && ((std::numeric_limits<cpp_double_float<FloatingPointType>>::digits10 > 15) && (std::numeric_limits<cpp_double_float<FloatingPointType>>::digits10 <= 37)))>::type const*>
void eval_exp(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x)
{
   const bool x_is_zero = x.is_zero();

   if((eval_fpclassify(x) != (int) FP_NORMAL) && (x_is_zero == false))
   {
      result = x;
   }
   else
   {
      using double_float_type = cpp_double_float<FloatingPointType>;
      using local_float_type  = typename cpp_double_float<FloatingPointType>::float_type;

      // Get a local copy of the argument and force it to be positive.
      const bool b_neg = x.is_neg();

      double_float_type xx;

      eval_fabs(xx, x);

      // Check the range of the input.
      // Will the result of exponentiation overflow/underflow?
      static const local_float_type max_exp_input = []() -> local_float_type { using std::log; const local_float_type e_max = (std::numeric_limits<double_float_type>::max)().crep().first; return log(e_max); }();
      static const local_float_type min_exp_input = []() -> local_float_type { using std::log; const local_float_type e_min = (std::numeric_limits<double_float_type>::min)().crep().first; return log(e_min); }();

      if(x_is_zero)
      {
         result = double_float_type(1U);
      }
      else if(x.crep().first < min_exp_input)
      {
         result = double_float_type(0U);
      }
      else if(xx.crep().first > max_exp_input)
      {
         result = double_float_type(std::numeric_limits<local_float_type>::infinity());
      }
      else if(xx.is_one())
      {
         static const double_float_type constant_e1         (std::string("2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274"));
         static const double_float_type constant_one_over_e1(std::string("0.36787944117144232159552377016146086744581113103176783450783680169746149574489980335714727434591964375"));

         result = ((b_neg == false) ? constant_e1 : constant_one_over_e1);
      }
      else
      {
         // Use an argument reduction algorithm for exp() in classic MPFUN-style.
         static const double_float_type constant_ln2         (std::string("0.69314718055994530941723212145817656807550013436025525412068000949339362196969471560586332699641868754"));
         static const double_float_type constant_one_over_ln2(std::string("1.4426950408889634073599246810018921374266459541529859341354494069311092191811850798855266228935063445"));

         double_float_type nf;

         eval_floor(nf, xx * constant_one_over_ln2);

         // Prepare the scaled variables.
         const bool b_scale = (xx.order02() > -5);

         double_float_type r;

         if(b_scale)
         {
           eval_ldexp(r, xx - (nf * constant_ln2), -5);
         }
         else
         {
           r = xx;
         }

         // PadeApproximant[Exp[r], {r, 0, 8, 8}]
         // FullSimplify[%]

         static const double_float_type n144    (    144U);
         static const double_float_type n3603600(3603600UL);
         static const double_float_type n120120 ( 120120UL);
         static const double_float_type n770    (    770U);

         static const double_float_type n518918400(518918400UL);
         static const double_float_type n259459200(259459200UL);
         static const double_float_type n60540480 ( 60540480UL);
         static const double_float_type n8648640  (  8648640UL);
         static const double_float_type n831600   (   831600UL);
         static const double_float_type n55440    (    55440U);
         static const double_float_type n2520     (     2520U);
         static const double_float_type n72       (       72U);

         const double_float_type r2 = r * r;

         const double_float_type top = (n144 * r) * (n3603600 + r2 * (n120120 + r2 * (n770 + r2)));
         const double_float_type bot = (n518918400 + r * (-n259459200 + r * (n60540480 + r * (-n8648640 + r * (n831600 + r * (-n55440 + r * (n2520 + r * (-n72 + r))))))));

         result = double_float_type(1U) + (top / bot);

         // Rescale the result.
         if(b_scale)
         {
            result *= result;
            result *= result;
            result *= result;
            result *= result;
            result *= result;

            int n;

            eval_convert_to(&n, nf);

            if(n > 0)
            {
               eval_ldexp(result, double_float_type(result), n);
            }
         }

         if(b_neg)
         {
            result = double_float_type(1U) / result;
         }
      }
   }
}

template<typename FloatingPointType,
         typename std::enable_if<(   (detail::is_floating_point_or_float128<FloatingPointType>::value == true)
                                  && (std::numeric_limits<cpp_double_float<FloatingPointType>>::digits10 > 37))>::type const*>
void eval_exp(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x)
{
   const bool x_is_zero = x.is_zero();

   if((eval_fpclassify(x) != (int) FP_NORMAL) && (x_is_zero == false))
   {
      result = x;
   }
   else
   {
      using double_float_type = cpp_double_float<FloatingPointType>;
      using local_float_type  = typename cpp_double_float<FloatingPointType>::float_type;

      // Get a local copy of the argument and force it to be positive.
      const bool b_neg = x.is_neg();

      double_float_type xx;

      eval_fabs(xx, x);

      // Check the range of the input.
      // Will the result of exponentiation overflow/underflow?
      static const local_float_type max_exp_input = []() -> local_float_type { using std::log; const local_float_type e_max = (std::numeric_limits<double_float_type>::max)().crep().first; return log(e_max); }();
      static const local_float_type min_exp_input = []() -> local_float_type { using std::log; const local_float_type e_min = (std::numeric_limits<double_float_type>::min)().crep().first; return log(e_min); }();

      if(x_is_zero)
      {
         result = double_float_type(1U);
      }
      else if(x.crep().first < min_exp_input)
      {
         result = double_float_type(0U);
      }
      else if(xx.crep().first > max_exp_input)
      {
         result = double_float_type(std::numeric_limits<local_float_type>::infinity());
      }
      else if(xx.is_one())
      {
         static const double_float_type constant_e1         (std::string("2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274"));
         static const double_float_type constant_one_over_e1(std::string("0.36787944117144232159552377016146086744581113103176783450783680169746149574489980335714727434591964375"));

         result = ((b_neg == false) ? constant_e1 : constant_one_over_e1);
      }
      else
      {
         // Use an argument reduction algorithm for exp() in classic MPFUN-style.
         static const double_float_type constant_ln2         (std::string("0.69314718055994530941723212145817656807550013436025525412068000949339362196969471560586332699641868754"));
         static const double_float_type constant_one_over_ln2(std::string("1.4426950408889634073599246810018921374266459541529859341354494069311092191811850798855266228935063445"));

         double_float_type nf;

         eval_floor(nf, xx * constant_one_over_ln2);

         // Prepare the scaled variables.
         const bool b_scale = (xx.order02() > -4);

         double_float_type xh;

         if(b_scale)
         {
           eval_ldexp(xh, xx - (nf * constant_ln2), -4);
         }
         else
         {
           xh = xx;
         }

         double_float_type x_pow_n_div_n_fact(xh);

         result = double_float_type(1U) + x_pow_n_div_n_fact;
         double_float_type dummy;

         // Series expansion of hypergeometric_0f0(; ; x).
         // For this high(er) digit count, a scaled argument with subsequent
         // Taylor series expansion is actually more precise than Pade approximation.
         for(unsigned n = 2U; n < 64U; ++n)
         {
           x_pow_n_div_n_fact *= xh;
           x_pow_n_div_n_fact /= typename double_float_type::float_type(n);

           int n_tol;

           eval_frexp(dummy, x_pow_n_div_n_fact, &n_tol);

           if((n > 4U) && (n_tol < -(std::numeric_limits<double_float_type>::digits - 6)))
           {
             break;
           }

           result += x_pow_n_div_n_fact;
         }

         // Rescale the result.
         if(b_scale)
         {
            result *= result;
            result *= result;
            result *= result;
            result *= result;

            int n;

            eval_convert_to(&n, nf);

            if(n > 0)
            {
               eval_ldexp(result, double_float_type(result), n);
            }
         }

         if(b_neg)
         {
            result = double_float_type(1U) / result;
         }
      }
   }
}

template<typename FloatingPointType>
void eval_log(cpp_double_float<FloatingPointType>& result, const cpp_double_float<FloatingPointType>& x)
{
   using double_float_type = cpp_double_float<FloatingPointType>;

   if(eval_fpclassify(x) != (int) FP_NORMAL)
   {
      result = x;
   }
   else if(x.is_neg())
   {
      result = std::numeric_limits<double_float_type>::quiet_NaN();
   }
   else if(x.is_one())
   {
      result = double_float_type(0U);
   }
   else
   {
      using std::log;

      // Get initial estimate using the standard math function log.
      const double_float_type s(log(x.crep().first));
            double_float_type E;

      eval_exp(E, s);

      // Do one single step of Newton-Raphson iteration
      result = s + (x - E) / E;
   }
}

template<typename FloatingPointType,
         typename R>
typename std::enable_if<std::is_integral<R>::value == true>::type eval_convert_to(R* result, const cpp_double_float<FloatingPointType>& backend)
{
   // TBD: Does boost::common_type have a C++ 11 replacement?
   using c_type = typename boost::common_type<R, FloatingPointType>::type;

   using std::fabs;

   BOOST_CONSTEXPR const c_type my_max = static_cast<c_type>((std::numeric_limits<R>::max)());
   BOOST_CONSTEXPR const c_type my_min = static_cast<c_type>((std::numeric_limits<R>::min)());
   c_type                       ct     = fabs(backend.crep().first);

   (void) my_min;

   if (ct > my_max)
      if (!std::is_unsigned<R>::value)
         *result = backend.crep().first >= typename cpp_double_float<FloatingPointType>::float_type(0U) ? (std::numeric_limits<R>::max)() : detail::minus_max<R>();
      else
         *result = (std::numeric_limits<R>::max)();
   else
   {
      *result  = static_cast<R>(backend.crep().first);
      *result += static_cast<R>(backend.crep().second);
   }
}

template<typename FloatingPointType,
         typename R>
typename std::enable_if<std::is_integral<R>::value == false>::type eval_convert_to(R* result, const cpp_double_float<FloatingPointType>& backend)
{
   *result  = R(backend.crep().first);
   *result += R(backend.crep().second);
}

template<typename FloatingPointType>
cpp_double_float<FloatingPointType> fabs(const cpp_double_float<FloatingPointType>& a)
{
   using double_float_type = cpp_double_float<FloatingPointType>;

   double_float_type result;

   eval_fabs(result, a);

   return result;
}

template<typename FloatingPointType>
std::size_t hash_value(const cpp_double_float<FloatingPointType>& a)
{
   return a.hash();
}

} } } // namespace boost::multiprecision::backends

namespace boost { namespace math {

template<typename FloatingPointType>
int (fpclassify)(const boost::multiprecision::backends::cpp_double_float<FloatingPointType>& o)
{
   using std::fpclassify;

   return (int) (fpclassify)(o.crep().first);
}

} }


namespace std {

// Specialization of numeric_limits for cpp_double_float<>
template <typename FloatingPointType>
class numeric_limits<boost::multiprecision::backends::cpp_double_float<FloatingPointType>>
  : public std::numeric_limits<FloatingPointType>
{
private:
   using base_class_type = std::numeric_limits<FloatingPointType>;

   using self_type = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;

public:
   static constexpr bool is_iec559   = false;
   static constexpr std::float_denorm_style has_denorm = std::denorm_absent;

   static constexpr int digits       = 2 * base_class_type::digits;
   static constexpr int digits10     = boost::multiprecision::detail::calc_digits10<digits>::value;
   static constexpr int max_digits10 = boost::multiprecision::detail::calc_max_digits10<digits>::value;

   static constexpr int max_exponent = std::numeric_limits<FloatingPointType>::max_exponent - base_class_type::digits;
   static constexpr int min_exponent = std::numeric_limits<FloatingPointType>::min_exponent + base_class_type::digits;

   // TODO Are these values rigorous?
   static const     self_type (min)         () noexcept { using std::ldexp; return self_type( ldexp(typename self_type::float_type(1), min_exponent)); }
   static const     self_type (max)         () noexcept { using std::ldexp; return self_type( ldexp((base_class_type::max)(), -base_class_type::digits)); }
   static const     self_type  lowest       () noexcept { return self_type(-(max)()); }
   static const     self_type  epsilon      () noexcept { using std::ldexp; return self_type( ldexp(typename self_type::float_type(1), 4 - digits)); }
   static constexpr self_type  round_error  () noexcept { return self_type( base_class_type::round_error()); } 
   static constexpr self_type  denorm_min   () noexcept { return self_type( (min)()); }
   
   static constexpr self_type  infinity     () noexcept { return self_type( base_class_type::infinity()); }
   static constexpr self_type  quiet_NaN    () noexcept { return self_type( base_class_type::quiet_NaN()); }
   static constexpr self_type  signaling_NaN() noexcept { return self_type( base_class_type::signaling_NaN()); }
};

// Specialization of numeric_limits for boost::multiprecision::number<cpp_double_float<>>
template <typename FloatingPointType,
          const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
class numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<FloatingPointType>, ExpressionTemplatesOption>>
   : public std::numeric_limits<FloatingPointType>
{
private:
   using base_class_type = std::numeric_limits<FloatingPointType>;

   using inner_self_type = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;

   using self_type =
      boost::multiprecision::number<inner_self_type, ExpressionTemplatesOption>;

public:
   static constexpr bool is_iec559                     = false;
   static constexpr std::float_denorm_style has_denorm = std::denorm_absent;

   static constexpr int digits       = 2 * base_class_type::digits;
   static constexpr int digits10     = boost::multiprecision::detail::calc_digits10<digits>::value;
   static constexpr int max_digits10 = boost::multiprecision::detail::calc_max_digits10<digits>::value;

   static constexpr int max_exponent = std::numeric_limits<FloatingPointType>::max_exponent - base_class_type::digits;
   static constexpr int min_exponent = std::numeric_limits<FloatingPointType>::min_exponent + base_class_type::digits;

   static const     self_type (min)         () noexcept { using std::ldexp; return self_type( ldexp(typename inner_self_type::float_type(1), min_exponent)); }
   static const     self_type (max)         () noexcept { using std::ldexp; return self_type( ldexp((base_class_type::max)(), -base_class_type::digits)); }
   static const     self_type  lowest       () noexcept { return self_type(-(max)()); }
   static const     self_type  epsilon      () noexcept { using std::ldexp; return self_type( ldexp(self_type(1), 4 - digits)); }
   static constexpr self_type  round_error  () noexcept { return self_type( base_class_type::round_error()); } 
   static const     self_type  denorm_min   () noexcept { return self_type( (min)()); }

   static constexpr self_type  infinity     () noexcept { return self_type( base_class_type::infinity()); }
   static constexpr self_type  quiet_NaN    () noexcept { return self_type( base_class_type::quiet_NaN()); }
   static constexpr self_type  signaling_NaN() noexcept { return self_type( base_class_type::signaling_NaN()); }
};

}

#endif // BOOST_MP_CPP_DOUBLE_FLOAT_2021_06_05_HPP
