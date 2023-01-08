///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2023.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_SQRT_2023_01_07_HPP
#define BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_SQRT_2023_01_07_HPP

#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_isinf.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_isnan.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_limits.hpp>

namespace boost { namespace multiprecision { namespace backends { namespace cpp_df_qf_detail { namespace ccmath {

namespace detail {

template <typename Real>
#if (defined(_MSC_VER) && (_MSC_VER <= 1900))
BOOST_MP_CXX14_CONSTEXPR
#else
constexpr
#endif
Real sqrt_impl_2(Real x, Real s, Real s2)
{
   return !(s < s2) ? s2 : sqrt_impl_2(x, (x / s + s) / 2, s);
}

template <typename Real>
#if (defined(_MSC_VER) && (_MSC_VER <= 1900))
BOOST_MP_CXX14_CONSTEXPR
#else
constexpr
#endif
Real sqrt_impl_1(Real x, Real s)
{
   return sqrt_impl_2(x, (x / s + s) / 2, s);
}

template <typename Real>
#if (defined(_MSC_VER) && (_MSC_VER <= 1900))
BOOST_MP_CXX14_CONSTEXPR
#else
constexpr
#endif
Real sqrt_impl(Real x)
{
   return sqrt_impl_1(x, x > 1 ? x : Real(1));
}

} // namespace detail

template <typename Real>
#if (defined(_MSC_VER) && (_MSC_VER <= 1900))
BOOST_MP_CXX14_CONSTEXPR
#else
constexpr
#endif
Real sqrt(Real x)
{
   return cpp_df_qf_detail::ccmath::isnan(x) ? ccmath::numeric_limits<Real>::quiet_NaN() :
          cpp_df_qf_detail::ccmath::isinf(x) ? ccmath::numeric_limits<Real>::infinity() :
          cpp_df_qf_detail::ccmath::detail::sqrt_impl<Real>(x);
}

} } } } } // namespace boost::multiprecision::backends::cpp_df_qf_detail::ccmath

#endif // BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_SQRT_2023_01_07_HPP
