/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "../../math/test/math_unit_test.hpp"
#include <cmath>
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/pslq.hpp>

using std::pow;
using boost::multiprecision::is_algebraic;

// A test: A := 2^1/6 + 3^1/5
// From: Applications of integer relation algorithms, Jonathan M. Borwein, Petr Lisonek, Discrete Mathematics 217 (2000) 65{82


// Getting algebraic numbers: https://www.davidhbailey.com/dhbpapers/pslq-cse.pdf
// B3=3.54409035955···[1], then B3 is the root of 4913 + 2108t^2 − 604t^3 − 977t^4 + 8t^5 + 44t^6 + 392t^7 − 193t^8 − 40t^9 + 48t^10 − 12t^11 + t^12


// This is test 1 from:
// https://www.davidhbailey.com/dhbpapers/mpfun2015.pdf
template<typename Real>
void test_degree_30_algebraic()
{
    Real x = pow(3, Real(1)/Real(5)) - pow(2, Real(1)/Real(6));
    // DHB's code requires 240 decimal digits.
    std::pair<Real, std::string> p{x,  "{3^1/5 - 2^1/6}"};
    auto b = boost::multiprecision::is_algebraic(p, Real(1e10));
    if (!b.empty()) {
        std::cout << "It is algebraic.\n";
    }
}

// This is test 3 from:
// https://www.davidhbailey.com/dhbpapers/mpfun2015.pdf
template<typename Real>
void test_degree_56_algebraic()
{
    Real x = pow(3, Real(1)/Real(7)) - pow(2, Real(1)/Real(8));
    // DHB's code requires 640 decimal digits.
    std::pair<Real, std::string> p{x,  "{3^1/7 - 2^1/8}"};
    auto b = boost::multiprecision::is_algebraic(p, Real(1e10));
    if (!b.empty()) {
        std::cout << "It is algebraic.\n";
    }
}

template<typename Real>
void test_degree_72_algebraic()
{
    Real x = pow(3, Real(1)/Real(8)) - pow(2, Real(1)/Real(9));
    // DHB's code requires 1100 decimal digits.
    std::pair<Real, std::string> p{x,  "{3^1/8 - 2^1/9}"};
    auto b = boost::multiprecision::is_algebraic(p, Real(1e10));
    if (!b.empty()) {
        std::cout << "It is algebraic.\n";
    }
}


int main() {
    using Real = boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<1000> >;
    test_degree_30_algebraic<Real>();
    test_degree_56_algebraic<Real>();
    test_degree_72_algebraic<Real>();
    return boost::math::test::report_errors();
}