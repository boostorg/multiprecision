///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#include <boost/multiprecision/fixed_int.hpp>
#include <iostream>

void t1()
{
   //[fixed_int_eg
   //=#include <boost/multiprecision/fixed_int.hpp>

   using namespace boost::multiprecision;

   mp_int128_t v = 1;

   // Do some arithmetic:
   for(unsigned i = 1; i <= 20; ++i)
      v *= i;

   std::cout << v << std::endl; // prints 20!
   v = -v;
   std::cout << std::hex << v << std::endl; // prints 2's complement representation

   //]
}

int main()
{
   t1();
   return 0;
}

