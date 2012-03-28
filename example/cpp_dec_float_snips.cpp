///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/special_functions/gamma.hpp>
#include <iostream>

void t1()
{
   //[cpp_dec_float_eg
   //=#include <boost/multiprecision/cpp_dec_float.hpp>

   using namespace boost::multiprecision;

   // Operations at fixed precision and full numeric_limits support:
   cpp_dec_float_100 b = 2;
   std::cout << std::numeric_limits<cpp_dec_float_100>::digits << std::endl;
   // We can use any C++ std lib function:
   std::cout << log(b) << std::endl; // print log(2)
   // We can also use any function from Boost.Math:
   std::cout << boost::math::tgamma(b) << std::endl;
   // These even work when the argument is an expression template:
   std::cout << boost::math::tgamma(b * b) << std::endl;
   //]
}

int main()
{
   t1();
   return 0;
}

