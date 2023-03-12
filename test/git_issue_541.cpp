///////////////////////////////////////////////////////////////////////////////
//  Copyright 2023 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <iostream>

using Float = boost::multiprecision::cpp_dec_float_50;

struct F
{
   Float f;
   F(const Float& f)
       : f(f)
   {}
};

void f(F) {}



int main()
{
#if BOOST_CXX_VERSION >= 201703L
   //
   // An expression template should be implicitly convertible to
   // anything that the number type is, we only have a fix for this
   // in C++17 and later which is super annoying, but the way that it is...
   //
   Float f1{2};
   f(f1 * 2);
   f(2 + f1);
   F ff = f1 * 2;
#endif
}
