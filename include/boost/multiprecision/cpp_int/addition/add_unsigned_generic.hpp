///////////////////////////////////////////////////////////////
//  Copyright 2020 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_MP_ADD_UNSIGNED_GENERIC_HPP
#define BOOST_MP_ADD_UNSIGNED_GENERIC_HPP

#include <boost/multiprecision/cpp_int/addition/add_unsigned_constexpr.hpp>

namespace boost { namespace multiprecision { namespace backends {

//
// This is the key addition routine where all the argument types are non-trivial cpp_int's:
//
template <class CppInt1, class CppInt2, class CppInt3>
inline BOOST_MP_CXX14_CONSTEXPR void add_unsigned(CppInt1& result, const CppInt2& a, const CppInt3& b) BOOST_MP_NOEXCEPT_IF(is_non_throwing_cpp_int<CppInt1>::value)
{
   add_unsigned_constexpr(result, a, b);
}


} } }  // namespaces


#endif


