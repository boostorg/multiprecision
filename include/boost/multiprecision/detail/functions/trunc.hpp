///////////////////////////////////////////////////////////////////////////////
//  Copyright 2022 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_DETAIL_FUNCTIONS_TRUNC_HPP
#define BOOST_MP_DETAIL_FUNCTIONS_TRUNC_HPP

#include <cmath>
#include <limits>
#include <stdexcept>
#include <boost/multiprecision/detail/standalone_config.hpp>
#include <boost/multiprecision/detail/no_exceptions_support.hpp>

#ifdef BOOST_MP_MATH_AVAILABLE
#include <boost/math/special_functions/trunc.hpp>
#endif

namespace boost { namespace multiprecision { namespace detail {

namespace impl {

template <typename T>
inline T floor BOOST_PREVENT_MACRO_SUBSTITUTION (const T arg)
{
    T result = 1;

    if(result < arg)
    {
        while(result < arg)
        {
            result *= 2;
        }
        while(result > arg)
        {
            --result;
        }

        return result;
    }
    else
    {
        return T(0);
    }
}

template <typename T>
inline T ceil BOOST_PREVENT_MACRO_SUBSTITUTION (const T arg)
{
    T result = boost::multiprecision::detail::impl::floor(arg);

    if(result == arg)
    {
        return result;
    }
    else
    {
        return result + 1;
    }
}

template <typename T>
inline T trunc BOOST_PREVENT_MACRO_SUBSTITUTION (const T arg)
{
    return (arg > 0) ? boost::multiprecision::detail::impl::floor(arg) : boost::multiprecision::detail::impl::ceil(arg);
}

} // namespace impl

#ifdef BOOST_MP_MATH_AVAILABLE

template <typename T>
inline long long lltrunc BOOST_PREVENT_MACRO_SUBSTITUTION (const T arg)
{
    return boost::math::lltrunc(arg);
}

#else

template <typename T>
inline long long lltrunc BOOST_PREVENT_MACRO_SUBSTITUTION (const T arg)
{
    if (arg > LLONG_MAX)
    {
        BOOST_MP_THROW_EXCEPTION(std::domain_error("arg cannot be converted into a long long"));
    }

    return static_cast<long long>(boost::multiprecision::detail::impl::trunc(arg));
}

#endif

}}} // Namespaces

#endif // BOOST_MP_DETAIL_FUNCTIONS_TRUNC_HPP
