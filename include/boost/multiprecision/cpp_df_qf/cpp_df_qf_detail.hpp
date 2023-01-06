///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 - 2023 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DF_QF_DETAIL_2023_01_02_HPP
#define BOOST_MP_CPP_DF_QF_DETAIL_2023_01_02_HPP

#include <cmath>
#include <limits>
#include <tuple>
#include <utility>

#include <boost/config.hpp>
#include <boost/multiprecision/number.hpp>

#ifdef BOOST_HAS_FLOAT128
#include <quadmath.h>
#endif

namespace boost { namespace multiprecision { namespace backends { namespace cpp_df_qf_detail {

template <class FloatingPointType>
constexpr FloatingPointType fabs_of_constituent(FloatingPointType x)
{
   return (x == static_cast<FloatingPointType>(-0))
      ? static_cast<FloatingPointType>(0)
      : ((x >= 0) ? x : -x);
};

template <class T> T             frexp_of_constituent               (T             x, int*)        { return T(); }
template <>        float         frexp_of_constituent<float>        (float         x, int* expptr) { return ::frexpf(x, expptr); }
template <>        double        frexp_of_constituent<double>       (double        x, int* expptr) { return ::frexp (x, expptr); }
template <>        long double   frexp_of_constituent<long double>  (long double   x, int* expptr) { return ::frexpl(x, expptr); }
#if defined(BOOST_HAS_FLOAT128)
template <>        float128_type frexp_of_constituent<float128_type>(float128_type x, int* expptr) { return ::frexpq(x, expptr); }
#endif

template <class T> T             ldexp_of_constituent               (T             x, int)        { return T(); }
template <>        float         ldexp_of_constituent<float>        (float         x, int expval) { return ::ldexpf(x, expval); }
template <>        double        ldexp_of_constituent<double>       (double        x, int expval) { return ::ldexp (x, expval); }
template <>        long double   ldexp_of_constituent<long double>  (long double   x, int expval) { return ::ldexpl(x, expval); }
#if defined(BOOST_HAS_FLOAT128)
template <>        float128_type ldexp_of_constituent<float128_type>(float128_type x, int expval) { return ::ldexpq(x, expval); }
#endif

template <class T> T             sqrt_of_constituent               (T             x) { return T(); }
template <>        float         sqrt_of_constituent<float>        (float         x) { return ::sqrtf(x); }
template <>        double        sqrt_of_constituent<double>       (double        x) { return ::sqrt (x); }
template <>        long double   sqrt_of_constituent<long double>  (long double   x) { return ::sqrtl(x); }
#if defined(BOOST_HAS_FLOAT128)
template <>        float128_type sqrt_of_constituent<float128_type>(float128_type x) { return ::sqrtq(x); }
#endif

template <class T> T             floor_of_constituent               (T             x) { return T(); }
template <>        float         floor_of_constituent<float>        (float         x) { return ::floorf(x); }
template <>        double        floor_of_constituent<double>       (double        x) { return ::floor (x); }
template <>        long double   floor_of_constituent<long double>  (long double   x) { return ::floorl(x); }
#if defined(BOOST_HAS_FLOAT128)
template <>        float128_type floor_of_constituent<float128_type>(float128_type x) { return ::floorq(x); }
#endif

template <class T> T             log_of_constituent               (T             x) { return T(); }
template <>        float         log_of_constituent<float>        (float         x) { return ::logf(x); }
template <>        double        log_of_constituent<double>       (double        x) { return ::log (x); }
template <>        long double   log_of_constituent<long double>  (long double   x) { return ::logl(x); }
#if defined(BOOST_HAS_FLOAT128)
template <>        float128_type log_of_constituent<float128_type>(float128_type x) { return ::logq(x); }
#endif

template <class T>
struct numeric_limits_partial_of_constituent
{
  static constexpr bool is_specialized = false;
  static constexpr int  digits         = 0;
  static constexpr int  digits10       = 0;
  static constexpr int  min_exponent   = 0;
  static constexpr int  min_exponent10 = 0;
  static constexpr int  max_exponent   = 0;
  static constexpr int  max_exponent10 = 0;

  static constexpr T (min)   () noexcept { return T(); }
  static constexpr T (max)   () noexcept { return T(); }
  static constexpr T  epsilon()          { return T(); }
};

template <>
struct numeric_limits_partial_of_constituent<float>
{
  static constexpr bool is_specialized = true;
  static constexpr int  digits         = std::numeric_limits<float>::digits;
  static constexpr int  digits10       = std::numeric_limits<float>::digits10;
  static constexpr int  min_exponent   = std::numeric_limits<float>::min_exponent;
  static constexpr int  min_exponent10 = std::numeric_limits<float>::min_exponent10;
  static constexpr int  max_exponent   = std::numeric_limits<float>::max_exponent;
  static constexpr int  max_exponent10 = std::numeric_limits<float>::max_exponent10;

