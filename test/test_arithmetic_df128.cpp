///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock.
//  Copyright 2021 - 2023 Christopher Kormanyos.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#if defined(_MSC_VER)
#define _SCL_SECURE_NO_WARNINGS
#endif

#ifdef BOOST_HAS_FLOAT128
#include <boost/multiprecision/cpp_double_fp.hpp>

#include "test_arithmetic.hpp"
#endif

int main()
{
#ifdef BOOST_HAS_FLOAT128
   using boost::multiprecision::cpp_double_float128;

   test<cpp_double_float128>();

   return boost::report_errors();
#endif
}
