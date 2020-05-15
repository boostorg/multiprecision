/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// Mathematica has an implementation of PSLQ which has the following interface:
// FindIntegerNullVector[{E, Pi}, 100000]
// FindIntegerNullVector::norel: There is no integer null vector for {E,\[Pi]} with norm less than or equal to 100000.
// Or:
// FindIntegerNullVector[{E, \[Pi]}]
// FindIntegerNullVector::rnfu: FindIntegerNullVector has not found an integer null vector for {E,\[Pi]}.
// I don't like this, because it should default to telling us the norm, as it's coproduced by the computation.

// Maple's Interface:
// with(IntegerRelations)
// v := [1.57079..., 1.4142135]
// u := PSLQ(v)
// u:= [-25474, 56589]
// Maple's interface is in fact worse, because it gives the wrong answer, instead of recognizing the precision provided.

// David Bailey's interface in tpslqm2.f90 in https://www.davidhbailey.com/dhbsoftware/  in  mpfun-fort-v19.tar.gz
// subroutine pslqm2(idb, n nwds, rb, eps, x, iq, r)
// idb is debug level
// n is the length of input vector and output relation r.
// nwds if the full precision level in words.
// rb is the log10 os max size Euclidean norm of relation
// eps tolerance for full precision relation detection.
// x input vector
// iq output flag: 0 (unsuccessful), 1 successful.
// r output integer relation vector, if successful.

#ifndef BOOST_MULTIPRECISION_PSLQ_HPP
#define BOOST_MULTIPRECISION_PSLQ_HPP
#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <map>
#include <cmath>
#include <boost/math/constants/constants.hpp>
#include <Eigen/Dense>

