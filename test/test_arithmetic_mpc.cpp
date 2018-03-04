///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#include <boost/multiprecision/mpc.hpp>

#include "libs/multiprecision/test/test_arithmetic.hpp"

int main()
{
   test<boost::multiprecision::mpc_float_50>();
   //test<boost::multiprecision::number<boost::multiprecision::cpp_bin_float<21> > >();
   //test<boost::multiprecision::number<boost::multiprecision::cpp_bin_float<1000, boost::multiprecision::digit_base_10, std::allocator<char> > > >();
   //test<boost::multiprecision::cpp_bin_float_quad>();
   return boost::report_errors();
}

