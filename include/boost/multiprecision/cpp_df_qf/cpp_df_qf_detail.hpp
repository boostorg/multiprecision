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

inline float                  split(const float)                  { return static_cast<float>      (1U + static_cast<unsigned long long>(static_cast<unsigned long long>(1U) << static_cast<unsigned>((cpp_df_qf_detail::ccmath::numeric_limits<float                 >::digits + 1) / 2))); }
inline double                 split(const double)                 { return static_cast<double>     (1U + static_cast<unsigned long long>(static_cast<unsigned long long>(1U) << static_cast<unsigned>((cpp_df_qf_detail::ccmath::numeric_limits<double                >::digits + 1) / 2))); }
inline long double            split(const long double)            { return static_cast<long double>(1U + static_cast<unsigned long long>(static_cast<unsigned long long>(1U) << static_cast<unsigned>((cpp_df_qf_detail::ccmath::numeric_limits<long double           >::digits + 1) / 2))); }
#if defined(BOOST_HAS_FLOAT128)
inline ::boost::float128_type split(const ::boost::float128_type) { return static_cast<::boost::float128_type>(1) + static_cast<::boost::float128_type>(static_cast<::boost::uint128_type>(1U) << static_cast<unsigned>((cpp_df_qf_detail::ccmath::numeric_limits<::boost::float128_type>::digits + 1) / 2)); }
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

   static
   #if (defined(_MSC_VER) && (_MSC_VER <= 1900))
   BOOST_MP_CXX14_CONSTEXPR
   #else
   constexpr
   #endif
   void normalize(float_pair& result, float_type a, float_type b)
   {
      // Converts a pair of floats to standard form.
      const float_pair tmp = fast_sum(a, b);

      result.first  = tmp.first;
      result.second = tmp.second;
   }
};

} } } } // namespace boost::multiprecision::backends::cpp_df_qf_detail

#endif // BOOST_MP_CPP_DF_QF_DETAIL_2023_01_02_HPP
