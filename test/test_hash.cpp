// Copyright John Maddock 2015.

// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef _MSC_VER
#  define _SCL_SECURE_NO_WARNINGS
#endif

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/debug_adaptor.hpp>
#include <boost/multiprecision/logged_adaptor.hpp>

#ifdef TEST_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#ifdef TEST_GMP
#include <boost/multiprecision/gmp.hpp>
#endif

#include <boost/functional/hash.hpp>

#include "test.hpp"
#include <iostream>
#include <iomanip>

template <class T>
void test()
{
   T val = 23;
   std::size_t t1 = boost::hash<T>()(val);
   BOOST_CHECK(t1);

#ifndef BOOST_NO_CXX11_HDR_FUNCTIONAL
   std::size_t t2 = std::hash<T>()(val);
   BOOST_CHECK_EQUAL(t1, t2);
#endif
}


int main()
{
   test<boost::multiprecision::cpp_int>();
   test<boost::multiprecision::checked_int1024_t>();
   test<boost::multiprecision::checked_uint512_t >();
   test<boost::multiprecision::number<boost::multiprecision::cpp_int_backend<64, 64, boost::multiprecision::unsigned_magnitude, boost::multiprecision::checked, void> > >();

   test<boost::multiprecision::cpp_bin_float_100>();
   test<boost::multiprecision::cpp_dec_float_100>();

   test<boost::multiprecision::cpp_rational>();

   test<boost::multiprecision::number<boost::multiprecision::debug_adaptor<boost::multiprecision::cpp_int::backend_type> > >();

   test<boost::multiprecision::number<boost::multiprecision::logged_adaptor<boost::multiprecision::cpp_int::backend_type> > >();

#ifdef TEST_FLOAT128
   //test<boost::multiprecision::float128>();
#endif
#ifdef TEST_GMP
   test<boost::multiprecision::mpz_int>();
#endif

   return boost::report_errors();
}

