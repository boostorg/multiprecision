///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock.
//  Copyright 2021 - 2023 Christopher Kormanyos.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#if defined(_MSC_VER)
#define _SCL_SECURE_NO_WARNINGS
#endif

#ifdef BOOST_MATH_USE_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/cpp_double_fp.hpp>

#include "test_arithmetic.hpp"

// cd /mnt/c/Users/User/Documents/Ks/PC_Software/Test
// g++-10 -O3 -Wall -Wextra -Wconversion -march=native -std=c++14   -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/test -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_81_0 test.cpp -o test.exe
// ./test_arithmetic.exe

// Handle interaction with Boost's wrap of libquadmath __float128.
// g++-10 -O3 -Wall -Wextra -Wconversion -march=native -std=gnu++14 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/test -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_81_0 -DBOOST_MATH_USE_FLOAT128 test.cpp -lquadmath -o test.exe

int main()
{
   using double_float_of_float_type    = boost::multiprecision::cpp_double_float;
   using double_float_of_double_type   = boost::multiprecision::cpp_double_double;
   using double_float_of_ldbl_type     = boost::multiprecision::cpp_double_long_double;
#ifdef BOOST_MATH_USE_FLOAT128
   using double_float_of_float128_type = boost::multiprecision::cpp_double_float128;
#endif

   test<double_float_of_float_type>();
   test<double_float_of_double_type>();
   test<double_float_of_ldbl_type>();
#ifdef BOOST_MATH_USE_FLOAT128
   test<double_float_of_float128_type>();
#endif
   return boost::report_errors();
}
