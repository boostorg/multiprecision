///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Christopher Kormanyos. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/multiprecision/cpp_dec_float.hpp>

#include "test.hpp"

namespace local
{
  template<typename DecimalType>
  void test_ceil_rounding()
  {
    using decimal_type = DecimalType;

    bool result_is_ok = true;

    for(std::uint32_t lo_index = 101U; lo_index < 1010U; lo_index += 7U)
    {
      for(std::uint32_t hi_index = 10001U; hi_index < 100010U; hi_index += 17U)
      {
        const std::uint32_t lo_hi = lo_index * hi_index;

        const decimal_type a = decimal_type { lo_hi } / decimal_type { lo_index };
        const decimal_type b = ceil(a);

        const std::string str_a = boost::lexical_cast<std::string>(a);
        const std::string str_b = boost::lexical_cast<std::string>(b);

        result_is_ok &= (str_a == str_b);

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
