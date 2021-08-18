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

#include <boost/config.hpp>
#include <boost/multiprecision/number.hpp>
#ifdef BOOST_MATH_USE_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/cpp_double_float.hpp>
#include <iostream>
#include <cstdlib>

// Test compilation, basic operatory
template <typename FloatingPointType>
void test_basic_io_manual()
{
   using double_float_t = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;

   std::cout << "Testing cpp_double_float< " << typeid(FloatingPointType).name() << " >...\n"
             << std::endl;

   double_float_t a, b;
   char           c;

   std::cout << "a = ";
   double d;
   std::cin >> d;
   a = FloatingPointType(d);
   //std::cin >> a;
   b = (a * FloatingPointType(std::rand())) / FloatingPointType(std::rand());

   int p;
   std::cout << "Precision: ";
   std::cin >> p;
   std::cout.precision(p);

   std::cout << "(f)ixed/(s)cientific/(d)efault: ";
   std::cin >> c;
   switch (c)
   {
   case 'f':
      std::cout.setf(std::ios::fixed);
      break;
   case 's':
      std::cout.setf(std::ios::scientific);
      break;
   case 'd':
      std::cout.unsetf(std::ios::floatfield);
   default:
      break;
   }

   std::cout << "showpoint (y/n): ";
   std::cin >> c;
   if (c == 'y')
      std::cout.setf(std::ios::showpoint);
   else if (c == 'n')
      std::cout.unsetf(std::ios::showpoint);

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

int main()
{
   test_basic_io_manual<boost::multiprecision::backends::cpp_double_float<double>>();
   test_basic_io_manual < boost::multiprecision::backends::cpp_double_float<float>>();
   //test_basic_io_manual < boost::multiprecision::backends::cpp_double_float<long double>>();
#ifdef BOOST_MATH_USE_FLOAT128
// FIXME:
// test_basic_io_manual<boost::multiprecision::float128>();
#endif

   return 0;
}
