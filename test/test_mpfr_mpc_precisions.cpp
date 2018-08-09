///////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2002 - 2011.
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_
//
// This work is based on an earlier work:
// "Algorithm 910: A Portable C++ Multiple-Precision System for Special-Function Calculations",
// in ACM TOMS, {VOL 37, ISSUE 4, (February 2011)} (C) ACM, 2011. http://doi.acm.org/10.1145/1916461.1916469

#ifdef _MSC_VER
#  define _SCL_SECURE_NO_WARNINGS
#endif

#include <boost/detail/lightweight_test.hpp>
#include "test.hpp"

#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/mpc.hpp>

template <class T>
T make_rvalue_copy(const T a)
{
   return a;
}

int main()
{
   using namespace boost::multiprecision;
   //
   // Test change of default precision:
   //
   mpfr_float::default_precision(100);
   mpfr_float a("0.1");
   BOOST_CHECK_EQUAL(a.precision(), 100);
   mpfr_float::default_precision(20);
   {
      // test assignment from lvalue:
      mpfr_float b(2);
      BOOST_CHECK_EQUAL(b.precision(), 20);
      b = a;
      BOOST_CHECK_EQUAL(b.precision(), 20);
   }
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   {
      // test assignment from rvalue:
      mpfr_float b(2);
      BOOST_CHECK_EQUAL(b.precision(), 20);
      b = make_rvalue_copy(a);
      BOOST_CHECK_EQUAL(b.precision(), 100);
   }
#endif
   mpfr_float::default_precision(20);
   {
      // test construct from lvalue:
      mpfr_float b(a);
      BOOST_CHECK_EQUAL(b.precision(), 100);
   }
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   {
      // test construct from rvalue:
      mpfr_float b(make_rvalue_copy(a));
      BOOST_CHECK_EQUAL(b.precision(), 100);
   }
#endif
   mpc_complex::default_precision(100);
   mpc_complex ca("0.1");
   BOOST_CHECK_EQUAL(ca.precision(), 100);
   mpc_complex::default_precision(20);
   {
      // test assignment from lvalue:
      mpc_complex b(2);
      BOOST_CHECK_EQUAL(b.precision(), 20);
      b = ca;
      BOOST_CHECK_EQUAL(b.precision(), 20);
   }
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   {
      // test assignment from rvalue:
      mpc_complex b(2);
      BOOST_CHECK_EQUAL(b.precision(), 20);
      b = make_rvalue_copy(ca);
      BOOST_CHECK_EQUAL(b.precision(), 100);
   }
#endif
   {
      // test construct from lvalue:
      mpc_complex b(ca);
      BOOST_CHECK_EQUAL(b.precision(), 100);
   }
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   {
      // test construct from rvalue:
      mpc_complex b(make_rvalue_copy(ca));
      BOOST_CHECK_EQUAL(b.precision(), 100);
   }
#endif
   // real and imaginary:
   BOOST_CHECK_EQUAL(ca.real().precision(), 100);
   BOOST_CHECK_EQUAL(ca.imag().precision(), 100);
   BOOST_CHECK_EQUAL(real(ca).precision(), 100);
   BOOST_CHECK_EQUAL(imag(ca).precision(), 100);
   // BOOST_CHECK_EQUAL(mpc_complex(conj(ca)).precision(), 100);
   // What happens to arithmetic if the precision has changed??
   //BOOST_CHECK_EQUAL(norm(ca).precision(), 100);


   return boost::report_errors();
}



