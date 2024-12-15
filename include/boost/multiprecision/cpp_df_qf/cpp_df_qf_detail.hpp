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
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath.hpp>

#ifdef BOOST_HAS_FLOAT128
#include <quadmath.h>
#endif

namespace boost { namespace multiprecision { namespace backends { namespace cpp_df_qf_detail {

inline float                  floor_of_constituent(float                  x) { return ::floorf(x); }
inline double                 floor_of_constituent(double                 x) { return ::floor (x); }
inline long double            floor_of_constituent(long double            x) { return ::floorl(x); }
#if defined(BOOST_HAS_FLOAT128)
inline ::boost::float128_type floor_of_constituent(::boost::float128_type x) { return ::floorq(x); }
#endif

inline float                  log_of_constituent(float                  x) { return ::logf(x); }
inline double                 log_of_constituent(double                 x) { return ::log (x); }
inline long double            log_of_constituent(long double            x) { return ::logl(x); }
#if defined(BOOST_HAS_FLOAT128)
inline ::boost::float128_type log_of_constituent(::boost::float128_type x) { return ::logq(x); }
#endif

inline constexpr float        split          (float)                  { return static_cast<float>      (1U + static_cast<unsigned long long>(static_cast<unsigned long long>(1U) << static_cast<unsigned>((cpp_df_qf_detail::ccmath::numeric_limits<float                 >::digits + 1) / 2))); }
inline constexpr double       split          (double)                 { return static_cast<double>     (1U + static_cast<unsigned long long>(static_cast<unsigned long long>(1U) << static_cast<unsigned>((cpp_df_qf_detail::ccmath::numeric_limits<double                >::digits + 1) / 2))); }
inline constexpr long double  split          (long double)            { return static_cast<long double>(1U + static_cast<unsigned long long>(static_cast<unsigned long long>(1U) << static_cast<unsigned>((cpp_df_qf_detail::ccmath::numeric_limits<long double           >::digits + 1) / 2))); }
#if defined(BOOST_HAS_FLOAT128)
inline constexpr ::boost::float128_type split(::boost::float128_type) { return static_cast<::boost::float128_type>(1) + static_cast<::boost::float128_type>(static_cast<::boost::uint128_type>(1U) << static_cast<unsigned>((cpp_df_qf_detail::ccmath::numeric_limits<::boost::float128_type>::digits + 1) / 2)); }
#endif

template <class FloatingPointType>
struct is_floating_point_or_float128
{
   static constexpr auto value =    std::is_same<FloatingPointType, float>::value
                                 || std::is_same<FloatingPointType, double>::value
                                 || std::is_same<FloatingPointType, long double>::value
#if defined(BOOST_HAS_FLOAT128)
                                 || std::is_same<FloatingPointType, ::boost::float128_type>::value
#endif
                                 ;
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

   static
   #if (defined(_MSC_VER) && (_MSC_VER <= 1900))
   BOOST_MP_CXX14_CONSTEXPR
   #else
   constexpr
   #endif
   float_pair fast_sum(float_type a, float_type b)
   {
      // Exact addition of two floating point numbers, given |a| > |b|
      const float_type a_plus_b = a + b;

      const float_pair result(a_plus_b, b - (a_plus_b - a));

      return result;
   }

   static constexpr void sum(float_pair& result, float_type a, float_type b)
   {
      // Exact addition of two floating point numbers
      const float_type a_plus_b = a + b;
      const float_type v        = a_plus_b - a;

      const float_pair tmp(a_plus_b, (a - (a_plus_b - v)) + (b - v));

      result.first  = tmp.first;
      result.second = tmp.second;
   }

   static constexpr void normalize(float_pair& result, float_type a, float_type b)
   {
      // Converts a pair of floats to standard form.
      const float_pair tmp = fast_sum(a, b);

      result.first  = tmp.first;
      result.second = tmp.second;
   }

   static constexpr float_pair split(const float_type& a)
   {
      // Split a floating point number in two (high and low) parts approximating the
      // upper-half and lower-half bits of the float

      static_assert(is_floating_point_or_float128<FloatingPointType>::value,
                    "Error: exact_arithmetic<>::split invoked with unknown floating-point type");

      // TODO Replace bit shifts with constexpr funcs or ldexp for better compaitibility
      constexpr int MantissaBits = cpp_df_qf_detail::ccmath::numeric_limits<float_type>::digits;
      constexpr int SplitBits    = MantissaBits / 2 + 1;

      // Check if the integer is wide enough to hold the Splitter.
      static_assert(std::numeric_limits<std::uintmax_t>::digits > SplitBits,
                    "Inadequate integer width for binary shifting needed in split(), try using ldexp instead");

      // If the above line gives an compilation error, replace the
      // line below it with the commented line

      constexpr float_type Splitter       = FloatingPointType((static_cast<std::uintmax_t>(UINT8_C(1)) << SplitBits) + 1);
      const     float_type SplitThreshold = (cpp_df_qf_detail::ccmath::numeric_limits<float_type>::max)() / (Splitter * 2);

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

   static constexpr float_pair product(const float_type& a, const float_type& b)
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
};

} } } } // namespace boost::multiprecision::backends::cpp_df_qf_detail

#endif // BOOST_MP_CPP_DF_QF_DETAIL_2023_01_02_HPP
