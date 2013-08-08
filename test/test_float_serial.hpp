///////////////////////////////////////////////////////////////
//  Copyright 2013 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/timer.hpp>
#include "test.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#ifndef BOOST_MP_TEST_FLOAT_SERIAL_HPP
#define BOOST_MP_TEST_FLOAT_SERIAL_HPP

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
      ss.clear();
      boost::archive::binary_oarchive ba(ss);
      ba << static_cast<const T&>(val);
      boost::archive::binary_iarchive ib(ss);
      ib >> val2;
      BOOST_CHECK_EQUAL(val, val2);
      
      val = -val;
      ss.clear();
      boost::archive::text_oarchive oa2(ss);
      oa2 << static_cast<const T&>(val);
      boost::archive::text_iarchive ia2(ss);
      ia2 >> val2;
      BOOST_CHECK_EQUAL(val, val2);
      ss.clear();
      boost::archive::binary_oarchive ba2(ss);
      ba2 << static_cast<const T&>(val);
      boost::archive::binary_iarchive ib2(ss);
      ib2 >> val2;
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

#endif