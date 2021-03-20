///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MULTIPRECISION_DETAIL_HASH_HPP
#define BOOST_MULTIPRECISION_DETAIL_HASH_HPP

#include <cstddef>
#include <functional>

namespace boost { namespace multiprecision { namespace detail {

inline void hash_combine(std::size_t&) {}

template <typename T, typename... Args>
inline void hash_combine(std::size_t& seed, const T& v, Args... args) 
{
    // gmp types require explicit casting
    seed = static_cast<std::size_t>(static_cast<T>(seed) ^ (v + 0x9e3779b9 + (seed<<6) + (seed>>2)));
    hash_combine(seed, args...);
}

template <typename T>
inline std::size_t hash_value(const T& v)
{
    std::hash<T> hasher;
    return hasher(v);
}

}}} // Namespaces

#endif // BOOST_MULTIPRECISION_DETAIL_HASH_HPP
