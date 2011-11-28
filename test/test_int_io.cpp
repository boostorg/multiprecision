// Copyright John Maddock 2011.

// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef _MSC_VER
#  define _SCL_SECURE_NO_WARNINGS
#endif

#if !defined(TEST_MPZ) && !defined(TEST_TOMMATH)
#  define TEST_TOMMATH
#  define TEST_MPZ

#ifdef _MSC_VER
#pragma message("CAUTION!!: No backend type specified so testing everything.... this will take some time!!")
#endif
#ifdef __GNUC__
#pragma warning "CAUTION!!: No backend type specified so testing everything.... this will take some time!!"
#endif

#endif

#if defined(TEST_MPZ)
#include <boost/multiprecision/gmp.hpp>
#endif
#if defined(TEST_TOMMATH)
#include <boost/multiprecision/tommath.hpp>
#endif

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include "test.hpp"
#include <iostream>
#include <iomanip>

template <class T>
T generate_random()
{
   static boost::random::uniform_int_distribution<unsigned> ui(0, 20);
   static boost::random::mt19937 gen;
   T val = gen();
   unsigned lim = ui(gen);
   for(unsigned i = 0; i < lim; ++i)
   {
      val *= (gen.max)();
      val += gen();
   }
   return val;
}

template <class T>
void do_round_trip(const T& val, std::ios_base::fmtflags f)
{
   std::stringstream ss;
   ss << std::setprecision(std::numeric_limits<T>::max_digits10);
   ss.flags(f);
   ss << val;
   T new_val = ss.str();
   BOOST_CHECK_EQUAL(new_val, val);
   new_val = val.str(0, f);
   BOOST_CHECK_EQUAL(new_val, val);
}

template <class T>
void do_round_trip(const T& val)
{
   do_round_trip(val, std::ios_base::fmtflags(0));
   if(val >= 0)
   {
      do_round_trip(val, std::ios_base::fmtflags(std::ios_base::showbase|std::ios_base::hex));
      do_round_trip(val, std::ios_base::fmtflags(std::ios_base::showbase|std::ios_base::oct));
   }
}

template <class T>
void test_round_trip()
{
   for(unsigned i = 0; i < 1000; ++i)
   {
      T val = generate_random<T>();
      do_round_trip(val);
      do_round_trip(T(-val));
   }

   BOOST_CHECK_EQUAL(T(1002).str(), "1002");
   BOOST_CHECK_EQUAL(T(1002).str(0, std::ios_base::showpos), "+1002");
   BOOST_CHECK_EQUAL(T(-1002).str(), "-1002");
   BOOST_CHECK_EQUAL(T(1002).str(0, std::ios_base::oct), "1752");
   BOOST_CHECK_EQUAL(T(1002).str(0, std::ios_base::oct|std::ios_base::showbase), "01752");
   BOOST_CHECK_EQUAL(boost::to_lower_copy(T(1002).str(0, std::ios_base::hex)), "3ea");
   BOOST_CHECK_EQUAL(boost::to_lower_copy(T(1002).str(0, std::ios_base::hex|std::ios_base::showbase)), "0x3ea");
   BOOST_CHECK_EQUAL(T(1002).str(0, std::ios_base::dec), "1002");
   BOOST_CHECK_EQUAL(T(1002).str(0, std::ios_base::dec|std::ios_base::showbase), "1002");

   BOOST_CHECK_THROW(T(-2).str(0, std::ios_base::oct), std::runtime_error);
   BOOST_CHECK_THROW(T(-2).str(0, std::ios_base::hex), std::runtime_error);
}

int main()
{
#ifdef TEST_MPZ
   test_round_trip<boost::multiprecision::mpz_int>();
#endif
#ifdef TEST_TOMMATH
   test_round_trip<boost::multiprecision::mp_int>();
#endif
   return boost::report_errors();
}

