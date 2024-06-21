///////////////////////////////////////////////////////////////
//  Copyright 2024 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include "test.hpp"

template <class T>
void test_divide_by_zero()
{
    T val = 42;

    BOOST_CHECK_THROW(val % 0, std::overflow_error);
    BOOST_CHECK_THROW(val % static_cast<T>(0), std::overflow_error);
    BOOST_CHECK_THROW(val % static_cast<std::uintmax_t>(0), std::overflow_error);
    BOOST_CHECK_THROW(val % static_cast<std::intmax_t>(0), std::overflow_error);

    val = -val;
    BOOST_CHECK_THROW(val % 0, std::overflow_error);
    BOOST_CHECK_THROW(val % static_cast<T>(0), std::overflow_error);
    BOOST_CHECK_THROW(val % static_cast<std::uintmax_t>(0), std::overflow_error);
    BOOST_CHECK_THROW(val % static_cast<std::intmax_t>(0), std::overflow_error);
}

int main()
{
    using int64_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<64, 64, boost::multiprecision::signed_magnitude>>;

    test_divide_by_zero<int64_t>();
    test_divide_by_zero<boost::multiprecision::int128_t>();
    test_divide_by_zero<boost::multiprecision::int256_t>();
    test_divide_by_zero<boost::multiprecision::int1024_t>();
}

