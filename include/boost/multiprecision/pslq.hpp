/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef BOOST_MULTIPRECISION_PSLQ_HPP
#define BOOST_MULTIPRECISION_PSLQ_HPP
#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <map>
#include <cmath>
#include <boost/math/constants/constants.hpp>

namespace boost::multiprecision {

template<typename Real>
auto small_pslq_dictionary() {
    using std::sqrt;
    using namespace boost::math::constants;
    std::map<double, std::string> m;
    m.emplace(one_div_euler<Real>(), "1/γ");
    m.emplace(root_pi<Real>(), "√π");
    m.emplace(pi<Real>(), "π");
    m.emplace(pi_sqr<Real>(), "π²");
    m.emplace(pi_cubed<Real>(), "π³");
    m.emplace(e<Real>(), "e");
    m.emplace(root_two<Real>(), "√2");
    m.emplace(root_three<Real>(), "√3");
    m.emplace(sqrt(static_cast<Real>(5)), "√5");
    m.emplace(sqrt(static_cast<Real>(7)), "√7");
    m.emplace(sqrt(static_cast<Real>(11)), "√11");
    m.emplace(ln_two<Real>(), "ln(2)");
    m.emplace(euler<Real>(), "γ");
    m.emplace(phi<Real>(), "φ");
    m.emplace(catalan<Real>(), "G");
    m.emplace(glaisher<Real>(), "A");
    m.emplace(khinchin<Real>(), "K₀");
    m.emplace(zeta_three<Real>(), "ζ(3)");
    return m;
}

// The PSLQ algorithm; partial sum of squares, lower trapezoidal decomposition.
// See: https://www.davidhbailey.com/dhbpapers/cpslq.pdf, section 3.
template<typename Real>
std::vector<std::pair<int64_t, Real>> pslq(std::vector<Real> const & x, Real gamma) {
    using std::sqrt;
    std::vector<std::pair<int64_t, Real>> m;
    if (gamma <= 2/sqrt(3)) {
        std::cerr << "γ > 2/√3 is required\n";
        return m;
    }
    Real tmp = 1/Real(4) + 1/(gamma*gamma);
    Real tau = 1/sqrt(tmp);
    if (tau <= 1 || tau >= 2) {
        std::cerr << "τ in (1, 2) is required.\n";
        return m;
    }

    if (x.size() < 2) {
        std::cerr << "At least two values are required to find an integer relation.\n";
        return m;
    }

    for (auto & t : x) {
        if (t == 0) {
            std::cerr << "Zero in the dictionary gives trivial relations.\n";
            return m;
        }
        if (t < 0) {
            std::cerr << "The algorithm is reflection invariant, so negative values in the dictionary should be removed.\n";
            return m;
        }
    }

    // stubbing it out . . .
    //m.push_back({-5, x[0]});
    //m.push_back({-7, x[1]});
    for (auto t : x) {
        m.push_back({-8, t});
    }
    //m.push_back({-7, x[4]});
    return m;
}

template<typename Real>
std::vector<std::pair<int64_t, Real>> pslq(std::vector<Real> const & x) {
    Real gamma = 2/sqrt(3) + 0.01;
    return pslq(x, gamma);
}

template<typename Real>
std::string pslq(std::map<Real, std::string> const & dictionary, Real gamma) {    
    std::vector<Real> values(dictionary.size());
    size_t i = 0;
    for (auto it = dictionary.begin(); it != dictionary.end(); ++it) {
        values[i++] = it->first;
    }

    auto m = pslq(values, gamma);
    if (m.size() > 0) {
        std::ostringstream oss;
        auto const & symbol = dictionary.find(m[0].second)->second;
        oss << "As ";
        Real sum = m[0].first*m[0].second;
        oss << m[0].first << "⋅" << m[0].second;
        for (size_t i = 1; i < m.size(); ++i)
        {
            if (m[i].first < 0) {
                oss << " - ";
            } else {
                oss << " + ";
            }
            oss << abs(m[i].first) << "⋅" << m[i].second;
            sum += m[i].first*m[i].second;
        }
        oss << " = " << sum << ", it is likely that ";

        oss << m[0].first << "⋅" << symbol;
        for (size_t i = 1; i < m.size(); ++i)
        {
            if (m[i].first < 0) {
                oss << " - ";
            } else {
                oss << " + ";
            }
            auto const & symbol = dictionary.find(m[i].second)->second;
            oss << abs(m[i].first) << "⋅" << symbol;
        }
        oss << " = 0.\n";

        return oss.str();
    }
    return "";
}

template<typename Real>
std::string pslq(std::map<Real, std::string> const & dictionary) {
    Real gamma = 2/sqrt(3) + 0.01;
    return pslq(dictionary, gamma);
}


}
#endif