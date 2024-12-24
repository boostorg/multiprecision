///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 - 2024 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DOUBLE_FP_2021_06_05_HPP
#define BOOST_MP_CPP_DOUBLE_FP_2021_06_05_HPP

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail.hpp>
#include <boost/multiprecision/detail/float_string_cvt.hpp>
#include <boost/multiprecision/detail/fpclassify.hpp>
#include <boost/multiprecision/detail/hash.hpp>
#include <boost/multiprecision/traits/max_digits10.hpp>

#ifdef BOOST_MP_MATH_AVAILABLE
//
// Headers required for Boost.Math integration:
//
#include <boost/math/policies/policy.hpp>
//
// Some includes we need from Boost.Math, since we rely on that library to provide these functions:
//
#include <boost/math/special_functions/acosh.hpp>
#include <boost/math/special_functions/asinh.hpp>
#include <boost/math/special_functions/atanh.hpp>
#include <boost/math/special_functions/cbrt.hpp>
#include <boost/math/special_functions/expm1.hpp>
#include <boost/math/special_functions/gamma.hpp>
#endif

#include <limits>
#include <string>
#include <type_traits>

#if (defined(__clang__) && (__clang_major__ <= 9))
#define BOOST_MP_DF_QF_NUM_LIMITS_CLASS_TYPE struct
#else
#define BOOST_MP_DF_QF_NUM_LIMITS_CLASS_TYPE class
#endif

namespace boost { namespace multiprecision { namespace backends {

template <typename FloatingPointType>
class cpp_double_fp_backend;

template <typename FloatingPointType>
constexpr void eval_add(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
constexpr void eval_add(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b);
template <typename FloatingPointType>
constexpr void eval_subtract(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
constexpr void eval_subtract(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b);
template <typename FloatingPointType>
constexpr void eval_multiply(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
constexpr void eval_multiply(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b);
template <typename FloatingPointType>
constexpr void eval_divide(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
constexpr void eval_divide(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b);
template <typename FloatingPointType>
constexpr bool eval_eq(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b);
template <typename FloatingPointType>
constexpr bool eval_lt(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b);
template <typename FloatingPointType>
constexpr bool eval_gt(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b);
template <typename FloatingPointType>
constexpr bool eval_is_zero(const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
constexpr int eval_signbit(const cpp_double_fp_backend<FloatingPointType>& x);

template <typename FloatingPointType>
constexpr void eval_fabs(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a);
template <typename FloatingPointType>
constexpr void eval_frexp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, int* v);
template <typename FloatingPointType>
constexpr void eval_ldexp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, int v);
template <typename FloatingPointType>
constexpr void eval_floor(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
constexpr void eval_ceil(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);
template <typename FloatingPointType>
constexpr int eval_fpclassify(const cpp_double_fp_backend<FloatingPointType>& o);

template <typename FloatingPointType>
constexpr void eval_sqrt(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& o);

template <typename FloatingPointType,
          typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && ((cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits10 * 2) < 16))>::type const* = nullptr>
constexpr void eval_exp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);

template <typename FloatingPointType,
          typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (((cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits10 * 2) >= 16) && ((cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits10 * 2) <= 36)))>::type const* = nullptr>
constexpr void eval_exp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);

template <typename FloatingPointType,
          typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && ((cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits10 * 2) > 36))>::type const* = nullptr>
constexpr void eval_exp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);

template <typename FloatingPointType>
constexpr void eval_log(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x);

template <typename FloatingPointType>
constexpr void eval_convert_to(signed long long* result, const cpp_double_fp_backend<FloatingPointType>& backend);

template <typename FloatingPointType>
constexpr void eval_convert_to(unsigned long long* result, const cpp_double_fp_backend<FloatingPointType>& backend);

#ifdef BOOST_HAS_INT128
template <typename FloatingPointType>
constexpr void eval_convert_to(int128_type* result, const cpp_double_fp_backend<FloatingPointType>& backend);

template <typename FloatingPointType>
constexpr void eval_convert_to(uint128_type* result, const cpp_double_fp_backend<FloatingPointType>& backend);
#endif

template <typename FloatingPointType,
          typename OtherFloatingPointType>
constexpr typename ::std::enable_if<cpp_df_qf_detail::is_floating_point_or_float128<OtherFloatingPointType>::value>::type eval_convert_to(OtherFloatingPointType* result, const cpp_double_fp_backend<FloatingPointType>& backend);

template <typename FloatingPointType,
          typename char_type,
          typename traits_type>
::std::basic_ostream<char_type, traits_type>& operator<<(std::basic_ostream<char_type, traits_type>& os,
                                                         const cpp_double_fp_backend<FloatingPointType>&  f);

template <typename FloatingPointType>
::std::size_t hash_value(const cpp_double_fp_backend<FloatingPointType>& a);

template <typename FloatingPointType>
constexpr cpp_double_fp_backend<FloatingPointType> fabs(const cpp_double_fp_backend<FloatingPointType>& a);

} } } // namespace boost::multiprecision::backends

namespace std {

// Foward declarations of various specializations of std::numeric_limits

template <typename FloatingPointType,
          const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
BOOST_MP_DF_QF_NUM_LIMITS_CLASS_TYPE numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >;

} // namespace std

namespace boost { namespace multiprecision {

template <typename FloatingPointType>
struct number_category<backends::cpp_double_fp_backend<FloatingPointType>> : public std::integral_constant<int, number_kind_floating_point> { };

namespace backends {

// A cpp_double_fp_backend is represented by an unevaluated sum of two floating-point
// units (say a0 and a1) which satisfy |a1| <= (1 / 2) * ulp(a0).
// The type of the floating-point constituents should adhere to IEEE754.

template <typename FloatingPointType>
class cpp_double_fp_backend
{
 public:
   using float_type = FloatingPointType;
   using rep_type   = std::pair<float_type, float_type>;
   using arithmetic = cpp_df_qf_detail::exact_arithmetic<float_type>;

   using signed_types   = std::tuple<signed char, signed short, signed int, signed long, signed long long, std::intmax_t>;
   using unsigned_types = std::tuple<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long, std::uintmax_t>;
   using float_types    = std::tuple<float, double, long double>;
   using exponent_type  = int;

   static constexpr int my_digits         = static_cast<int>(cpp_df_qf_detail::ccmath::numeric_limits<float_type>::digits * static_cast<int>(INT8_C(2)));
   static constexpr int my_digits10       = static_cast<int>(boost::multiprecision::detail::calc_digits10    <static_cast<unsigned>(my_digits)>::value);
   static constexpr int my_max_digits10   = static_cast<int>(boost::multiprecision::detail::calc_max_digits10<static_cast<unsigned>(my_digits)>::value);
   static constexpr int my_max_exponent   = cpp_df_qf_detail::ccmath::numeric_limits<float_type>::max_exponent;
   static constexpr int my_max_exponent10 = cpp_df_qf_detail::ccmath::numeric_limits<float_type>::max_exponent10;

   static constexpr int my_min_exponent =
      static_cast<int>
      (
           cpp_df_qf_detail::ccmath::numeric_limits<float_type>::min_exponent
         + cpp_df_qf_detail::ccmath::numeric_limits<float_type>::digits
      );

   static constexpr int my_min_exponent10 =
      static_cast<int>
      (
         -static_cast<int>
          (
             boost::multiprecision::detail::calc_digits10<static_cast<unsigned>(-my_min_exponent)>::value
          )
      );

   // TBD: Did we justify this static assertion during the GSoC?
   // Does anyone remember what the meaning of the number 77 is?

   static_assert(((my_max_exponent - my_digits) >= 77),
                 "Error: floating-point constituent does not have wide enough exponent range");

   // Default constructor.
   constexpr cpp_double_fp_backend() noexcept { }

   // Copy constructor.
   constexpr cpp_double_fp_backend(const cpp_double_fp_backend& other) : data(other.data) { }

   // Move constructor.
   constexpr cpp_double_fp_backend(cpp_double_fp_backend&& other) noexcept : data(static_cast<rep_type&&>(other.data)) { }

