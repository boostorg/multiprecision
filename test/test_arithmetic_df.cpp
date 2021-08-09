///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#ifdef BOOST_MATH_USE_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/cpp_double_float.hpp>

#include "test_arithmetic.hpp"

// cd /mnt/c/Users/User/Documents/Ks/PC_Software/Test
// g++ -O3 -Wall -march=native -std=c++11 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/test -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 test.cpp -o test_arithmetic.exe
// ./test_arithmetic.exe

// Handle interaction with Boost's wrap of libquadmath __float128.
// g++ -O3 -Wall -march=native -std=gnu++11 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/test -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 -DBOOST_MATH_USE_FLOAT128 test.cpp -lquadmath -o test_arithmetic.exe

int main()
{
   using double_float_of_float_type    = boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<float>, boost::multiprecision::et_off>;
   using double_float_of_double_type   = boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<double>, boost::multiprecision::et_off>;
   using double_float_of_ldbl_type     = boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<long double>, boost::multiprecision::et_off>;
#ifdef BOOST_MATH_USE_FLOAT128
   using double_float_of_float128_type = boost::multiprecision::number<boost::multiprecision::backends::cpp_double_float<boost::multiprecision::float128>, boost::multiprecision::et_off>;
#endif

   test<double_float_of_float_type>();
   test<double_float_of_double_type>();
   test<double_float_of_ldbl_type>();
#ifdef BOOST_MATH_USE_FLOAT128
   test<double_float_of_float128_type>();
#endif
   return boost::report_errors();
}
