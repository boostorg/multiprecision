/*
 * Copyright Nick Thompson 2020
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#include <random>
#include <benchmark/benchmark.h>
#include <boost/multiprecision/float128.hpp>
#include <boost/multiprecision/mpfr.hpp>
#if __has_include(<immintrin.h>)
#include <immintrin.h>
#endif
template<typename Real>
inline Real rsqrt(Real const & x) {
    if constexpr (std::is_same_v<Real, float>) {
        __m128 b, t;
        b = _mm_set_ss (x);
        t = _mm_rsqrt_ss (b);
        float res;
         _mm_store_ss (&res, t);
        //res = res*(3-x*res*res)/2;
        res = res + res*(1-x*res*res)/2;
        return res;
    }
    if constexpr (std::is_same_v<Real, double> ||  std::is_same_v<Real, long double>) {
        double a = rsqrt(float(x));
        a = a + a*(1-x*a*a)/2;
        return a;
        //return 1/sqrt(x);
    }
    if constexpr (std::is_same_v<Real, boost::multiprecision::float128>) {
        boost::multiprecision::float128 a = rsqrt(static_cast<long double>(x));
        // one newton iteration:
        a = a*(3-x*a*a)/2;
        // a second newton iterate:
        return a*(3-x*a*a)/2;
    }
    else if constexpr (sizeof(Real) > 16) {
        Real res;
        mpfr_rec_sqrt (res.backend().data(), x.backend().data(), GMP_RNDN);
        return res;
    }
}

template<typename Real>
void RSqrtBM(benchmark::State& state)
{
    Real x = 0.01;
    for (auto _ : state) {
        benchmark::DoNotOptimize(rsqrt(x));
        x += std::numeric_limits<Real>::epsilon();
    }

    state.SetComplexityN(8*sizeof(Real));
}

BENCHMARK_TEMPLATE(RSqrtBM, float)->Complexity();
BENCHMARK_TEMPLATE(RSqrtBM, double)->Complexity();
BENCHMARK_TEMPLATE(RSqrtBM, long double)->Complexity();
BENCHMARK_TEMPLATE(RSqrtBM, boost::multiprecision::float128)->Complexity();
BENCHMARK_TEMPLATE(RSqrtBM, boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<100>>)->Complexity();
BENCHMARK_TEMPLATE(RSqrtBM, boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<200>>)->Complexity();
BENCHMARK_TEMPLATE(RSqrtBM, boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<300>>)->Complexity();
BENCHMARK_TEMPLATE(RSqrtBM, boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<400>>)->Complexity();
BENCHMARK_TEMPLATE(RSqrtBM, boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<500>>)->Complexity();
BENCHMARK_TEMPLATE(RSqrtBM, boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<1000>>)->Complexity();

BENCHMARK_MAIN();