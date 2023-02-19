///////////////////////////////////////////////////////////////////////////////
//  Copyright 2023 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/multiprecision/tommath.hpp>
#include "test_format.hpp"


int main()
{
   test_integer<boost::multiprecision::tom_int>();
   return 0;
}
