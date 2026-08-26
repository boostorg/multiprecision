//  (C) Copyright Marco Ribeiro 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint>
#include <limits>
#include <boost/config.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include "test.hpp"


#if !defined(BOOST_MP_NO_CONSTEXPR_DETECTION) \
  && !defined(DISABLE_TESTS) \
  && defined(BOOST_MP_HAS_CONSTEXPR_DYNAMIC_ALLOC)

typedef boost::multiprecision::number<
  boost::multiprecision::cpp_int_backend<0, 32>,
  boost::multiprecision::et_off
> int_backend_32;

typedef boost::multiprecision::number<
  boost::multiprecision::cpp_int_backend<0, 64>,
  boost::multiprecision::et_off
> int_backend_64;

typedef boost::multiprecision::number<
  boost::multiprecision::cpp_int_backend<0, 128>,
  boost::multiprecision::et_off
> int_backend_128;

typedef boost::multiprecision::number<
  boost::multiprecision::cpp_int_backend<0, 256>,
  boost::multiprecision::et_off
> int_backend_256;

typedef boost::multiprecision::number<
  boost::multiprecision::cpp_int_backend<0, 512>,
  boost::multiprecision::et_off
> int_backend_512;

typedef boost::multiprecision::number<
  boost::multiprecision::cpp_int_backend<>,
  boost::multiprecision::et_off
> int_backend_inf;

typedef boost::multiprecision::number<
  boost::multiprecision::cpp_int_backend<0, 32>,
  boost::multiprecision::et_on
> int_backend_32_et;

typedef boost::multiprecision::number<
  boost::multiprecision::cpp_int_backend<0, 512>,
  boost::multiprecision::et_on
> int_backend_512_et;

typedef boost::multiprecision::number<
  boost::multiprecision::cpp_int_backend<>,
  boost::multiprecision::et_on
> int_backend_inf_et;

template <typename T>
constexpr T factorial(T n)
{
  T result = 1;

  for (T k = 2; k <= n; ++k)
    result *= k;

  return result;
}

template <typename T>
constexpr T fibonacci(T n)
{
  T a = 0;
  T b = 1;

  for (T k = 0; k < n; ++k)
  {
    T c = a + b;
    a = std::move(b);
    b = std::move(c);
  }

  return a;
}

