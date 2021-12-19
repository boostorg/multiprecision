///////////////////////////////////////////////////////////////
//  Copyright 2012 - 2021 John Maddock. 
//  Copyright 2021 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "standalone_test_arithmetic.hpp"

#define BOOST_MP_STANDALONE

#include <boost/multiprecision/float128.hpp>

int main()
{
   test<boost::multiprecision::float128>();
   return boost::report_errors();
}