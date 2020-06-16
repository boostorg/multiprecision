///////////////////////////////////////////////////////////////////////////////
//  Copyright 2020 Nick Thompson.
//  Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MULTIPRECISION_REPRESENTATIONS_HPP
#define BOOST_MULTIPRECISION_REPRESENTATIONS_HPP
#include <string>
#include <ostream>
#include <iomanip>
#include <limits>
#include <vector>
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/rational_adaptor.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace boost::multiprecision {
template<typename Real>
class representations {
public:
    representations(Real const & x, std::string symbol = "x") : x_{x}, symbol_{symbol} {}

    template<typename T>
    friend std::ostream& operator<<(std::ostream& out, representations<T>& rep);

    auto simple_continued_fraction();

private:
    Real x_;
    std::string symbol_;
};

template<typename Real>
auto representations<Real>::simple_continued_fraction() {
    using boost::multiprecision::numerator;
    using boost::multiprecision::denominator;
    using boost::multiprecision::divide_qr;
    boost::multiprecision::cpp_rational rat(this->x_);
    auto n = numerator(rat);
    auto d = denominator(rat);
    std::vector<decltype(n)> a;
    // dummy values to get the type right:
    auto q = n;
    auto r = n;

    while(r != 0) {
        divide_qr(n, d, q, r);
        a.push_back(q);
        n = d;
        d = r;
    }

    return a;
}

template<typename Real>
std::ostream& operator<<(std::ostream& out, representations<Real>& rep)
{
    constexpr int p = std::numeric_limits<Real>::max_digits10;
    out << std::setprecision(p);
    if constexpr (p == 2147483647) {
        out << std::setprecision(rep.x_.backend().precision());
    }

    out << rep.symbol_ << " = " << rep.x_ << "\n";
    // TODO:
    // Luroth and alternating Luroth representation.
    // Bolyai-Renyi representation.
    // Hexadecimal
    // Is it Khinchin-regular?
    // Are digits uniformly distributed?
    // Continued fraction. Nearest integer continued fraction.
    // Lehmer's continued cotangent representation.
    // Pierce expansion.
    // Engel expansion (ascending continued fraction).

    //boost::multiprecision::mpq_rational rat(rep.x_);
    boost::multiprecision::cpp_rational r(rep.x_);
    std::string whitespace(rep.symbol_.size() - 1, ' ');
    out << whitespace << "= " << r << "\n";

    auto a = rep.simple_continued_fraction();
    if (a.size() == 1) {
        out << whitespace << "= [" << a[0] << "]\n";
    } else {
        out << "[" << a[0] << "; ";
        for (size_t i = 1; i < a.size() -1; ++i) {
            out << a[i] << ", ";
        }
        out << a.back() << "]\n";
    }

    return out;
}

}
#endif