inline int run_constexpr_cpp_int_tests()
{
  constexpr std::int32_t i32_min = (std::numeric_limits<std::int32_t>::min)();
  constexpr std::uint32_t u32_max = (std::numeric_limits<std::uint32_t>::max)();

  {
    // construction from integral
    static_assert(int_backend_32(u32_max) == u32_max);
    static_assert(int_backend_64(u32_max) == u32_max);
    static_assert(int_backend_128(u32_max) == u32_max);
    static_assert(int_backend_256(u32_max) == u32_max);
    static_assert(int_backend_512(u32_max) == u32_max);
    static_assert(int_backend_inf(u32_max) == u32_max);
    static_assert(int_backend_32_et(u32_max) == u32_max);
    static_assert(int_backend_512_et(u32_max) == u32_max);
    static_assert(int_backend_inf_et(u32_max) == u32_max);

    static_assert(int_backend_32(i32_min) == i32_min);
    static_assert(int_backend_64(i32_min) == i32_min);
    static_assert(int_backend_128(i32_min) == i32_min);
    static_assert(int_backend_256(i32_min) == i32_min);
    static_assert(int_backend_512(i32_min) == i32_min);
    static_assert(int_backend_inf(i32_min) == i32_min);
    static_assert(int_backend_32_et(i32_min) == i32_min);
    static_assert(int_backend_512_et(i32_min) == i32_min);
    static_assert(int_backend_inf_et(i32_min) == i32_min);
  }
  {
    // construction from floating point
#   if defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202306L
    static_assert(int_backend_32(12345.12345) == 12345);
    static_assert(int_backend_64(12345.12345) == 12345);
    static_assert(int_backend_128(12345.12345) == 12345);
    static_assert(int_backend_256(12345.12345) == 12345);
    static_assert(int_backend_512(12345.12345) == 12345);
    static_assert(int_backend_inf(12345.12345) == 12345);
    static_assert(int_backend_32_et(12345.12345) == 12345);
    static_assert(int_backend_512_et(12345.12345) == 12345);
    static_assert(int_backend_inf_et(12345.12345) == 12345);

    static_assert(int_backend_32(-12345.12345) == -12345);
    static_assert(int_backend_64(-12345.12345) == -12345);
    static_assert(int_backend_128(-12345.12345) == -12345);
    static_assert(int_backend_256(-12345.12345) == -12345);
    static_assert(int_backend_512(-12345.12345) == -12345);
    static_assert(int_backend_32_et(-12345.12345) == -12345);
    static_assert(int_backend_512_et(-12345.12345) == -12345);
    static_assert(int_backend_inf_et(-12345.12345) == -12345);
#   endif
  }
  {
    // construction from string
    static_assert(int_backend_32("12345678") == 12345678);
    static_assert(int_backend_32("-12345678") == -12345678);
    static_assert(int_backend_32("0xb0057") == 720983);
    static_assert(int_backend_32("-0xb0057") == -720983);
    static_assert(int_backend_32("0777") == 511);
    static_assert(int_backend_32("-0777") == -511);

    static_assert(int_backend_64("12345678") == 12345678);
    static_assert(int_backend_64("-12345678") == -12345678);
    static_assert(int_backend_64("0xb0057") == 720983);
    static_assert(int_backend_64("-0xb0057") == -720983);
    static_assert(int_backend_64("0777") == 511);
    static_assert(int_backend_64("-0777") == -511);

    static_assert(int_backend_128("12345678") == 12345678);
    static_assert(int_backend_128("-12345678") == -12345678);
    static_assert(int_backend_128("0xb0057") == 720983);
    static_assert(int_backend_128("-0xb0057") == -720983);
    static_assert(int_backend_128("0777") == 511);
    static_assert(int_backend_128("-0777") == -511);

    static_assert(int_backend_256("12345678") == 12345678);
    static_assert(int_backend_256("-12345678") == -12345678);
    static_assert(int_backend_256("0xb0057") == 720983);
    static_assert(int_backend_256("-0xb0057") == -720983);
    static_assert(int_backend_256("0777") == 511);
    static_assert(int_backend_256("-0777") == -511);

    static_assert(int_backend_512("12345678") == 12345678);
    static_assert(int_backend_512("-12345678") == -12345678);
    static_assert(int_backend_512("0xb0057") == 720983);
    static_assert(int_backend_512("-0xb0057") == -720983);
    static_assert(int_backend_512("0777") == 511);
    static_assert(int_backend_512("-0777") == -511);

    static_assert(int_backend_inf("12345678") == 12345678);
    static_assert(int_backend_inf("-12345678") == -12345678);
    static_assert(int_backend_inf("0xb0057") == 720983);
    static_assert(int_backend_inf("-0xb0057") == -720983);
    static_assert(int_backend_inf("0777") == 511);
    static_assert(int_backend_inf("-0777") == -511);

    static_assert(int_backend_32_et("12345678") == 12345678);
    static_assert(int_backend_32_et("-12345678") == -12345678);
    static_assert(int_backend_32_et("0xb0057") == 720983);
    static_assert(int_backend_32_et("-0xb0057") == -720983);
    static_assert(int_backend_32_et("0777") == 511);
    static_assert(int_backend_32_et("-0777") == -511);

    static_assert(int_backend_512_et("12345678") == 12345678);
    static_assert(int_backend_512_et("-12345678") == -12345678);
    static_assert(int_backend_512_et("0xb0057") == 720983);
    static_assert(int_backend_512_et("-0xb0057") == -720983);
    static_assert(int_backend_512_et("0777") == 511);
    static_assert(int_backend_512_et("-0777") == -511);

    static_assert(int_backend_inf_et("12345678") == 12345678);
    static_assert(int_backend_inf_et("-12345678") == -12345678);
    static_assert(int_backend_inf_et("0xb0057") == 720983);
    static_assert(int_backend_inf_et("-0xb0057") == -720983);
    static_assert(int_backend_inf_et("0777") == 511);
    static_assert(int_backend_inf_et("-0777") == -511);
  }
  {
    // construction from cpp_int
    static_assert(int_backend_32(int_backend_32(12345678)) == 12345678);
    static_assert(int_backend_32(int_backend_64(12345678)) == 12345678);
    static_assert(int_backend_32(int_backend_128(12345678)) == 12345678);
    static_assert(int_backend_32(int_backend_256(12345678)) == 12345678);
    static_assert(int_backend_32(int_backend_512(12345678)) == 12345678);
    static_assert(int_backend_32(int_backend_inf(12345678)) == 12345678);
    static_assert(int_backend_32(int_backend_32_et(12345678)) == 12345678);
    static_assert(int_backend_32(int_backend_512_et(12345678)) == 12345678);
    static_assert(int_backend_32(int_backend_inf_et(12345678)) == 12345678);
    static_assert(int_backend_32(boost::multiprecision::uint1024_t(12345678)) == 12345678);
    static_assert(int_backend_32(boost::multiprecision::int1024_t(12345678)) == 12345678);
    static_assert(int_backend_32(boost::multiprecision::int1024_t(-12345678)) == -12345678);

    static_assert(int_backend_64(int_backend_32(12345678)) == 12345678);
    static_assert(int_backend_64(int_backend_64(12345678)) == 12345678);
    static_assert(int_backend_64(int_backend_128(12345678)) == 12345678);
    static_assert(int_backend_64(int_backend_256(12345678)) == 12345678);
    static_assert(int_backend_64(int_backend_512(12345678)) == 12345678);
    static_assert(int_backend_64(int_backend_inf(12345678)) == 12345678);
    static_assert(int_backend_64(int_backend_32_et(12345678)) == 12345678);
    static_assert(int_backend_64(int_backend_512_et(12345678)) == 12345678);
    static_assert(int_backend_64(int_backend_inf_et(12345678)) == 12345678);
    static_assert(int_backend_64(boost::multiprecision::uint1024_t(12345678)) == 12345678);
    static_assert(int_backend_64(boost::multiprecision::int1024_t(12345678)) == 12345678);
    static_assert(int_backend_64(boost::multiprecision::int1024_t(-12345678)) == -12345678);

    static_assert(int_backend_128(int_backend_32(12345678)) == 12345678);
    static_assert(int_backend_128(int_backend_64(12345678)) == 12345678);
    static_assert(int_backend_128(int_backend_128(12345678)) == 12345678);
    static_assert(int_backend_128(int_backend_256(12345678)) == 12345678);
    static_assert(int_backend_128(int_backend_512(12345678)) == 12345678);
    static_assert(int_backend_128(int_backend_inf(12345678)) == 12345678);
    static_assert(int_backend_128(int_backend_32_et(12345678)) == 12345678);
    static_assert(int_backend_128(int_backend_512_et(12345678)) == 12345678);
    static_assert(int_backend_128(int_backend_inf_et(12345678)) == 12345678);
    static_assert(int_backend_128(boost::multiprecision::uint1024_t(12345678)) == 12345678);
    static_assert(int_backend_128(boost::multiprecision::int1024_t(12345678)) == 12345678);
    static_assert(int_backend_128(boost::multiprecision::int1024_t(-12345678)) == -12345678);

    static_assert(int_backend_256(int_backend_32(12345678)) == 12345678);
    static_assert(int_backend_256(int_backend_64(12345678)) == 12345678);
    static_assert(int_backend_256(int_backend_128(12345678)) == 12345678);
    static_assert(int_backend_256(int_backend_256(12345678)) == 12345678);
    static_assert(int_backend_256(int_backend_512(12345678)) == 12345678);
    static_assert(int_backend_256(int_backend_inf(12345678)) == 12345678);
    static_assert(int_backend_256(int_backend_32_et(12345678)) == 12345678);
    static_assert(int_backend_256(int_backend_512_et(12345678)) == 12345678);
    static_assert(int_backend_256(int_backend_inf_et(12345678)) == 12345678);
    static_assert(int_backend_256(boost::multiprecision::uint1024_t(12345678)) == 12345678);
    static_assert(int_backend_256(boost::multiprecision::int1024_t(12345678)) == 12345678);
    static_assert(int_backend_256(boost::multiprecision::int1024_t(-12345678)) == -12345678);

    static_assert(int_backend_512(int_backend_32(12345678)) == 12345678);
    static_assert(int_backend_512(int_backend_64(12345678)) == 12345678);
    static_assert(int_backend_512(int_backend_128(12345678)) == 12345678);
    static_assert(int_backend_512(int_backend_256(12345678)) == 12345678);
    static_assert(int_backend_512(int_backend_512(12345678)) == 12345678);
    static_assert(int_backend_512(int_backend_inf(12345678)) == 12345678);
    static_assert(int_backend_512(int_backend_32_et(12345678)) == 12345678);
    static_assert(int_backend_512(int_backend_512_et(12345678)) == 12345678);
    static_assert(int_backend_512(int_backend_inf_et(12345678)) == 12345678);
    static_assert(int_backend_512(boost::multiprecision::uint1024_t(12345678)) == 12345678);
    static_assert(int_backend_512(boost::multiprecision::int1024_t(12345678)) == 12345678);
    static_assert(int_backend_512(boost::multiprecision::int1024_t(-12345678)) == -12345678);

    static_assert(int_backend_inf(int_backend_32(12345678)) == 12345678);
    static_assert(int_backend_inf(int_backend_64(12345678)) == 12345678);
    static_assert(int_backend_inf(int_backend_128(12345678)) == 12345678);
    static_assert(int_backend_inf(int_backend_256(12345678)) == 12345678);
    static_assert(int_backend_inf(int_backend_512(12345678)) == 12345678);
    static_assert(int_backend_inf(int_backend_inf(12345678)) == 12345678);
    static_assert(int_backend_inf(int_backend_32_et(12345678)) == 12345678);
    static_assert(int_backend_inf(int_backend_512_et(12345678)) == 12345678);
    static_assert(int_backend_inf(int_backend_inf_et(12345678)) == 12345678);
    static_assert(int_backend_inf(boost::multiprecision::uint1024_t(12345678)) == 12345678);
    static_assert(int_backend_inf(boost::multiprecision::int1024_t(12345678)) == 12345678);
    static_assert(int_backend_inf(boost::multiprecision::int1024_t(-12345678)) == -12345678);

    static_assert(int_backend_32_et(int_backend_32(12345678)) == 12345678);
    static_assert(int_backend_32_et(int_backend_64(12345678)) == 12345678);
    static_assert(int_backend_32_et(int_backend_128(12345678)) == 12345678);
    static_assert(int_backend_32_et(int_backend_256(12345678)) == 12345678);
    static_assert(int_backend_32_et(int_backend_512(12345678)) == 12345678);
    static_assert(int_backend_32_et(int_backend_inf(12345678)) == 12345678);
    static_assert(int_backend_32_et(int_backend_32_et(12345678)) == 12345678);
    static_assert(int_backend_32_et(int_backend_512_et(12345678)) == 12345678);
    static_assert(int_backend_32_et(int_backend_inf_et(12345678)) == 12345678);
    static_assert(int_backend_32_et(boost::multiprecision::uint1024_t(12345678)) == 12345678);
    static_assert(int_backend_32_et(boost::multiprecision::int1024_t(12345678)) == 12345678);
    static_assert(int_backend_32_et(boost::multiprecision::int1024_t(-12345678)) == -12345678);

    static_assert(int_backend_512_et(int_backend_32(12345678)) == 12345678);
    static_assert(int_backend_512_et(int_backend_64(12345678)) == 12345678);
    static_assert(int_backend_512_et(int_backend_128(12345678)) == 12345678);
    static_assert(int_backend_512_et(int_backend_256(12345678)) == 12345678);
    static_assert(int_backend_512_et(int_backend_512(12345678)) == 12345678);
    static_assert(int_backend_512_et(int_backend_inf(12345678)) == 12345678);
    static_assert(int_backend_512_et(int_backend_32_et(12345678)) == 12345678);
    static_assert(int_backend_512_et(int_backend_512_et(12345678)) == 12345678);
    static_assert(int_backend_512_et(int_backend_inf_et(12345678)) == 12345678);
    static_assert(int_backend_512_et(boost::multiprecision::uint1024_t(12345678)) == 12345678);
    static_assert(int_backend_512_et(boost::multiprecision::int1024_t(12345678)) == 12345678);
    static_assert(int_backend_512_et(boost::multiprecision::int1024_t(-12345678)) == -12345678);

    static_assert(int_backend_inf_et(int_backend_32(12345678)) == 12345678);
    static_assert(int_backend_inf_et(int_backend_64(12345678)) == 12345678);
    static_assert(int_backend_inf_et(int_backend_128(12345678)) == 12345678);
    static_assert(int_backend_inf_et(int_backend_256(12345678)) == 12345678);
    static_assert(int_backend_inf_et(int_backend_512(12345678)) == 12345678);
    static_assert(int_backend_inf_et(int_backend_inf(12345678)) == 12345678);
    static_assert(int_backend_inf_et(int_backend_32_et(12345678)) == 12345678);
    static_assert(int_backend_inf_et(int_backend_512_et(12345678)) == 12345678);
    static_assert(int_backend_inf_et(int_backend_inf_et(12345678)) == 12345678);
    static_assert(int_backend_inf_et(boost::multiprecision::uint1024_t(12345678)) == 12345678);
    static_assert(int_backend_inf_et(boost::multiprecision::int1024_t(12345678)) == 12345678);
    static_assert(int_backend_inf_et(boost::multiprecision::int1024_t(-12345678)) == -12345678);
  }
  {
    // cast to integral
    static_assert(static_cast<std::uint32_t>(int_backend_32(12345678)) == 12345678U);
    static_assert(static_cast<std::uint32_t>(int_backend_64(12345678)) == 12345678U);
    static_assert(static_cast<std::uint32_t>(int_backend_128(12345678)) == 12345678U);
    static_assert(static_cast<std::uint32_t>(int_backend_256(12345678)) == 12345678U);
    static_assert(static_cast<std::uint32_t>(int_backend_512(12345678)) == 12345678U);
    static_assert(static_cast<std::uint32_t>(int_backend_inf(12345678)) == 12345678U);
    static_assert(static_cast<std::uint32_t>(int_backend_32_et(12345678)) == 12345678U);
    static_assert(static_cast<std::uint32_t>(int_backend_512_et(12345678)) == 12345678U);
    static_assert(static_cast<std::uint32_t>(int_backend_inf_et(12345678)) == 12345678U);

    static_assert(static_cast<std::int32_t>(int_backend_32(12345678)) == 12345678);
    static_assert(static_cast<std::int32_t>(int_backend_64(12345678)) == 12345678);
    static_assert(static_cast<std::int32_t>(int_backend_128(12345678)) == 12345678);
    static_assert(static_cast<std::int32_t>(int_backend_256(12345678)) == 12345678);
    static_assert(static_cast<std::int32_t>(int_backend_512(12345678)) == 12345678);
    static_assert(static_cast<std::int32_t>(int_backend_inf(12345678)) == 12345678);
    static_assert(static_cast<std::int32_t>(int_backend_32_et(12345678)) == 12345678);
    static_assert(static_cast<std::int32_t>(int_backend_512_et(12345678)) == 12345678);
    static_assert(static_cast<std::int32_t>(int_backend_inf_et(12345678)) == 12345678);

    static_assert(static_cast<std::int32_t>(int_backend_32(-12345678)) == -12345678);
    static_assert(static_cast<std::int32_t>(int_backend_64(-12345678)) == -12345678);
    static_assert(static_cast<std::int32_t>(int_backend_128(-12345678)) == -12345678);
    static_assert(static_cast<std::int32_t>(int_backend_256(-12345678)) == -12345678);
    static_assert(static_cast<std::int32_t>(int_backend_512(-12345678)) == -12345678);
    static_assert(static_cast<std::int32_t>(int_backend_inf(-12345678)) == -12345678);
    static_assert(static_cast<std::int32_t>(int_backend_32_et(-12345678)) == -12345678);
    static_assert(static_cast<std::int32_t>(int_backend_512_et(-12345678)) == -12345678);
    static_assert(static_cast<std::int32_t>(int_backend_inf_et(-12345678)) == -12345678);
  }
  {
#   if defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202306L
    // cast to floating point only works for values that fit on a limb_type
    static_assert(static_cast<double>(int_backend_32(12345678)) == 12345678.0);
    static_assert(static_cast<double>(int_backend_64(12345678)) == 12345678.0);
    static_assert(static_cast<double>(int_backend_128(12345678)) == 12345678.0);
    static_assert(static_cast<double>(int_backend_256(12345678)) == 12345678.0);
    static_assert(static_cast<double>(int_backend_512(12345678)) == 12345678.0);
    static_assert(static_cast<double>(int_backend_inf(12345678)) == 12345678.0);
    static_assert(static_cast<double>(int_backend_32_et(12345678)) == 12345678.0);
    static_assert(static_cast<double>(int_backend_512_et(12345678)) == 12345678.0);
    static_assert(static_cast<double>(int_backend_inf_et(12345678)) == 12345678.0);

    static_assert(static_cast<double>(int_backend_32(-12345678)) == -12345678.0);
    static_assert(static_cast<double>(int_backend_64(-12345678)) == -12345678.0);
    static_assert(static_cast<double>(int_backend_128(-12345678)) == -12345678.0);
    static_assert(static_cast<double>(int_backend_256(-12345678)) == -12345678.0);
    static_assert(static_cast<double>(int_backend_512(-12345678)) == -12345678.0);
    static_assert(static_cast<double>(int_backend_inf(-12345678)) == -12345678.0);
    static_assert(static_cast<double>(int_backend_32_et(-12345678)) == -12345678.0);
    static_assert(static_cast<double>(int_backend_512_et(-12345678)) == -12345678.0);
    static_assert(static_cast<double>(int_backend_inf_et(-12345678)) == -12345678.0);

    static_assert(static_cast<double>(int_backend_128("295147905179352825856")) == 295147905179352825856.0);
    static_assert(static_cast<double>(int_backend_256("295147905179352825856")) == 295147905179352825856.0);
    static_assert(static_cast<double>(int_backend_512("295147905179352825856")) == 295147905179352825856.0);
    static_assert(static_cast<double>(int_backend_inf("295147905179352825856")) == 295147905179352825856.0);
    static_assert(static_cast<double>(int_backend_512_et("295147905179352825856")) == 295147905179352825856.0);
    static_assert(static_cast<double>(int_backend_inf_et("295147905179352825856")) == 295147905179352825856.0);
#   endif
  }
  {
    // bit functions
    auto const test_bit_functions = [] <typename T> (T&&) constexpr noexcept {
      static_assert(boost::multiprecision::msb(T(1) << 500) == 500);
      static_assert(boost::multiprecision::lsb(T(1) << 500) == 500);
      static_assert(boost::multiprecision::bit_test(T(1) << 500, 500));

      static_assert(boost::multiprecision::msb(int_backend_inf_et(1) << 500) == 500);
      static_assert(boost::multiprecision::lsb(int_backend_inf_et(1) << 500) == 500);
      static_assert(boost::multiprecision::bit_test(int_backend_inf_et(1) << 500, 500));

      static_assert([] () constexpr noexcept -> T {
        T a = 0;
        return boost::multiprecision::bit_set(a, 500);
      }() == (T(1) << 500));

      static_assert([] () constexpr noexcept -> T {
        T a = T(1) << 500;
        return boost::multiprecision::bit_set(a, 500);
      }() == (T(1) << 500));

      static_assert([] () constexpr noexcept -> T {
        T a = 0;
        return boost::multiprecision::bit_unset(a, 500);
      }() == 0);

      static_assert([] () constexpr noexcept -> T {
        T a = T(1) << 500;
        return boost::multiprecision::bit_unset(a, 500);
      }() == 0);

      static_assert([] () constexpr noexcept -> T {
        T a = 0;
        return boost::multiprecision::bit_flip(a, 500);
      }() == (T(1) << 500));

      static_assert([] () constexpr noexcept -> T {
        T a = T(1) << 500;
        return boost::multiprecision::bit_flip(a, 500);
      }() == 0);

      return true;
    };

    static_assert(test_bit_functions(int_backend_512{}));
    static_assert(test_bit_functions(int_backend_inf{}));
    static_assert(test_bit_functions(int_backend_512_et{}));
    static_assert(test_bit_functions(int_backend_inf_et{}));
  }
  {
    // math operations
    auto const test_math_operations = [] <typename T> (T&&) constexpr noexcept {
      static_assert([] () constexpr noexcept -> T {
        T const a((std::numeric_limits<std::uint32_t>::max)());
        T const b = -(a * a * a * a * a * a * a * a * a) + (a * a) - (a * a * a);
        T q;
        T r;

        boost::multiprecision::divide_qr(b, T(5), q, r);
        return ((q + r) + 1) * -3;
      }() == T(
        "298393941220594703994056741294897077225167327209541862571731534281447031079324380613832"
      ));

      return true;
    };

    static_assert(test_math_operations(int_backend_512{}));
    static_assert(test_math_operations(int_backend_512_et{}));
    static_assert(test_math_operations(int_backend_inf{}));
    static_assert(test_math_operations(int_backend_inf_et{}));
  }
  {
    // boost integral functions
    auto const test_boost_integral_functions = [] <typename T> (T&&) constexpr noexcept {
      static_assert(
        boost::multiprecision::gcd(
          T("19045691428043967582979989657743215704243597051686049733150514095808"),
          T("13707112918842075981093200133901249713363763549144813434262669433146")
        ) == 2
      );

      static_assert(
        boost::multiprecision::sqrt(
          T("19045691428043967582979989657743215704243597051686049733150514095808")
        ) == T("4364136962567051192681890285117644")
      );

      static_assert(
        boost::multiprecision::integer_modulus(
          T("19045691428043967582979989657743215704243597051686049733150514095808"),
          u32_max
        ) == 2377187843
      );

      static_assert(
        boost::multiprecision::abs(
          T("19045691428043967582979989657743215704243597051686049733150514095808")
        ) == T("19045691428043967582979989657743215704243597051686049733150514095808")
      );

      static_assert(
        boost::multiprecision::abs(
          T("-19045691428043967582979989657743215704243597051686049733150514095808")
        ) == T("19045691428043967582979989657743215704243597051686049733150514095808")
      );

      static_assert(
        boost::multiprecision::powm(
          T(123), 95, T("347524627690950719232228793293134097314")
        ) == T("139046631754840920912974963240540036501")
      );

      return true;
    };

    static_assert(test_boost_integral_functions(int_backend_512{}));
    static_assert(test_boost_integral_functions(int_backend_inf{}));
    static_assert(test_boost_integral_functions(int_backend_512_et{}));
    static_assert(test_boost_integral_functions(int_backend_inf_et{}));
  }
  {
    // handwritten functions
    static_assert(
      factorial(int_backend_512(50))
      == int_backend_512("30414093201713378043612608166064768844377641568960512000000000000")
    );

    static_assert(
      factorial(int_backend_inf(50))
      == int_backend_inf("30414093201713378043612608166064768844377641568960512000000000000")
    );

    static_assert(
      factorial(int_backend_512_et(50))
      == int_backend_512_et("30414093201713378043612608166064768844377641568960512000000000000")
    );

    static_assert(
      factorial(int_backend_inf_et(50))
      == int_backend_inf_et("30414093201713378043612608166064768844377641568960512000000000000")
    );

    static_assert(factorial(int_backend_32(12)) == 479001600);
    static_assert(factorial(int_backend_32_et(12)) == 479001600);

    static_assert(
      fibonacci(int_backend_512(200))
      == int_backend_512("280571172992510140037611932413038677189525")
    );

    static_assert(
      fibonacci(int_backend_inf(200))
      == int_backend_inf("280571172992510140037611932413038677189525")
    );

    static_assert(
      fibonacci(int_backend_512_et(200))
      == int_backend_512_et("280571172992510140037611932413038677189525")
    );

    static_assert(
      fibonacci(int_backend_inf_et(200))
      == int_backend_inf_et("280571172992510140037611932413038677189525")
    );

    static_assert(fibonacci(int_backend_32(47)) == 2971215073);
    static_assert(fibonacci(int_backend_32_et(47)) == 2971215073);
  }
  {
    // limits
    static_assert((std::numeric_limits<int_backend_32>::min)() == -int_backend_32(u32_max));
    static_assert((std::numeric_limits<int_backend_32>::max)() == u32_max);

    static_assert((std::numeric_limits<int_backend_32_et>::min)() == -int_backend_32_et(u32_max));
    static_assert((std::numeric_limits<int_backend_32_et>::max)() == u32_max);

    // limits
    static_assert(
      (std::numeric_limits<int_backend_512>::min)()
      == int_backend_512(
        "-0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      )
    );

    static_assert(
      (std::numeric_limits<int_backend_512>::max)()
      == int_backend_512(
        "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      )
    );

    static_assert(
      (std::numeric_limits<int_backend_512_et>::min)()
      == int_backend_512_et(
        "-0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      )
    );

    static_assert(
      (std::numeric_limits<int_backend_512_et>::max)()
      == int_backend_512_et(
        "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
      )
    );
  }

  return boost::report_errors();
}
#else
inline int run_constexpr_cpp_int_tests(){
  return 0;
}
#endif
