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

#if !defined(TEST_MPF) && !defined(TEST_MPFR) && !defined(TEST_MPFI) && !defined(TEST_MPC)
#define TEST_MPF
#define TEST_MPFR
#define TEST_MPFI
#define TEST_MPC

#ifdef _MSC_VER
#pragma message("CAUTION!!: No backend type specified so testing everything.... this will take some time!!")
#endif
#ifdef __GNUC__
#pragma warning "CAUTION!!: No backend type specified so testing everything.... this will take some time!!"
#endif

#endif

#if defined(TEST_MPFR)
#include <boost/multiprecision/mpfr.hpp>
#endif
#if defined(TEST_MPF)
#include <boost/multiprecision/gmp.hpp>
#endif
#if defined(TEST_MPFI)
#include <boost/multiprecision/mpfi.hpp>
#endif
#if defined(TEST_MPC)
#include <boost/multiprecision/mpc.hpp>
#endif


template <class T>
void test()
{
   T::thread_default_precision(100);
   T::thread_default_variable_precision_options(boost::multiprecision::variable_precision_options::preserve_target_precision);

   T hp1("0.1"), hp2("0.3"), hp3("0.11"), hp4("0.1231");

   BOOST_CHECK_EQUAL(hp1.precision(), 100);
   BOOST_CHECK_EQUAL(hp2.precision(), 100);

   T::thread_default_precision(35);

   T a(hp1);
   BOOST_CHECK_EQUAL(a.precision(), 35);
   a = hp1;
   BOOST_CHECK_EQUAL(a.precision(), 35);
   a = std::move(hp1);
   BOOST_CHECK_EQUAL(a.precision(), 35);
   T b(std::move(hp2));
   BOOST_CHECK_EQUAL(b.precision(), 35);

   a = b + hp3;
   BOOST_CHECK_EQUAL(a.precision(), 35);
   a = hp3 * b;
   BOOST_CHECK_EQUAL(a.precision(), 35);
   a += hp3;
   BOOST_CHECK_EQUAL(a.precision(), 35);
   a *= hp4;
   BOOST_CHECK_EQUAL(a.precision(), 35);
   a -= b * hp3;
   BOOST_CHECK_EQUAL(a.precision(), 35);

   if constexpr (!std::is_same_v<T, typename T::value_type>)
   {
      //
      // If we have a component type: ie we are an interval or a complex number, then
      // operations involving the component type should match those of T:
      //
      using component_t = typename T::value_type;
      component_t::thread_default_precision(100);
      component_t::thread_default_variable_precision_options(boost::multiprecision::variable_precision_options::preserve_source_precision);

      component_t cp1("0.1"), cp2("0.3"), cp3("0.11"), cp4("0.1231");

      BOOST_CHECK_EQUAL(cp1.precision(), 100);
      BOOST_CHECK_EQUAL(cp2.precision(), 100);

      T::thread_default_precision(35);

      T aa(cp1);
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      T cc(cp1, cp2);
      BOOST_CHECK_EQUAL(cc.precision(), 35);
      T dd(cp1, cp2, 20);
      BOOST_CHECK_EQUAL(dd.precision(), 20);
      aa = cp1;
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      aa = std::move(cp1);
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      T bb(std::move(cp2));
      BOOST_CHECK_EQUAL(bb.precision(), 35);

      aa = bb + cp3;
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      aa = cp3 * bb;
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      aa += cp3;
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      aa -= cp3;
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      aa *= cp4;
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      aa /= cp4;
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      aa -= bb * cp3;
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      
      aa.assign(cp1);
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      aa.assign(cp1, cp2);
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      aa.assign(cp1, cp2, 20);
      BOOST_CHECK_EQUAL(aa.precision(), 20);
   }
   else
   {
      T aa(hp4, 20);
      BOOST_CHECK_EQUAL(aa.precision(), 20);
      aa.precision(35);
      aa.assign(hp4);
      BOOST_CHECK_EQUAL(aa.precision(), 35);
      aa.assign(hp4, 20);
      BOOST_CHECK_EQUAL(aa.precision(), 20);
   }
}

int main()
{
#ifdef TEST_MPF
   test<boost::multiprecision::mpf_float>();
   test<boost::multiprecision::number<boost::multiprecision::gmp_float<0>, boost::multiprecision::et_off>>();
#endif
#ifdef TEST_MPFR
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
