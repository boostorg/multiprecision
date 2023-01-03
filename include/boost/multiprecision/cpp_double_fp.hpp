///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 - 2023 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DOUBLE_FP_2021_06_05_HPP
#define BOOST_MP_CPP_DOUBLE_FP_2021_06_05_HPP

#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

#include <boost/multiprecision/cpp_double_quad_fp/arithmetic.hpp>
#include <boost/multiprecision/detail/float_string_cvt.hpp>
#include <boost/multiprecision/detail/hash.hpp>
#include <boost/multiprecision/traits/max_digits10.hpp>

namespace boost { namespace multiprecision { namespace backends {

template <typename FloatingPointType>
class cpp_double_fp_backend;

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_add(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_subtract(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_multiply(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_divide(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR bool eval_eq(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR bool eval_lt(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR bool eval_gt(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR bool eval_is_zero(const cpp_double_fp_backend<FloatingPointType>& x);

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_fabs(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_frexp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, int* v);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_ldexp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, int v);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_floor(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_ceil(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR int eval_fpclassify(const cpp_double_fp_backend<FloatingPointType>& o);
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_sqrt(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& o);

template <typename FloatingPointType,
          typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && ((std::numeric_limits<FloatingPointType>::digits10 * 2) < 16))>::type const* = nullptr>
BOOST_MP_CXX14_CONSTEXPR void eval_exp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);

template <typename FloatingPointType,
          typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (((std::numeric_limits<FloatingPointType>::digits10 * 2) >= 16) && ((std::numeric_limits<FloatingPointType>::digits10 * 2) <= 36)))>::type const* = nullptr>
BOOST_MP_CXX14_CONSTEXPR void eval_exp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);

template <typename FloatingPointType,
          typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && ((std::numeric_limits<FloatingPointType>::digits10 * 2) > 36))>::type const* = nullptr>
BOOST_MP_CXX14_CONSTEXPR void eval_exp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_log(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_convert_to(signed long long* result, const cpp_double_fp_backend<FloatingPointType>& backend);

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_convert_to(unsigned long long* result, const cpp_double_fp_backend<FloatingPointType>& backend);

template <typename FloatingPointType,
          typename R>
BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<detail::is_floating_point_or_float128<R>::value>::type eval_convert_to(R* result, const cpp_double_fp_backend<FloatingPointType>& backend);

template <typename FloatingPointType,
          typename char_type,
          typename traits_type>
std::basic_ostream<char_type, traits_type>& operator<<(std::basic_ostream<char_type, traits_type>& os,
                                                       const cpp_double_fp_backend<FloatingPointType>&  f);

template <typename FloatingPointType>
std::size_t hash_value(const cpp_double_fp_backend<FloatingPointType>& a);

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend<FloatingPointType> fabs(const cpp_double_fp_backend<FloatingPointType>& a);

} } } // namespace boost::multiprecision::backends

#if 0
namespace boost { namespace math {

// TBD: Is this really needed?
template <typename FloatingPointType>
int(fpclassify)(const boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>& o);

} } // namespace boost::math
#endif

#if (defined(__clang__) && (__clang_major__ <= 9))
#define BOOST_MP_DF_QF_NUM_LIMITS_CLASS_TYPE struct
#else
#define BOOST_MP_DF_QF_NUM_LIMITS_CLASS_TYPE class
#endif

namespace std {

// Foward declarations of various specializations of std::numeric_limits

template <typename FloatingPointType,
          const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
BOOST_MP_DF_QF_NUM_LIMITS_CLASS_TYPE numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >;

} // namespace std

namespace boost { namespace multiprecision {

template <typename FloatingPointType>
struct number_category<backends::cpp_double_fp_backend<FloatingPointType> >
    : public std::integral_constant<int, number_kind_floating_point> { };

namespace backends {

namespace detail {

template <typename R>
typename std::enable_if<!boost::multiprecision::detail::is_unsigned<R>::value, R>::type minus_max()
{
   return boost::multiprecision::detail::is_signed<R>::value ? (std::numeric_limits<R>::min)() : -(std::numeric_limits<R>::max)();
}

template <typename R>
typename std::enable_if<boost::multiprecision::detail::is_unsigned<R>::value, R>::type minus_max()
{
   return 0;
}

} // namespace detail

// A cpp_double_fp_backend is represented by an unevaluated sum of two floating-point
// units (say a0 and a1) which satisfy |a1| <= (1 / 2) * ulp(a0).
// The type of the floating-point constituents should adhere to IEEE754.

template <typename FloatingPointType>
class cpp_double_fp_backend
{
 public:
   using float_type = FloatingPointType;
   using rep_type   = std::pair<float_type, float_type>;
   using arithmetic = detail::exact_arithmetic<float_type>;

   using signed_types   = std::tuple<signed char, signed short, signed int, signed long, signed long long, std::intmax_t>;
   using unsigned_types = std::tuple<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long, std::uintmax_t>;
   using float_types    = std::tuple<float, double, long double>;
   using exponent_type  = int;

   static constexpr int my_digits         = 2 * std::numeric_limits<float_type>::digits;
   static constexpr int my_digits10       = boost::multiprecision::detail::calc_digits10<my_digits>::value;
   static constexpr int my_max_digits10   = boost::multiprecision::detail::calc_max_digits10<my_digits>::value;
   static constexpr int my_max_exponent   = std::numeric_limits<float_type>::max_exponent;
   static constexpr int my_min_exponent   = std::numeric_limits<float_type>::min_exponent + std::numeric_limits<float_type>::digits;
   static constexpr int my_max_exponent10 = (int)(float(my_max_exponent) * 0.301F);
   static constexpr int my_min_exponent10 = (int)(float(my_min_exponent) * 0.301F);

   static_assert(((my_max_exponent - my_digits) >= 77),
                 "Error: floating-point constituent does not have wide enough exponent range");

   // Default constructor.
   constexpr cpp_double_fp_backend() { }

   // Copy constructor.
   constexpr cpp_double_fp_backend(const cpp_double_fp_backend& other) : data(other.data) { }

   // Move constructor.
   constexpr cpp_double_fp_backend(cpp_double_fp_backend&& other) noexcept : data(other.data) { }

   // Constructors from other floating-point types.
   template <typename OtherFloatType,
             typename std::enable_if<(    detail::is_floating_point_or_float128<OtherFloatType>::value
                                      && (std::numeric_limits<OtherFloatType>::digits <= std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(const OtherFloatType& f)
      : data(f, static_cast<float_type>(0.0F)) { }

   template <typename OtherFloatType,
             typename std::enable_if<(    detail::is_floating_point_or_float128<OtherFloatType>::value
                                      && (std::numeric_limits<OtherFloatType>::digits > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(const OtherFloatType& f)
      : data(static_cast<float_type>(f),
             static_cast<float_type>(f - (OtherFloatType) static_cast<float_type>(f))) { }

   // Construtor from another kind of cpp_double_fp_backend<> object.

   // TBD: Need to keep widening conversion implicit,
   // but ensure that narrowing conversion is explicit.
   template <typename OtherFloatType,
             typename std::enable_if<(    detail::is_floating_point_or_float128<OtherFloatType>::value
                                      && (!std::is_same<FloatingPointType, OtherFloatType>::value))>::type const* = nullptr>
   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend(const cpp_double_fp_backend<OtherFloatType>& a)
       : cpp_double_fp_backend(a.my_first())
   {
      // TBD: Maybe specialize this constructor for cases either wider or less wide.
      operator+=(a.my_second());
   }

   // Constructors from integers
   template <typename SignedIntegralType,
             typename std::enable_if<(    boost::multiprecision::detail::is_integral<SignedIntegralType>::value
                                      && !boost::multiprecision::detail::is_unsigned<SignedIntegralType>::value
                                      && (static_cast<int>(sizeof(SignedIntegralType) * 8u) <= std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(const SignedIntegralType& n)
      : data(std::make_pair(static_cast<float_type>(n), static_cast<float_type>(0.0F))) { }

   template <typename UnsignedIntegralType,
             typename std::enable_if<(    boost::multiprecision::detail::is_integral<UnsignedIntegralType>::value
                                      &&  boost::multiprecision::detail::is_unsigned<UnsignedIntegralType>::value
                                      && (static_cast<int>(sizeof(UnsignedIntegralType) * 8u) <= std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(const UnsignedIntegralType& u)
      : data(static_cast<float_type>(u), static_cast<float_type>(0.0F)) { }

   // Constructors from integers which hold more information than *this can contain
   template <typename UnsignedIntegralType,
             typename std::enable_if<(    boost::multiprecision::detail::is_integral<UnsignedIntegralType>::value
                                      &&  boost::multiprecision::detail::is_unsigned<UnsignedIntegralType>::value
                                      && (static_cast<int>(sizeof(UnsignedIntegralType) * 8u) > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(UnsignedIntegralType u)
      : data
        (
           std::make_pair
           (
              static_cast<float_type>(u),
              static_cast<float_type>
              (
                 static_cast<UnsignedIntegralType>
                 (
                    u - static_cast<UnsignedIntegralType>(static_cast<float_type>(u))
                 )
              )
           )
        ) { }

   template <typename SignedIntegralType,
             typename std::enable_if<(    boost::multiprecision::detail::is_integral<SignedIntegralType>::value
                                      && !boost::multiprecision::detail::is_unsigned<SignedIntegralType>::value
                                      && (static_cast<int>(sizeof(SignedIntegralType) * 8u) > std::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(SignedIntegralType n)
      : data
        (
           std::make_pair
           (
              static_cast<float_type>(n),
              static_cast<float_type>
              (
                 static_cast<SignedIntegralType>
                 (
                    n - static_cast<SignedIntegralType>(static_cast<float_type>(n))
                 )
              )
           )
        ) { }

   constexpr cpp_double_fp_backend(const float_type& a, const float_type& b) noexcept : data(a, b) { }

   constexpr cpp_double_fp_backend(const std::pair<float_type, float_type>& p) noexcept : data(p) { }

   cpp_double_fp_backend(const std::string& str)
   {
      boost::multiprecision::detail::convert_from_string(*this, str.c_str());
   }

   cpp_double_fp_backend(const char* pstr)
   {
      boost::multiprecision::detail::convert_from_string(*this, pstr);
   }

   // Assignment operator.
   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend& operator=(const cpp_double_fp_backend& other)
   {
      if (this != &other)
      {
         data = other.data;
      }

      return *this;
   }

   // Move assignment operator.
   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend& operator=(cpp_double_fp_backend&& other) noexcept
   {
      data = other.data;

      return *this;
   }

   // Assignment operator from another kind of cpp_double_fp_backend<> object.
   template <typename OtherFloatType,
             typename std::enable_if<(    detail::is_floating_point_or_float128<OtherFloatType>::value
                                      && (!std::is_same<FloatingPointType, OtherFloatType>::value))>::type const* = nullptr>
   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend& operator=(const cpp_double_fp_backend<OtherFloatType>& other)
   {
     return operator=(cpp_double_fp_backend(other));
   }

   std::size_t hash() const
   {
      // Here we first convert to scientific string, then
      // hash the charactgers in the scientific string.
      // TBD: Is there a faster or more simple hash method?

      const std::string str_to_hash = str(cpp_double_fp_backend::my_digits10, std::ios::scientific);

      std::size_t result = 0;

      for (auto i = static_cast<std::string::size_type>(UINT8_C(0)); i < str_to_hash.length(); ++i)
      {
         boost::multiprecision::detail::hash_combine(result, str_to_hash.at(i));
      }

      return result;
   }

   // Methods
   constexpr bool is_neg () const { return (data.first < 0); }
   constexpr bool is_zero() const { return (compare(cpp_double_fp_backend(0U)) == 0); }
   constexpr bool is_one () const { return (compare(cpp_double_fp_backend(1U)) == 0); }

   BOOST_MP_CXX14_CONSTEXPR void negate()
   {
      // TBD: Can this be simplified?

      const auto fpc = eval_fpclassify(*this);

      const auto isinf_u  = (fpc == FP_INFINITE);
      const auto isnan_u  = (fpc == FP_NAN);
      const auto iszero_u = (fpc == FP_ZERO);

      if(iszero_u || isnan_u)
      {
      }
      else if(isinf_u)
      {
         data.first  = -data.first;
      }
      else
      {
         data.first  = -data.first;
         data.second = -data.second;

         arithmetic::normalize(data);
      }
   }

   constexpr bool isneg() const noexcept { return (data.first < 0); }

   // Getters/Setters
   constexpr const float_type& my_first () const noexcept { return data.first; }
   constexpr const float_type& my_second() const noexcept { return data.second; }

   BOOST_MP_CXX14_CONSTEXPR       rep_type& rep ()       noexcept { return data; }
   BOOST_MP_CXX14_CONSTEXPR const rep_type& rep () const noexcept { return data; }
   BOOST_MP_CXX14_CONSTEXPR const rep_type& crep() const noexcept { return data; }

   std::string raw_str() const
   {
      // Retrieve debug string.

      std::stringstream ss;
      ss << std::hexfloat << std::showpos << data.first << " + " << std::hexfloat << data.second;
      return ss.str();
   }

   // Unary add/sub/mul/div.
   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend& operator+=(const cpp_double_fp_backend& v)
   {
      const auto fpc_u = eval_fpclassify(*this);
      const auto fpc_v = eval_fpclassify(v);

      const auto isnan_u = (fpc_u == FP_NAN);

      if (isnan_u)
      {
         return *this;
      }

      const auto isinf_u = (fpc_u == FP_INFINITE);
      const auto isinf_v = (fpc_v == FP_INFINITE);

      if (isinf_u)
      {
         if (isinf_v && (isneg() != v.isneg()))
         {
            *this = cpp_double_fp_backend(std::numeric_limits<float_type>::quiet_NaN());
         }
         return *this;
      }

      const auto iszero_u = (fpc_u == FP_ZERO);

      if (iszero_u)
      {
         return operator=(v);
      }

      const auto isnan_v = (fpc_v == FP_NAN);

      if (isnan_v || isinf_v)
      {
         *this = v;
         return *this;
      }

      float_type sec = my_second();
      data = arithmetic::sum(my_first(), v.my_first());

      const rep_type t = arithmetic::sum(sec, v.my_second());

      data.second += t.first;
      arithmetic::normalize(data);
      data.second += t.second;
      arithmetic::normalize(data);

      return *this;
   }

   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend& operator-=(const cpp_double_fp_backend& v)
   {
      // Use *this - v = -(-*this + v).
      negate();
      *this += v;
      negate();
      return *this;
   }

   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend& operator*=(const cpp_double_fp_backend& other)
   {
      cpp_double_fp_backend v(other);

      // Evaluate the sign of the result.
      const auto isneg_u =   isneg();
      const auto isneg_v = v.isneg();

      const bool b_result_is_neg = (isneg_u != isneg_v);

      // Artificially set the sign of the result to be positive.
      if(isneg_u) {   negate(); }
      if(isneg_v) { v.negate(); }

      const auto fpc_u = eval_fpclassify(*this);
      const auto fpc_v = eval_fpclassify(v);

      // Handle special cases like zero, inf and NaN.
      const bool isinf_u  = (fpc_u == FP_INFINITE);
      const bool isinf_v  = (fpc_v == FP_INFINITE);
      const bool isnan_u  = (fpc_u == FP_NAN);
      const bool isnan_v  = (fpc_v == FP_NAN);
      const bool iszero_u = (fpc_u == FP_ZERO);
      const bool iszero_v = (fpc_v == FP_ZERO);

      if ((isnan_u || isnan_v) || (isinf_u && iszero_v) || (isinf_v && iszero_u))
      {
         *this = cpp_double_fp_backend(std::numeric_limits<float_type>::quiet_NaN());
         return *this;
      }

      if (isinf_u || isinf_v)
      {
         *this = cpp_double_fp_backend(std::numeric_limits<float_type>::infinity());
         if (b_result_is_neg)
            negate();
         return *this;
      }

      if (iszero_u || iszero_v)
      {
         return *this = cpp_double_fp_backend(0);
      }

      rep_type tmp = arithmetic::product(data.first, v.data.first);

      tmp.second += (data.first * v.data.second + data.second * v.data.first);

      data = tmp;

      if(b_result_is_neg) { negate(); }

      return *this;
   }

   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend& operator/=(const cpp_double_fp_backend& v)
   {
      // TBD: Do I like this implementation of divide or not?
      // It looks like an estimate followed by one single Newton-Raphson iteration.
      // Intuitively, I would have gone for Briggs'/Shoup's implementation from the
      // original double-double work.
      // TBD: Figure out which implementation has advantages in terms of
      // speed/accuracy, exactness, etc.

      // Handle special cases like zero, inf and NaN.
      const auto fpc_u = eval_fpclassify(*this);
      const auto fpc_v = eval_fpclassify(v);

      const auto isnan_u = (fpc_u == FP_NAN);
      const auto isnan_v = (fpc_v == FP_NAN);

      if (isnan_u || isnan_v)
      {
         *this = cpp_double_fp_backend(std::numeric_limits<float_type>::quiet_NaN());
      }

      const auto iszero_u = (fpc_u == FP_ZERO);
      const auto iszero_v = (fpc_v == FP_ZERO);

      const bool b_neg = isneg();

      if (b_neg) { negate(); }

      if (iszero_u)
      {
         if (iszero_v)
         {
            return *this = cpp_double_fp_backend(std::numeric_limits<float_type>::quiet_NaN());
         }
         else
         {
            return (*this = cpp_double_fp_backend(0));
         }
      }

      // Handle more special cases like zero, inf and NaN.
      if (iszero_v)
      {
         *this = cpp_double_fp_backend(std::numeric_limits<float_type>::infinity());
         if (b_neg)
            negate();
         return *this;
      }

      const auto isinf_v = (fpc_v == FP_INFINITE);
      const auto isinf_u = (fpc_u == FP_INFINITE);

      if (isinf_u)
      {
         if (isinf_v)
         {
            return (*this = cpp_double_fp_backend(std::numeric_limits<float_type>::quiet_NaN()));
         }
         else
         {
            return (*this = cpp_double_fp_backend(std::numeric_limits<float_type>::infinity()));
         }
      }

      if (isinf_v)
      {
         return (*this = cpp_double_fp_backend(0));
      }

      if(b_neg) { negate(); }

      rep_type p;

      // First approximation
      p.first = my_first() / v.my_first();

      cpp_double_fp_backend r = *this - (v * static_cast<cpp_double_fp_backend>(p.first));

      p.second = r.my_first() / v.my_first();
      r -= v * static_cast<cpp_double_fp_backend>(p.second);

      const FloatingPointType p_prime = r.my_first() / v.my_first();

      arithmetic::normalize(p);

      data = p;

      operator+=(p_prime);

      return *this;
   }

   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend operator++(int)
   {
      cpp_double_fp_backend t(*this);
      ++*this;
      return t;
   }

   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend operator--(int)
   {
      cpp_double_fp_backend t(*this);
      --*this;
      return t;
   }

   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend& operator++() { return *this += cpp_double_fp_backend<float_type>(float_type(1.0F)); }
   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend& operator--() { return *this -= cpp_double_fp_backend<float_type>(float_type(1.0F)); }

   BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend operator-() const
   {
      cpp_double_fp_backend v(*this);
      v.negate();
      return v;
   }

   // Helper functions
   BOOST_MP_CXX14_CONSTEXPR static cpp_double_fp_backend pown(const cpp_double_fp_backend& x, int p)
   {
      using local_float_type = cpp_double_fp_backend;

      local_float_type result;

      if (p < 0)
         result = pown(local_float_type(1U) / x, -p);
      else if (p == 0)
         result = local_float_type(1U);
      else if (p == 1)
         result = x;
      else if (p == 2)
         result = local_float_type(x * x);
      else if (p == 3)
         result = local_float_type((x * x) * x);
      else
      {
         result = local_float_type(1U);

         local_float_type y(x);

         auto p_local = static_cast<std::uint32_t>(p);

         for (;;)
         {
            if (static_cast<std::uint_fast8_t>(p_local & 1U) != static_cast<std::uint_fast8_t>(UINT8_C(0)))
            {
               result *= y;
            }

            p_local >>= 1U;

            if (p_local == static_cast<std::uint32_t>(UINT8_C(0)))
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

   BOOST_MP_CXX14_CONSTEXPR void swap(cpp_double_fp_backend& other)
   {
      if(this != &other)
      {
         const rep_type tmp = data;

         data = other.data;

         other.data = tmp;
      }
   }

   BOOST_MP_CXX14_CONSTEXPR void swap(cpp_double_fp_backend&& other) noexcept
   {
      const rep_type tmp = data;

      data = other.data;

      other.data = tmp;
   }

   BOOST_MP_CXX14_CONSTEXPR int compare(const cpp_double_fp_backend& other) const
   {
      // Return 1 for *this > other, -1 for *this < other, 0 for *this = other.
      #if 0
      return (my_first() > other.my_first()) ?  1 : (my_first()  < other.my_first())
                                             ? -1 : (my_second() > other.my_second())
                                             ?  1 : (my_second() < other.my_second())
                                             ? -1 : 0;

      #else
      const auto other_is_neg = (other.my_first() < 0);
      const auto my_is_neg    = (my_first() < 0);

      auto n_result = static_cast<int>(INT8_C(0));

      if(my_is_neg && (!other_is_neg))
      {
        n_result = static_cast<int>(INT8_C(-1));
      }
      else if((!my_is_neg) && other_is_neg)
      {
        n_result = static_cast<int>(INT8_C(1));
      }
      else
      {
        auto a = *this; if(my_is_neg)    { a.negate(); }
        auto b = other; if(other_is_neg) { b.negate(); }

        if(a.crep() > b.crep())
        {
          n_result = (!my_is_neg) ? 1 : -1;
        }
        else if(a.crep() < b.crep())
        {
          n_result = (!my_is_neg) ? -1 : 1;
        }
      }

      return n_result;
      #endif
   }

   std::string str(std::streamsize number_of_digits, const std::ios::fmtflags format_flags) const
   {
      if (number_of_digits == 0)
         number_of_digits = cpp_double_fp_backend::my_digits10;

      const std::string my_str = boost::multiprecision::detail::convert_to_string(*this, number_of_digits, format_flags);

      return my_str;
   }

   BOOST_MP_CXX14_CONSTEXPR int order02() const
   {
      // TBD: Is there another option to get the base-2 log
      // that's more unequivocally closer to constexpr?

      // TBD: Either way, integrate this (or something like it)
      // into any potential implementation of eval_ilogb().

      int e2 { };
      cpp_double_fp_backend dummy { };

      eval_frexp(dummy, *this, &e2);

      return e2;
   }

   BOOST_MP_CXX14_CONSTEXPR int order10() const { return static_cast<int>(static_cast<float>(order02()) * 0.301F); }

   BOOST_MP_CXX14_CONSTEXPR bool small_arg() const { return (order10() < static_cast<int>(-my_digits10 / 6)); }

   BOOST_MP_CXX14_CONSTEXPR bool near_one() const
   {
      cpp_double_fp_backend delta_one { };

      eval_subtract(delta_one, cpp_double_fp_backend(1U), *this);

      if(delta_one().isneg())
      {
         delta_one.negate();
      }

      return delta_one.small_arg();
   }

   static BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend my_value_max() noexcept
   {
      using std::ldexp;
      using std::sqrt;
#if defined(BOOST_MATH_USE_FLOAT128)
      using boost::multiprecision::ldexp;
      using boost::multiprecision::sqrt;
#endif

      // TBD: Can this be simplified in any way?
      // Is the use of the square root the best way to go?
      // Can this be made traditional C++11 constexpr?

      return
         cpp_double_fp_backend
         (
            arithmetic::fast_sum
            (
               float_type(  (std::numeric_limits<float_type>::max)()
                          * float_type(float_type(1.0F) - float_type(1.5F) * sqrt(std::numeric_limits<float_type>::epsilon()))),
               ldexp
               (
                   (std::numeric_limits<float_type>::max)(),
                  -(std::numeric_limits<float_type>::digits + 1)
               )
            )
         );
   }

   static BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend my_value_min() noexcept
   {
      using std::ldexp;
#if defined(BOOST_MATH_USE_FLOAT128)
      using boost::multiprecision::ldexp;
#endif

      return cpp_double_fp_backend(ldexp(float_type(1), my_min_exponent));
   }

   static BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend my_value_eps() noexcept
   {
      using std::ldexp;
#if defined(BOOST_MATH_USE_FLOAT128)
      using boost::multiprecision::ldexp;
#endif

      // TBD: Do we need a better value here.
      return []() -> cpp_double_fp_backend {
         cpp_double_fp_backend result;

         eval_ldexp(result, cpp_double_fp_backend(1), 4 - my_digits);

         return result;
      }();
   }

   static BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend my_value_nan() noexcept
   {
      return cpp_double_fp_backend(std::numeric_limits<float_type>::quiet_NaN());
   }

   static BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend from_parts(float_type f, float_type s) { return std::make_pair(f, s); }

 private:
   rep_type data;

   template <typename OtherFloatingPointType,
             typename std::enable_if<(detail::is_floating_point_or_float128<OtherFloatingPointType>::value && ((std::numeric_limits<OtherFloatingPointType>::digits10 * 2) < 16))>::type const*>
   friend BOOST_MP_CXX14_CONSTEXPR void eval_exp(cpp_double_fp_backend<OtherFloatingPointType>& result, const cpp_double_fp_backend<OtherFloatingPointType>& x);

   template <typename OtherFloatingPointType,
             typename std::enable_if<(detail::is_floating_point_or_float128<OtherFloatingPointType>::value && (((std::numeric_limits<OtherFloatingPointType>::digits10 * 2) >= 16) && ((std::numeric_limits<OtherFloatingPointType>::digits10 * 2) <= 36)))>::type const*>
   friend BOOST_MP_CXX14_CONSTEXPR void eval_exp(cpp_double_fp_backend<OtherFloatingPointType>& result, const cpp_double_fp_backend<OtherFloatingPointType>& x);

   template <typename OtherFloatingPointType,
             typename std::enable_if<(detail::is_floating_point_or_float128<OtherFloatingPointType>::value && ((std::numeric_limits<OtherFloatingPointType>::digits10 * 2) > 36))>::type const*>
   friend BOOST_MP_CXX14_CONSTEXPR void eval_exp(cpp_double_fp_backend<OtherFloatingPointType>& result, const cpp_double_fp_backend<OtherFloatingPointType>& x);
};

namespace detail {

   // N[Pi, 101]
   // 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170680

   // 3.14159250259,                            1.50995788317e-07
   // 3.141592653589793116,                     1.2246467991473529607e-16
   // 3.14159265358979323851281,                -5.01655761266833202345176e-20
   // 3.14159265358979323846264338327950279748, 8.67181013012378102479704402604335225411e-35

   template <typename FloatingPointType> constexpr auto constant_df_pi() -> typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (std::numeric_limits<FloatingPointType>::digits ==  24)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(3.14159250259L),                            static_cast<FloatingPointType>(1.50995788317e-07L)); }
   template <typename FloatingPointType> constexpr auto constant_df_pi() -> typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (std::numeric_limits<FloatingPointType>::digits ==  53)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(3.141592653589793116L),                     static_cast<FloatingPointType>(1.2246467991473529607e-16L)); }
   template <typename FloatingPointType> constexpr auto constant_df_pi() -> typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (std::numeric_limits<FloatingPointType>::digits ==  64)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(3.14159265358979323851281L),                static_cast<FloatingPointType>(-5.01655761266833202345176e-20L)); }
   #if defined(BOOST_MATH_USE_FLOAT128)
   template <typename FloatingPointType> constexpr auto constant_df_pi() -> typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (std::numeric_limits<FloatingPointType>::digits == 113)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(3.14159265358979323846264338327950279748Q), static_cast<FloatingPointType>(8.67181013012378102479704402604335225411e-35Q)); }
   #endif

   // N[Log[2], 101]
   // 0.69314718055994530941723212145817656807550013436025525412068000949339362196969471560586332699641868754

   // 0.69314712286,                             5.76999887869e-08
   // 0.6931471805599451752,                     1.3421277060097865271e-16
   // 0.69314718055994530942869,                 -1.14583527267987328094768e-20
   // 0.693147180559945309417232121458176575084, -7.00813947454958516341266200877162272784e-36

   template <typename FloatingPointType> constexpr auto constant_df_ln_two() -> typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (std::numeric_limits<FloatingPointType>::digits ==  24)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(0.69314712286L),                             static_cast<FloatingPointType>(5.76999887869e-08L)); }
   template <typename FloatingPointType> constexpr auto constant_df_ln_two() -> typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (std::numeric_limits<FloatingPointType>::digits ==  53)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(0.6931471805599451752L),                     static_cast<FloatingPointType>(1.3421277060097865271e-16L)); }
   template <typename FloatingPointType> constexpr auto constant_df_ln_two() -> typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (std::numeric_limits<FloatingPointType>::digits ==  64)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(0.69314718055994530942869L),                 static_cast<FloatingPointType>(-1.14583527267987328094768e-20L)); }
   #if defined(BOOST_MATH_USE_FLOAT128)
   template <typename FloatingPointType> constexpr auto constant_df_ln_two() -> typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (std::numeric_limits<FloatingPointType>::digits == 113)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(0.693147180559945309417232121458176575084Q), static_cast<FloatingPointType>(-7.00813947454958516341266200877162272784e-36Q)); }
   #endif

   // N[Exp[1], 101]
   // 2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274

   // 2.71828174591,                            8.25483965627e-08
   // 2.7182818284590450908,                    1.4456468917292501578e-16
   // 2.71828182845904523521133,                1.4895979785582304563159e-19
   // 2.71828182845904523536028747135266231436, 1.83398825226506410712297736767396397644e-34

   template <typename FloatingPointType> constexpr auto constant_df_exp1() -> typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (std::numeric_limits<FloatingPointType>::digits ==  24)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(2.71828174591L),                            static_cast<FloatingPointType>(8.25483965627e-08L)); }
   template <typename FloatingPointType> constexpr auto constant_df_exp1() -> typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (std::numeric_limits<FloatingPointType>::digits ==  53)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(2.7182818284590450908L),                    static_cast<FloatingPointType>(1.4456468917292501578e-16L)); }
   template <typename FloatingPointType> constexpr auto constant_df_exp1() -> typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (std::numeric_limits<FloatingPointType>::digits ==  64)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(2.71828182845904523521133L),                static_cast<FloatingPointType>(1.4895979785582304563159e-19L)); }
   #if defined(BOOST_MATH_USE_FLOAT128)
   template <typename FloatingPointType> constexpr auto constant_df_exp1() -> typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (std::numeric_limits<FloatingPointType>::digits == 113)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(2.71828182845904523536028747135266231436Q), static_cast<FloatingPointType>(1.83398825226506410712297736767396397644e-34Q)); }
   #endif

}

