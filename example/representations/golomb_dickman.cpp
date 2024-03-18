/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "analyze.hpp"
#include <boost/multiprecision/mpfr.hpp>
#include <boost/math/quadrature/tanh_sinh.hpp>
#include <boost/math/special_functions/expint.hpp>
#include <boost/math/constants/constants.hpp>
using boost::multiprecision::mpfr_float;
using boost::math::quadrature::tanh_sinh;
using std::exp;
using std::log;
using boost::math::expint;
using boost::math::constants::euler;

template<typename Real>
Real golomb_dickman()
{
    auto integrator = tanh_sinh<Real>(18);
    auto f = [](Real t) { return exp(expint(log(t)));};
    Real Q = integrator.integrate(f, Real(0), Real(1));
    return Q;
}

int main()
{
    using Real = mpfr_float;
    int p = 500;
    mpfr_float::default_precision(p);
    std::cout << std::setprecision(p);
    Real lambda = golomb_dickman<Real>();
    std::cout << "Expected from OEIS = 0.624329988543550870992936383100837244179642620180529286973551902495638088855113254462460276195539868869\n";
    std::cout << "Computed           = " << lambda << "\n";
    std::string symbol = "λ";
    analyze(lambda, symbol);

    lambda *= exp(-euler<Real>());
    symbol = "λexp(-𝛾)";
    analyze(lambda, symbol);

}