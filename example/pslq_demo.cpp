/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <boost/multiprecision/pslq.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

template<typename Real>
void ln2_plus_pi()
{
    using namespace boost::math::constants;
    std::map<Real, std::string> m;
    m.emplace(pi<Real>() + ln_two<Real>(), "(π+ln(2))");
    m.emplace(pi<Real>(), "π");
    m.emplace(ln_two<Real>(), "ln(2)");
    auto s = boost::multiprecision::pslq<Real>(m);
    if (!s.empty())
    {
        std::cout << s << "\n";
    }
    else
    {
        std::cout << "No relations found.\n";
    }

}

int main() {
    using Real = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<300> >;
    ln2_plus_pi<Real>();
    /*std::map<Real, std::string> m = boost::multiprecision::small_pslq_dictionary<Real>();
    auto s = boost::multiprecision::pslq<Real>(m);
    if (!s.empty())
    {
        std::cout << s << "\n";
    }
    else
    {
        std::cout << "No relations found.\n";
    }*/
}