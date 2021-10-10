///////////////////////////////////////////////////////////////
//  Copyright 2012-2021 John Maddock. Distributed under the Boost
//  Copyright 2021 Matt Borland.
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#define BOOST_MP_STANDALONE

#include <cstdint>
#include <random>
#include <limits>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/math/special_functions/prime.hpp>
#include "test.hpp"

template <class Integer>
void test()
{
   std::mt19937 gen;

   //
   // Test the primes in our table as all these should return true:
   //
   for (unsigned i = 1; i < boost::math::max_prime; ++i)
   {
      BOOST_TEST(boost::multiprecision::miller_rabin_test(Integer(boost::math::prime(i)), 25, gen));
   }
}

int main()
{
    test<int>();
    test<unsigned>();
    test<long>();
    test<unsigned long>();
    test<long long>();
    test<unsigned long long>();

    return boost::report_errors();
}
