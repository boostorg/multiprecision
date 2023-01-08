///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2023.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_LDEXP_2023_01_07_HPP
#define BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_LDEXP_2023_01_07_HPP

#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_fabs.hpp>
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
Real ldexp_impl(Real arg, int exp) noexcept
{
    while(exp > 0)
    {
        arg *= 2;
        --exp;
    }
    while(exp < 0)
    {
        arg /= 2;
        ++exp;
    }

    return arg;
}

} // Namespace detail

template <typename Real>
inline constexpr Real ldexp(Real arg, int exp) noexcept
{
   return cpp_df_qf_detail::ccmath::fabs(arg) == Real(0) ? arg :
          cpp_df_qf_detail::ccmath::isinf(arg) ? arg :
          cpp_df_qf_detail::ccmath::isnan(arg) ? arg :
          cpp_df_qf_detail::ccmath::detail::ldexp_impl(arg, exp);
}

} } } } } // namespace boost::multiprecision::backends::cpp_df_qf_detail::ccmath

#endif // BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_LDEXP_2023_01_07_HPP