   // Constructors from other floating-point types.
   template <typename OtherFloatType,
             typename ::std::enable_if<(    cpp_df_qf_detail::is_floating_point_or_float128<OtherFloatType>::value
                                        && (cpp_df_qf_detail::ccmath::numeric_limits<OtherFloatType>::digits <= cpp_df_qf_detail::ccmath::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(const OtherFloatType& f)
      : data(f, static_cast<float_type>(0.0F)) { }

   template <typename OtherFloatType,
             typename ::std::enable_if<(    cpp_df_qf_detail::is_floating_point_or_float128<OtherFloatType>::value
                                        && (cpp_df_qf_detail::ccmath::numeric_limits<OtherFloatType>::digits > cpp_df_qf_detail::ccmath::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(const OtherFloatType& f)
      : data(static_cast<float_type>(f),
             static_cast<float_type>(f - static_cast<OtherFloatType>(static_cast<float_type>(f)))) { }

   // Construtor from another kind of cpp_double_fp_backend<> object.

   // TBD: Need to keep widening conversion implicit,
   // but ensure that narrowing conversion is explicit.
   template <typename OtherFloatType,
             typename ::std::enable_if<(    cpp_df_qf_detail::is_floating_point_or_float128<OtherFloatType>::value
                                        && (!std::is_same<FloatingPointType, OtherFloatType>::value))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(const cpp_double_fp_backend<OtherFloatType>& a)
      : cpp_double_fp_backend(a.my_first())
   {
      // TBD: Maybe specialize this constructor for cases either wider or less wide.
      operator+=(a.my_second());
   }

   // Constructors from integers
   template <typename SignedIntegralType,
             typename ::std::enable_if<(     boost::multiprecision::detail::is_integral<SignedIntegralType>::value
                                        && (!boost::multiprecision::detail::is_unsigned<SignedIntegralType>::value)
                                        && (static_cast<int>(sizeof(SignedIntegralType) * 8u) <= cpp_df_qf_detail::ccmath::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(const SignedIntegralType& n)
      : data(static_cast<float_type>(n), static_cast<float_type>(0.0F)) { }

   template <typename UnsignedIntegralType,
             typename ::std::enable_if<(    boost::multiprecision::detail::is_integral<UnsignedIntegralType>::value
                                        &&  boost::multiprecision::detail::is_unsigned<UnsignedIntegralType>::value
                                        && (static_cast<int>(sizeof(UnsignedIntegralType) * 8u) <= cpp_df_qf_detail::ccmath::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(const UnsignedIntegralType& u)
      : data(static_cast<float_type>(u), static_cast<float_type>(0.0F)) { }

   // Constructors from integers which hold more information than *this can contain
   template <typename UnsignedIntegralType,
             typename ::std::enable_if<(    boost::multiprecision::detail::is_integral<UnsignedIntegralType>::value
                                        &&  boost::multiprecision::detail::is_unsigned<UnsignedIntegralType>::value
                                        && (static_cast<int>(sizeof(UnsignedIntegralType) * 8u) > cpp_df_qf_detail::ccmath::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(UnsignedIntegralType u)
      : data
        (
           static_cast<float_type>(u),
           static_cast<float_type>
           (
              static_cast<UnsignedIntegralType>
              (
                 u - static_cast<UnsignedIntegralType>(static_cast<float_type>(u))
              )
           )
        ) { }

   template <typename SignedIntegralType,
             typename ::std::enable_if<(     boost::multiprecision::detail::is_integral<SignedIntegralType>::value
                                        && (!boost::multiprecision::detail::is_unsigned<SignedIntegralType>::value)
                                        && (static_cast<int>(sizeof(SignedIntegralType) * 8u) > cpp_df_qf_detail::ccmath::numeric_limits<float_type>::digits))>::type const* = nullptr>
   constexpr cpp_double_fp_backend(SignedIntegralType n)
      : data
        (
           static_cast<float_type>(n),
           static_cast<float_type>
           (
              static_cast<SignedIntegralType>
              (
                 n - static_cast<SignedIntegralType>(static_cast<float_type>(n))
              )
           )
        ) { }

   constexpr cpp_double_fp_backend(const float_type& a, const float_type& b) noexcept : data(a, b) { }

   constexpr cpp_double_fp_backend(const std::pair<float_type, float_type>& p) noexcept : data(p) { }

   cpp_double_fp_backend(const char* s)
   {
      *this = s;
   }

   // Assignment operator.
   constexpr cpp_double_fp_backend& operator=(const cpp_double_fp_backend& other)
   {
      if (this != &other)
      {
         data = other.data;
      }

      return *this;
   }

   // Move assignment operator.
   constexpr cpp_double_fp_backend& operator=(cpp_double_fp_backend&& other) noexcept
   {
      data = static_cast<rep_type&&>(other.data);

      return *this;
   }

   // Assignment operator from another kind of cpp_double_fp_backend<> object.
   template <typename OtherFloatType,
             typename ::std::enable_if<(    cpp_df_qf_detail::is_floating_point_or_float128<OtherFloatType>::value
                                      && (!std::is_same<FloatingPointType, OtherFloatType>::value))>::type const* = nullptr>
   constexpr cpp_double_fp_backend& operator=(const cpp_double_fp_backend<OtherFloatType>& other)
   {
     return operator=(cpp_double_fp_backend(other));
   }

   template <typename OtherFloatType>
   constexpr typename ::std::enable_if<cpp_df_qf_detail::is_floating_point_or_float128<OtherFloatType>::value, cpp_double_fp_backend&>::type operator=(const OtherFloatType f)
   {
     return operator=(cpp_double_fp_backend(f));
   }

   template <typename IntegralType>
   constexpr typename ::std::enable_if<boost::multiprecision::detail::is_integral<IntegralType>::value, cpp_double_fp_backend&>::type operator=(const IntegralType n)
   {
     return operator=(cpp_double_fp_backend(n));
   }

   cpp_double_fp_backend& operator=(const char* v)
   {
      rd_string(v);

      return *this;
   }

   std::size_t hash() const
   {
      // Here we first convert to scientific string, then
      // hash the charactgers in the scientific string.
      // TBD: Is there a faster or more simple hash method?
      // TBD: Is there any constexpr support for rudimentary hashing?

      const std::string str_to_hash { str(cpp_double_fp_backend::my_digits10, std::ios::scientific) };

      auto result = static_cast<std::size_t>(UINT8_C(0));

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

   constexpr void negate()
   {
      // TBD: Can this be simplified?

      const auto fpc = eval_fpclassify(*this);

      const auto isinf_u  = (fpc == FP_INFINITE);
      const auto isnan_u  = (fpc == FP_NAN);
      const auto iszero_u = (fpc == FP_ZERO);

      if (iszero_u || isnan_u)
      {
      }
      else if (isinf_u)
      {
         data.first = -data.first;
      }
      else
      {
         data.first = -data.first;

         if (data.second != 0) { data.second = -data.second; }

         arithmetic::normalize(data, data.first, data.second);
      }
   }

   constexpr bool isneg() const noexcept
   {
     return (data.first < 0);
   }

   // Getters/Setters
   constexpr const float_type& my_first () const noexcept { return data.first; }
   constexpr const float_type& my_second() const noexcept { return data.second; }

   constexpr rep_type& rep() noexcept { return data; }

   constexpr const rep_type& rep() const noexcept { return data; }

   constexpr const rep_type& crep() const noexcept { return data; }

   // Unary add/sub/mul/div follow in the upcoming paragraphs.

   constexpr cpp_double_fp_backend& operator+=(const cpp_double_fp_backend& v)
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
            *this = cpp_double_fp_backend::my_value_nan();
         }
         return *this;
      }

      const auto iszero_u = ((fpc_u == FP_ZERO) || (fpc_u == FP_SUBNORMAL));
      const auto isnan_v  = (fpc_v == FP_NAN);

      if (iszero_u || (isnan_v || isinf_v))
      {
         if (iszero_u)
         {
            data.first  = float_type { 0.0F };
            data.second = float_type { 0.0F };
         }

         const auto iszero_v = ((fpc_v == FP_ZERO) || (fpc_v == FP_SUBNORMAL));

         return ((!iszero_v) ? operator=(v) : *this);
      }

      if (this == &v)
      {
         return operator+=(cpp_double_fp_backend(v));
      }

      const float_type xlo { data.second };

      data = two_sum(data.first, v.data.first);

      const rep_type thi_tlo { two_sum(xlo, v.data.second) };

      data = two_hilo_sum(data.first, data.second + thi_tlo.first);

      data = two_hilo_sum(data.first, thi_tlo.second + data.second);

      return *this;
   }

   constexpr cpp_double_fp_backend& operator-=(const cpp_double_fp_backend& v)
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
         if (isinf_v && (isneg() == v.isneg()))
         {
            *this = cpp_double_fp_backend::my_value_nan();
         }
         return *this;
      }

      const auto iszero_u = ((fpc_u == FP_ZERO) || (fpc_u == FP_SUBNORMAL));
      const auto isnan_v  = (fpc_v == FP_NAN);

      if (iszero_u || (isnan_v || isinf_v))
      {
         if (iszero_u)
         {
            data.first  = float_type { 0.0F };
            data.second = float_type { 0.0F };
         }

         const auto iszero_v = ((fpc_v == FP_ZERO) || (fpc_v == FP_SUBNORMAL));

         return ((!iszero_v) ? operator=(-v) : *this);
      }

      if (this == &v)
      {
         data.first  = float_type { 0.0F };
         data.second = float_type { 0.0F };

         return *this;
      }

      const float_type xlo { data.second };

      data = two_diff(data.first, v.data.first);

      const rep_type thi_tlo { two_diff(xlo, v.data.second) };

      data = two_hilo_sum(data.first, data.second + thi_tlo.first);

      data = two_hilo_sum(data.first, thi_tlo.second + data.second);

      return *this;
   }

   constexpr cpp_double_fp_backend& operator*=(const cpp_double_fp_backend& v)
   {
      // Evaluate the sign of the result.
      const auto isneg_u =   isneg();
      const auto isneg_v = v.isneg();

      const bool b_result_is_neg = (isneg_u != isneg_v);

      const auto fpc_u = eval_fpclassify(*this);
      const auto fpc_v = eval_fpclassify(v);

      // Handle special cases like zero, inf and NaN.
      const bool isinf_u  { (fpc_u == FP_INFINITE) };
      const bool isinf_v  { (fpc_v == FP_INFINITE) };
      const bool isnan_u  { (fpc_u == FP_NAN) };
      const bool isnan_v  { (fpc_v == FP_NAN) };
      const bool iszero_u { (fpc_u == FP_ZERO) };
      const bool iszero_v { (fpc_v == FP_ZERO) };

      if ((isnan_u || isnan_v) || (isinf_u && iszero_v) || (isinf_v && iszero_u))
      {
         return operator=( cpp_double_fp_backend::my_value_nan());
      }

      if (isinf_u || isinf_v)
      {
         *this = cpp_double_fp_backend::my_value_inf();

         if (b_result_is_neg)
            negate();
         return *this;
      }

      if (iszero_u || iszero_v)
      {
         return operator=(cpp_double_fp_backend(0));
      }

      if (this == &v)
      {
         return operator*=(cpp_double_fp_backend(v));
      }

      // The multiplication algorithm has been taken from Victor Shoup,
      // package WinNTL-5_3_2. It might originally be related to the
      // K. Briggs work. The algorithm has been significantly simplified
      // while still atempting to retain proper rounding corrections.

      float_type C { cpp_df_qf_detail::split(float_type()) * data.first };

      const float_type hu { C - float_type { C - data.first } };

      C = data.first * v.data.first;

      if (cpp_df_qf_detail::ccmath::isinf(C))
      {
         *this = cpp_double_fp_backend::my_value_inf();

         if (b_result_is_neg)
            negate();
         return *this;
      }

      float_type c { cpp_df_qf_detail::split(float_type()) * v.data.first };

      const float_type hv { c - float_type { c - v.data.first } };

      const float_type tv { v.data.first - hv };

      float_type t1 { cpp_df_qf_detail::ccmath::fma(hu, hv, -C) };

      t1 = cpp_df_qf_detail::ccmath::fma(hu, tv, t1);

      const float_type tu { data.first - hu };

      t1 = cpp_df_qf_detail::ccmath::fma(tu, hv, t1);

      c =    cpp_df_qf_detail::ccmath::fma(tu, tv, t1)
          + (data.first * v.data.second)
          + (data.second * v.data.first);

      // Perform even more simplifications compared to Victor Shoup.
      data.first  = C + c;
      data.second = float_type { C - data.first } + c;

      return *this;
   }

   constexpr cpp_double_fp_backend& operator/=(const cpp_double_fp_backend& v)
   {
      // Handle special cases like zero, inf and NaN.
      const auto fpc_u = eval_fpclassify(*this);
      const auto fpc_v = eval_fpclassify(v);

      const auto isnan_u = (fpc_u == FP_NAN);
      const auto isnan_v = (fpc_v == FP_NAN);

      if (isnan_u || isnan_v)
      {
         return operator=( cpp_double_fp_backend::my_value_nan());
      }

      const auto iszero_u = (fpc_u == FP_ZERO);
      const auto iszero_v = (fpc_v == FP_ZERO);

      if (this == &v)
      {
         data.first  = float_type { 1.0F };
         data.second = float_type { 0.0F };

         return *this;
      }

      if (iszero_u)
      {
         if (iszero_v)
         {
            return operator=(cpp_double_fp_backend::my_value_nan());
         }
         else
         {
            return operator=(cpp_double_fp_backend(0));
         }
      }

      // Handle more special cases like zero, inf and NaN.
      if (iszero_v)
      {
         const bool b_neg = isneg();

         *this = cpp_double_fp_backend::my_value_inf();

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
            return operator=(cpp_double_fp_backend::my_value_nan());
         }
         else
         {
            const bool b_neg { isneg() };

            return operator=((!b_neg) ? cpp_double_fp_backend::my_value_inf() : -cpp_double_fp_backend::my_value_inf());
         }
      }

      if (isinf_v)
      {
         return operator=(cpp_double_fp_backend(0));
      }

      // The division algorithm has been taken from Victor Shoup,
      // package WinNTL-5_3_2. It might originally be related to the
      // K. Briggs work. The algorithm has been significantly simplified
      // while still atempting to retain proper rounding corrections.

      const float_type C { data.first / v.data.first };

      float_type c { cpp_df_qf_detail::split(float_type()) * C };

      float_type u { cpp_df_qf_detail::split(float_type()) * v.data.first };

      if (cpp_df_qf_detail::ccmath::isinf(u) || cpp_df_qf_detail::ccmath::isinf(c))
      {
         // Evidently we have some very large operands. Let's take a last chance
         // for finite division. Use the ratio of square roots and subsequently
         // square the ratio, handling the sign of the result separately.

         const bool u_neg {   isneg() };
         const bool v_neg { v.isneg() };
         const bool b_neg { u_neg != v_neg };

         cpp_double_fp_backend uu { *this };
         cpp_double_fp_backend vv { v };

         cpp_double_fp_backend sqrt_u { };
         cpp_double_fp_backend sqrt_v { };

         if(u_neg) { uu.negate(); }
         if(v_neg) { vv.negate(); }

         eval_sqrt(sqrt_u, uu);
         eval_sqrt(sqrt_v, vv);

         cpp_double_fp_backend sqrt_ratio { sqrt_u / sqrt_v };

         *this = sqrt_ratio;

         eval_multiply(*this, sqrt_ratio);

         if (b_neg)
            negate();
         return *this;
      }

      const float_type hc { c - float_type { c - C } };

      const float_type hv { u - float_type { u - v.data.first } };

      {
         const float_type U { C * v.data.first };

         u = cpp_df_qf_detail::ccmath::fma(hc, hv, -U);
         c = data.first - U;
      }

      const float_type tv { v.data.first - hv };

      u = cpp_df_qf_detail::ccmath::fma(hc, tv, u);

      {
         const float_type tc { C - hc };

         u = cpp_df_qf_detail::ccmath::fma(tc, hv, u);
         u = cpp_df_qf_detail::ccmath::fma(tc, tv, u);
      }

      c  = float_type { c - u } + data.second;

      c = (c - float_type { C * v.data.second }) / v.data.first;

      // Perform even more simplifications compared to Victor Shoup.
      data.first  = C + c;
      data.second = float_type { C - data.first } + c;

      return *this;
   }

   constexpr cpp_double_fp_backend operator++(int)
   {
      cpp_double_fp_backend t(*this);
      ++*this;
      return t;
   }

   constexpr cpp_double_fp_backend operator--(int)
   {
      cpp_double_fp_backend t(*this);
      --*this;
      return t;
   }

   constexpr cpp_double_fp_backend& operator++() { return *this += cpp_double_fp_backend<float_type>(float_type(1.0F)); }
   constexpr cpp_double_fp_backend& operator--() { return *this -= cpp_double_fp_backend<float_type>(float_type(1.0F)); }

   constexpr cpp_double_fp_backend operator-() const
   {
      cpp_double_fp_backend v(*this);
      v.negate();
      return v;
   }

   // Helper functions
   constexpr static cpp_double_fp_backend pown(const cpp_double_fp_backend& x, int p)
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
      else if (p == 4)
         { const local_float_type x2 { x * x }; result = x2 * x2; }
      else
      {
         result = local_float_type(1U);

         local_float_type y(x);

         auto p_local = static_cast<std::uint32_t>(p);

         for (;;)
         {
            if (static_cast<std::uint_fast8_t>(p_local & static_cast<std::uint32_t>(UINT8_C(1))) != static_cast<std::uint_fast8_t>(UINT8_C(0)))
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

   constexpr void swap(cpp_double_fp_backend& other)
   {
      if (this != &other)
      {
         const rep_type tmp = data;

         data = other.data;

         other.data = tmp;
      }
   }

   constexpr void swap(cpp_double_fp_backend&& other) noexcept
   {
      const rep_type tmp = data;

      data = other.data;

      other.data = tmp;
   }

   constexpr int compare(const cpp_double_fp_backend& other) const
   {
      // Return 1 for *this > other, -1 for *this < other, 0 for *this = other.
      return (my_first() > other.my_first()) ?  1 : (my_first()  < other.my_first())
                                             ? -1 : (my_second() > other.my_second())
                                             ?  1 : (my_second() < other.my_second())
                                             ? -1 : 0;
   }

   std::string str(std::streamsize number_of_digits, const std::ios::fmtflags format_flags) const
   {
      if (number_of_digits == 0)
         number_of_digits = cpp_double_fp_backend::my_digits10;

      // Use cpp_dec_float to write string (as is similarly done to read string).

      cpp_dec_float_read_write_type f_dec { 0 };

      f_dec  = data.first;
      f_dec += data.second;

      return f_dec.str(number_of_digits, format_flags);
   }

   constexpr int order02() const
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

   constexpr int order10() const { return static_cast<int>(static_cast<float>(order02()) * 0.301F); }

   constexpr bool small_arg() const { return (order10() < static_cast<int>(-my_digits10 / 6)); }

   constexpr bool near_one() const
   {
      cpp_double_fp_backend delta_one { };

      eval_subtract(delta_one, cpp_double_fp_backend(1U), *this);

      if (delta_one().isneg())
      {
         delta_one.negate();
      }

      return delta_one.small_arg();
   }

   static constexpr cpp_double_fp_backend my_value_max() noexcept
   {
      // TBD: Can this be simplified in any way?
      // Is the use of the square root the best way to go?
      // Can this be made more friendly to C++14 constexpr?

      return
         cpp_double_fp_backend
         (
            arithmetic::fast_sum
            (
               static_cast<float_type>
               (
                    (cpp_df_qf_detail::ccmath::numeric_limits<float_type>::max)()
                  * (
                         static_cast<float_type>(1.0F)
                       - static_cast<float_type>(1.5F) * cpp_df_qf_detail::ccmath::sqrt(cpp_df_qf_detail::ccmath::numeric_limits<float_type>::epsilon())
                    )
               ),
               cpp_df_qf_detail::ccmath::ldexp
               (
                   (cpp_df_qf_detail::ccmath::numeric_limits<float_type>::max)(),
                  -(cpp_df_qf_detail::ccmath::numeric_limits<float_type>::digits + 1)
               )
            )
         );
   }

   static constexpr cpp_double_fp_backend my_value_min() noexcept
   {
      return
         cpp_double_fp_backend
         (
            cpp_df_qf_detail::ccmath::ldexp
            (
               (cpp_df_qf_detail::ccmath::numeric_limits<float_type>::min)(),
                cpp_df_qf_detail::ccmath::numeric_limits<float_type>::digits
            )
         );
   }

   static constexpr cpp_double_fp_backend my_value_eps() noexcept
   {
      // TBD: Do we need a better value here.

      // Or should we tune the value of epsilon() depending
      // on the width of the underlying floating-point constituent.

      cpp_double_fp_backend result { };

      eval_ldexp(result, cpp_double_fp_backend(1), 4 - my_digits);

      return result;
   }

   static constexpr cpp_double_fp_backend my_value_nan() noexcept
   {
      return cpp_double_fp_backend(static_cast<float_type>(NAN));
   }

   static constexpr cpp_double_fp_backend my_value_inf() noexcept
   {
      return cpp_double_fp_backend(static_cast<float_type>(HUGE_VAL)); // conversion from double infinity OK
   }

 private:
   rep_type data;

   static constexpr auto cpp_dec_float_read_write_digits10 =
      static_cast<int>
      (
         static_cast<float>
         (
            static_cast<float>(my_digits) * 0.9F
         )
      );

   using cpp_dec_float_read_write_type     = boost::multiprecision::cpp_dec_float<static_cast<unsigned>(cpp_dec_float_read_write_digits10), std::int32_t, std::allocator<void>>;
   using cpp_dec_float_read_write_exp_type = typename cpp_dec_float_read_write_type::exponent_type;

   bool rd_string(const char* pstr);

   static constexpr rep_type two_sum(const float_type a, const float_type b)
   {
     const float_type hi { a + b };
     const float_type a1 { hi - b };
     const float_type b1 { hi - a1 };

     return { hi, float_type { (a - a1) + (b - b1) } };
   }

   static constexpr rep_type two_diff(const float_type a, const float_type b)
   {
     const float_type hi { a - b };
     const float_type a1 { hi + b };
     const float_type b1 { hi - a1 };

     return { hi, float_type { (a - a1) - (b + b1) } };
   }

   static constexpr rep_type two_hilo_sum(const float_type a, const float_type b)
   {
      const float_type hi { a + b };

      return { hi, float_type { b - (hi - a) } };
   }

   template <typename OtherFloatingPointType,
             typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<OtherFloatingPointType>::value && ((cpp_df_qf_detail::ccmath::numeric_limits<OtherFloatingPointType>::digits10 * 2) < 16))>::type const*>
   friend constexpr void eval_exp(cpp_double_fp_backend<OtherFloatingPointType>& result, const cpp_double_fp_backend<OtherFloatingPointType>& x);

   template <typename OtherFloatingPointType,
             typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<OtherFloatingPointType>::value && (((cpp_df_qf_detail::ccmath::numeric_limits<OtherFloatingPointType>::digits10 * 2) >= 16) && ((cpp_df_qf_detail::ccmath::numeric_limits<OtherFloatingPointType>::digits10 * 2) <= 36)))>::type const*>
   friend constexpr void eval_exp(cpp_double_fp_backend<OtherFloatingPointType>& result, const cpp_double_fp_backend<OtherFloatingPointType>& x);

   template <typename OtherFloatingPointType,
             typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<OtherFloatingPointType>::value && ((cpp_df_qf_detail::ccmath::numeric_limits<OtherFloatingPointType>::digits10 * 2) > 36))>::type const*>
   friend constexpr void eval_exp(cpp_double_fp_backend<OtherFloatingPointType>& result, const cpp_double_fp_backend<OtherFloatingPointType>& x);
};

template <typename FloatingPointType>
bool cpp_double_fp_backend<FloatingPointType>::rd_string(const char* pstr)
{
   // TBD: Do we need-to / want-to throw-catch on invalid string input?

   using local_double_fp_type = cpp_double_fp_backend<FloatingPointType>;

   cpp_dec_float_read_write_type f_dec = pstr;

   const auto fpc = eval_fpclassify(f_dec);

   const auto is_definitely_nan = (fpc == FP_NAN);

   if (is_definitely_nan)
   {
      static_cast<void>(operator=(local_double_fp_type::my_value_nan()));
   }
   else
   {
      cpp_dec_float_read_write_type dummy_frexp { };
      auto e2_from_f_dec = int { };
      eval_frexp(dummy_frexp, f_dec, &e2_from_f_dec);

      const auto is_definitely_zero =
      (
         // TBD: A detailed, clear comparison (or something close to a comparison)
         // is still needed for input values having (e2_from_f_dec == my_min_exponent).
            (fpc == FP_ZERO)
         || (e2_from_f_dec < static_cast<cpp_dec_float_read_write_exp_type>(local_double_fp_type::my_min_exponent))
      );

      if (is_definitely_zero)
      {
         data.first  = float_type { 0.0F };
         data.second = float_type { 0.0F };
      }
      else
      {
         // TBD: A detailed, clear comparison (or something close to a comparison)
         // is still needed for input values having (e2_from_f_dec == my_max_exponent).
         const auto is_definitely_inf = (e2_from_f_dec > static_cast<cpp_dec_float_read_write_exp_type>(local_double_fp_type::my_max_exponent));

         if (is_definitely_inf)
         {
            const bool b_neg { f_dec.isneg() };

            static_cast<void>(operator=(local_double_fp_type::my_value_inf()));

            if (b_neg) { negate(); }
         }
         else
         {
            data.first  = float_type { 0.0F };
            data.second = float_type { 0.0F };

            using local_builtin_float_type = double;

            constexpr auto dig_lim =
               static_cast<unsigned>
               (
                  static_cast<int>
                  (
                         (local_double_fp_type::my_digits / cpp_df_qf_detail::ccmath::numeric_limits<local_builtin_float_type>::digits)
                     + (((local_double_fp_type::my_digits % cpp_df_qf_detail::ccmath::numeric_limits<local_builtin_float_type>::digits) != 0) ? 1 : 0)
                  )
                  * cpp_df_qf_detail::ccmath::numeric_limits<local_builtin_float_type>::digits
               );


            using local_double_fp_constituent_type = typename local_double_fp_type::float_type;

            constexpr auto pow2_scaling_for_small_input = cpp_df_qf_detail::ccmath::numeric_limits<local_double_fp_constituent_type>::digits;

            const auto has_pow2_scaling_for_small_input =
            (
               e2_from_f_dec < static_cast<int>(local_double_fp_type::my_min_exponent + pow2_scaling_for_small_input)
            );

            if (has_pow2_scaling_for_small_input)
            {
               f_dec *= cpp_dec_float_read_write_type::pow2(static_cast<long long>(pow2_scaling_for_small_input));
            }

            for(auto i = static_cast<unsigned>(UINT8_C(0));
                     i < dig_lim;
                     i = static_cast<unsigned>(i + static_cast<unsigned>(cpp_df_qf_detail::ccmath::numeric_limits<local_builtin_float_type>::digits)))
            {
               cpp_dec_float_read_write_type f_dec_abs { };

               eval_fabs(f_dec_abs, f_dec);

               if (f_dec_abs.compare((cpp_dec_float_read_write_type::min)()) <= 0)
               {
                  break;
               }

               auto f = local_builtin_float_type { };

               eval_convert_to(&f, f_dec);

               f_dec -= f;

               eval_add(*this, local_double_fp_type(f));
            }

            if (has_pow2_scaling_for_small_input)
            {
               eval_ldexp(*this, local_double_fp_type(*this), static_cast<int>(-pow2_scaling_for_small_input));
            }
         }
      }
   }

   return true;
}

namespace cpp_df_qf_detail {

   // N[Pi, 101]
   // 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170680

   // 3.14159250259,                            1.50995788317e-07
   // 3.141592653589793116,                     1.2246467991473529607e-16
   // 3.14159265358979323851281,                -5.01655761266833202345176e-20
   // 3.14159265358979323846264338327950279748, 8.67181013012378102479704402604335225411e-35

   template <typename FloatingPointType> constexpr auto constant_df_pi() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits ==  24)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(3.14159250259L),                            static_cast<FloatingPointType>(1.50995788317e-07L)); }
   template <typename FloatingPointType> constexpr auto constant_df_pi() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits ==  53)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(3.141592653589793116L),                     static_cast<FloatingPointType>(1.2246467991473529607e-16L)); }
   template <typename FloatingPointType> constexpr auto constant_df_pi() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits ==  64)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(3.14159265358979323851281L),                static_cast<FloatingPointType>(-5.01655761266833202345176e-20L)); }
   #if defined(BOOST_HAS_FLOAT128)

   #if defined(__STRICT_ANSI__)
   template <typename FloatingPointType> constexpr auto constant_df_pi() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits == 113)), cpp_double_fp_backend<FloatingPointType>>::type
   {
     return   cpp_double_fp_backend<FloatingPointType>(3.141592653589793116)
            + cpp_double_fp_backend<FloatingPointType>(1.2246467991473532072e-16)
            + cpp_double_fp_backend<FloatingPointType>(-2.9947698097183396659e-33)
            + cpp_double_fp_backend<FloatingPointType>(1.1124542208633652815e-49)
            + cpp_double_fp_backend<FloatingPointType>(5.6750854105517547202e-66)
            ;
   }
   #else
   template <typename FloatingPointType> constexpr auto constant_df_pi() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits == 113)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(3.14159265358979323846264338327950279748Q), static_cast<FloatingPointType>(8.67181013012378102479704402604335225411e-35Q)); }
   #endif
   #endif

   // N[Log[2], 101]
   // 0.69314718055994530941723212145817656807550013436025525412068000949339362196969471560586332699641868754

   // 0.69314712286,                             5.76999887869e-08
   // 0.6931471805599451752,                     1.3421277060097865271e-16
   // 0.69314718055994530942869,                 -1.14583527267987328094768e-20
   // 0.693147180559945309417232121458176575084, -7.00813947454958516341266200877162272784e-36

   template <typename FloatingPointType> constexpr auto constant_df_ln_two() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits ==  24)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(0.69314712286L),                             static_cast<FloatingPointType>(5.76999887869e-08L)); }
   template <typename FloatingPointType> constexpr auto constant_df_ln_two() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits ==  53)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(0.6931471805599451752L),                     static_cast<FloatingPointType>(1.3421277060097865271e-16L)); }
   template <typename FloatingPointType> constexpr auto constant_df_ln_two() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits ==  64)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(0.69314718055994530942869L),                 static_cast<FloatingPointType>(-1.14583527267987328094768e-20L)); }
   #if defined(BOOST_HAS_FLOAT128)
   #if defined(__STRICT_ANSI__)
   template <typename FloatingPointType> constexpr auto constant_df_ln_two() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits == 113)), cpp_double_fp_backend<FloatingPointType>>::type
   {
      return   cpp_double_fp_backend<FloatingPointType>(0.69314718055994528623)
             + cpp_double_fp_backend<FloatingPointType>(2.3190468138462995584e-17)
             + cpp_double_fp_backend<FloatingPointType>(5.7077084384162120658e-34)
             + cpp_double_fp_backend<FloatingPointType>(-3.5824322106018114234e-50)
             + cpp_double_fp_backend<FloatingPointType>(-1.3492237373125495291e-66)
             ;
   }
   #else
   template <typename FloatingPointType> constexpr auto constant_df_ln_two() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits == 113)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(0.693147180559945309417232121458176575084Q), static_cast<FloatingPointType>(-7.00813947454958516341266200877162272784e-36Q)); }
   #endif
   #endif

   // N[Exp[1], 101]
   // 2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274

   // 2.71828174591,                            8.25483965627e-08
   // 2.7182818284590450908,                    1.4456468917292501578e-16
   // 2.71828182845904523521133,                1.4895979785582304563159e-19
   // 2.71828182845904523536028747135266231436, 1.83398825226506410712297736767396397644e-34

   template <typename FloatingPointType> constexpr auto constant_df_exp1() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits ==  24)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(2.71828174591L),                            static_cast<FloatingPointType>(8.25483965627e-08L)); }
   template <typename FloatingPointType> constexpr auto constant_df_exp1() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits ==  53)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(2.7182818284590450908L),                    static_cast<FloatingPointType>(1.4456468917292501578e-16L)); }
   template <typename FloatingPointType> constexpr auto constant_df_exp1() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits ==  64)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(2.71828182845904523521133L),                static_cast<FloatingPointType>(1.4895979785582304563159e-19L)); }
   #if defined(BOOST_HAS_FLOAT128)
   #if defined(__STRICT_ANSI__)
   template <typename FloatingPointType> constexpr auto constant_df_exp1() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits == 113)), cpp_double_fp_backend<FloatingPointType>>::type
   {
      return   cpp_double_fp_backend<FloatingPointType>(2.7182818284590450908)
             + cpp_double_fp_backend<FloatingPointType>(1.4456468917292501578e-16)
             + cpp_double_fp_backend<FloatingPointType>(-2.1277171080381767645e-33)
             + cpp_double_fp_backend<FloatingPointType>(1.5156301598412191438e-49)
             + cpp_double_fp_backend<FloatingPointType>(-9.347199499732301892e-66)
             ;
   }
   #else
   template <typename FloatingPointType> constexpr auto constant_df_exp1() -> typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits == 113)), cpp_double_fp_backend<FloatingPointType>>::type { return cpp_double_fp_backend<FloatingPointType>(static_cast<FloatingPointType>(2.71828182845904523536028747135266231436Q), static_cast<FloatingPointType>(1.83398825226506410712297736767396397644e-34Q)); }
   #endif
   #endif

} // namespace cpp_df_qf_detail

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
::std::basic_ostream<char_type, traits_type>&
operator<<(::std::basic_ostream<char_type, traits_type>& os, const cpp_double_fp_backend<FloatingPointType>& f)
{
   const auto str_result = f.str(os.precision(), os.flags());

   return (os << str_result);
}

