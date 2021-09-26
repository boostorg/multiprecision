///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Christopher Kormanyos. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <ctime>
#include <random>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include "test.hpp"

namespace local
{
  template<typename DecimalType>
  void test_ceil_rounding()
  {
    static std::minstd_rand0 eng(static_cast<std::minstd_rand0::result_type>(std::clock()));

    static const std::uniform_int_distribution<unsigned> dist_sign
    (
       0U,
       1U
    );

    using decimal_type = DecimalType;

    bool result_is_ok = true;

    for(std::int32_t lo_index = 101U; lo_index < 1010U; lo_index += 7U)
    {
      for(std::int32_t hi_index = 10001U; hi_index < 100010U; hi_index += 17U)
      {
        const bool lo_is_neg = ((dist_sign(eng) % 2U) == 0U);
        const bool hi_is_neg = ((dist_sign(eng) % 2U) == 0U);

        const std::int32_t lo = ((lo_is_neg == false) ? lo_index : -lo_index);
        const std::int32_t hi = ((hi_is_neg == false) ? hi_index : -hi_index);

        const std::int32_t lo_hi = lo * hi;

        const decimal_type a = decimal_type { lo_hi } / decimal_type { lo };
        const decimal_type b = ceil(a);

        result_is_ok &= (static_cast<std::int32_t>(a) == b);

        if(result_is_ok == false)
        {
          break;
        }
      }
    }

    BOOST_CHECK_EQUAL(result_is_ok, true);
  }
}

int main(int, char**)
{
  bool result_is_ok = true;

  {
    using decimal = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<10>,
                                                  boost::multiprecision::et_off>;

    local::test_ceil_rounding<decimal>();
  }

  {
    using decimal = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<12>,
                                                  boost::multiprecision::et_off>;

    local::test_ceil_rounding<decimal>();
  }

   return boost::report_errors();
}
