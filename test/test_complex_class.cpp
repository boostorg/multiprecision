///////////////////////////////////////////////////////////////////////////////
//  Copyright 2024 Matt Borland. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/multiprecision/complex.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/core/lightweight_test.hpp>
#include <limits>
#include <complex>
#include <iostream>
#include <cmath>

using namespace boost::multiprecision;

template <typename T>
bool test_equal(T lhs, T rhs, int tol = 10) noexcept
{
    using std::fabs;
    const bool res = fabs(lhs - rhs) < static_cast<T>(tol) * std::numeric_limits<T>::epsilon();

    if (!res)
    {
        // LCOV_EXCL_START
        std::cerr << "LHS: " << lhs
                  << "\nRHS: " << rhs
                  << "\nDist: " << fabs(lhs - rhs) / std::numeric_limits<T>::epsilon() << std::endl;
        // LCOV_EXCL_STOP
    }

    return res;
}

template <typename T>
void test_construction()
{
    using std::complex;
    using std::polar;
    using complex_scalar = decltype(polar(T(), T()));
    std::cerr << typeid(complex_scalar).name() << std::endl;

    complex_scalar v {};
    BOOST_TEST_EQ(v.real(), T{0});
    BOOST_TEST_EQ(v.imag(), T{0});

    complex_scalar v1 {T{1}};
    BOOST_TEST_EQ(v1.real(), T{1});
    BOOST_TEST_EQ(v1.imag(), T{0});

    complex_scalar v2 {T{2}, T{2}};
    BOOST_TEST_EQ(v2.real(), T{2});
    BOOST_TEST_EQ(v2.imag(), T{2});
}

template <typename T>
void test_unary_operators()
{
    using std::complex;
    using std::polar;
    using complex_scalar = decltype(polar(T(), T()));

    const complex_scalar val {T{2}, T{-2}};
    complex_scalar pos_val = +val;
    BOOST_TEST_EQ(val.real(), pos_val.real());
    BOOST_TEST_EQ(val.imag(), pos_val.imag());

    complex_scalar neg_val = -val;
    BOOST_TEST_EQ(neg_val.real(), T{-2});
    BOOST_TEST_EQ(neg_val.imag(), T{2});
}

template <typename T>
void test_addition()
{
    using std::complex;
    using std::polar;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs_1 {T{1}, T{1}};
    complex_scalar rhs_1 {T{2}, T{2}};
    complex_scalar res_1 = lhs_1 + rhs_1;

    BOOST_TEST_EQ(res_1.real(), T{3});
    BOOST_TEST_EQ(res_1.imag(), T{3});
}

template <typename T>
void test_subtraction()
{
    using std::complex;
    using std::polar;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs_1 {T{1}, T{1}};
    complex_scalar rhs_1 {T{2}, T{2}};
    complex_scalar res_1 = lhs_1 - rhs_1;

    BOOST_TEST_EQ(res_1.real(), T{-1});
    BOOST_TEST_EQ(res_1.imag(), T{-1});
}

template <typename T>
void test_multiplication()
{
    using std::complex;
    using std::polar;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs_1 {T{-3}, T{3}};
    complex_scalar rhs_1 {T{2}, T{2}};
    complex_scalar res_1 = lhs_1 * rhs_1;

    BOOST_TEST_EQ(res_1.real(), T{-12});
    BOOST_TEST_EQ(res_1.imag(), T{0});
}

template <typename T>
void test_division()
{
    using std::complex;
    using std::polar;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs_1 {T{6}, T{2}};
    complex_scalar rhs_1 {T{2}, T{2}};
    complex_scalar res_1 = lhs_1 / rhs_1;

    BOOST_TEST_EQ(res_1.real(), T{2});
    BOOST_TEST_EQ(res_1.imag(), T{-1});
}

template <typename T>
void test_equality()
{
    using std::complex;
    using std::polar;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs {T{2}, T{-1}};
    complex_scalar rhs {T{2}, T{1}};

    BOOST_TEST(lhs != rhs);
    BOOST_TEST(!(lhs == rhs));

    T scalar_rhs {T{2}};

    BOOST_TEST(lhs != scalar_rhs);
    BOOST_TEST(!(lhs == scalar_rhs));

    lhs = complex_scalar{T{2}, T{0}};
    BOOST_TEST(lhs == scalar_rhs);
    BOOST_TEST(!(lhs != scalar_rhs));
}

