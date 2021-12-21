///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_MP_STD_INTEGER_TRAITS_HPP
#define BOOST_MP_STD_INTEGER_TRAITS_HPP

#include <type_traits>
#include <boost/multiprecision/detail/standalone_config.hpp>

namespace boost {
namespace multiprecision {
namespace detail {

template <class T>
struct is_signed : public std::is_signed<T> {};
template <class T>
struct is_unsigned : public std::is_unsigned<T> {};
template <class T>
struct is_integral : public std::is_integral<T> {};
template <class T>
struct is_arithmetic : public std::is_arithmetic<T> {};
template <class T>
struct make_unsigned : public std::make_unsigned<T> {};
template <class T>
struct make_signed : public std::make_signed<T> {};

#ifdef BOOST_HAS_INT128

template <>
struct is_signed<boost::multiprecision::int128_type> : public std::true_type {};
template <>
struct is_signed<boost::multiprecision::uint128_type> : public std::false_type {};
template <>
struct is_unsigned<boost::multiprecision::int128_type> : public std::false_type {};
template <>
struct is_unsigned<boost::multiprecision::uint128_type> : public std::true_type {};
template <>
struct is_integral<boost::multiprecision::int128_type> : public std::true_type {};
template <>
struct is_integral<boost::multiprecision::uint128_type> : public std::true_type {};
template <>
struct is_arithmetic<boost::multiprecision::int128_type> : public std::true_type {};
template <>
struct is_arithmetic<boost::multiprecision::uint128_type> : public std::true_type {};
template <>
struct make_unsigned<boost::multiprecision::int128_type>
{
   using type = boost::multiprecision::uint128_type;
};
template <>
struct make_unsigned<boost::multiprecision::uint128_type>
{
   using type = boost::multiprecision::uint128_type;
};
template <>
struct make_signed<boost::multiprecision::int128_type>
{
   using type = boost::multiprecision::int128_type;
};
template <>
struct make_signed<boost::multiprecision::uint128_type>
{
   using type = boost::multiprecision::int128_type;
};

#endif

}}} // namespace boost::multiprecision::detail

#endif
