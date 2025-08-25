///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2024 - 2025.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_FMA_2024_12_17_HPP
#define BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_FMA_2024_12_17_HPP

#include <cmath>
#include <type_traits>

namespace boost { namespace multiprecision { namespace backends { namespace cpp_df_qf_detail { namespace ccmath {

namespace unsafe {

namespace detail {

template <typename Real>
constexpr Real fma_impl(const Real x, const Real y, const Real z) noexcept
{
   #if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
   BOOST_IF_CONSTEXPR (std::is_same_v<Real, float>)
   {
      return __builtin_fmaf(x, y, z);
   }
   else BOOST_IF_CONSTEXPR (std::is_same_v<Real, double>)
   {
      return __builtin_fma(x, y, z);
   }
   else BOOST_IF_CONSTEXPR (std::is_same_v<Real, long double>)
   {
      return __builtin_fmal(x, y, z);
   }
   #endif

   // If we can't use compiler intrinsics hope that -fma flag optimizes this call to fma instruction
   return (x * y) + z;
}

} // namespace detail

template <typename Real>
constexpr auto fma(Real x, Real y, Real z) -> Real
{
   if (BOOST_MP_IS_CONST_EVALUATED(x))
   {
      return detail::fma_impl(x, y, z);
   }
   else
   {
      using std::fma;

      return fma(x, y, z);
   }
}

} // namespace unsafe

} } } } } // namespace boost::multiprecision::backends::cpp_df_qf_detail::ccmath

#endif // BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_FMA_2024_12_17_HPP
