///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/math/special_functions/prime.hpp>
#include <iostream>
#include <iomanip>
#include "test.hpp"

int main()
{
   //
   // Very simple test program to verify that the GMP's Miller-Rabin
   // implementation and this one agree on whether some random numbers
   // are prime or not.  Of course these are probabilistic tests so there's
   // no reason why they should actually agree - except the probability of
   // disagreement for 25 trials is almost infinitely small.
   //
   using namespace boost::random;
   using namespace boost::multiprecision;

   independent_bits_engine<mt11213b, 256, mpz_int> gen;
   //
   // We must use a different generator for the tests and number generation, otherwise
   // we get false positives.  Further we use the same random number engine for the
   // Miller Rabin test as GMP uses internally:
   //
   mt19937 gen2;

   //
   // Begin by testing the primes in our table as all these should return true:
   //
   for(unsigned i = 1; i < boost::math::max_prime; ++i)
   {
      BOOST_TEST(miller_rabin_test(mpz_int(boost::math::prime(i)), 25, gen));
   }
   //
   // Now test some random values and compare GMP's native routine with ours.
   //
   for(unsigned i = 0; i < 10000; ++i)
   {
      mpz_int n = gen();
      bool is_prime_boost = miller_rabin_test(n, 25, gen2);
      bool is_gmp_prime = mpz_probab_prime_p(n.backend().data(), 25);
      if(is_prime_boost && is_gmp_prime)
      {
         std::cout << "We have a prime: " << std::hex << std::showbase << n << std::endl;
      }
      if(is_prime_boost != is_gmp_prime)
         std::cout << std::hex << std::showbase << "n = " << n << std::endl;
      BOOST_CHECK_EQUAL(is_prime_boost, is_gmp_prime);
   }
   return 0;
}


