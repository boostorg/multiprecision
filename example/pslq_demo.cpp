/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <boost/multiprecision/pslq.hpp>
#include <boost/math/constants/constants.hpp>

int main() {
    std::map<std::string, double> m;
    m.emplace("pi", boost::math::constants::pi<double>());
    auto s = boost::multiprecision::pslq<double>(1.5, m);
    if (s) {
        std::cout << s.value() << "\n";
    }
}