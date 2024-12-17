///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2024.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_FMA_2024_12_17_HPP
#define BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_FMA_2024_12_17_HPP

#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_isnan.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_isinf.hpp>

namespace boost { namespace multiprecision { namespace backends { namespace cpp_df_qf_detail { namespace ccmath {

namespace detail {

template <class T>
constexpr T fma_imp(T x, T y, T z) noexcept
{
   #if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)

   BOOST_IF_CONSTEXPR(::std::is_same<T, float>::value)
   {
      return __builtin_fmaf(x, y, z);
   }
   else BOOST_IF_CONSTEXPR(::std::is_same<T, double>::value)
   {
      return __builtin_fma(x, y, z);
   }
   else BOOST_IF_CONSTEXPR(::std::is_same<T, long double>::value)
   {
      return __builtin_fmal(x, y, z);
   }
   #endif

   // If we can't use compiler intrinsics hope that -fma flag optimizes this call to fma instruction
   return (x * y) + z;
};

} // namespace detail

template <typename Real>
constexpr Real fma(Real x, Real y, Real z) noexcept
{
   if (x == 0 && cpp_df_qf_detail::ccmath::isinf(y))
   {
      return std::numeric_limits<Real>::quiet_NaN();
   }
   else if (y == 0 && cpp_df_qf_detail::ccmath::isinf(x))
   {
      return std::numeric_limits<Real>::quiet_NaN();
   }
   else if (cpp_df_qf_detail::ccmath::isnan(x))
   {
      return std::numeric_limits<Real>::quiet_NaN();
   }
   else if (cpp_df_qf_detail::ccmath::isnan(y))
   {
      return std::numeric_limits<Real>::quiet_NaN();
   }
   else if (cpp_df_qf_detail::ccmath::isnan(z))
   {
      return std::numeric_limits<Real>::quiet_NaN();
   }

   return detail::fma_imp(x, y, z);
}

} } } } } // namespace boost::multiprecision::backends::cpp_df_qf_detail::ccmath

#endif // BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_FMA_2024_12_17_HPP
