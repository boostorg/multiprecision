///////////////////////////////////////////////////////////////
//  Copyright 2021 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt
//

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include <thread>
#include <boost/detail/lightweight_test.hpp>
#include <boost/array.hpp>
#include <boost/math/special_functions/relative_difference.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <boost/math/quadrature/tanh_sinh.hpp>
#include "test.hpp"

#if !defined(TEST_MPF_50) && !defined(TEST_MPFR_50) && !defined(TEST_MPFI) && !defined(TEST_MPC)
#define TEST_MPF_50
#define TEST_MPFR_50
#define TEST_MPFI
#define TEST_MPC

#ifdef _MSC_VER
#pragma message("CAUTION!!: No backend type specified so testing everything.... this will take some time!!")
#endif
#ifdef __GNUC__
#pragma warning "CAUTION!!: No backend type specified so testing everything.... this will take some time!!"
#endif

#endif

#if defined(TEST_MPFR_50)
#include <boost/multiprecision/mpfr.hpp>
#endif
#if defined(TEST_MPF_50)
#include <boost/multiprecision/gmp.hpp>
#endif
#if defined(TEST_MPFI)
#include <boost/multiprecision/mpfi.hpp>
#endif
#if defined(TEST_MPC)
#include <boost/multiprecision/mpc.hpp>
#endif

template <class T>
T new_value()
{
   return T("0.1");
}


template <class T>
void test()
{
   T::thread_default_precision(100);
   T::thread_default_variable_precision_options(boost::multiprecision::variable_precision_options::preserve_source_precision);

   T hp1("0.1"), hp2("0.3"), hp3("0.11"), hp4("0.1231");

   BOOST_CHECK_EQUAL(hp1.precision(), 100);
   BOOST_CHECK_EQUAL(hp2.precision(), 100);

   T::thread_default_precision(35);

   T a(hp1);
   BOOST_CHECK_EQUAL(a.precision(), 100);
   a = new_value<T>();
   BOOST_CHECK_EQUAL(a.precision(), 35);
   a = hp1;
   BOOST_CHECK_EQUAL(a.precision(), 100);
   a = new_value<T>();
   BOOST_CHECK_EQUAL(a.precision(), 35);
   a = std::move(hp1);
   BOOST_CHECK_EQUAL(a.precision(), 100);
   a = new_value<T>();
   BOOST_CHECK_EQUAL(a.precision(), 35);
   T b(std::move(hp2));
   BOOST_CHECK_EQUAL(b.precision(), 100);
   b = new_value<T>();
   BOOST_CHECK_EQUAL(b.precision(), 35);

   a = b + hp3;
   BOOST_CHECK_EQUAL(a.precision(), 100);
   a = new_value<T>();
   BOOST_CHECK_EQUAL(a.precision(), 35);
   a = hp3 * b;
   BOOST_CHECK_EQUAL(a.precision(), 100);
   a = new_value<T>();
   BOOST_CHECK_EQUAL(a.precision(), 35);
   a += hp3;
   BOOST_CHECK_EQUAL(a.precision(), 100);
   a = new_value<T>();
   BOOST_CHECK_EQUAL(a.precision(), 35);
   a *= hp4;
   BOOST_CHECK_EQUAL(a.precision(), 100);
   a = new_value<T>();
   BOOST_CHECK_EQUAL(a.precision(), 35);
   a -= b * hp3;
   BOOST_CHECK_EQUAL(a.precision(), 100);
   a = new_value<T>();
   BOOST_CHECK_EQUAL(a.precision(), 35);
}

int main()
{
#ifdef TEST_MPF_50
   test<boost::multiprecision::mpf_float>();
   test<boost::multiprecision::number<boost::multiprecision::gmp_float<0>, boost::multiprecision::et_off>>();
#endif
#ifdef TEST_MPFR_50
   test<boost::multiprecision::mpfr_float>();
   test<boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<0>, boost::multiprecision::et_off> >();
#endif
#ifdef TEST_MPFI
   test<boost::multiprecision::mpfi_float>();
   test<boost::multiprecision::number<boost::multiprecision::mpfi_float_backend<0>, boost::multiprecision::et_off> >();
#endif
#ifdef TEST_MPC
   test<boost::multiprecision::mpc_complex>();
   test<boost::multiprecision::number<boost::multiprecision::mpc_complex_backend<0>, boost::multiprecision::et_off> >();
#endif
   return boost::report_errors();
}
