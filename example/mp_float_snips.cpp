///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#include <boost/multiprecision/mp_float.hpp>
#include <iostream>

void t1()
{
   //[mp_float_eg
   //=#include <boost/multiprecision/mp_float.hpp>

   using namespace boost::multiprecision;

   // Operations at fixed precision and full numeric_limits support:
   mp_float_100 b = 2;
   std::cout << std::numeric_limits<mp_float_100>::digits << std::endl;
   std::cout << log(b) << std::endl; // print log(2)
   //]
}

int main()
{
   t1();
   return 0;
}

