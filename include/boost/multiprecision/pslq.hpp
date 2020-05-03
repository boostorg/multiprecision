/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef BOOST_MULTIPRECISION_PSLQ_HPP
#define BOOST_MULTIPRECISION_PSLQ_HPP
#include <optional>
#include <iostream>
#include <stdexcept>
#include <map>
#include <cmath>

namespace boost::multiprecision {

// The PSLQ algorithm; partial sum of squares, lower trapezoidal decomposition.
// See: https://www.davidhbailey.com/dhbpapers/cpslq.pdf, section 3.

template<typename Real>
std::optional<std::vector<int64_t>> pslq(Real tau, std::map<std::string, Real> const & dictionary) {
    using std::sqrt;
    if (tau <= 1 || tau >= 2)
    {
        std::cerr << "tau in open interval (1, 2) is required.\n";
        return {};
    }
    //
    Real gamma = 1/sqrt(1/(tau*tau) - Real(1)/Real(4));

    for (auto [key, value] : dictionary) {
        if (value == 0) {
            std::cerr << "Zero in the dictionary gives trivial relations.\n";
            return {};
        }
        if (value < 0) {
            std::cerr << "The algorithm is reflection invariant, so negative values in the dictionary should be removed.\n";
            return {};
        }
    }
    return "yo";
}

}
#endif