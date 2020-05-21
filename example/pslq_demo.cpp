/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <chrono>
#include <boost/multiprecision/pslq.hpp>
#include <boost/multiprecision/mpfr.hpp>

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
    using Real = boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<900> >;
    //ln2_plus_pi<Real>();

    std::cout << "Preparing dictionary. . . ";
    std::map<Real, std::string> m = boost::multiprecision::small_pslq_dictionary<Real>();
    std::cout << "done.\n";
    auto start = std::chrono::steady_clock::now();
    auto s = boost::multiprecision::pslq<Real>(m);
    std::cout << std::endl;
    if (!s.empty())
    {
        std::cout << s << "\n";
    }
    else
    {
        std::cout << "No relations found.\n";
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "Elapsed time is " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds.\n";
}