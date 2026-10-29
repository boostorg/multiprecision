////////////////////////////////////////////////////////////////
//  Copyright 2026 Christopher Kormanyos.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <boost/core/lightweight_test.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace Mp = boost::multiprecision;
using namespace boost::multiprecision::literals;

using uint384_t = Mp::number<Mp::cpp_int_backend<384, 384, Mp::unsigned_magnitude, Mp::unchecked, void>, Mp::et_off>;
using int512_t = Mp::number<Mp::cpp_int_backend<512, 512, Mp::signed_magnitude, Mp::unchecked, void>, Mp::et_off>;

template <char... TStr>
constexpr auto operator""_u384() -> uint384_t
{
  return operator""_cppui384 < TStr...>();
}

template <char... TStr>
constexpr auto operator""_i512() -> int512_t
{
  return operator""_cppi512 < TStr...>();
}

auto main() -> int
{
  {
    constexpr auto value { 0x00000001111111111111111111111111111111111111111111111111111111111111111111111111111111111111_u384 };
    constexpr auto mask  { 0x0000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF_u384 };

    constexpr auto masked { value & mask };

    constexpr bool result_mask_is_ok { (value == masked) };
    constexpr bool result_size_is_ok { (value.backend().size() == masked.backend().size()) };

    static_assert(result_mask_is_ok, "Error: unexpected value results in non-equality");
    static_assert(result_size_is_ok, "Error: unexpected size results in non-equality");

    BOOST_TEST(result_mask_is_ok);
    BOOST_TEST(result_size_is_ok);
  }

  {
    constexpr auto value { 0x00000001111111111111111111111111111111111111111111111111111111111111111111111111111111111111_i512 };
    constexpr auto mask  { 0x0000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF_i512 };

    constexpr auto masked { value & mask };

    constexpr bool result_mask_is_ok { (value == masked) };
    constexpr bool result_size_is_ok { (value.backend().size() == masked.backend().size()) };

    static_assert(result_mask_is_ok, "Error: unexpected value results in non-equality");
    static_assert(result_size_is_ok, "Error: unexpected size results in non-equality");

    BOOST_TEST(result_mask_is_ok);
    BOOST_TEST(result_size_is_ok);
  }

  return boost::report_errors();
}
