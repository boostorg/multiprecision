///////////////////////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/multiprecision/cpp_int.hpp>

#ifndef BOOST_NO_CONSTEXPR

template <class T>
void test1()
{
   constexpr T i1 = 2u;
   constexpr T i2;
   constexpr T i3 = -3;
   constexpr T i4(i1);
}
template <class T>
void test2()
{
   constexpr T i1 = 2u;
   constexpr T i2;
   constexpr T i3 = -3;
}
template <class T>
void test3()
{
   constexpr T i1 = 2u;
   constexpr T i2;
}

using namespace boost::multiprecision;

template void test1<number<backends::cpp_int_backend<64, false, void, true>, et_off> >();
template void test1<number<backends::cpp_int_backend<64, true, void, true>, et_off> >();
template void test3<number<backends::cpp_int_backend<2048, false, void>, et_off> >();
template void test2<number<backends::cpp_int_backend<2048, true, void>, et_off> >();

#endif