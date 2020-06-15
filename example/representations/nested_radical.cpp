/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <boost/multiprecision/pslq.hpp>
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/representations.hpp>

using std::abs;
using boost::multiprecision::identify;
using boost::multiprecision::is_algebraic;
using boost::multiprecision::mpfr_float;
using boost::multiprecision::representations;

// This example comes from Steven R. Finch's book "Mathematical Constants", Section 1.2.1.
// Ideally, we'd know how many terms to use just on the precision of the Real number.
// But we don't know that!
template<typename Real>
Real nested_123(int64_t max_n)
{
    assert(max_n > 1);
    using std::sqrt;
    Real r = sqrt(max_n);
    for (int64_t n = max_n - 1; n >= 1; --n) {
        r = sqrt(n + r);
    }
    return r;
}

int main()
{
    using Real = mpfr_float;
    int p = 1000;
    mpfr_float::default_precision(p);
    int n = 10000;
    Real x1 = nested_123<Real>(n/2);
    std::cout << std::setprecision(p);
    Real x2 = nested_123<Real>(n);
    std::string symbol = "√(1 + √(2 + √(3 +...)";
    Real delta = abs(x1 - x2);
    if (delta > 0) {
        std::cerr << std::scientific << "delta = " << abs(x2 - x1) << "\n";
        std::cerr << "n must be increased to get all the bits correct.\n";
        return 1;
    }


    auto r = representations(x2, symbol);
    std::cout << r << "\n";
    std::cout << "Is it algebraic?\n";
    std::string s = is_algebraic(x2, symbol, Real(1e10));
    if (!s.empty()) {
        std::cout << s << "\n";
    }
    else {
        std::cout << "It is probably not algebraic.\n";
    }

    std::cout << "Searching for closed forms.\n";
    s = identify(x2, symbol, Real(1e10));
    if (!s.empty()) {
        std::cout << s << "\n";
    }
    else {
        std::cout << "No relations found.\n";
    }

}