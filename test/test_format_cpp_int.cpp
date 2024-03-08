///////////////////////////////////////////////////////////////////////////////
//  Copyright 2023 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/multiprecision/cpp_int.hpp>
#include "test_format.hpp"

int main()
{
   test_integer<boost::multiprecision::cpp_int>();
   test_integer<boost::multiprecision::uint128_t>();
   test_integer<boost::multiprecision::int256_t>();
   test_integer<boost::multiprecision::checked_int256_t>();
   return 0;
}