template <typename FloatingPointType, typename char_type, typename traits_type>
::std::basic_istream<char_type, traits_type>&
operator>>(::std::basic_istream<char_type, traits_type>& is, cpp_double_fp_backend<FloatingPointType>& f)
{
   std::string input_str;

   is >> input_str;

   f = input_str.c_str();

   return is;
}

template <typename FloatingPointType>
constexpr void eval_add(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x) { result += x; }
template <typename FloatingPointType>
constexpr void eval_add(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { result = cpp_double_fp_backend<FloatingPointType>(a) += b; }
template <typename FloatingPointType>
constexpr void eval_subtract(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x) { result -= x; }
template <typename FloatingPointType>
constexpr void eval_subtract(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { result = cpp_double_fp_backend<FloatingPointType>(a) -= b; }
template <typename FloatingPointType>
constexpr void eval_multiply(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x) { result *= x; }
template <typename FloatingPointType>
constexpr void eval_multiply(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { result = cpp_double_fp_backend<FloatingPointType>(a) *= b; }
template <typename FloatingPointType>
constexpr void eval_divide(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x) { result /= x; }
template <typename FloatingPointType>
constexpr void eval_divide(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { result = cpp_double_fp_backend<FloatingPointType>(a) /= b; }
template <typename FloatingPointType>
constexpr bool eval_eq(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { return (a.compare(b) == 0); }
template <typename FloatingPointType>
constexpr bool eval_lt(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { return (a.compare(b) == -1); }
template <typename FloatingPointType>
constexpr bool eval_gt(const cpp_double_fp_backend<FloatingPointType>& a, const cpp_double_fp_backend<FloatingPointType>& b) { return (a.compare(b) == 1); }
template <typename FloatingPointType>

constexpr bool eval_is_zero(const cpp_double_fp_backend<FloatingPointType>& x)
{
   return x.is_zero();
}

template <typename FloatingPointType>
constexpr int eval_signbit(const cpp_double_fp_backend<FloatingPointType>& x)
{
   return (x.isneg() ? 1 : 0);
}

template <typename FloatingPointType>
constexpr void eval_fabs(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a)
{
   result = a;

   if (a.is_neg())
   {
      result.negate();
   }
}

template <typename FloatingPointType>
constexpr void eval_frexp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, int* v)
{
   result.rep().first  = cpp_df_qf_detail::ccmath::frexp(a.rep().first, v);
   result.rep().second = cpp_df_qf_detail::ccmath::ldexp(a.rep().second, -*v);
}

template <typename FloatingPointType>
constexpr void eval_ldexp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& a, int v)
{
   result.rep().first  = cpp_df_qf_detail::ccmath::ldexp(a.crep().first,  v);
   result.rep().second = cpp_df_qf_detail::ccmath::ldexp(a.crep().second, v);

   using local_backend_type = cpp_double_fp_backend<FloatingPointType>;

   local_backend_type::arithmetic::normalize(result.rep(), result.rep().first, result.rep().second);
}

template <typename FloatingPointType>
constexpr void eval_floor(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x)
{
   using double_float_type = cpp_double_fp_backend<FloatingPointType>;

   const typename double_float_type::float_type fhi = cpp_df_qf_detail::floor_of_constituent(x.rep().first);

   if (fhi != x.my_first())
   {
      result.rep().first  = fhi;
      result.rep().second = static_cast<typename double_float_type::float_type>(0.0F);
   }
   else
   {
      result.rep().first  = fhi;
      result.rep().second = cpp_df_qf_detail::floor_of_constituent(x.rep().second);

      double_float_type::arithmetic::normalize(result.rep(), result.rep().first, result.rep().second);
   }
}

template <typename FloatingPointType>
constexpr void eval_ceil(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x)
{
   // Compute -floor(-x);
   eval_floor(result, -x);

   result.negate();
}

template <typename FloatingPointType>
constexpr int eval_fpclassify(const cpp_double_fp_backend<FloatingPointType>& o)
{
   // The eval_fpclassify implementation is slightly modelled
   // after Matt Borland's constexpr-focussed work in <ccmath.hpp>.

   return  cpp_df_qf_detail::ccmath::isnan(o.crep().first) ? FP_NAN       :
           cpp_df_qf_detail::ccmath::isinf(o.crep().first) ? FP_INFINITE  :
           eval_is_zero(o)                                 ? FP_ZERO      :
          (   (cpp_df_qf_detail::ccmath::fabs(o.crep().first) > 0)
           && (cpp_df_qf_detail::ccmath::fabs(o.crep().first) < (cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::min)()))
                                                           ? FP_SUBNORMAL : FP_NORMAL;
}

template <typename FloatingPointType>
constexpr void eval_sqrt(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& o)
{
   using double_float_type = cpp_double_fp_backend<FloatingPointType>;
   using local_float_type = typename double_float_type::float_type;

   const int fpc { eval_fpclassify(o) };

   const bool isneg_o { o.isneg() };

   if ((fpc != FP_NORMAL) || isneg_o)
   {
      if ((fpc == FP_ZERO) || (fpc == FP_SUBNORMAL))
      {
         result = double_float_type(0);
         return;
      }
      else if ((fpc == FP_NAN) || isneg_o)
      {
         result = double_float_type::my_value_nan();
         return;
      }
      else if (fpc == FP_INFINITE)
      {
         result = double_float_type::my_value_inf();
         return;
      }
   }

   const local_float_type c = cpp_df_qf_detail::ccmath::sqrt(o.crep().first);

   constexpr auto MantissaBits = cpp_df_qf_detail::ccmath::numeric_limits<local_float_type>::digits;
   constexpr auto SplitBits    = static_cast<int>((MantissaBits / 2) + 1);
   constexpr auto Splitter     = static_cast<local_float_type>((1ULL << SplitBits) + 1);

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
          typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && ((cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits10 * 2) < 16))>::type const*>
constexpr void eval_exp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x)
{
   const int fpc { eval_fpclassify(x) };

   const bool x_is_zero { fpc == FP_ZERO };

   using double_float_type = cpp_double_fp_backend<FloatingPointType>;

   if (fpc == FP_ZERO)
   {
      result = double_float_type(1);
   }
   else if (fpc != FP_NORMAL)
   {
      if (fpc == FP_INFINITE)
      {
         result = (x.isneg() ? double_float_type(0) : double_float_type::my_value_inf());
      }
      else if (fpc == FP_NAN)
      {
         result = x;
      }
   }
   else
   {
      using local_float_type = typename double_float_type::float_type;

      // Get a local copy of the argument and force it to be positive.
      const auto b_neg = x.is_neg();

      double_float_type xx { };

      eval_fabs(xx, x);

      // Check the range of the input.
      const double_float_type max_exp_input
      {
         []() -> local_float_type
         {
            local_float_type mx { };
            eval_convert_to(&mx, double_float_type::my_value_max());

            const local_float_type log_of_mx = cpp_df_qf_detail::log_of_constituent(mx);

            return log_of_mx;
         }()
      };

      const double_float_type min_exp_input
      {
         []() -> local_float_type
         {
            local_float_type mn { };
            eval_convert_to(&mn, double_float_type::my_value_min());

            const local_float_type log_of_mn = cpp_df_qf_detail::log_of_constituent(mn);

            return log_of_mn;
         }()
      };

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
         result = double_float_type::my_value_inf();
      }
      else if (xx.is_one())
      {
         result =
            ((!b_neg)
               ?                         cpp_df_qf_detail::constant_df_exp1<local_float_type>()
               : double_float_type(1U) / cpp_df_qf_detail::constant_df_exp1<local_float_type>());
      }
      else
      {
         // Use an argument reduction algorithm for exp() in classic MPFUN-style.

         double_float_type nf { };

         eval_floor(nf, xx / cpp_df_qf_detail::constant_df_ln_two<local_float_type>());

         // Prepare the scaled variables.
         const auto b_scale = (xx.order02() > -1);

         double_float_type r { };

         if (b_scale)
         {
            eval_ldexp(r, xx - (nf * cpp_df_qf_detail::constant_df_ln_two<local_float_type>()), -2);
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

            const int n { static_cast<int>(lln) };

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
          typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && (((cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits10 * 2) >= 16) && ((cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits10 * 2) <= 36)))>::type const*>
constexpr void eval_exp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x)
{
   const int fpc { eval_fpclassify(x) };

   const bool x_is_zero { fpc == FP_ZERO };

   using double_float_type = cpp_double_fp_backend<FloatingPointType>;

   if (fpc == FP_ZERO)
   {
      result = double_float_type(1);
   }
   else if (fpc != FP_NORMAL)
   {
      if (fpc == FP_INFINITE)
      {
         result = (x.isneg() ? double_float_type(0) : double_float_type::my_value_inf());
      }
      else if (fpc == FP_NAN)
      {
         result = x;
      }
   }
   else
   {
      using local_float_type  = typename double_float_type::float_type;

      // Get a local copy of the argument and force it to be positive.
      const auto b_neg = x.is_neg();

      double_float_type xx { };

      eval_fabs(xx, x);

      // Check the range of the input.
      const double_float_type max_exp_input
      {
         []() -> local_float_type
         {
            local_float_type mx { };
            eval_convert_to(&mx, double_float_type::my_value_max());

            const local_float_type log_of_mx = cpp_df_qf_detail::log_of_constituent(mx);

            return log_of_mx;
         }()
      };

      const double_float_type min_exp_input
      {
         []() -> local_float_type
         {
            local_float_type mn { };
            eval_convert_to(&mn, double_float_type::my_value_min());

            const local_float_type log_of_mn = cpp_df_qf_detail::log_of_constituent(mn);

            return log_of_mn;
         }()
      };

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
         result = double_float_type::my_value_inf();
      }
      else if (xx.is_one())
      {
         result =
            ((!b_neg)
               ?                         cpp_df_qf_detail::constant_df_exp1<local_float_type>()
               : double_float_type(1U) / cpp_df_qf_detail::constant_df_exp1<local_float_type>());
      }
      else
      {
         // Use an argument reduction algorithm for exp() in classic MPFUN-style.

         double_float_type nf { };

         eval_floor(nf, xx / cpp_df_qf_detail::constant_df_ln_two<local_float_type>());

         // Prepare the scaled variables.
         const auto b_scale = (xx.order02() > -4);

         double_float_type r { };

         if (b_scale)
         {
            eval_ldexp(r, xx - (nf * cpp_df_qf_detail::constant_df_ln_two<local_float_type>()), -4);
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

            const int n { static_cast<int>(lln) };

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
          typename ::std::enable_if<(cpp_df_qf_detail::is_floating_point_or_float128<FloatingPointType>::value && ((cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits10 * 2) > 36))>::type const*>
constexpr void eval_exp(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x)
{
   const int fpc { eval_fpclassify(x) };

   const bool x_is_zero { fpc == FP_ZERO };

   using double_float_type = cpp_double_fp_backend<FloatingPointType>;

   if (fpc == FP_ZERO)
   {
      result = double_float_type(1);
   }
   else if (fpc != FP_NORMAL)
   {
      if (fpc == FP_INFINITE)
      {
         result = (x.isneg() ? double_float_type(0) : double_float_type::my_value_inf());
      }
      else if (fpc == FP_NAN)
      {
         result = x;
      }
   }
   else
   {
      using local_float_type = typename double_float_type::float_type;

      // Get a local copy of the argument and force it to be positive.
      const auto b_neg = x.is_neg();

      double_float_type xx { };

      eval_fabs(xx, x);

      // Check the range of the input.
      const double_float_type max_exp_input
      {
         []() -> local_float_type
         {
            local_float_type mx { };
            eval_convert_to(&mx, double_float_type::my_value_max());

            const local_float_type log_of_mx = cpp_df_qf_detail::log_of_constituent(mx);

            return log_of_mx;
         }()
      };

      const double_float_type min_exp_input
      {
         []() -> local_float_type
         {
            local_float_type mn { };
            eval_convert_to(&mn, double_float_type::my_value_min());

            const local_float_type log_of_mn = cpp_df_qf_detail::log_of_constituent(mn);

            return log_of_mn;
         }()
      };

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
         result = double_float_type::my_value_inf();
      }
      else if (xx.is_one())
      {
         result =
            ((!b_neg)
               ?                         cpp_df_qf_detail::constant_df_exp1<local_float_type>()
               : double_float_type(1U) / cpp_df_qf_detail::constant_df_exp1<local_float_type>());
      }
      else
      {
         // Use an argument reduction algorithm for exp() in classic MPFUN-style.

         double_float_type nf { };

         eval_floor(nf, xx / cpp_df_qf_detail::constant_df_ln_two<local_float_type>());

         // Prepare the scaled variables.
         const auto b_scale = (xx.order02() > -4);

         double_float_type xh { };

         if (b_scale)
         {
            eval_ldexp(xh, xx - (nf * cpp_df_qf_detail::constant_df_ln_two<local_float_type>()), -4);
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

            const int n { static_cast<int>(lln) };

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
constexpr void eval_log(cpp_double_fp_backend<FloatingPointType>& result, const cpp_double_fp_backend<FloatingPointType>& x)
{
   using double_float_type = cpp_double_fp_backend<FloatingPointType>;

   const auto fpc = eval_fpclassify(x);

   const auto x_is_zero = (fpc == FP_ZERO);

   if ((fpc != FP_NORMAL) && (!x_is_zero))
   {
      result = x;
   }
   else if (x.is_neg())
   {
      result = double_float_type::my_value_nan();
   }
   else if (x_is_zero)
   {
      result = double_float_type::my_value_inf();
   }
   else if (eval_lt(x, double_float_type(1)))
   {
      // TBD: Optimize check with 1.
      // TBD: Optimize inversion and negation.

      double_float_type x_inv;
      eval_divide(x_inv, double_float_type(1), x);
      eval_log(result, x_inv);
      result.negate();
   }
   else if (eval_gt(x, double_float_type(1)))
   {
      // TBD: Optimize check with 1.

      // TBD: Optimize to only use eval_frexp if (and only if) the exponent
      // is actually large/small in the sense of above/below a defined cutoff.

      double_float_type x2 { };

      int n2 { };

      eval_frexp(x2, x, &n2);

      // Get initial estimate using the (wrapped) standard math function log.
      const double_float_type s(cpp_df_qf_detail::log_of_constituent(x2.crep().first));

      double_float_type E { };

      eval_exp(E, s);

      // Do one single step of Newton-Raphson iteration.
      result = s + ((x2 - E) / E);

      double_float_type xn2 { n2 };

      eval_multiply(xn2, cpp_df_qf_detail::constant_df_ln_two<typename double_float_type::float_type>());

      eval_add(result, xn2);
   }
   else
   {
      result = 0;
   }
}

template <typename FloatingPointType>
constexpr void eval_convert_to(signed long long* result, const cpp_double_fp_backend<FloatingPointType>& backend)
{
   const auto fpc = eval_fpclassify(backend);

   if (fpc != FP_NORMAL)
   {
      *result = static_cast<signed long long>(backend.crep().first);

      return;
   }

   constexpr signed long long my_max_val = (std::numeric_limits<signed long long>::max)();
   constexpr signed long long my_min_val = (std::numeric_limits<signed long long>::min)();

   using c_type = typename std::common_type<signed long long, FloatingPointType>::type;

   constexpr c_type my_max = static_cast<c_type>(my_max_val);
   const     c_type ct     = cpp_df_qf_detail::ccmath::fabs(backend.crep().first);

   if (ct > my_max)
   {
      *result = backend.crep().first >= typename cpp_double_fp_backend<FloatingPointType>::float_type(0U)
         ? my_max_val
         : my_min_val;
   }
   else
   {
      BOOST_IF_CONSTEXPR(std::numeric_limits<signed long long>::digits >= cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits)
      {
         *result  = static_cast<signed long long>(backend.crep().first);
         *result += static_cast<signed long long>(backend.crep().second);
      }
      else
      {
         cpp_double_fp_backend<FloatingPointType> source = backend;

         *result = 0;

         for(auto digit_count  = 0;
                  digit_count  < cpp_double_fp_backend<FloatingPointType>::my_digits;
                  digit_count += std::numeric_limits<signed long long>::digits)
         {
            const auto next = static_cast<signed long long>(source.crep().first);

            *result += next;

            eval_subtract(source, cpp_double_fp_backend<FloatingPointType>(next));
         }
      }
   }
}

template <typename FloatingPointType>
constexpr void eval_convert_to(unsigned long long* result, const cpp_double_fp_backend<FloatingPointType>& backend)
{
   const auto fpc = eval_fpclassify(backend);

   if (fpc != FP_NORMAL)
   {
      *result = static_cast<unsigned long long>(backend.crep().first);

      return;
   }

   constexpr unsigned long long my_max_val = (std::numeric_limits<unsigned long long>::max)();

   using c_type = typename std::common_type<unsigned long long, FloatingPointType>::type;

   constexpr c_type my_max = static_cast<c_type>(my_max_val);
   const     c_type ct     = cpp_df_qf_detail::ccmath::fabs(backend.crep().first);

   if (ct > my_max)
   {
      *result = my_max_val;
   }
   else
   {
      BOOST_IF_CONSTEXPR(std::numeric_limits<unsigned long long>::digits >= cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits)
      {
         *result  = static_cast<unsigned long long>(backend.crep().first);
         *result += static_cast<unsigned long long>(backend.crep().second);
      }
      else
      {
         cpp_double_fp_backend<FloatingPointType> source = backend;

         *result = 0;

         for(auto digit_count  = 0;
                  digit_count  < cpp_double_fp_backend<FloatingPointType>::my_digits;
                  digit_count += std::numeric_limits<unsigned long long>::digits)
         {
            const auto next = static_cast<unsigned long long>(source.crep().first);

            *result += next;

            eval_subtract(source, cpp_double_fp_backend<FloatingPointType>(next));
         }
      }
   }
}

#ifdef BOOST_HAS_INT128
template <typename FloatingPointType>
constexpr void eval_convert_to(int128_type* result, const cpp_double_fp_backend<FloatingPointType>& backend)
{
   const auto fpc = eval_fpclassify(backend);

   if (fpc != FP_NORMAL)
   {
      *result = static_cast<int128_type>(backend.crep().first);

      return;
   }

   constexpr int128_type my_max_val = (((static_cast<int128_type>(1) << (sizeof(int128_type) * CHAR_BIT - 2)) - 1) << 1) + 1;
   constexpr int128_type my_min_val = static_cast<int128_type>(-my_max_val - 1);

   using c_type = typename std::common_type<int128_type, FloatingPointType>::type;

   constexpr c_type my_max = static_cast<c_type>(my_max_val);
   const     c_type ct     = cpp_df_qf_detail::ccmath::fabs(backend.crep().first);

   if (ct > my_max)
   {
      *result = backend.crep().first >= typename cpp_double_fp_backend<FloatingPointType>::float_type(0U)
         ? my_max_val
         : my_min_val;
   }
   else
   {
      BOOST_IF_CONSTEXPR(static_cast<int>(static_cast<int>(sizeof(int128_type)) * CHAR_BIT) >= cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits)
      {
         *result  = static_cast<int128_type>(backend.crep().first);
         *result += static_cast<int128_type>(backend.crep().second);
      }
      else
      {
         cpp_double_fp_backend<FloatingPointType> source = backend;

         *result = 0;

         for(auto digit_count  = static_cast<int>(0);
                  digit_count  < cpp_double_fp_backend<FloatingPointType>::my_digits;
                  digit_count += static_cast<int>(static_cast<int>(sizeof(int128_type)) * CHAR_BIT))
         {
            const auto next = static_cast<int128_type>(source.crep().first);

            *result += next;

            eval_subtract(source, cpp_double_fp_backend<FloatingPointType>(next));
         }
      }
   }
}

template <typename FloatingPointType>
constexpr void eval_convert_to(uint128_type* result, const cpp_double_fp_backend<FloatingPointType>& backend)
{
   const auto fpc = eval_fpclassify(backend);

   if (fpc != FP_NORMAL)
   {
      *result = static_cast<uint128_type>(backend.crep().first);

      return;
   }

   uint128_type my_max_val { };

   BOOST_IF_CONSTEXPR(std::is_same<FloatingPointType, float>::value && (std::numeric_limits<float>::digits == 24))
   {
      my_max_val = static_cast<uint128_type>(FLT_MAX);
   }
   else
   {
      my_max_val = static_cast<uint128_type>(~static_cast<uint128_type>(0));
   }

   using c_type = typename std::common_type<uint128_type, FloatingPointType>::type;

   const c_type my_max = static_cast<c_type>(my_max_val);
   const c_type ct     = cpp_df_qf_detail::ccmath::fabs(backend.crep().first);

   if (ct > my_max)
   {
      *result = my_max_val;
   }
   else
   {
      BOOST_IF_CONSTEXPR(static_cast<int>(static_cast<int>(sizeof(uint128_type)) * CHAR_BIT) >= cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits)
      {
         *result  = static_cast<int128_type>(backend.crep().first);
         *result += static_cast<int128_type>(backend.crep().second);
      }
      else
      {
         cpp_double_fp_backend<FloatingPointType> source = backend;

         *result = 0;

         for(auto digit_count  = static_cast<int>(0);
                  digit_count  < cpp_double_fp_backend<FloatingPointType>::my_digits;
                  digit_count += static_cast<int>(static_cast<int>(sizeof(uint128_type)) * CHAR_BIT))
         {
            const auto next = static_cast<uint128_type>(source.crep().first);

            *result += next;

            eval_subtract(source, cpp_double_fp_backend<FloatingPointType>(next));
         }
      }
   }
}
#endif

template <typename FloatingPointType,
          typename OtherFloatingPointType>
constexpr typename ::std::enable_if<cpp_df_qf_detail::is_floating_point_or_float128<OtherFloatingPointType>::value>::type eval_convert_to(OtherFloatingPointType* result, const cpp_double_fp_backend<FloatingPointType>& backend)
{
   const auto fpc = eval_fpclassify(backend);

   // TBD: Implement min/max chek for the destination floating-point type result.

   if (fpc != FP_NORMAL)
   {
      *result = static_cast<OtherFloatingPointType>(backend.crep().first);

      return;
   }
   else
   {
      BOOST_IF_CONSTEXPR(cpp_df_qf_detail::ccmath::numeric_limits<OtherFloatingPointType>::digits >= cpp_df_qf_detail::ccmath::numeric_limits<FloatingPointType>::digits)
      {
         *result  = static_cast<OtherFloatingPointType>(backend.crep().first);
         *result += static_cast<OtherFloatingPointType>(backend.crep().second);
      }
      else
      {
         cpp_double_fp_backend<FloatingPointType> source = backend;

         *result = 0;

         for(auto digit_count  = static_cast<int>(0);
                  digit_count  < cpp_double_fp_backend<FloatingPointType>::my_digits;
                  digit_count += cpp_df_qf_detail::ccmath::numeric_limits<OtherFloatingPointType>::digits)
         {
            const auto next = static_cast<OtherFloatingPointType>(source.crep().first);

            *result += next;

            eval_subtract(source, cpp_double_fp_backend<FloatingPointType>(next));
         }
      }
   }
}

template <typename FloatingPointType>
std::size_t hash_value(const cpp_double_fp_backend<FloatingPointType>& a)
{
   return a.hash();
}

} // namespace backends

using backends::cpp_double_fp_backend;

using cpp_double_float       = number<cpp_double_fp_backend<float>,                  boost::multiprecision::et_off>;
using cpp_double_double      = number<cpp_double_fp_backend<double>,                 boost::multiprecision::et_off>;
using cpp_double_long_double = number<cpp_double_fp_backend<long double>,            boost::multiprecision::et_off>;
#ifdef BOOST_HAS_FLOAT128
using cpp_double_float128    = number<cpp_double_fp_backend<::boost::float128_type>, boost::multiprecision::et_off>;
#endif

} } // namespace boost::multiprecision

namespace std {

// Specialization of numeric_limits for boost::multiprecision::number<cpp_double_fp_backend<>>
template <typename FloatingPointType,
          const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
BOOST_MP_DF_QF_NUM_LIMITS_CLASS_TYPE numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >
{
 private:
   using local_float_type = FloatingPointType;
   using inner_self_type  = boost::multiprecision::cpp_double_fp_backend<local_float_type>;

   using self_type = boost::multiprecision::number<inner_self_type, ExpressionTemplatesOption>;

 public:
   static constexpr bool                    is_specialized                = true;
   static constexpr bool                    is_signed                     = true;
   static constexpr bool                    is_integer                    = false;
   static constexpr bool                    is_exact                      = false;
   static constexpr bool                    is_bounded                    = true;
   static constexpr bool                    is_modulo                     = false;
   static constexpr bool                    is_iec559                     = false;
   static constexpr float_denorm_style      has_denorm                    = denorm_absent;
   static constexpr bool                    has_infinity                  = true;
   static constexpr bool                    has_quiet_NaN                 = true;
   static constexpr bool                    has_signaling_NaN             = false;
   static constexpr bool                    has_denorm_loss               = true;
   static constexpr bool                    traps                         = false;
   static constexpr bool                    tinyness_before               = false;
   static constexpr float_round_style       round_style                   = round_to_nearest;

   static constexpr int radix                          = 2;
   static constexpr int digits                         = inner_self_type::my_digits;
   static constexpr int digits10                       = inner_self_type::my_digits10;
   static constexpr int max_digits10                   = inner_self_type::my_max_digits10;

   static constexpr int max_exponent                   = inner_self_type::my_max_exponent;
   static constexpr int min_exponent                   = inner_self_type::my_min_exponent;
   static constexpr int max_exponent10                 = inner_self_type::my_max_exponent10;
   static constexpr int min_exponent10                 = inner_self_type::my_min_exponent10;

   static constexpr self_type(min)         () noexcept { return static_cast<self_type>(inner_self_type::my_value_min()); }
   static constexpr self_type(max)         () noexcept { return static_cast<self_type>(inner_self_type::my_value_max()); }
   static constexpr self_type lowest       () noexcept { return static_cast<self_type>(-(max)()); }
   static constexpr self_type epsilon      () noexcept { return static_cast<self_type>(inner_self_type::my_value_eps()); }
   static constexpr self_type round_error  () noexcept { return static_cast<self_type>(static_cast<local_float_type>(0.5)); }
   static constexpr self_type denorm_min   () noexcept { return static_cast<self_type>((min)()); }
   static constexpr self_type infinity     () noexcept { return static_cast<self_type>(inner_self_type::my_value_inf()); }
   static constexpr self_type quiet_NaN    () noexcept { return static_cast<self_type>(inner_self_type::my_value_nan()); }
   static constexpr self_type signaling_NaN() noexcept { return static_cast<self_type>(static_cast<local_float_type>(0.0)); }
};

} // namespace std

template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_specialized;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_signed;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_integer;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_exact;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_bounded;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_modulo;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::is_iec559;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr std::float_denorm_style std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::has_denorm;


template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::has_infinity;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::has_quiet_NaN;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::has_signaling_NaN;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::has_denorm_loss;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::traps;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr bool                    std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::tinyness_before;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr std::float_round_style  std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::round_style;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int                     std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::radix;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int                     std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::digits;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int                     std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::digits10;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int                     std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::max_digits10;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int                     std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::max_exponent;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int                     std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::min_exponent;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int                     std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::max_exponent10;
template <typename FloatingPointType, const boost::multiprecision::expression_template_option ExpressionTemplatesOption>
constexpr int                     std::numeric_limits<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplatesOption> >::min_exponent10;

#if defined(BOOST_MP_MATH_AVAILABLE)
namespace boost { namespace math { namespace policies {

template <class FloatingPointType, class Policy, boost::multiprecision::expression_template_option ExpressionTemplates>
struct precision<boost::multiprecision::number<boost::multiprecision::cpp_double_fp_backend<FloatingPointType>, ExpressionTemplates>, Policy>
{
private:
   using my_multiprecision_backend_type = boost::multiprecision::cpp_double_fp_backend<FloatingPointType>;

   using digits_2 = digits2<my_multiprecision_backend_type::my_digits>;

public:
   using precision_type = typename Policy::precision_type;

   using type =
      typename std::conditional<
         ((digits_2::value <= precision_type::value) || (precision_type::value <= 0)),
         digits_2,                  // Default case: Full precision for RealType.
         precision_type>::type;     // User customized precision.
};

} } } // namespace boost::math::policies
#endif

#endif // BOOST_MP_CPP_DOUBLE_FP_2021_06_05_HPP
