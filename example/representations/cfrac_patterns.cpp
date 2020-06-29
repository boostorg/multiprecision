/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include "analyze.hpp"
#include <utility>
#include <boost/math/tools/fraction.hpp>
#include <boost/multiprecision/mpfr.hpp>
using boost::multiprecision::mpfr_float;
using boost::math::tools::continued_fraction_a;

// This example comes from Steven R. Finch's book "Mathematical Constants", Section 6.2.
// "No one knows the outcome if we instead repeat each denominator in c_1,
//  although numerically we find c_2 = 0.5851972651...."
template <typename T>
class repeated_denom_generator
{
public:
    repeated_denom_generator() { i = 0; even = true;}
    typedef T result_type;

    result_type operator()()
    {
        if (even)
        {
            i += 1;
            even = false;
        }
        else
        {
            even = true;
        }
        return i;
    }
private:
    T i;
    bool even;
};
template<typename Real>
Real repeated_denom()
{
    auto gen = repeated_denom_generator<Real>();
    Real res = continued_fraction_a(gen, std::numeric_limits<Real>::epsilon());
    return res;
}

int main()
{
    using Real = mpfr_float;
    mpfr_float::default_precision(1000);
    Real x = repeated_denom<Real>();
    std::string symbol = "1|/|1 + 1|/|1 + 1|/|2 + 1|/|2 + ";
    analyze(x, symbol);
}