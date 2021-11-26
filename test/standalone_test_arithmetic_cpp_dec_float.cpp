///////////////////////////////////////////////////////////////
//  Copyright 2012 - 2021 John Maddock.
//  Copyright 2021 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#include "test_arithmetic.hpp"

#define BOOST_MP_STANDALONE

#include <boost/multiprecision/cpp_dec_float.hpp>

template <unsigned D>
struct related_type<boost::multiprecision::number<boost::multiprecision::cpp_dec_float<D>>>
{
   using type = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<D / 2>>;
};

int main()
{
   test<boost::multiprecision::cpp_dec_float_50>();
   return boost::report_errors();
}
