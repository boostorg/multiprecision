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

   //
   // Construction at specific precision:
   //
   {
      mpfr_float f150(mpfr_float(), 150u);
      BOOST_CHECK_EQUAL(f150.precision(), 150);
   }
   {
      mpfr_float f150(2, 150);
      BOOST_CHECK_EQUAL(f150.precision(), 150);
   }
   {
      mpfr_float f150("1.2", 150);
      BOOST_CHECK_EQUAL(f150.precision(), 150);
   }
   //
   // Copying precision:
   //
   {
      mpc_complex c(ca.backend().data());
      BOOST_CHECK_EQUAL(c.precision(), 100);
      mpc_complex_100 c100(2);
      mpc_complex d(c100);
      BOOST_CHECK_EQUAL(d.precision(), 100);
      mpfr_float_100 f100(2);
      mpc_complex e(f100);
      BOOST_CHECK_EQUAL(d.precision(), 100);
   }
   //
   // Check that the overloads for precision don't mess up 2-arg
   // construction:
   //
   {
      mpc_complex c(2, 3u);
      BOOST_CHECK_EQUAL(c.real(), 2);
      BOOST_CHECK_EQUAL(c.imag(), 3);
   }
   //
   // 3-arg complex number construction with 3rd arg a precision:
   //
   {
      mpc_complex c(2, 3, 100);
      BOOST_CHECK_EQUAL(c.precision(), 100);
   }
   //
   // From https://github.com/boostorg/multiprecision/issues/65
   //
   {
      mpfr_float a(2);
      a.precision(100);
      BOOST_CHECK_EQUAL(a, 2);
      BOOST_CHECK_EQUAL(a.precision(), 100);
   }
   {
      mpc_complex a(2, 3);
      a.precision(100);
      BOOST_CHECK_EQUAL(a.real(), 2);
      BOOST_CHECK_EQUAL(a.imag(), 3);
      BOOST_CHECK_EQUAL(a.precision(), 100);
   }



   return boost::report_errors();
}



