///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2024.
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

#if defined(BOOST_HAS_FLOAT128)
template <class T>
auto fma_impl(T x, T y, T z) noexcept -> typename ::std::enable_if<::std::is_same<T, ::boost::float128_type>::value, T>::type
{
   return ::fmaq(x);
}
#endif

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
template <class T>
auto fma_impl(T x, T y, T z) noexcept -> typename ::std::enable_if<::std::is_same<T, float>::value, T>::type
{
  return __builtin_fmaf(x, y, z);
}

template <class T>
auto fma_impl(T x, T y, T z) noexcept -> typename ::std::enable_if<::std::is_same<T, double>::value, T>::type
{
  return __builtin_fma(x, y, z);
}

template <class T>
auto fma_impl(T x, T y, T z) noexcept -> typename ::std::enable_if<::std::is_same<T, long double>::value, T>::type
{
  return __builtin_fmal(x, y, z);
}
#else
template <class T>
auto fma_impl(T x, T y, T z) noexcept -> typename ::std::enable_if<::std::is_floating_point<T>::value, T>::type
{
  // Default to the written-out operations.

   return (x * y) + z;
}
#endif

} // namespace detail

template <typename Real>
auto fma(Real x, Real y, Real z) noexcept -> Real
{
   return detail::fma_impl(x, y, z);
}

} // namespace unsafe

} } } } } // namespace boost::multiprecision::backends::cpp_df_qf_detail::ccmath

#endif // BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_FMA_2024_12_17_HPP
