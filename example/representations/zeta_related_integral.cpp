// Copyright Nick Thompson, 2020
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)
#include "analyze.hpp"
#include <boost/math/quadrature/tanh_sinh.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/multiprecision/mpfr.hpp>

using std::log;
using std::sin;
using std::abs;
using boost::math::quadrature::tanh_sinh;
using boost::multiprecision::mpfr_float;
using boost::math::constants::pi;
using boost::math::constants::zeta_three;

int main() {
    using Real = mpfr_float;
    mpfr_float::default_precision(1000);
    Real x = -(7*zeta_three<Real>() - pi<Real>()*pi<Real>()*log(static_cast<Real>(4)))/16;;
    std::string symbol = "(-7zeta(3) + pi^2ln(4))/16";
    analyze(x, symbol);
    return 0;
}