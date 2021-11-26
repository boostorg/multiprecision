///////////////////////////////////////////////////////////////
//  Copyright 2012-2021 John Maddock. 
//  Copyright 2021 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#include <boost/config.hpp>

#define BOOST_MP_STANDALONE

#ifdef BOOST_HAS_FLOAT128
#include <boost/multiprecision/complex128.hpp>
#endif

#include "libs/multiprecision/test/test_arithmetic.hpp"

int main()
{
#ifdef BOOST_HAS_FLOAT128
   test<boost::multiprecision::complex128>();
#endif
   return boost::report_errors();
}