  static constexpr float (min)   () noexcept { return (std::numeric_limits<float>::min)(); }
  static constexpr float (max)   () noexcept { return (std::numeric_limits<float>::max)(); }
  static constexpr float  epsilon()          { return  std::numeric_limits<float>::epsilon(); }
};

template <>
struct numeric_limits_partial_of_constituent<double>
{
  static constexpr bool is_specialized = true;
  static constexpr int  digits         = std::numeric_limits<double>::digits;
  static constexpr int  digits10       = std::numeric_limits<double>::digits10;
  static constexpr int  min_exponent   = std::numeric_limits<double>::min_exponent;
  static constexpr int  min_exponent10 = std::numeric_limits<double>::min_exponent10;
  static constexpr int  max_exponent   = std::numeric_limits<double>::max_exponent;
  static constexpr int  max_exponent10 = std::numeric_limits<double>::max_exponent10;

  static constexpr double (min)   () noexcept { return (std::numeric_limits<double>::min)(); }
  static constexpr double (max)   () noexcept { return (std::numeric_limits<double>::max)(); }
  static constexpr double  epsilon()          { return  std::numeric_limits<double>::epsilon(); }
};

template <>
struct numeric_limits_partial_of_constituent<long double>
{
  static constexpr bool is_specialized = true;
  static constexpr int  digits         = std::numeric_limits<long double>::digits;
  static constexpr int  digits10       = std::numeric_limits<long double>::digits10;
  static constexpr int  min_exponent   = std::numeric_limits<long double>::min_exponent;
  static constexpr int  min_exponent10 = std::numeric_limits<long double>::min_exponent10;
  static constexpr int  max_exponent   = std::numeric_limits<long double>::max_exponent;
  static constexpr int  max_exponent10 = std::numeric_limits<long double>::max_exponent10;

  static constexpr long double (min)   () noexcept { return (std::numeric_limits<long double>::min)(); }
  static constexpr long double (max)   () noexcept { return (std::numeric_limits<long double>::max)(); }
  static constexpr long double  epsilon()          { return  std::numeric_limits<long double>::epsilon(); }
};

#if defined(BOOST_HAS_FLOAT128)
template <>
struct numeric_limits_partial_of_constituent<float128_type>
{
  static constexpr bool is_specialized = true;
  static constexpr int  digits         = 113;
  static constexpr int  digits10       = 33;
  static constexpr int  min_exponent   = -16381;
  static constexpr int  min_exponent10 = static_cast<int>(static_cast<long>(min_exponent) * 301L / 1000L);
  static constexpr int  max_exponent   = 16384;
  static constexpr int  max_exponent10 = static_cast<int>(static_cast<long>(max_exponent) * 301L / 1000L);

  static constexpr float128_type (min)() noexcept
  {
    return   static_cast<float128_type>(1)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN)
           * static_cast<float128_type>(DBL_MIN) / 1073741824;
  }

  static constexpr float128_type (max)() noexcept
  {
    // This has one bit set only.
    constexpr double dbl_mult = 8.9884656743115795386e+307;

    return   (static_cast<float128_type>(1) - 9.62964972193617926527988971292463659e-35) // This now has all bits sets to 1
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult)
           *  static_cast<float128_type>(dbl_mult) * 65536;
  }

  static constexpr float128_type epsilon()
  {
    // This double value has only one bit set and so is exact.
    return 1.92592994438723585305597794258492732e-34;
  }
};
#endif

template <class T>
struct is_floating_point_or_float128
{
   static constexpr auto value = numeric_limits_partial_of_constituent<T>::is_specialized;
};

template <typename FloatingPointType>
struct exact_arithmetic
{
   // The exact_arithmetic<> struct implements extended precision
   // techniques that are used in cpp_double_fp_backend and cpp_quad_float.

   static_assert(is_floating_point_or_float128<FloatingPointType>::value,
                 "Error: exact_arithmetic<> invoked with unknown floating-point type");

   using float_type  = FloatingPointType;
   using float_pair  = std::pair<float_type, float_type>;
   using float_tuple = std::tuple<float_type, float_type, float_type, float_type>;

   static BOOST_MP_CXX14_CONSTEXPR float_pair split(const float_type& a)
   {
      // Split a floating point number in two (high and low) parts approximating the
      // upper-half and lower-half bits of the float

      static_assert(is_floating_point_or_float128<FloatingPointType>::value,
                    "Error: exact_arithmetic<>::split invoked with unknown floating-point type");

      // TODO Replace bit shifts with constexpr funcs or ldexp for better compaitibility
      constexpr int MantissaBits = numeric_limits_partial_of_constituent<float_type>::digits;
      constexpr int SplitBits    = MantissaBits / 2 + 1;

      // Check if the integer is wide enough to hold the Splitter.
      static_assert(std::numeric_limits<std::uintmax_t>::digits > SplitBits,
                    "Inadequate integer width for binary shifting needed in split(), try using ldexp instead");

      // If the above line gives an compilation error, replace the
      // line below it with the commented line

      constexpr float_type Splitter       = FloatingPointType((static_cast<std::uintmax_t>(UINT8_C(1)) << SplitBits) + 1);
      const     float_type SplitThreshold = (numeric_limits_partial_of_constituent<float_type>::max)() / (Splitter * 2);

      float_type hi { };
      float_type lo { };

      // Handle if multiplication with the splitter would cause overflow
      if (a > SplitThreshold || a < -SplitThreshold)
      {
         constexpr float_type Normalizer = float_type(1ULL << (SplitBits + 1));

         const float_type a_ = a / Normalizer;

         const float_type temp = Splitter * a_;

         hi   = temp - (temp - a_);
         lo   = a_ - hi;

         hi *= Normalizer;
         lo *= Normalizer;
      }
      else
      {
         const float_type temp = Splitter * a;

         hi   = temp - (temp - a);
         lo   = a - hi;
      }

      return std::make_pair(hi, lo);
   }