template <typename FloatingPointType>
constexpr int cpp_double_fp_backend<FloatingPointType>::my_digits;
template <typename FloatingPointType>
constexpr int cpp_double_fp_backend<FloatingPointType>::my_digits10;
template <typename FloatingPointType>
constexpr int cpp_double_fp_backend<FloatingPointType>::my_max_digits10;
template <typename FloatingPointType>
constexpr int cpp_double_fp_backend<FloatingPointType>::my_max_exponent;
template <typename FloatingPointType>
constexpr int cpp_double_fp_backend<FloatingPointType>::my_min_exponent;
template <typename FloatingPointType>
constexpr int cpp_double_fp_backend<FloatingPointType>::my_max_exponent10;
template <typename FloatingPointType>
constexpr int cpp_double_fp_backend<FloatingPointType>::my_min_exponent10;

template <typename FloatingPointType>
constexpr cpp_double_fp_backend<FloatingPointType> operator+(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { return cpp_double_fp_backend<FloatingPointType>(a) += b; }
template <typename FloatingPointType>
constexpr cpp_double_fp_backend<FloatingPointType> operator-(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { return cpp_double_fp_backend<FloatingPointType>(a) -= b; }
template <typename FloatingPointType>
constexpr cpp_double_fp_backend<FloatingPointType> operator*(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { return cpp_double_fp_backend<FloatingPointType>(a) *= b; }
template <typename FloatingPointType>
constexpr cpp_double_fp_backend<FloatingPointType> operator/(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { return cpp_double_fp_backend<FloatingPointType>(a) /= b; }

// Input/Output Streaming
template <typename FloatingPointType, typename char_type, typename traits_type>
std::basic_ostream<char_type, traits_type>&
operator<<(std::basic_ostream<char_type, traits_type>& os, const cpp_double_fp_backend<FloatingPointType>& f)
{
   const auto str_result = f.str(os.precision(), os.flags());

   return (os << str_result);
}

template <typename FloatingPointType, typename char_type, typename traits_type>
std::basic_istream<char_type, traits_type>&
operator>>(std::basic_istream<char_type, traits_type>& is, cpp_double_fp_backend<FloatingPointType>& f)
{
   std::string str;
   is >> str;
   boost::multiprecision::detail::convert_from_string(f, str.c_str());
   return is;
}

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_add(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x) { result += x; }
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_subtract(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x) { result -= x; }
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_multiply(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x) { result *= x; }
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_divide(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x) { result /= x; }
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR bool eval_eq(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { return (a.compare(b) == 0); }
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR bool eval_lt(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { return (a.compare(b) == -1); }
template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR bool eval_gt(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { return (a.compare(b) == 1); }
template <typename FloatingPointType>

BOOST_MP_CXX14_CONSTEXPR bool eval_is_zero(const cpp_double_fp_backend<FloatingPointType>& x)
{
   auto my_iszero =
      [](FloatingPointType a) -> bool
      {
         return ((a == static_cast<FloatingPointType>(0)) || (a == static_cast<FloatingPointType>(-0)));
      };

   return (my_iszero(x.crep().first) && my_iszero(x.crep().second));
}

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_fabs(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a)
{
   result = a;

   if (a.is_neg())
   {
      result.negate();
   }
}

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_frexp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, int* v)
{
   using std::frexp;
   using std::ldexp;

   result.rep().first  = frexp(a.rep().first, v);
   result.rep().second = ldexp(a.rep().second, -*v);
}

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_ldexp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, int v)
{
   using std::ldexp;

   result.rep() =
      std::make_pair
      (
         ldexp(a.crep().first,  v),
         ldexp(a.crep().second, v)
      );

   using local_backend_type = cpp_double_fp_backend<FloatingPointType>;

   local_backend_type::arithmetic::normalize(result.rep());
}

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_floor(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x)
{
   using double_float_type = cpp_double_fp_backend<FloatingPointType>;

   using std::floor;

   const typename double_float_type::float_type fhi = floor(x.rep().first);

   if (fhi != x.my_first())
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

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_ceil(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x)
{
   // Compute -floor(-x);
   eval_floor(result, -x);

   result.negate();
}

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR int eval_fpclassify(const cpp_double_fp_backend<FloatingPointType>& o)
{
   // The eval_fpclassify implementation is modelled after Matt Borland's work in <ccmath.hpp>.

   auto my_isinf = [](FloatingPointType x) -> bool { return ((x == std::numeric_limits<FloatingPointType>::infinity()) || (-x == std::numeric_limits<FloatingPointType>::infinity())); };
   auto my_isnan = [](FloatingPointType x) -> bool { return (x != x); };

   auto my_abs =
      [](FloatingPointType x) -> FloatingPointType
      {
         if (x != x)
         {
            return std::numeric_limits<FloatingPointType>::quiet_NaN();
         }
         else if (x == static_cast<FloatingPointType>(-0))
         {
            return static_cast<FloatingPointType>(0);
         }

         return ((x >= 0) ? x : -x);
      };

      return  my_isnan(o.crep().first) ? FP_NAN       :
              my_isinf(o.crep().first) ? FP_INFINITE  :
              eval_is_zero(o)          ? FP_ZERO      :
             (my_abs(o.crep().first) > 0) && (my_abs(o.crep().first) < (std::numeric_limits<FloatingPointType>::min)())
                                       ? FP_SUBNORMAL : FP_NORMAL;
}

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_sqrt(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& o)
{
   using double_float_type = cpp_double_fp_backend<FloatingPointType>;
   using local_float_type = typename double_float_type::float_type;

   const auto fpc = eval_fpclassify(o);

   const auto isneg_o = o.isneg();

   if((fpc != FP_NORMAL) || isneg_o)
   {
      if((fpc == FP_ZERO) || (fpc == FP_SUBNORMAL))
      {
         result = double_float_type(0);
         return;
      }
      else if(fpc == FP_NAN)
      {
         result = double_float_type(std::numeric_limits<local_float_type>::quiet_NaN());
         return;
      }
      else if((fpc == FP_INFINITE) || isneg_o)
      {
         result = double_float_type(std::numeric_limits<local_float_type>::quiet_NaN());
         return;
      }
   }

   using std::sqrt;

   const local_float_type c = sqrt(o.crep().first);

   constexpr int              MantissaBits = std::numeric_limits<local_float_type>::digits;
   constexpr int              SplitBits    = MantissaBits / 2 + 1;
   constexpr local_float_type Splitter     = local_float_type((1ULL << SplitBits) + 1);

   local_float_type p  = Splitter * c;
   local_float_type hx = (c - p);
                    hx = hx + p;
   local_float_type tx = c  - hx;
                    p  = hx * hx;
   local_float_type q  = hx * tx;
                    q  = q  + q;

   local_float_type u  = p  + q;
   local_float_type uu = p  - u;
                    uu = uu + q;
   local_float_type t1 = tx * tx;
                    uu = uu + t1;

   local_float_type cc = o.crep().first - u;
   cc = cc - uu;
   cc = cc + o.crep().second;
   t1 = c + c;
   cc = cc / t1;

   hx = c + cc;
   tx = c - hx;
   tx = tx + cc;

   result.rep().first  = hx;
   result.rep().second = tx;
}

template <typename FloatingPointType,
          typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && ((std::numeric_limits<FloatingPointType>::digits10 * 2) < 16))>::type const*>
BOOST_MP_CXX14_CONSTEXPR void eval_exp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x)
{
   const auto x_is_zero = x.is_zero();

   if ((eval_fpclassify(x) != static_cast<int>(FP_NORMAL)) && (!x_is_zero))
   {
      result = x;
   }
   else
   {
      using double_float_type = cpp_double_fp_backend<FloatingPointType>;
      using local_float_type  = typename double_float_type::float_type;

      // Get a local copy of the argument and force it to be positive.
      const auto b_neg = x.is_neg();

      double_float_type xx;

      eval_fabs(xx, x);

      // Check the range of the input.
      const double_float_type max_exp_input =
      []() -> double_float_type
      {
         using std::log;

         const double_float_type lg_x0 = log(double_float_type::my_value_max().crep().first);
         const double_float_type dx    =   double_float_type(double_float_type::my_value_max().crep().second)
                                         / double_float_type(double_float_type::my_value_max().crep().first);

         return lg_x0 + dx;
      }();

      const double_float_type min_exp_input =
      []() -> double_float_type
      {
         using std::log;

         const double_float_type lg_x0 = log(double_float_type::my_value_min().crep().first);

         return lg_x0;
      }();

      if (x_is_zero)
      {
         result = double_float_type(1U);
      }
      else if (eval_lt(x, min_exp_input))
      {
         result = double_float_type(0U);
      }
      else if (eval_gt(xx, max_exp_input))
      {
         result = double_float_type(std::numeric_limits<local_float_type>::infinity());
      }
      else if (xx.is_one())
      {
         result =
            ((!b_neg)
               ?                         detail::constant_df_exp1<local_float_type>()
               : double_float_type(1U) / detail::constant_df_exp1<local_float_type>());
      }
      else
      {
         // Use an argument reduction algorithm for exp() in classic MPFUN-style.

         double_float_type nf;

         eval_floor(nf, xx / detail::constant_df_ln_two<local_float_type>());

         // Prepare the scaled variables.
         const auto b_scale = (xx.order02() > -1);

         double_float_type r;

         if (b_scale)
         {
            eval_ldexp(r, xx - (nf * detail::constant_df_ln_two<local_float_type>()), -2);
         }
         else
         {
            r = xx;
         }

         // PadeApproximant[Exp[x] - 1, {x, 0, {6, 6}}]
         // FullSimplify[%]
         //   (84 x (7920 + 240 x^2 + x^4))
         // / (665280 + x (-332640 + x (75600 + x (-10080 + x (840 + (-42 + x) x)))))

         const double_float_type n84(84);
         const double_float_type n240(240);
         const double_float_type n7920(7920);

         const double_float_type n665280(665280);
         const double_float_type n332640(332640);
         const double_float_type n75600(75600);
         const double_float_type n10080(10080);
         const double_float_type n840(840);
         const double_float_type n42(42);

         const double_float_type r2 = r * r;

         result = double_float_type(1U) + ((n84 * r) * (n7920 + r2 * (n240 + r2))) / (n665280 + r * (-n332640 + r * (n75600 + r * (-n10080 + r * (n840 + (-n42 + r) * r)))));

         // Use the small-argument Pade approximation having coefficients shown above.
         const double_float_type top = (n84 * r * (n7920 + (n240 + r2) * r2));
         const double_float_type bot = (n665280 + r * (-n332640 + r * (n75600 + r * (-n10080 + r * (n840 + (-n42 + r) * r)))));

         result = double_float_type(1U) + (top / bot);

         // Rescale the result.
         if (b_scale)
         {
            result *= result;
            result *= result;

            signed long long lln { };

            eval_convert_to(&lln, nf);

            const auto n = static_cast<int>(lln);

            if (n > 0)
            {
               eval_ldexp(result, double_float_type(result), n);
            }
         }

         if (b_neg)
         {
            result = double_float_type(1U) / result;
         }
      }
   }
}

template <typename FloatingPointType,
          typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && (((std::numeric_limits<FloatingPointType>::digits10 * 2) >= 16) && ((std::numeric_limits<FloatingPointType>::digits10 * 2) <= 36)))>::type const*>
BOOST_MP_CXX14_CONSTEXPR void eval_exp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x)
{
   const auto x_is_zero = x.is_zero();

   const auto fpc = eval_fpclassify(x);

   if ((fpc != static_cast<int>(FP_NORMAL)) && (!x_is_zero))
   {
      result = x;
   }
   else
   {
      using double_float_type = cpp_double_fp_backend<FloatingPointType>;
      using local_float_type  = typename double_float_type::float_type;

      // Get a local copy of the argument and force it to be positive.
      const bool b_neg = x.is_neg();

      double_float_type xx;

      eval_fabs(xx, x);

      // Check the range of the input.
      const double_float_type max_exp_input =
      []() -> double_float_type
      {
         using std::log;

         const double_float_type lg_x0 = log(double_float_type::my_value_max().crep().first);
         const double_float_type dx    =   double_float_type(double_float_type::my_value_max().crep().second)
                                         / double_float_type(double_float_type::my_value_max().crep().first);

         return lg_x0 + dx;
      }();

      const double_float_type min_exp_input =
      []() -> double_float_type
      {
         using std::log;

         const double_float_type lg_x0 = log(double_float_type::my_value_min().crep().first);

         return lg_x0;
      }();

      if (x_is_zero)
      {
         result = double_float_type(1U);
      }
      else if (eval_lt(x, min_exp_input))
      {
         result = double_float_type(0U);
      }
      else if (eval_gt(xx, max_exp_input))
      {
         result = double_float_type(std::numeric_limits<local_float_type>::infinity());
      }
      else if (xx.is_one())
      {
         result =
            ((!b_neg)
               ?                         detail::constant_df_exp1<local_float_type>()
               : double_float_type(1U) / detail::constant_df_exp1<local_float_type>());
      }
      else
      {
         // Use an argument reduction algorithm for exp() in classic MPFUN-style.

         double_float_type nf;

         eval_floor(nf, xx / detail::constant_df_ln_two<local_float_type>());

         // Prepare the scaled variables.
         const bool b_scale = (xx.order02() > -4);

         double_float_type r;

         if (b_scale)
         {
            eval_ldexp(r, xx - (nf * detail::constant_df_ln_two<local_float_type>()), -4);
         }
         else
         {
            r = xx;
         }

         // PadeApproximant[Exp[r], {r, 0, 8, 8}]
         // FullSimplify[%]

         const double_float_type n144(144U);
         const double_float_type n3603600(3603600UL);
         const double_float_type n120120(120120UL);
         const double_float_type n770(770U);

         const double_float_type n518918400(518918400UL);
         const double_float_type n259459200(259459200UL);
         const double_float_type n60540480(60540480UL);
         const double_float_type n8648640(8648640UL);
         const double_float_type n831600(831600UL);
         const double_float_type n55440(55440U);
         const double_float_type n2520(2520U);
         const double_float_type n72(72U);

         const double_float_type r2 = r * r;

         const double_float_type top = (n144 * r) * (n3603600 + r2 * (n120120 + r2 * (n770 + r2)));
         const double_float_type bot = (n518918400 + r * (-n259459200 + r * (n60540480 + r * (-n8648640 + r * (n831600 + r * (-n55440 + r * (n2520 + r * (-n72 + r))))))));

         result = double_float_type(1U) + (top / bot);

         // Rescale the result.
         if (b_scale)
         {
            result *= result;
            result *= result;
            result *= result;
            result *= result;

            signed long long lln { };

            eval_convert_to(&lln, nf);

            const auto n = static_cast<int>(lln);

            if (n > 0)
            {
               eval_ldexp(result, double_float_type(result), n);
            }
         }

         if (b_neg)
         {
            result = double_float_type(1U) / result;
         }
      }
   }
}

template <typename FloatingPointType,
          typename std::enable_if<(detail::is_floating_point_or_float128<FloatingPointType>::value && ((std::numeric_limits<FloatingPointType>::digits10 * 2) > 36))>::type const*>
BOOST_MP_CXX14_CONSTEXPR void eval_exp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x)
{
   const auto x_is_zero = x.is_zero();

   const auto fpc = eval_fpclassify(x);

   if ((fpc != static_cast<int>(FP_NORMAL)) && (!x_is_zero))
   {
      result = x;
   }
   else
   {
      using double_float_type = cpp_double_fp_backend<FloatingPointType>;
      using local_float_type  = typename double_float_type::float_type;

      // Get a local copy of the argument and force it to be positive.
      const bool b_neg = x.is_neg();

      double_float_type xx;

      eval_fabs(xx, x);

      // Check the range of the input.
      const double_float_type max_exp_input =
      []() -> double_float_type
      {
         using std::log;

         const double_float_type lg_x0 = log(double_float_type::my_value_max().crep().first);
         const double_float_type dx    =   double_float_type(double_float_type::my_value_max().crep().second)
                                         / double_float_type(double_float_type::my_value_max().crep().first);

         return lg_x0 + dx;
      }();

      const double_float_type min_exp_input =
      []() -> double_float_type
      {
         using std::log;

         const double_float_type lg_x0 = log(double_float_type::my_value_min().crep().first);

         return lg_x0;
      }();

      if (x_is_zero)
      {
         result = double_float_type(1U);
      }
      else if (eval_lt(x, min_exp_input))
      {
         result = double_float_type(0U);
      }
      else if (eval_gt(xx, max_exp_input))
      {
         result = double_float_type(std::numeric_limits<local_float_type>::infinity());
      }
      else if (xx.is_one())
      {
         result =
            ((!b_neg)
               ?                         detail::constant_df_exp1<local_float_type>()
               : double_float_type(1U) / detail::constant_df_exp1<local_float_type>());
      }
      else
      {
         // Use an argument reduction algorithm for exp() in classic MPFUN-style.

         double_float_type nf;

         eval_floor(nf, xx / detail::constant_df_ln_two<local_float_type>());

         // Prepare the scaled variables.
         const bool b_scale = (xx.order02() > -4);

         double_float_type xh;

         if (b_scale)
         {
            eval_ldexp(xh, xx - (nf * detail::constant_df_ln_two<local_float_type>()), -4);
         }
         else
         {
            xh = xx;
         }

         double_float_type x_pow_n_div_n_fact(xh);

         result = double_float_type(1U) + x_pow_n_div_n_fact;

         double_float_type dummy { };

         // Series expansion of hypergeometric_0f0(; ; x).
         // For this high(er) digit count, a scaled argument with subsequent
         // Taylor series expansion is actually more precise than Pade approximation.
         for (unsigned n = 2U; n < 64U; ++n)
         {
            x_pow_n_div_n_fact *= xh;
            x_pow_n_div_n_fact /= typename double_float_type::float_type(n);

            int n_tol { };

            eval_frexp(dummy, x_pow_n_div_n_fact, &n_tol);

            if ((n > 4U) && (n_tol < -(double_float_type::my_digits - 6)))
            {
               break;
            }

            result += x_pow_n_div_n_fact;
         }

         // Rescale the result.
         if (b_scale)
         {
            result *= result;
            result *= result;
            result *= result;
            result *= result;

            signed long long lln { };

            eval_convert_to(&lln, nf);

            const auto n = static_cast<int>(lln);

            if (n > 0)
            {
               eval_ldexp(result, double_float_type(result), n);
            }
         }

         if (b_neg)
         {
            result = double_float_type(1U) / result;
         }
      }
   }
}

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_log(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x)
{
   using double_float_type = cpp_double_fp_backend<FloatingPointType>;

   const auto x_is_zero = x.is_zero();

   const auto fpc = eval_fpclassify(x);

   if ((fpc != static_cast<int>(FP_NORMAL)) && (!x_is_zero))
   {
      result = x;
   }
   else if (x.is_neg())
   {
      result = double_float_type::my_value_nan();
   }
   else if (x.is_one())
   {
      result = double_float_type(0U);
   }
   else
   {
      using std::log;

      // Get initial estimate using the standard math function log.
      const double_float_type s(log(x.crep().first));
      double_float_type       E;

      eval_exp(E, s);

      // Do one single step of Newton-Raphson iteration
      result = s + ((x - E) / E);
   }
}

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_convert_to(signed long long* result, const cpp_double_fp_backend<FloatingPointType>& backend)
{
   using c_type = typename std::common_type<signed long long, FloatingPointType>::type;

   using std::fabs;

   constexpr c_type my_max = static_cast<c_type>((std::numeric_limits<signed long long>::max)());
   c_type           ct     = fabs(backend.crep().first);

   if (ct > my_max)
   {
      *result = backend.crep().first >= typename cpp_double_fp_backend<FloatingPointType>::float_type(0U) ? (std::numeric_limits<signed long long>::max)() : detail::minus_max<signed long long>();
   }
   else
   {
      *result  = static_cast<signed long long>(backend.crep().first);
      *result += static_cast<signed long long>(backend.crep().second);
   }
}

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR void eval_convert_to(unsigned long long* result, const cpp_double_fp_backend<FloatingPointType>& backend)
{
   using c_type = typename std::common_type<unsigned long long, FloatingPointType>::type;

   using std::fabs;

   constexpr c_type my_max = static_cast<c_type>((std::numeric_limits<unsigned long long>::max)());
   c_type           ct     = fabs(backend.crep().first);

   if (ct > my_max)
   {
      *result = (std::numeric_limits<unsigned long long>::max)();
   }
   else
   {
      *result  = static_cast<unsigned long long>(backend.crep().first);
      *result += static_cast<unsigned long long>(backend.crep().second);
   }
}

template <typename FloatingPointType,
          typename R>
BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<detail::is_floating_point_or_float128<R>::value>::type eval_convert_to(R* result, const cpp_double_fp_backend<FloatingPointType>& backend)
{
   *result  = static_cast<R>(backend.crep().first);
   *result += static_cast<R>(backend.crep().second);
}

template <typename FloatingPointType>
BOOST_MP_CXX14_CONSTEXPR cpp_double_fp_backend<FloatingPointType> fabs(const cpp_double_fp_backend<FloatingPointType>& a)
{
   using double_float_type = cpp_double_fp_backend<FloatingPointType>;

   double_float_type result;

   eval_fabs(result, a);

   return result;
}

template <typename FloatingPointType>
std::size_t hash_value(const cpp_double_fp_backend<FloatingPointType>& a)
{
   return a.hash();
}

} // namespace backends

