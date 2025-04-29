// Copyright 2025 Christopher Kormanyos
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace local {

template<typename FloatType, typename LongIshType>
auto test_max_convert() -> void
{
  using float_type = FloatType;

  using float_array_type = std::array<float_type, std::size_t { UINT8_C(6) }>;

  const float_array_type factors =
  {
    float_type { "0.999" },
    float_type { "1.011" },
    float_type { "2.022" },
    float_type { "3.033" },
    float_type { "4.044" },
    float_type { "5.055" }
  };

  for(std::size_t i = std::size_t { UINT8_C(0) }; i < std::tuple_size<float_array_type>::value; ++i)
  {
    static_cast<void>(i);

    using long_type = LongIshType;

    const float_type
      flt_more_than_max_of_built_in_integral
      {
        float_type { (std::numeric_limits<long_type>::max)() } * factors[i]
      };

    const long_type conversion_result { static_cast<long_type>(flt_more_than_max_of_built_in_integral) };

    if(i == std::size_t { UINT8_C(0) })
    {
      BOOST_TEST(conversion_result < (std::numeric_limits<long_type>::max)());
    }
    else
    {
      BOOST_TEST(conversion_result == (std::numeric_limits<long_type>::max)());
    }
  }
}

} // namespace local

auto main() -> int
{
  local::test_max_convert<::boost::multiprecision::cpp_dec_float_50, unsigned long long>();

  local::test_max_convert<::boost::multiprecision::cpp_bin_float_50, unsigned long long>();

  return boost::report_errors();
}