namespace boost::multiprecision {

// For debugging:
template<typename Real>
auto tiny_pslq_dictionary() {
    using std::sqrt;
    using namespace boost::math::constants;
    std::map<Real, std::string> m;
    m.emplace(pi<Real>(), "π");
    m.emplace(e<Real>(), "e");
    m.emplace(root_two<Real>(), "√2");
    m.emplace(ln_two<Real>(), "ln(2)");
    return m;
}
template<typename Real>
auto small_pslq_dictionary() {
    using std::sqrt;
    using std::log;
    using namespace boost::math::constants;
    std::map<Real, std::string> m;
    m.emplace(one_div_euler<Real>(), "1/γ");
    m.emplace(root_pi<Real>(), "√π");
    m.emplace(pi<Real>(), "π");
    m.emplace(log(pi<Real>()), "ln(π)");
    m.emplace(pi_sqr<Real>(), "π²");
    m.emplace(pi_cubed<Real>(), "π³");
    m.emplace(e<Real>(), "e");
    m.emplace(root_two<Real>(), "√2");
    m.emplace(root_three<Real>(), "√3");
    m.emplace(sqrt(static_cast<Real>(5)), "√5");
    m.emplace(sqrt(static_cast<Real>(7)), "√7");
    m.emplace(sqrt(static_cast<Real>(11)), "√11");
    m.emplace(euler<Real>(), "γ");
    // φ is linearly dependent on √5; its logarithm is not.
    m.emplace(log(phi<Real>()), "ln(φ)");
    m.emplace(catalan<Real>(), "G");
    m.emplace(glaisher<Real>(), "A");
    m.emplace(khinchin<Real>(), "K₀");
    m.emplace(zeta_three<Real>(), "ζ(3)");
    // To recover multiplicative relations we need the logarithms of small primes.
    m.emplace(log(static_cast<Real>(2)), "ln(2)");
    m.emplace(log(static_cast<Real>(3)), "ln(3)");
    m.emplace(log(static_cast<Real>(5)), "ln(5)");
    m.emplace(log(static_cast<Real>(7)), "ln(7)");
    m.emplace(log(static_cast<Real>(11)), "ln(11)");
    m.emplace(log(static_cast<Real>(13)), "ln(13)");
    m.emplace(log(static_cast<Real>(17)), "ln(17)");
    m.emplace(log(static_cast<Real>(19)), "ln(19)");
    return m;
}

// The PSLQ algorithm; partial sum of squares, lower trapezoidal decomposition.
// See: https://www.davidhbailey.com/dhbpapers/cpslq.pdf, section 3.
template<typename Real>
std::vector<std::pair<int64_t, Real>> pslq(std::vector<Real> & x, Real gamma) {
    std::vector<std::pair<int64_t, Real>> m;
    if (!std::is_sorted(x.begin(), x.end())) {
        std::cerr << "Elements must be sorted in increasing order.\n";
        return m;
    }
    using std::sqrt;
    if (gamma <= 2/sqrt(3)) {
        std::cerr << "γ > 2/√3 is required\n";
        return m;
    }
    Real tmp = 1/Real(4) + 1/(gamma*gamma);
    Real tau = 1/sqrt(tmp);
    if (tau <= 1 || tau >= 2) {
        std::cerr << "τ ∈ (1, 2) is required.\n";
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

    // Are we computing too many square roots??? Should we use s instead?
    std::vector<Real> s_sq(x.size());
    s_sq.back() = x.back()*x.back();
    int64_t n = x.size();
    for (int64_t i = n - 2; i >= 0; --i) {
        s_sq[i] = s_sq[i+1] + x[i]*x[i];
    }
    
    Eigen::Matrix<Real, Eigen::Dynamic, Eigen::Dynamic> Hx(n, n-1);
    for (int64_t i = 0; i < n - 1; ++i) {
        for (int64_t j = 0; j < n - 1; ++j) {
            if (i < j) {
                Hx(i,j) = 0;
            }
            else if (i == j) {
                Hx(i,i) = sqrt(s_sq[i+1]/s_sq[i]);
            }
            else {
                // i > j:
                Hx(i,j) = -x[i]*x[j]/sqrt(s_sq[j]*s_sq[j+1]);
            }
        }
    }
    for (int64_t j = 0; j < n - 1; ++j) {
        Hx(n-1, j) = -x[n-1]*x[j]/sqrt(s_sq[j]*s_sq[j+1]);
    }

    // This validates that Hx is indeed lower trapezoidal,
    // but that's trival and verbose:
    //std::cout << "Hx = \n";
    //std::cout << Hx << "\n";

    // Validate the conditions of Lemma 1 in the referenced paper:
    // These tests should eventually be removed once we're confident that the code is correct.
    auto Hxnorm_sq = Hx.squaredNorm();
    if (abs(Hxnorm_sq/(n-1) - 1) > sqrt(std::numeric_limits<Real>::epsilon())) {
        std::cerr << "‖Hₓ‖² ≠ n - 1. Hence Lemma 1.ii of the reference has numerically failed; this is a bug.\n";
        return m;
    }

    Eigen::Matrix<Real, Eigen::Dynamic, 1> x_copy(x.size());
    for (int64_t i = 0; i < n; ++i) {
        x_copy[i] = x[i];
    }
    auto v = x_copy.transpose()*Hx;
    for (int64_t i = 0; i < n - 1; ++i) {
        if (abs(v[i])/(n-1) > sqrt(std::numeric_limits<Real>::epsilon())) {
            std::cerr << "xᵀHₓ ≠ 0; Lemma 1.iii of the reference cpslq has numerically failed; this is a bug.\n";
            return m;
        }
    }
    //std::cout << "H, pre-reduction:\n" << Hx << "\n";
    
    using std::round;
    // Matrix D of Definition 4:
    Eigen::Matrix<Real, Eigen::Dynamic, Eigen::Dynamic> D = Eigen::Matrix<Real, Eigen::Dynamic, Eigen::Dynamic>::Identity(n, n);
    for (int64_t i = 1; i < n; ++i) {
        for (int64_t j = i - 1; j >= 0; --j) {
            Real q = round(Hx(i,j)/Hx(j,j));
            // This happens a lot because x_0 < x_1 < ...!
            // Sort them in decreasing order and it almost never happens.
            if (q == 0) {
                continue;
            }
            for (int64_t k = 0; k <= j; ++k)
            {
                Hx(i,k) = Hx(i,k) - q*Hx(j,k);
            }
            for (int64_t k = 0; k < n; ++k) {
                D(i,k) = D(i,k) - q*D(j,k);
            }
        }
    }
    //std::cout << "D = \n" << D << "\n";
    //std::cout << "H, post-reduction:\n" << Hx << "\n";
    // stubbing it out . . .
    for (auto t : x) {
        m.push_back({-8, t});
    }
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
        oss << "As\n\t";
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
        oss << " = " << sum << ",\nit is likely that\n\t";

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
        oss << " = 0.";

        return oss.str();
    }
    return "";
}

template<typename Real>
std::string pslq(std::map<Real, std::string> const & dictionary) {
    Real gamma = 2/sqrt(3) + 0.01;
    return pslq(dictionary, gamma);
}

template<typename Real>
bool is_algebraic(Real x, std::vector<int64_t>& m) {
    // TODO: Figure out this interface.
    return false;
}

}
#endif