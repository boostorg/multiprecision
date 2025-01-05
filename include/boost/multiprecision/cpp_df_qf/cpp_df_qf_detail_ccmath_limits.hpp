///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2023 - 2024.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_LIMITS_2023_01_07_HPP
#define BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_LIMITS_2023_01_07_HPP

#include <cfloat>
#include <cmath>
#include <limits>
#include <type_traits>

namespace boost { namespace multiprecision { namespace backends { namespace cpp_df_qf_detail { namespace ccmath {

template <class FloatingPointType,
          class EnableType = void>
struct numeric_limits
{
   static constexpr bool is_specialized = false;
};

template <class FloatingPointType>
struct numeric_limits<FloatingPointType,
                      typename std::enable_if<(   std::is_same<FloatingPointType, float>::value
                                               || std::is_same<FloatingPointType, double>::value
                                               || std::is_same<FloatingPointType, long double>::value
#if defined(BOOST_MP_CPP_DOUBLE_FP_HAS_FLOAT128)
                                               || std::is_same<FloatingPointType, ::boost::float128_type>::value
#endif
                                               )>::type>
{
private:
   using float_type = FloatingPointType;

public:
   static constexpr bool                    is_specialized                = std::numeric_limits<float_type>::is_specialized;
   static constexpr bool                    is_signed                     = std::numeric_limits<float_type>::is_signed;
   static constexpr bool                    is_integer                    = std::numeric_limits<float_type>::is_integer;
   static constexpr bool                    is_exact                      = std::numeric_limits<float_type>::is_exact;
   static constexpr bool                    is_bounded                    = std::numeric_limits<float_type>::is_bounded;
   static constexpr bool                    is_modulo                     = std::numeric_limits<float_type>::is_modulo;
   static constexpr bool                    is_iec559                     = std::numeric_limits<float_type>::is_iec559;
   static constexpr std::float_denorm_style has_denorm                    = std::numeric_limits<float_type>::has_denorm;
   static constexpr bool                    has_infinity                  = std::numeric_limits<float_type>::has_infinity;
   static constexpr bool                    has_quiet_NaN                 = std::numeric_limits<float_type>::has_quiet_NaN;
   static constexpr bool                    has_signaling_NaN             = std::numeric_limits<float_type>::has_signaling_NaN;
   static constexpr bool                    has_denorm_loss               = std::numeric_limits<float_type>::has_denorm_loss;
   static constexpr bool                    traps                         = std::numeric_limits<float_type>::traps;
   static constexpr bool                    tinyness_before               = std::numeric_limits<float_type>::tinyness_before;
   static constexpr std::float_round_style  round_style                   = std::numeric_limits<float_type>::round_style;

   static constexpr int radix                          = std::numeric_limits<float_type>::radix;
   static constexpr int digits                         = std::numeric_limits<float_type>::digits;
   static constexpr int digits10                       = std::numeric_limits<float_type>::digits10;
   static constexpr int max_digits10                   = std::numeric_limits<float_type>::max_digits10;

   static constexpr int max_exponent                   = std::numeric_limits<float_type>::max_exponent;
   static constexpr int min_exponent                   = std::numeric_limits<float_type>::min_exponent;
   static constexpr int max_exponent10                 = std::numeric_limits<float_type>::max_exponent10;
   static constexpr int min_exponent10                 = std::numeric_limits<float_type>::min_exponent10;

   static constexpr auto (min)         () noexcept -> float_type { return (std::numeric_limits<float_type>::min)         (); }
   static constexpr auto (max)         () noexcept -> float_type { return (std::numeric_limits<float_type>::max)         (); }
   static constexpr auto  lowest       () noexcept -> float_type { return  std::numeric_limits<float_type>::lowest       (); }
   static constexpr auto  epsilon      () noexcept -> float_type { return  std::numeric_limits<float_type>::epsilon      (); }
   static constexpr auto  round_error  () noexcept -> float_type { return  std::numeric_limits<float_type>::round_error  (); }
   static constexpr auto  denorm_min   () noexcept -> float_type { return  std::numeric_limits<float_type>::denorm_min   (); }
   static constexpr auto  infinity     () noexcept -> float_type { return  std::numeric_limits<float_type>::infinity     (); }
   static constexpr auto  quiet_NaN    () noexcept -> float_type { return  std::numeric_limits<float_type>::quiet_NaN    (); }
   static constexpr auto  signaling_NaN() noexcept -> float_type { return  std::numeric_limits<float_type>::signaling_NaN(); }
};

} } } } } // namespace boost::backends::cpp_df_qf_detail::ccmath

#endif // BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_LIMITS_2023_01_07_HPP