template <typename T>
void test_non_member_real_imag()
{
    using std::complex;
    using std::polar;
    using std::real;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs {T{2}, T{-1}};

    BOOST_TEST_EQ(real(lhs), lhs.real());
    BOOST_TEST_EQ(imag(lhs), lhs.imag());
}

template <typename T>
void test_abs()
{
    using std::complex;
    using std::polar;
    using std::abs;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs {T{1}, T{1}};

    BOOST_TEST(test_equal(static_cast<T>(abs(lhs)), static_cast<T>(sqrt(T{2}))));
}

template <typename T>
void test_arg()
{
    using std::complex;
    using std::polar;
    using std::arg;
    using boost::math::constants::pi;
    using boost::math::constants::half_pi;
    using complex_scalar = decltype(polar(T(), T()));

    BOOST_TEST(test_equal(arg(complex_scalar{T{1}, T{0}}), T{0}));
    BOOST_TEST(test_equal(arg(complex_scalar{T{0}, T{0}}), T{0}));
    BOOST_TEST(test_equal(arg(complex_scalar{T{0}, T{1}}), half_pi<T>()));
    BOOST_TEST(test_equal(arg(complex_scalar{T{-1}, T{0}}), pi<T>()));
}

template <typename T>
void test_norm()
{
    using std::complex;
    using std::polar;
    using std::norm;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs {T{3}, T{4}};

    BOOST_TEST(test_equal(norm(lhs), T{25}));
}

template <typename T>
void test_conj()
{
    using std::complex;
    using std::polar;
    using std::conj;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs {T{1}, T{1}};
    complex_scalar rhs {T{1}, T{-1}};

    BOOST_TEST_EQ(conj(lhs), rhs);
}

template <typename T>
void test_proj()
{
    using std::complex;
    using std::polar;
    using std::proj;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs {T{1}, T{1}};
    BOOST_TEST_EQ(lhs, proj(lhs));

    lhs = complex_scalar{T{std::numeric_limits<T>::infinity()}, T{1}};
    complex_scalar rhs = complex_scalar{T{std::numeric_limits<T>::infinity()}, T{0}};
    BOOST_TEST_EQ(proj(lhs), rhs);

    lhs = complex_scalar{T{std::numeric_limits<T>::infinity()}, T{-1}};
    rhs = complex_scalar{T{std::numeric_limits<T>::infinity()}, T{-0}};
    BOOST_TEST_EQ(proj(lhs), rhs);
}

template <typename T>
void test_exp()
{
    using std::complex;
    using std::polar;
    using std::exp;
    using boost::math::constants::pi;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs {T{0}, pi<T>()};
    lhs = exp(lhs);
    complex_scalar rhs {T{-1}, T{0}};
    BOOST_TEST(test_equal(lhs.real(), rhs.real()));
    BOOST_TEST(test_equal(lhs.imag(), rhs.imag()));
}

template <typename T>
void test_log()
{
    using std::complex;
    using std::polar;
    using std::log;
    using boost::math::constants::half_pi;
    using boost::math::constants::pi;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs {T{0}, T{1}};
    lhs = log(lhs);
    complex_scalar rhs {T{0}, half_pi<T>()};

    BOOST_TEST(test_equal(lhs.real(), rhs.real()));
    BOOST_TEST(test_equal(lhs.imag(), rhs.imag()));

    lhs = {T{-1}, T{0}};
    lhs = log(lhs);
    rhs = {T{0}, pi<T>()};

    BOOST_TEST(test_equal(lhs.real(), rhs.real()));
    BOOST_TEST(test_equal(lhs.imag(), rhs.imag()));

    BOOST_IF_CONSTEXPR (!std::is_same<T, cpp_dec_float_50>::value)
    {
        // Other side of the cut line
        lhs = {T {-1}, -T {0}};
        lhs = log(lhs);
        rhs = {T {0}, -pi<T>()};

        BOOST_TEST(test_equal(lhs.real(), rhs.real()));
        BOOST_TEST(test_equal(lhs.imag(), rhs.imag()));
    }
}

