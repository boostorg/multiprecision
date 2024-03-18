///////////////////////////////////////////////////////////////////////////////
//  Copyright 2020 Nick Thompson.
//  Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/multiprecision/representations.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

using boost::math::constants::gauss;


int main() {
    using boost::multiprecision::mpfr_float;
    using boost::multiprecision::representations;
    mpfr_float::default_precision(100);
    auto G = boost::math::constants::gauss<mpfr_float>();
    representations<mpfr_float> r(G, "G");

    //using Real = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<400>>;
    //auto G = gauss<Real>();
    //representations<Real> r(G, "G");
    std::cout << r << "\n";
}
