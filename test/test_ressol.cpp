//---------------------------------------------------------------------------//
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2018-2020 Pavel Kharitonov <ipavrus@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "test.hpp"

#if !defined(TEST_MPZ) && !defined(TEST_TOMMATH) && !defined(TEST_CPP_INT)
#define TEST_TOMMATH
#define TEST_MPZ
#define TEST_CPP_INT

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
#ifdef TEST_CPP_INT
#include <boost/multiprecision/cpp_int.hpp>
#endif

#include <boost/multiprecision/ressol.hpp>

template <typename T>
void test()
{
   using namespace boost::multiprecision;

   BOOST_CHECK_EQUAL(ressol(T(5), T(11)), 4);
   BOOST_CHECK_EQUAL(ressol(T(5), T("6864797660130609714981900799081393217269435300143305409394463459185543183397656052122559640661454554977296311391480858037121987999716643812574028291115057151")), T("5128001483797946816458955548662741861156429216952843873274631897232136999791540518339021539968609345897897688700798659762992302941280478805021587896033442584"));
   BOOST_CHECK_EQUAL(ressol(T(4), T("6864797660130609714981900799081393217269435300143305409394463459185543183397656052122559640661454554977296311391480858037121987999716643812574028291115057149")), -1);
   BOOST_CHECK_EQUAL(ressol(T("20749193632488214633180774027217139706413443729200940480695355894185"), T("26959946667150639794667015087019630673557916260026308143510066298881")), T("1825097171398375765346899906888660610489759292065918530856859649959"));
   BOOST_CHECK_EQUAL(ressol(T(64), T(85)), -1);
   BOOST_CHECK_EQUAL(ressol(T(181), T(217)), -1);
   BOOST_CHECK_EQUAL(ressol(T(4225), T(33153)), -1);
   BOOST_CHECK_EQUAL(ressol(T(2048), T(31417)), -1);
   BOOST_CHECK_EQUAL(ressol(T(2), T(4369)), -1);
   BOOST_CHECK_EQUAL(ressol(T(1024), T("0x40000000000000000000000000000000000000000000000000000000000c100000000000000ffff")), 32);
   BOOST_CHECK_EQUAL(ressol(T(1024), T(174763)), 174731);
   BOOST_CHECK_EQUAL(ressol(T(1025), T("0x40000000000000000000000000000000000000000000000000000000000c100000000000000ffff")), T("7195614950510915163755738138441999335431224576038191833055420996031360079131617522512565985187"));
   BOOST_CHECK_EQUAL(ressol(T(16), T("0x40000000000000000000000000000000000000000000000000000000000c100000000000000ffff")), 4);
   BOOST_CHECK_EQUAL(ressol(T(120846049), T("0x40000000000000000000000000000000000000000000000000000000000c100000000000000ffff")), T("0x40000000000000000000000000000000000000000000000000000000000c100000000000000d50e"));
}

int main()
{
#ifdef TEST_CPP_INT
   test<boost::multiprecision::cpp_int>();
#endif
#ifdef TEST_MPZ
   test<boost::multiprecision::mpz_int>();
#endif
#if defined(TEST_TOMMATH)
   test<boost::multiprecision::tom_int>();
#endif

   return boost::report_errors();
}
