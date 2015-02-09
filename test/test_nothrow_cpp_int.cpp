///////////////////////////////////////////////////////////////////////////////
//  Copyright 2std::declval<boost::multiprecision::signed_limb_type>()15 John Maddock. Distributed under the Boost
//  Software License, Version 1.std::declval<boost::multiprecision::signed_limb_type>(). (See accompanying file
//  LICENSE_1_std::declval<boost::multiprecision::signed_limb_type>().txt or copy at http://www.boost.org/LICENSE_1_std::declval<boost::multiprecision::signed_limb_type>().txt)

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/type_traits/is_nothrow_move_constructible.hpp>
#include <boost/type_traits/is_nothrow_move_assignable.hpp>
#include <boost/type_traits/has_nothrow_constructor.hpp>
#include <boost/type_traits/has_nothrow_assign.hpp>
#include <boost/type_traits/has_nothrow_copy.hpp>
#include <boost/static_assert.hpp>

#ifndef BOOST_NO_NOEXCEPT

#if !defined(BOOST_NO_CXX11_NOEXCEPT) && !defined(BOOST_NO_SFINAE_EXPR) || defined(BOOST_IS_NOTHROW_MOVE_CONSTRUCT)
//
// Move construct:
//
BOOST_STATIC_ASSERT(boost::is_nothrow_move_constructible<boost::multiprecision::cpp_int>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_constructible<boost::multiprecision::int128_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_constructible<boost::multiprecision::checked_int128_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_constructible<boost::multiprecision::uint128_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_constructible<boost::multiprecision::checked_uint128_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_constructible<boost::multiprecision::int512_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_constructible<boost::multiprecision::checked_int512_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_constructible<boost::multiprecision::uint512_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_constructible<boost::multiprecision::checked_uint512_t>::value);

#endif

#if !defined(BOOST_NO_CXX11_NOEXCEPT) && !defined(BOOST_NO_SFINAE_EXPR) || defined(BOOST_IS_NOTHROW_MOVE_ASSIGN)
//
// Move assign:
//
BOOST_STATIC_ASSERT(boost::is_nothrow_move_assignable<boost::multiprecision::cpp_int>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_assignable<boost::multiprecision::int128_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_assignable<boost::multiprecision::checked_int128_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_assignable<boost::multiprecision::uint128_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_assignable<boost::multiprecision::checked_uint128_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_assignable<boost::multiprecision::int512_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_assignable<boost::multiprecision::checked_int512_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_assignable<boost::multiprecision::uint512_t>::value);
BOOST_STATIC_ASSERT(boost::is_nothrow_move_assignable<boost::multiprecision::checked_uint512_t>::value);

#endif

