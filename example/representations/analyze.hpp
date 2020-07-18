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
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/math/tools/simple_continued_fraction.hpp>
#include <boost/math/tools/centered_continued_fraction.hpp>
#include <boost/math/tools/luroth_expansion.hpp>
#include <boost/math/tools/engel_expansion.hpp>


template<typename Real>
void analyze(Real x, std::string symbol)
{
    using std::abs;
    using std::exp;
    using std::log;
    using boost::multiprecision::identify;
    using boost::multiprecision::is_algebraic;
    using boost::math::tools::centered_continued_fraction;
    using boost::math::tools::simple_continued_fraction;
    using boost::math::tools::luroth_expansion;
    using boost::math::tools::engel_expansion;
    usign boost::multiprecision::checked_int1024_t;
    constexpr int p = std::numeric_limits<Real>::max_digits10;
    std::cout << std::setprecision(p);
    if constexpr (p == 2147483647) {
        std::cout << std::setprecision(x.backend().precision());
    }

    std::cout << symbol << " ≈ " << x << "\n";
    auto scf = simple_continued_fraction(x);
    std::cout << " ≈ " << scf  << "\n";
    auto ccf = centered_continued_fraction(x);
    std::cout << " ≈ " << ccf  << "\n";
    auto lur = luroth_expansion(x);
    std::cout << " ≈ " << lur  << "\n";
    auto eng = engel_expansion<Real, checked_int1024_t>(x);
    std::cout << " ≈ " << eng  << "\n";
    std::cout << std::setprecision(11);
    std::cout << "Empirical Khinchin geometric mean of simple cfrac        : " << scf.khinchin_geometric_mean() << "\n";
    std::cout << "Expected Khinchin geometric mean if the value is 'random': 2.6854520010.\n";
    std::cout << "Empirical Khinchin harmonic mean of simple cfrac         : " << scf.khinchin_harmonic_mean() << "\n";
    std::cout << "Expected Khinchin harmonic mean if the value is 'random' : 1.74540566240.\n";
    std::cout << "Empirical Khinchin geometric mean from centered continued fraction: " << ccf.khinchin_geometric_mean() << "\n";
    std::cout << "Expected Khinchin geometric mean of ccf if the value is 'random'  : 5.454517244545585756966057724\n";
    std::cout << "Empirical digit geometric mean of Luroth expansion     : " << lur.digit_geometric_mean() << "\n";
    std::cout << "Expected digit geometric mean if the value is 'random' : 2.200161058\n";


    Real expx = exp(x);
    auto scf_exp = simple_continued_fraction(expx);
    auto ccf_exp = centered_continued_fraction(expx);
    auto lur_exp = luroth_expansion(expx);
    std::cout << "exp(" << symbol << ")";
    std::cout << " ≈ " << scf_exp  << "\n";
    std::cout << " ≈ " << ccf_exp  << "\n";
    std::cout << " ≈ " << lur_exp  << "\n";
    std::cout << std::setprecision(11);
    std::cout << "Empirical Khinchin geometric mean of simple cfrac        : " << scf_exp.khinchin_geometric_mean() << "\n";
    std::cout << "Expected Khinchin geometric mean if the value is 'random': 2.6854520010.\n";
    std::cout << "Empirical Khinchin harmonic mean of simple cfrac         : " << scf_exp.khinchin_harmonic_mean() << "\n";
    std::cout << "Expected Khinchin harmonic mean if the value is 'random' : 1.74540566240.\n";
    std::cout << "Empirical Khinchin geometric mean from centered continued fraction: " << ccf_exp.khinchin_geometric_mean() << "\n";
    std::cout << "Expected Khinchin geometric mean of ccf if the value is 'random'  : 5.454517244545585756966057724\n";
    std::cout << "Empirical digit geometric mean of Luroth expansion     : " << lur_exp.digit_geometric_mean() << "\n";
    std::cout << "Expected digit geometric mean if the value is 'random' : 2.200161058\n";

    Real lnx = log(x);
    auto scf_log = simple_continued_fraction(lnx);
    auto ccf_log = centered_continued_fraction(lnx);
    auto lur_log = luroth_expansion(lnx);
    std::cout << "ln(" << symbol << ")";
    std::cout << " ≈ " << scf_log  << "\n";
    std::cout << " ≈ " << ccf_log  << "\n";
    std::cout << " ≈ " << lur_log  << "\n";
    std::cout << std::setprecision(11);
    std::cout << "Empirical Khinchin geometric mean of simple cfrac        : " << scf_log.khinchin_geometric_mean() << "\n";
    std::cout << "Expected Khinchin geometric mean if the value is 'random': 2.6854520010.\n";
    std::cout << "Empirical Khinchin harmonic mean of simple cfrac         : " << scf_log.khinchin_harmonic_mean() << "\n";
    std::cout << "Expected Khinchin harmonic mean if the value is 'random' : 1.74540566240.\n";
    std::cout << "Empirical Khinchin geometric mean from centered continued fraction: " << ccf_log.khinchin_geometric_mean() << "\n";
    std::cout << "Expected Khinchin geometric mean of ccf if the value is 'random'  : 5.454517244545585756966057724\n";
    std::cout << "Empirical digit geometric mean of Luroth expansion     : " << lur_log.digit_geometric_mean() << "\n";
    std::cout << "Expected digit geometric mean if the value is 'random' : 2.200161058\n";

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
