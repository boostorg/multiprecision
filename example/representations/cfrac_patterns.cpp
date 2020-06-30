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
// Mathematica notation: FromContinuedFraction[{0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10}]
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

// FromContinuedFraction[{0,1,1,4,9,16,25,36,49,64,81,100,121,144}] = 0.554226
template <typename T>
class denom_squared_generator
{
public:
    denom_squared_generator() { i = 1; first = true; }
    typedef T result_type;

    result_type operator()()
    {
        if (first)
        {
            first = false;
            return 1;
        }
        T res = i*i;
        ++i;
        return res;
    }
private:
    T i;
    bool first;
};

template<typename Real>
Real denom_squared()
{
    auto gen = denom_squared_generator<Real>();
    Real res = continued_fraction_a(gen, std::numeric_limits<Real>::epsilon());
    return res;
}


// FromContinuedFraction[{0,1,1,2^3,3^3, 4^3,...}] = 0.52984
template <typename T>
class denom_cubed_generator
{
public:
    denom_cubed_generator() { i = 1; first = true; }
    typedef T result_type;

    result_type operator()()
    {
        if (first)
        {
            first = false;
            return 1;
        }
        T res = i*i*i;
        ++i;
        return res;
    }
private:
    T i;
    bool first;
};

template<typename Real>
Real denom_cubed()
{
    auto gen = denom_cubed_generator<Real>();
    Real res = continued_fraction_a(gen, std::numeric_limits<Real>::epsilon());
    return res;
}

// FromContinuedFraction[{0, 1, 1, 2^4, 3^4, 4^4,...}] = 0.51514
template <typename T>
class denom_quarted_generator
{
public:
    denom_quarted_generator() { i = 1; first = true; }
    typedef T result_type;

    result_type operator()()
    {
        if (first)
        {
            first = false;
            return 1;
        }
        T res = i*i*i*i;
        ++i;
        return res;
    }
private:
    T i;
    bool first;
};

template<typename Real>
Real denom_quarted()
{
    auto gen = denom_quarted_generator<Real>();
    Real res = continued_fraction_a(gen, std::numeric_limits<Real>::epsilon());
    return res;
}

int main()
{
    using Real = mpfr_float;
    mpfr_float::default_precision(1000);
    Real x = denom_quarted<Real>();
    std::string symbol = "[0; 1, 1⁴, 2⁴, 3⁴, 4⁴, 5⁴, ...]";
    analyze(x, symbol);

    x = denom_cubed<Real>();
    symbol = "[0; 1, 1³, 2³, 3³, 4³, 5³, ...]";
    analyze(x, symbol);

    x = denom_squared<Real>();
    symbol = "[0; 1, 1², 2², 3², 4², 5², ...]";
    analyze(x, symbol);
    
    
    x = repeated_denom<Real>();
    symbol = "1|/|1 + 1|/|1 + 1|/|2 + 1|/|2 + ";
    analyze(x, symbol);
}