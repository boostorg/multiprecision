#include <boost/config.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include <cerrno>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>

namespace local {

using cpp_dec_float_type = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<50>, boost::multiprecision::et_off>;
using cpp_bin_float_type = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<50>, boost::multiprecision::et_off>;

template<typename NumericType>
auto is_close_fraction(const NumericType& a,
                       const NumericType& b,
                       const NumericType& tol = std::numeric_limits<NumericType>::epsilon() * 16) noexcept -> bool
{
    using std::fabs;

    auto result_is_ok = bool { };

    NumericType delta { };

    if (b == static_cast<NumericType>(0))
    {
        delta = fabs(a - b); // LCOV_EXCL_LINE

        result_is_ok = (delta < tol); // LCOV_EXCL_LINE
    }
    else
    {
        delta = fabs(1 - (a / b));

        result_is_ok = (delta < tol);
    }

    return result_is_ok;
}

template<typename FloatType>
void test_fmod_case(const char* x, const char* y, const char* expected)
{
    using std::fmod;

    const FloatType fmod_mp { fmod(FloatType(x), FloatType(y)) };

    BOOST_TEST(is_close_fraction(fmod_mp, FloatType(expected)));
}

template<typename FloatType>
void test_fmod_nearly_whole_divisor_case(const char* x, const char* y, const bool expect_negative)
{
    using std::fmod;

    const FloatType dividend { x };
    const FloatType divisor { y };
    const FloatType quotient { expect_negative ? -10 : 10 };
    const FloatType expected { dividend - (quotient * divisor) };
    const FloatType fmod_mp { fmod(dividend, divisor) };

    BOOST_TEST(divisor != FloatType("10"));
    BOOST_TEST(is_close_fraction(fmod_mp, expected));
}

template<typename FloatType>
void test_eval_fmod_backend_case(const char* x, const char* y, const char* expected)
{
    FloatType result;
    const FloatType dividend { x };
    const FloatType divisor { y };

    boost::multiprecision::default_ops::eval_fmod(result.backend(), dividend.backend(), divisor.backend());

    BOOST_TEST(is_close_fraction(result, FloatType(expected)));
}

template<typename FloatType>
void test_fmod_zero_denominator()
{
    using std::fmod;

    errno = 0;

    const FloatType fmod_mp { fmod(FloatType("1"), FloatType("0")) };

    BOOST_TEST(boost::multiprecision::isnan(fmod_mp));
    BOOST_TEST(errno == EDOM);
}

template<typename FloatType>
void test_fmod_result_aliases_first_argument()
{
    FloatType result { "17.75" };
    const FloatType divisor { "5" };

    boost::multiprecision::default_ops::eval_fmod(result.backend(), result.backend(), divisor.backend());

    BOOST_TEST(is_close_fraction(result, FloatType("2.75")));
}

template<typename FloatType>
void test_fmod_result_aliases_second_argument()
{
    const FloatType dividend { "17.75" };
    FloatType result { "5" };

    boost::multiprecision::default_ops::eval_fmod(result.backend(), dividend.backend(), result.backend());

    BOOST_TEST(is_close_fraction(result, FloatType("2.75")));
}

template<typename FloatType>
void test_fmod_arithmetic_overloads()
{
    using std::fmod;

    const FloatType mp_arithmetic_rhs { fmod(FloatType("17.75"), 5) };
    const FloatType arithmetic_lhs_mp { fmod(17.75, FloatType("5")) };

    BOOST_TEST(is_close_fraction(mp_arithmetic_rhs, FloatType("2.75")));
    BOOST_TEST(is_close_fraction(arithmetic_lhs_mp, FloatType("2.75")));
}

template<typename FloatType>
void test_eval_fmod_backend_branch_cases()
{
    test_eval_fmod_backend_case<FloatType>("6", "3", "0");
    test_eval_fmod_backend_case<FloatType>("7", "3", "1");
    test_eval_fmod_backend_case<FloatType>("-7", "3", "-1");
    test_eval_fmod_backend_case<FloatType>("7", "-3", "1");
    test_eval_fmod_backend_case<FloatType>("-7", "-3", "-1");
    test_eval_fmod_backend_case<FloatType>("3", "7", "3");
    test_eval_fmod_backend_case<FloatType>("-3", "7", "-3");
    test_eval_fmod_backend_case<FloatType>("3", "-7", "3");
    test_eval_fmod_backend_case<FloatType>("-3", "-7", "-3");
    test_eval_fmod_backend_case<FloatType>("1.75", "0.5", "0.25");
    test_eval_fmod_backend_case<FloatType>("-1.75", "0.5", "-0.25");
    test_eval_fmod_backend_case<FloatType>("1.75", "-0.5", "0.25");
    test_eval_fmod_backend_case<FloatType>("-1.75", "-0.5", "-0.25");
}

template<typename FloatType>
void test_fmod_special_cases()
{
    using std::fmod;
    using std::sqrt;

    const FloatType x_mp_nan   (sqrt(FloatType(-2)));
    const FloatType y_mp_finite(3);

    FloatType fmod_mp { fmod(x_mp_nan, y_mp_finite) };

    BOOST_TEST(boost::multiprecision::isnan(fmod_mp));

    const FloatType x_mp_inf(FloatType(1) / FloatType(0));
    fmod_mp = fmod(x_mp_inf, y_mp_finite);

    BOOST_TEST(boost::multiprecision::isnan(fmod_mp));
}

template<typename FloatType>
void test_fmod_against_double_case()
{
    using std::fmod;

    const double x_db(-36.09543294408234714865102432668209L);
    const double y_db(99.88277539957428530215111095458269L);
    const double fmod_db { fmod(x_db, y_db) };

    const FloatType x_mp("-36.09543294408234714865102432668209");
    const FloatType y_mp("99.88277539957428530215111095458269");
    const FloatType fmod_mp { fmod(x_mp, y_mp) };

    const bool result_fmod_is_ok { is_close_fraction(static_cast<double>(fmod_mp), fmod_db) };

    BOOST_TEST(result_fmod_is_ok);
}

template<typename FloatType>
void test_fmod()
{
    test_fmod_against_double_case<FloatType>();
    test_fmod_special_cases<FloatType>();
    test_fmod_case<FloatType>("0", "5", "0");
    test_fmod_case<FloatType>("17.75", "5", "2.75");
    test_fmod_case<FloatType>("-17.75", "5", "-2.75");
    test_fmod_case<FloatType>("17.75", "-5", "2.75");
    test_fmod_case<FloatType>("-17.75", "-5", "-2.75");
    test_fmod_case<FloatType>("5", "17.75", "5");
    test_fmod_case<FloatType>("-5", "17.75", "-5");
    test_fmod_case<FloatType>("6", "3", "0");
    test_fmod_case<FloatType>("100", "10", "0");
    test_fmod_case<FloatType>("-100", "10", "0");
    test_fmod_case<FloatType>("100.125", "0.25", "0.125");
    test_fmod_case<FloatType>("-100.125", "0.25", "-0.125");
    test_fmod_case<FloatType>("100000000000000000007", "3", "2");
    test_fmod_case<FloatType>("-100000000000000000007", "3", "-2");
    test_fmod_nearly_whole_divisor_case<FloatType>("100", "9.999999999999", false);
    test_fmod_nearly_whole_divisor_case<FloatType>("-100", "9.999999999999", true);
    test_fmod_zero_denominator<FloatType>();
    test_fmod_result_aliases_first_argument<FloatType>();
    test_fmod_result_aliases_second_argument<FloatType>();
    test_fmod_arithmetic_overloads<FloatType>();
    test_eval_fmod_backend_branch_cases<FloatType>();
}

} // namespace local

int main()
{
  local::test_fmod<local::cpp_dec_float_type>();
  local::test_fmod<local::cpp_bin_float_type>();

  return boost::report_errors();
}
