/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <boost/multiprecision/pslq.hpp>
#include <boost/math/constants/constants.hpp>

int main() {
    std::map<double, std::string> m;
    m.emplace(boost::math::constants::pi<double>(), "π");
    m.emplace(boost::math::constants::e<double>(), "e");
    auto s = boost::multiprecision::pslq<double>(m);
    if (s.size() > 0) {
        std::cout << s << "\n";
    } else {
        std::cout << "No relations found.\n";
    }
}