using cpp_double_float       = number<backends::cpp_double_fp_backend<float>>;
using cpp_double_double      = number<backends::cpp_double_fp_backend<double>>;
using cpp_double_long_double = number<backends::cpp_double_fp_backend<long double>>;
#ifdef BOOST_MATH_USE_FLOAT128
using cpp_double_float128    = number<backends::cpp_double_fp_backend<float128>>;
#endif

}} // namespace boost::multiprecision

namespace std {

// Specialization of numeric_limits for boost::multiprecision::number<cpp_double_fp_backend<>>
template <typename FloatingPointType,
          const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
BOOST_MP_DF_QF_NUM_LIMITS_CLASS_TYPE numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >
    : public std::numeric_limits<FloatingPointType>
{
 private:
   using base_class_type = std::numeric_limits<FloatingPointType>;

   using inner_self_type = boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>;

   using self_type =
       boost::multiprecision::number<inner_self_type, ExpressionTemplatesOption>;

 public:
   static constexpr bool is_specialized                = true;
   static constexpr bool is_signed                     = true;
   static constexpr bool is_integer                    = false;
   static constexpr bool is_exact                      = false;
   static constexpr bool is_bounded                    = true;
   static constexpr bool is_modulo                     = false;
   static constexpr bool is_iec559                     = false;
   static constexpr std::float_denorm_style has_denorm = std::denorm_absent;

   static constexpr int digits                         = inner_self_type::my_digits;
   static constexpr int digits10                       = inner_self_type::my_digits10;
   static constexpr int max_digits10                   = inner_self_type::my_max_digits10;

   static constexpr int max_exponent                   = inner_self_type::my_max_exponent;
   static constexpr int min_exponent                   = inner_self_type::my_min_exponent;
   static constexpr int max_exponent10                 = inner_self_type::my_max_exponent10;
   static constexpr int min_exponent10                 = inner_self_type::my_min_exponent10;

   static BOOST_MP_CXX14_CONSTEXPR self_type(min)         () noexcept { return self_type(inner_self_type::my_value_min()); }
   static BOOST_MP_CXX14_CONSTEXPR self_type(max)         () noexcept { return self_type(inner_self_type::my_value_max()); }
   static BOOST_MP_CXX14_CONSTEXPR self_type lowest       () noexcept { return self_type(-(max)()); }
   static BOOST_MP_CXX14_CONSTEXPR self_type epsilon      () noexcept { return self_type(inner_self_type::my_value_eps()); }
   static BOOST_MP_CXX14_CONSTEXPR self_type round_error  () noexcept { return self_type(base_class_type::round_error()); }
   static BOOST_MP_CXX14_CONSTEXPR self_type denorm_min   () noexcept { return self_type((min)()); }
   static BOOST_MP_CXX14_CONSTEXPR self_type infinity     () noexcept { return self_type(base_class_type::infinity()); }
   static BOOST_MP_CXX14_CONSTEXPR self_type quiet_NaN    () noexcept { return self_type(base_class_type::quiet_NaN()); }
   static BOOST_MP_CXX14_CONSTEXPR self_type signaling_NaN() noexcept { return self_type(base_class_type::signaling_NaN()); }
};

} // namespace std

template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_specialized;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_signed;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_integer;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_exact;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_bounded;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_modulo;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_iec559;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr std::float_denorm_style std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::has_denorm;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::digits;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::digits10;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::max_digits10;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::max_exponent;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::min_exponent;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::max_exponent10;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::min_exponent10;

#endif // BOOST_MP_CPP_DOUBLE_FP_2021_06_05_HPP