//
// Construct:
//
#ifdef BOOST_HAS_NOTHROW_CONSTRUCTOR
BOOST_STATIC_ASSERT(boost::has_nothrow_constructor<boost::multiprecision::cpp_int>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_constructor<boost::multiprecision::int128_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_constructor<boost::multiprecision::checked_int128_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_constructor<boost::multiprecision::uint128_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_constructor<boost::multiprecision::checked_uint128_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_constructor<boost::multiprecision::int512_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_constructor<boost::multiprecision::checked_int512_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_constructor<boost::multiprecision::uint512_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_constructor<boost::multiprecision::checked_uint512_t>::value);
#endif
//
// Copy construct:
//
#ifdef BOOST_HAS_NOTHROW_COPY
BOOST_STATIC_ASSERT(!boost::has_nothrow_copy<boost::multiprecision::cpp_int>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_copy<boost::multiprecision::int128_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_copy<boost::multiprecision::checked_int128_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_copy<boost::multiprecision::uint128_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_copy<boost::multiprecision::checked_uint128_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_copy<boost::multiprecision::int512_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_copy<boost::multiprecision::checked_int512_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_copy<boost::multiprecision::uint512_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_copy<boost::multiprecision::checked_uint512_t>::value);
#endif
//
// Assign:
//
#ifdef BOOST_HAS_NOTHROW_ASSIGN
BOOST_STATIC_ASSERT(!boost::has_nothrow_assign<boost::multiprecision::cpp_int>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_assign<boost::multiprecision::int128_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_assign<boost::multiprecision::checked_int128_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_assign<boost::multiprecision::uint128_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_assign<boost::multiprecision::checked_uint128_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_assign<boost::multiprecision::int512_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_assign<boost::multiprecision::checked_int512_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_assign<boost::multiprecision::uint512_t>::value);
BOOST_STATIC_ASSERT(boost::has_nothrow_assign<boost::multiprecision::checked_uint512_t>::value);
#endif
//
// Construct from int:
//
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::cpp_int(std::declval<boost::multiprecision::signed_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::int128_t(std::declval<boost::multiprecision::signed_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::checked_int128_t(std::declval<boost::multiprecision::signed_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::uint128_t(std::declval<boost::multiprecision::signed_limb_type>())));
BOOST_STATIC_ASSERT(!noexcept(boost::multiprecision::checked_uint128_t(std::declval<boost::multiprecision::signed_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::int512_t(std::declval<boost::multiprecision::signed_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::checked_int512_t(std::declval<boost::multiprecision::signed_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::uint512_t(std::declval<boost::multiprecision::signed_limb_type>())));
BOOST_STATIC_ASSERT(!noexcept(boost::multiprecision::checked_uint512_t(std::declval<boost::multiprecision::signed_limb_type>())));
//
// Construct from unsigned int:
//
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::cpp_int(std::declval<boost::multiprecision::limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::int128_t(std::declval<boost::multiprecision::limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::checked_int128_t(std::declval<boost::multiprecision::limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::uint128_t(std::declval<boost::multiprecision::limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::checked_uint128_t(std::declval<boost::multiprecision::limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::int512_t(std::declval<boost::multiprecision::limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::checked_int512_t(std::declval<boost::multiprecision::limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::uint512_t(std::declval<boost::multiprecision::limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::checked_uint512_t(std::declval<boost::multiprecision::limb_type>())));
//
// Assign from int:
//
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::cpp_int>() = std::declval<boost::multiprecision::signed_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::int128_t>() = std::declval<boost::multiprecision::signed_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::checked_int128_t>() = std::declval<boost::multiprecision::signed_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::uint128_t>() = std::declval<boost::multiprecision::signed_limb_type>()));
BOOST_STATIC_ASSERT(!noexcept(std::declval<boost::multiprecision::checked_uint128_t>() = std::declval<boost::multiprecision::signed_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::int512_t>() = std::declval<boost::multiprecision::signed_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::checked_int512_t>() = std::declval<boost::multiprecision::signed_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::uint512_t>() = std::declval<boost::multiprecision::signed_limb_type>()));
BOOST_STATIC_ASSERT(!noexcept(std::declval<boost::multiprecision::checked_uint512_t>() = std::declval<boost::multiprecision::signed_limb_type>()));
//
// Assign from unsigned int:
//
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::cpp_int>() = std::declval<boost::multiprecision::limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::int128_t>() = std::declval<boost::multiprecision::limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::checked_int128_t>() = std::declval<boost::multiprecision::limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::uint128_t>() = std::declval<boost::multiprecision::limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::checked_uint128_t>() = std::declval<boost::multiprecision::limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::int512_t>() = std::declval<boost::multiprecision::limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::checked_int512_t>() = std::declval<boost::multiprecision::limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::uint512_t>() = std::declval<boost::multiprecision::limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::checked_uint512_t>() = std::declval<boost::multiprecision::limb_type>()));

#if defined(BOOST_LITTLE_ENDIAN)
//
// We can also nothrow construct from a double_limb_type (or smaller obviously) as long as double_limb_type is smaller than the type
// in question (so don't test 128-bit integers in case double_limb_type is __int128).
//
// Construct from int:
//
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::cpp_int(std::declval<boost::multiprecision::signed_double_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::int512_t(std::declval<boost::multiprecision::signed_double_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::checked_int512_t(std::declval<boost::multiprecision::signed_double_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::uint512_t(std::declval<boost::multiprecision::signed_double_limb_type>())));
BOOST_STATIC_ASSERT(!noexcept(boost::multiprecision::checked_uint512_t(std::declval<boost::multiprecision::signed_double_limb_type>())));
//
// Construct from unsigned int:
//
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::cpp_int(std::declval<boost::multiprecision::double_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::int512_t(std::declval<boost::multiprecision::double_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::checked_int512_t(std::declval<boost::multiprecision::double_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::uint512_t(std::declval<boost::multiprecision::double_limb_type>())));
BOOST_STATIC_ASSERT(noexcept(boost::multiprecision::checked_uint512_t(std::declval<boost::multiprecision::double_limb_type>())));
//
// Assign from int:
//
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::cpp_int>() = std::declval<boost::multiprecision::signed_double_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::int512_t>() = std::declval<boost::multiprecision::signed_double_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::checked_int512_t>() = std::declval<boost::multiprecision::signed_double_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::uint512_t>() = std::declval<boost::multiprecision::signed_double_limb_type>()));
BOOST_STATIC_ASSERT(!noexcept(std::declval<boost::multiprecision::checked_uint512_t>() = std::declval<boost::multiprecision::signed_double_limb_type>()));
//
// Assign from unsigned int:
//
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::cpp_int>() = std::declval<boost::multiprecision::double_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::int512_t>() = std::declval<boost::multiprecision::double_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::checked_int512_t>() = std::declval<boost::multiprecision::double_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::uint512_t>() = std::declval<boost::multiprecision::double_limb_type>()));
BOOST_STATIC_ASSERT(noexcept(std::declval<boost::multiprecision::checked_uint512_t>() = std::declval<boost::multiprecision::double_limb_type>()));

#endif // little endian
#endif // noexcept

