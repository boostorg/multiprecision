///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MULTIPRECISION_DETAIL_HASH_HPP
#define BOOST_MULTIPRECISION_DETAIL_HASH_HPP

#include <cstddef>
#include <functional>
#include <system_error>

namespace boost { namespace multiprecision { namespace detail {

inline void hash_combine(std::size_t& seed) 
{ 
    (void)seed; // suppress warning: unused parameter 'seed'
}

template <typename T, typename... Args>
inline void hash_combine(std::size_t& seed, const T& v, Args... args) 
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    hash_combine(seed, args...);
}

inline std::size_t hash_value(std::error_condition const& v) 
{
    std::size_t seed = 0;
    hash_combine(seed, v.value());
    hash_combine(seed, &v.category());
    return seed;
}

}}} // Namespaces

#endif // BOOST_MULTIPRECISION_DETAIL_HASH_HPP