template <typename T>
void test_scalar_addition()
{
    using std::complex;
    using std::polar;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs_1 {T{1}, T{1}};
    T rhs_1 {T{2}};
    complex_scalar res_1 = lhs_1 + rhs_1;

    BOOST_TEST_EQ(res_1.real(), T{3});
    BOOST_TEST_EQ(res_1.imag(), T{1});
}

template <typename T>
void test_scalar_subtraction()
{
    using std::complex;
    using std::polar;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs_1 {T{1}, T{1}};
    T rhs_1 {T{2}};
    complex_scalar res_1 = lhs_1 - rhs_1;

    BOOST_TEST_EQ(res_1.real(), T{-1});
    BOOST_TEST_EQ(res_1.imag(), T{1});
}

template <typename T>
void test_scalar_multiplication()
{
    using std::complex;
    using std::polar;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs_1 {T{3}, T{2}};
    T rhs_1 {T{2}};
    complex_scalar res_1 = lhs_1 * rhs_1;

    BOOST_TEST_EQ(res_1.real(), T{6});
    BOOST_TEST_EQ(res_1.imag(), T{4});
}

template <typename T>
void test_scalar_division()
{
    using std::complex;
    using std::polar;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs_1 {T{4}, T{2}};
    T rhs_1 {T{2}};
    complex_scalar res_1 = lhs_1 / rhs_1;

    BOOST_TEST_EQ(res_1.real(), T{2});
    BOOST_TEST_EQ(res_1.imag(), T{1});
}

template <typename T>
void test_log10()
{
    using std::complex;
    using std::polar;
    using std::log10;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs {T{-100}, T{0}};
    lhs = log10(lhs);
    complex_scalar rhs {T{2}, static_cast<T>(1.36438)};
    BOOST_TEST(test_equal(lhs.real(), rhs.real()));
}

template <typename T>
void test_pow()
{
    using std::complex;
    using std::polar;
    using std::pow;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs {T{1}, T{2}};
    lhs = pow(lhs, T{2});
    complex_scalar rhs {T{-3}, T{4}};
    BOOST_TEST(test_equal(lhs.real(), rhs.real()));
    BOOST_TEST(test_equal(lhs.imag(), rhs.imag(), 100));

    lhs = {T{-1}, T{0}};
    lhs = pow(lhs, T{1}/2);
    rhs = {T{0}, T{1}};
    BOOST_TEST(test_equal(lhs.real(), rhs.real()));
    BOOST_TEST(test_equal(lhs.imag(), rhs.imag()));

    // Check other side of the cut
    BOOST_IF_CONSTEXPR (!std::is_same<T, cpp_dec_float_50>::value)
    {
        lhs = {T {-1}, -T {0}};
        lhs = pow(lhs, T {1} / 2);
        rhs = {T {0}, T {-1}};
        BOOST_TEST(test_equal(lhs.real(), rhs.real()));
        BOOST_TEST(test_equal(lhs.imag(), rhs.imag()));
    }
}

template <typename T>
void test_sqrt()
{
    using std::complex;
    using std::polar;
    using std::sqrt;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs {T{4}, T{0}};
    lhs = sqrt(lhs);
    complex_scalar rhs {T{2}, T{0}};
    BOOST_TEST(test_equal(lhs.real(), rhs.real()));
    BOOST_TEST(test_equal(lhs.imag(), rhs.imag()));

    // Check other side of the cut
    BOOST_IF_CONSTEXPR (!std::is_same<T, cpp_dec_float_50>::value)
    {
        lhs = {T {4}, -T {0}};
        lhs = sqrt(lhs);
        rhs = {T {2}, -T {0}};
        BOOST_TEST(test_equal(lhs.real(), rhs.real()));
        BOOST_TEST(test_equal(lhs.imag(), rhs.imag()));
    }
}

