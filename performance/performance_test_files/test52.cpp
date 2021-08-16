///////////////////////////////////////////////////////////////
//  Copyright 2019 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#include "../performance_test.hpp"
#if defined(TEST_CPP_DOUBLE_FLOAT)
#if defined(BOOST_MATH_USE_FLOAT128)
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/cpp_double_float.hpp>
#endif

void test52()
{
#ifdef TEST_CPP_DOUBLE_FLOAT
   using double_float_of_double_type = boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<double>, boost::multiprecision::et_off>;

   test<double_float_of_double_type>("cpp_double_float<double>", 1024*16);
#endif
}
