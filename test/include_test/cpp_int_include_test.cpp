///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;

mp_number<cpp_int_backend<> > a;
mp_number<cpp_int_backend<>, false> b;
mp_number<cpp_int_backend<64, true, void>, false> c;
mp_number<cpp_int_backend<128, false, void>, false> d;
mp_number<cpp_int_backend<500, true, void>, false> e;


