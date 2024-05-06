///////////////////////////////////////////////////////////////////////////////
//  Copyright 2024 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/multiprecision/complex.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/core/lightweight_test.hpp>
#include <complex>
#include <cmath>

using namespace boost::multiprecision;

template <typename T>
void test_construction()
{
    using std::complex;
    using std::polar;
    using complex_scalar = decltype(polar(T(), T()));
}

int main()
{
    test_construction<float>();
    test_construction<double>();
    test_construction<cpp_bin_float_50>();

    return boost::report_errors();
}
