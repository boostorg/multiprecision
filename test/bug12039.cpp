///////////////////////////////////////////////////////////////////////////////
//  Copyright 2016 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#include <boost/multiprecision/cpp_bin_float.hpp>

int main()
{
   typedef boost::multiprecision::number<boost::multiprecision::backends::cpp_bin_float<256> > ext_float_t;

   ext_float_t x = 5e15;
   x += 0.5;
   ext_float_t x1 = x + 255.0 / (1 << 20); // + 2^-12 - eps
   ext_float_t x2 = x + 257.0 / (1 << 20); // + 2^-12 + eps
   double d1 = x1.convert_to<double>();
   double d2 = x2.convert_to<double>();
   return d1 == d2 ? 0 : 1;
}