template <typename T>
void test_sinh()
{
    using std::complex;
    using std::polar;
    using std::sinh;
    using complex_scalar = decltype(polar(T(), T()));

    complex_scalar lhs {T{1}, T{0}};
    lhs = sinh(lhs);
    complex_scalar rhs {sinh(T{1}), T{0}};
    BOOST_TEST(test_equal(lhs.real(), rhs.real()));
    BOOST_TEST(test_equal(lhs.imag(), rhs.imag()));

    lhs = {T{0}, T{1}};
    lhs = sinh(lhs);
    rhs = {T{0}, sin(T{1})};
    BOOST_TEST(test_equal(lhs.real(), rhs.real()));
    BOOST_TEST(test_equal(lhs.imag(), rhs.imag()));
}

int main()
{
    test_construction<float>();
    test_construction<double>();
    test_construction<cpp_bin_float_50>();
    test_construction<cpp_dec_float_50>();

    test_unary_operators<float>();
    test_unary_operators<double>();
    test_unary_operators<cpp_bin_float_50>();
    test_unary_operators<cpp_dec_float_50>();

    test_addition<float>();
    test_addition<double>();
    test_addition<cpp_bin_float_50>();
    test_addition<cpp_dec_float_50>();

    test_subtraction<float>();
    test_subtraction<double>();
    test_subtraction<cpp_bin_float_50>();
    test_subtraction<cpp_dec_float_50>();

    test_multiplication<float>();
    test_multiplication<double>();
    test_multiplication<cpp_bin_float_50>();
    test_multiplication<cpp_dec_float_50>();

    test_division<float>();
    test_division<double>();
    test_division<cpp_bin_float_50>();
    test_division<cpp_dec_float_50>();

    test_equality<float>();
    test_equality<double>();
    test_equality<cpp_bin_float_50>();
    test_equality<cpp_dec_float_50>();

    test_non_member_real_imag<float>();
    test_non_member_real_imag<double>();
    test_non_member_real_imag<cpp_bin_float_50>();
    test_non_member_real_imag<cpp_dec_float_50>();

    test_abs<float>();
    test_abs<double>();
    test_abs<cpp_bin_float_50>();
    test_abs<cpp_dec_float_50>();

    test_arg<float>();
    test_arg<double>();
    test_arg<cpp_bin_float_50>();
    test_arg<cpp_dec_float_50>();

    test_norm<float>();
    test_norm<double>();
    test_norm<cpp_bin_float_50>();
    test_norm<cpp_dec_float_50>();

    test_conj<float>();
    test_conj<double>();
    test_conj<cpp_bin_float_50>();
    test_conj<cpp_dec_float_50>();

    test_proj<float>();
    test_proj<double>();
    test_proj<cpp_bin_float_50>();
    test_proj<cpp_dec_float_50>();

    test_exp<float>();
    test_exp<double>();
    test_exp<cpp_bin_float_50>();
    test_exp<cpp_dec_float_50>();

    test_log<float>();
    test_log<double>();
    test_log<cpp_bin_float_50>();
    test_log<cpp_dec_float_50>();

    test_scalar_addition<float>();
    test_scalar_addition<double>();
    test_scalar_addition<cpp_bin_float_50>();
    test_scalar_addition<cpp_dec_float_50>();

    test_scalar_subtraction<float>();
    test_scalar_subtraction<double>();
    test_scalar_subtraction<cpp_bin_float_50>();
    test_scalar_subtraction<cpp_dec_float_50>();

    test_scalar_multiplication<float>();
    test_scalar_multiplication<double>();
    test_scalar_multiplication<cpp_bin_float_50>();
    test_scalar_multiplication<cpp_dec_float_50>();

    test_scalar_division<float>();
    test_scalar_division<double>();
    test_scalar_division<cpp_bin_float_50>();
    test_scalar_division<cpp_dec_float_50>();

    test_log10<float>();
    test_log10<double>();
    test_log10<cpp_bin_float_50>();
    test_log10<cpp_dec_float_50>();

    test_pow<float>();
    test_pow<double>();
    test_pow<cpp_bin_float_50>();
    test_pow<cpp_dec_float_50>();

    test_sqrt<float>();
    test_sqrt<double>();
    test_sqrt<cpp_bin_float_50>();
    test_sqrt<cpp_dec_float_50>();

    test_sinh<float>();
    test_sinh<double>();
    test_sinh<cpp_bin_float_50>();
    test_sinh<cpp_dec_float_50>();

    return boost::report_errors();
}
