///////////////////////////////////////////////////////////////
//  Copyright 2013 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

//
// Compare arithmetic results using fixed_int to GMP results.
//

#ifdef _MSC_VER
#  define _SCL_SECURE_NO_WARNINGS
#endif

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/timer.hpp>
#include "test.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

template <class T>
T generate_random(unsigned bits_wanted)
{
   typedef typename T::backend_type::exponent_type e_type;
   static boost::random::mt19937 gen;
   T val = gen();
   T prev_val = -1;
   while(val != prev_val)
   {
      val *= (gen.max)();
      prev_val = val;
      val += gen();
   }
   e_type e;
   val = frexp(val, &e);

   static boost::random::uniform_int_distribution<e_type> ui(std::numeric_limits<T>::min_exponent + 1, std::numeric_limits<T>::max_exponent - 1);
   return ldexp(val, ui(gen));
}

template <class T>
void test()
{
   boost::timer tim;

   while(true)
   {
      T val = generate_random<T>(boost::math::tools::digits<T>());
      std::stringstream ss;
      boost::archive::text_oarchive oa(ss);
      oa << static_cast<const T&>(val);
      boost::archive::text_iarchive ia(ss);
      T val2;
      ia >> val2;
      BOOST_CHECK_EQUAL(val, val2);
      //
      // Check to see if test is taking too long.
      // Tests run on the compiler farm time out after 300 seconds,
      // so don't get too close to that:
      //
      if(tim.elapsed() > 150)
      {
         std::cout << "Timeout reached, aborting tests now....\n";
         break;
      }
   }
}

#if !defined(TEST1) && !defined(TEST2)
#  define TEST1
#  define TEST2
#endif


int main()
{
   using namespace boost::multiprecision;
#ifdef TEST1
   test<cpp_dec_float_50>();
#endif
#ifndef TEST2
   test<number<cpp_dec_float<100, boost::int64_t, std::allocator<void> > > >();
#endif
   return boost::report_errors();
}



