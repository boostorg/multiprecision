///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Basic I/O tests for cpp_double_float<>
// Note that the I/O of cpp_double_float<> is currently extremely underdeveloped

#include <boost/multiprecision/cpp_double_float.hpp>
#include <iostream>
#include <cstdlib>

// Test compilation, basic operatory
template <typename FloatingPointType>
void test_basic_io()
{
   using double_float_t = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;

   std::cout << "Testing cpp_double_float< " << typeid(FloatingPointType).name() << " >...\n"
             << std::endl;

   double_float_t a, b;

   std::cout << "a = ";
   std::cin >> a;
   std::cout << "b = ";
   std::cin >> b;

   std::cout.precision(std::numeric_limits<FloatingPointType>::digits10 * 2);

   std::cout << "a: " << a << "\tb: " << b << std::endl;
   std::cout << std::endl;

   std::cout << "a + b = " << a + b << std::endl;
   std::cout << "a - b = " << a - b << std::endl;
   std::cout << "a * b = " << a * b << std::endl;
   std::cout << "a / b = " << a / b << std::endl;
   std::cout << std::endl;

   std::cout << "a += b";
   a += b;
   std::cout << " (a = " << a << ")" << std::endl;

   std::cout << "a -= b";
   a -= b;
   std::cout << " (a = " << a << ")" << std::endl;

   std::cout << "a *= b";
   a *= b;
   std::cout << " (a = " << a << ")" << std::endl;

   std::cout << "a /= b";
   a /= b;
   std::cout << " (a = " << a << ")" << std::endl;

   std::cout << std::endl;

   std::cout << "a   = " << a << std::endl;
   std::cout << "++a = " << ++a << " (a = " << a << ")" << std::endl;
   std::cout << "--a = " << --a << " (a = " << a << ")" << std::endl;
   std::cout << "a++ = " << a++ << " (a = " << a << ")" << std::endl;
   std::cout << "a-- = " << a-- << " (a = " << a << ")" << std::endl;
   std::cout << std::endl;

   std::cout << "     a =  " << a << std::endl;
   std::cout << "    -a = " << -a << std::endl;
   std::cout << "a + -a =  " << a + -a << std::endl;

   std::cout << std::endl;

   std::cout << "(a / b) * (b / a) = ";
   std::cout << (a / b) * (b / a) << std::endl;

   std::cout << std::endl;
}

int  main()
{
   test_basic_io<double>();
   test_basic_io<float >();

  return 0;
}