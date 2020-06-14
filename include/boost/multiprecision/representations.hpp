///////////////////////////////////////////////////////////////////////////////
//  Copyright 2020 Nick Thompson.
//  Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MULTIPRECISION_REPRESENTATIONS
#define BOOST_MULTIPRECISION_REPRESENTATIONS
#include <string>
#include <ostream>
#include <iomanip>
#include <limits>

namespace boost::multiprecision {
template<typename Real>
class representations {
public:
    representations(Real const & x, std::string symbol) : x_{x}, symbol_{symbol} {}

    template<typename T>
    friend std::ostream& operator<<(std::ostream& out, representations<T>& rep);


private:
    Real x_;
    std::string symbol_;
};


template<typename Real>
std::ostream& operator<<(std::ostream& out, representations<Real>& rep)
{
    out << std::setprecision(std::numeric_limits<Real>::max_digits10);
    out << rep.symbol_ << " = " << rep.x_ << "\n";
    // TODO:
    // Luroth and alternating Luroth representation.
    // Bolyai-Renyi representation.
    // Hexadecimal
    // Is it Khinchin-regular?
    // Are digits uniformly distributed?
    // Continued fraction. Nearest integer continued fraction.
    // Lehmer's continued cotangent representation.

    return out;
}

}
#endif
