///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/complex_adaptor.hpp>

#include "libs/multiprecision/test/test_arithmetic.hpp"
/*
template <unsigned D>
struct related_type<boost::multiprecision::number< boost::multiprecision::mpc_complex_backend<D> > >
{
   typedef boost::multiprecision::number< boost::multiprecision::mpfr_float_backend<D> > type;
};
*/

int main()
{
   test<boost::multiprecision::number<boost::multiprecision::complex_adaptor<boost::multiprecision::cpp_bin_float<50> > > >();
   return boost::report_errors();
}

