/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "analyze.hpp"
#include <boost/multiprecision/mpfr.hpp>
#include <boost/math/quadrature/exp_sinh.hpp>
#include <boost/math/special_functions/gamma.hpp>

using boost::multiprecision::mpfr_float;
using boost::math::quadrature::exp_sinh;
using std::exp;
using std::log;

template<typename Real>
Real fransen_robinson()
{
    auto integrator = exp_sinh<Real>(18);
    auto f = [](Real t)->Real {
        Real y;
        try {
            y = boost::math::tgamma(t);
        } catch (std::exception const & e) {
            std::cout << "Problem at t= " << t << ":" << e.what() << "\n";
            return Real(0);
        }
        
        return 1/y;
    };
    Real Q = integrator.integrate(f);
    return Q;
}

int main()
{
    using Real = mpfr_float;
    int p = 500;
    mpfr_float::default_precision(p);
    std::cout << std::setprecision(p);
    Real F = fransen_robinson<Real>();
    std::cout << "Expected = 2.8077702420285\n";
    std::cout << "Computed = " <<F << "\n";
    std::string symbol = "F";
    analyze(F, symbol);
}
