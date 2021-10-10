///////////////////////////////////////////////////////////////
//  Copyright 2021 Matt Borland.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   https://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_COMMON_FACTOR_HPP
#define BOOST_MP_COMMON_FACTOR_HPP

#include <boost/multiprecision/detail/standalone_config.hpp>

#ifndef BOOST_MP_STANDALONE

#include <boost/integer/common_factor_rt.hpp>

namespace boost { namespace multiprecision { namespace detail { 
    
template <typename T>
inline BOOST_CXX14_CONSTEXPR T constexpr_gcd(T a, T b) noexcept
{
    return boost::integer::gcd(a, b);
}

template <typename T>
inline BOOST_CXX14_CONSTEXPR T constexpr_lcm(T a, T b) noexcept
{
    return boost::integer::lcm(a, b);
}
}}} // Namespaces

#else 

namespace boost { namespace multiprecision { namespace detail { 

template <typename T>
inline BOOST_CXX14_CONSTEXPR void constexpr_swap(T& a, T& b) noexcept
{
   const T temp = a;
   a = b;
   b = temp;
}

template <typename T>
inline BOOST_CXX14_CONSTEXPR T constexpr_gcd(T a, T b) noexcept
{
    while (b != T(0))
    {
        a %= b;
        boost::multiprecision::detail::constexpr_swap(a, b);
    }
    return a;
}

template <typename T>
inline BOOST_CXX14_CONSTEXPR T constexpr_lcm(T a, T b) noexcept
{
    const T ab_gcd = boost::multiprecision::detail::constexpr_gcd(a, b);
    return (a * b) / ab_gcd;
}

}}} // Namespaces

#endif // BOOST_MP_STANDALONE

#endif // BOOST_MP_COMMON_FACTOR_HPP