   static BOOST_MP_CXX14_CONSTEXPR float_pair fast_sum(const float_type& a, const float_type& b)
   {
      // Exact addition of two floating point numbers, given |a| > |b|
      const float_type a_plus_b = a + b;

      return float_pair(a_plus_b, b - (a_plus_b - a));
   }

   static BOOST_MP_CXX14_CONSTEXPR float_pair sum(const float_type& a, const float_type& b)
   {
      // Exact addition of two floating point numbers
      const float_type a_plus_b = a + b;
      const float_type v        = a_plus_b - a;

      return float_pair(a_plus_b, (a - (a_plus_b - v)) + (b - v));
   }

   static BOOST_MP_CXX14_CONSTEXPR void three_sum(float_type& a, float_type& b, float_type& c)
   {
      using std::get;
      using std::tie;

      std::tuple<float_type, float_type, float_type> t;

      tie(get<0>(t), get<1>(t)) = sum(a, b);
      tie(a, get<2>(t))         = sum(c, get<0>(t));
      tie(b, c)                 = sum(get<1>(t), get<2>(t));
   }

   static BOOST_MP_CXX14_CONSTEXPR void sum(float_pair& p, float_type& e)
   {
      using std::tie;

      float_pair t;
      float_type t_;

      t                = sum(p.first, p.second);
      tie(p.first, t_) = sum(e, t.first);
      tie(p.second, e) = sum(t.second, t_);
   }

   static BOOST_MP_CXX14_CONSTEXPR float_tuple four_sum(float_type a, float_type b, float_type c, float_type d)
   {
      float_tuple out = std::make_tuple(a, b, c, d);
      normalize(out);
      return out;
   }

   static BOOST_MP_CXX14_CONSTEXPR float_pair difference(const float_type& a, const float_type& b)
   {
      // Exact subtraction of two floating point numbers
      const float_type a_minus_b = a - b;
      const float_type v         = a_minus_b - a;

      return float_pair(a_minus_b, (a - (a_minus_b - v)) - (b + v));
   }

   static BOOST_MP_CXX14_CONSTEXPR float_pair product(const float_type& a, const float_type& b)
   {
      // Exact product of two floating point numbers
      const float_pair a_split = split(a);
      const float_pair b_split = split(b);

      const volatile float_type pf = a * b;

      return
        std::make_pair
        (
          const_cast<const float_type&>(pf),
            (
                ((a_split.first  * b_split.first) - const_cast<const float_type&>(pf))
              +  (a_split.first  * b_split.second)
              +  (a_split.second * b_split.first)
            )
          +
            (a_split.second * b_split.second)
        );
   }

   static BOOST_MP_CXX14_CONSTEXPR void normalize(float_pair& p, bool fast = true)
   {
      // Converts a pair of floats to standard form.

      p = (fast ? fast_sum(p.first, p.second) : sum(p.first, p.second));
   }

   static BOOST_MP_CXX14_CONSTEXPR void normalize(float_tuple& t)
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

         (get<2>(s) != 0) ? tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), get<3>(t))
                          : tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<3>(t));
      }
      else
      {
         tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), get<2>(t));

         (get<1>(s) != 0) ? tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<3>(t))
                          : tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), get<3>(t));
      }

      t = s;
   }

   static BOOST_MP_CXX14_CONSTEXPR void normalize(float_tuple& t, float_type e)
   {
      using std::get;
      using std::tie;

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

            if(get<3>(s) != 0)
            {
              get<3>(s) += e;
            }
            else
            {
              tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), e);
            }
         }
         else
         {
            tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<3>(t));

            (get<2>(s) != 0) ? tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), e)
                             : tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), e);
         }
      }
      else
      {
         tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), get<2>(t));

         if (get<1>(s) != 0)
         {
            tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), get<3>(t));

            (get<2>(s) != 0) ? tie(get<2>(s), get<3>(s)) = fast_sum(get<2>(s), e)
                             : tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), e);
         }
         else
         {
            tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), get<3>(t));

            (get<1>(s) != 0) ? tie(get<1>(s), get<2>(s)) = fast_sum(get<1>(s), e)
                             : tie(get<0>(s), get<1>(s)) = fast_sum(get<0>(s), e);
         }
      }

      t = s;
   }
};

} } } } // namespace boost::multiprecision::backends::cpp_df_qf_detail

#endif // BOOST_MP_CPP_DF_QF_DETAIL_2023_01_02_HPP
