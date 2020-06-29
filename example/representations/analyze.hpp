/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef EXAMPLE_REPRESENTATIONS_ANALYZE_HPP
#define EXAMPLE_REPRESENTATIONS_ANALYZE_HPP
#include <string>
#include <iomanip>
#include <boost/multiprecision/pslq.hpp>
#include <boost/math/tools/simple_continued_fraction.hpp>
#include <boost/math/tools/centered_continued_fraction.hpp>

template<typename Real>
void analyze(Real x, std::string symbol)
{
    using std::abs;
    using boost::multiprecision::identify;
    using boost::multiprecision::is_algebraic;
    using boost::math::tools::centered_continued_fraction;
    using boost::math::tools::simple_continued_fraction;

    auto scf = simple_continued_fraction(x);
    auto ccf = centered_continued_fraction(x);
    std::cout << ccf << "\n";

    std::cout << symbol << " ≈ " << x << "\n";
    std::cout << " ≈ " << scf  << "\n";
    std::cout << " ≈ " << ccf  << "\n";
    std::cout << std::setprecision(11);
    std::cout << "Empirical Khinchin geometric mean of simple cfrac        : " << scf.khinchin_geometric_mean() << "\n";
    std::cout << "Expected Khinchin geometric mean if the value is 'random': 2.6854520010.\n";
    std::cout << "Empirical Khinchin harmonic mean of simple cfrac         : " << scf.khinchin_harmonic_mean() << "\n";
    std::cout << "Expected Khinchin harmonic mean if the value is 'random' : 1.74540566240.\n";
    std::cout << "Empirical Khinchin geometric mean from centered continued fraction: " << ccf.khinchin_geometric_mean() << "\n";
    std::cout << "Expected Khinchin geometric mean of ccf if the value is 'random'  : 5.454517244545585756966057724\n";
    std::cout << "Is it algebraic?\n";
    std::string s = is_algebraic(x, symbol, Real(1e10));
    if (!s.empty())
    {
        std::cout << s << "\n";
    }
    else
    {
        std::cout << "It is probably not algebraic.\n";
    }

    std::cout << "Searching for closed forms.\n";
    s = identify(x, symbol, Real(1e10));
    if (!s.empty())
    {
        std::cout << s << "\n";
    }
    else
    {
        std::cout << "No relations found.\n";
    }
}
#endif