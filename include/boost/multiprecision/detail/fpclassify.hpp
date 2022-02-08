///////////////////////////////////////////////////////////////////////////////
//  Copyright 2022 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_DETAIL_FPCLASSIFY_HPP
#define BOOST_MP_DETAIL_FPCLASSIFY_HPP

#include <cmath>
#include <limits>
#include <boost/multiprecision/detail/standalone_config.hpp>
#include <boost/multiprecision/detail/number_base.hpp>

#ifdef BOOST_MP_MATH_AVAILABLE
#include <boost/math/special_functions/fpclassify.hpp>

namespace boost { namespace multiprecision { namespace detail {

template <typename T>
inline bool isnan BOOST_PREVENT_MACRO_SUBSTITUTION (const T val)
{
    return boost::math::isnan(val);
}

template <typename T>
inline bool isinf BOOST_PREVENT_MACRO_SUBSTITUTION (const T val)
{
    return boost::math::isinf(val);
}

template <typename T>
inline int fpclassify BOOST_PREVENT_MACRO_SUBSTITUTION (const T val)
{
    return boost::math::fpclassify(val);
}

}}} // Namespaces

#else

namespace boost { namespace multiprecision { namespace detail {

// Assume IEEE 754 compliance and compiler optimizations do not affect this
template <typename T>
inline constexpr bool isnan BOOST_PREVENT_MACRO_SUBSTITUTION (const T val) noexcept
{
    return val != val;
}

template <typename T>
inline constexpr bool isinf BOOST_PREVENT_MACRO_SUBSTITUTION (const T val) noexcept
{
    return val == std::numeric_limits<T>::infinity() || -val == std::numeric_limits<T>::infinity();
}

template <typename T>
inline constexpr int fpclassify BOOST_PREVENT_MACRO_SUBSTITUTION (const T val) noexcept
{
    return (boost::multiprecision::detail::isnan)(val) ? FP_NAN :
           (boost::multiprecision::detail::isinf)(val) ? FP_INFINITE :
           boost::multiprecision::detail::abs(val) == T(0) ? FP_ZERO :
           boost::multiprecision::detail::abs(val) > 0 && boost::multiprecision::detail::abs(val) < (std::numeric_limits<T>::min)() ? FP_SUBNORMAL : FP_NORMAL;
}

}}} // Namespaces

#endif // BOOST_MP_MATH_AVAILABLE

#endif // BOOST_MP_DETAIL_FPCLASSIFY_HPP
