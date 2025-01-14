// Copyright 2023 - 2024 Matt Borland
// Copyright 2023 - 2025 Christopher Kormanyos
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Some parts of this test file have been taken from the Boost.Decimal project.

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_double_fp.hpp>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <boost/core/lightweight_test.hpp>

template<typename FloatType> auto my_zero() noexcept -> FloatType&;
template<typename FloatType> auto my_one () noexcept -> FloatType&;
template<typename FloatType> auto my_inf () noexcept -> FloatType&;
template<typename FloatType> auto my_nan () noexcept -> FloatType&;
template<typename FloatType> auto my_exp1() noexcept -> FloatType&;

namespace local
{
  template<typename IntegralTimePointType,
           typename ClockType = std::chrono::high_resolution_clock>
  auto time_point() noexcept -> IntegralTimePointType
  {
    using local_integral_time_point_type = IntegralTimePointType;
    using local_clock_type               = ClockType;

    const auto current_now =
      static_cast<std::uintmax_t>
      (
        std::chrono::duration_cast<std::chrono::nanoseconds>
        (
          local_clock_type::now().time_since_epoch()
        ).count()
      );

    return static_cast<local_integral_time_point_type>(current_now);
  }

  template<typename NumericType>
  auto is_close_fraction(const NumericType& a,
                         const NumericType& b,
                         const NumericType& tol) noexcept -> bool
  {
    using std::fabs;

    auto result_is_ok = bool { };

    NumericType delta { };

    if(b == static_cast<NumericType>(0))
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

  template<class FloatType>
  bool test_edges()
  {
    using float_type = FloatType;
    using ctrl_type  = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100>, boost::multiprecision::et_off>;

    std::mt19937_64 gen;

    gen.seed(time_point<typename std::mt19937_64::result_type>());

    auto dis =
      std::uniform_real_distribution<float>
      {
        static_cast<float>(1.01F),
        static_cast<float>(1.04F)
      };

    bool result_is_ok { true };

    {
      float_type flt_val { (std::numeric_limits<float_type>::max)() };
      ctrl_type  ctl_val { flt_val };

      for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
      {
        static_cast<void>(i);

        float_type flt_denom { 2345.6F * dis(gen) };

        flt_val /= flt_denom;
        ctl_val /= ctrl_type { flt_denom };

        const bool result_div_is_ok { local::is_close_fraction(flt_val, float_type { ctl_val }, std::numeric_limits<float_type>::epsilon() * 32) };

        BOOST_TEST(result_div_is_ok);

        result_is_ok = (result_div_is_ok && result_is_ok);
      }
    }

    {
      float_type flt_val { (std::numeric_limits<float_type>::min)() };
      ctrl_type  ctl_val { flt_val };

      for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
      {
        static_cast<void>(i);

        float_type flt_factor { 2345.6F * dis(gen) };

        flt_val *= flt_factor;
        ctl_val *= ctrl_type { flt_factor };

        const bool result_mul_is_ok { local::is_close_fraction(flt_val, float_type { ctl_val }, std::numeric_limits<float_type>::epsilon() * 32) };

        BOOST_TEST(result_mul_is_ok);

        result_is_ok = (result_mul_is_ok && result_is_ok);
      }
    }

    {
      float_type flt_val { (std::numeric_limits<float_type>::max)() };
      ctrl_type  ctl_val { flt_val };

      for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
      {
        static_cast<void>(i);

        float_type flt_factor { 1234.56e-12F * dis(gen) };

        flt_val *= flt_factor;
        ctl_val *= ctrl_type { flt_factor };

        const bool result_mul_is_ok { local::is_close_fraction(flt_val, float_type { ctl_val }, std::numeric_limits<float_type>::epsilon() * 32) };

        BOOST_TEST(result_mul_is_ok);

        result_is_ok = (result_mul_is_ok && result_is_ok);
      }
    }

    {
      float_type flt_val { };
      ctrl_type  ctl_val { };

      for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
      {
        static_cast<void>(i);

        flt_val = sqrt((std::numeric_limits<float_type>::max)());
        ctl_val = ctrl_type { flt_val };

        float_type flt_factor { 123.456F * dis(gen) };

        unsigned j { static_cast<unsigned>(UINT8_C(0)) };

        for(; j < static_cast<unsigned>(UINT16_C(8192)); ++j)
        {
          if(unsigned { j % unsigned { UINT8_C(3) } } == unsigned { UINT8_C(0)})
          {
            flt_val = -flt_val;
            ctl_val = -ctl_val;
          }

          if(unsigned { j % unsigned { UINT8_C(2) } } == unsigned { UINT8_C(0)})
          {
            flt_factor = -flt_factor;
          }

          flt_val *= flt_factor;
          ctl_val *= ctrl_type { flt_factor };

          if(isinf(flt_val)) { break; }

          const bool result_finite_mul_is_ok
          {
                local::is_close_fraction(flt_val, float_type { ctl_val }, std::numeric_limits<float_type>::epsilon() * 32)
            && (signbit(flt_val) == signbit(ctl_val))
          };

          BOOST_TEST(result_finite_mul_is_ok);
        }

        const bool result_mul_is_ok
        {
             isinf(flt_val)
          && (j < unsigned { UINT16_C(8192) })
          && (signbit(flt_val) == signbit(ctl_val))
        };

        BOOST_TEST(result_mul_is_ok);

        result_is_ok = (result_mul_is_ok && result_is_ok);
      }
    }

    {
      const float_type inf_pos_01 { "1e100001" };
      const float_type inf_pos_02 { "1e100002" };
      const float_type inf_pos_03 { "1e100003" };
      const float_type inf_neg_01 { "-1e100001" };
      const float_type inf_neg_02 { "-1e100002" };
      const float_type inf_neg_03 { "-1e100003" };

      const float_type zero_01 { "1e-100001" };
      const float_type zero_02 { "1e-100002" };
      const float_type zero_03 { "1e-100003" };

      BOOST_TEST(result_is_ok = (isinf(inf_pos_01) && result_is_ok));
      BOOST_TEST(result_is_ok = (isinf(inf_pos_02) && result_is_ok));
      BOOST_TEST(result_is_ok = (isinf(inf_pos_03) && result_is_ok));

      BOOST_TEST(result_is_ok = (isinf(inf_neg_01) && signbit(inf_neg_01) && result_is_ok));
      BOOST_TEST(result_is_ok = (isinf(inf_neg_02) && signbit(inf_neg_02) && result_is_ok));
      BOOST_TEST(result_is_ok = (isinf(inf_neg_03) && signbit(inf_neg_03) && result_is_ok));

      BOOST_TEST(result_is_ok = ((fpclassify(zero_01) == FP_ZERO) && result_is_ok));
      BOOST_TEST(result_is_ok = ((fpclassify(zero_02) == FP_ZERO) && result_is_ok));
      BOOST_TEST(result_is_ok = ((fpclassify(zero_03) == FP_ZERO) && result_is_ok));
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(8)); ++i)
    {
      static_cast<void>(i);

      float_type flt_x { 2345.6F + static_cast<float>(i) * dis(gen) };

      const float_type quotient_one = flt_x /= flt_x;

      BOOST_TEST(result_is_ok = ((quotient_one == 1) && result_is_ok));
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(8)); ++i)
    {
      static_cast<void>(i);

      float_type flt_x { 1000.0F + static_cast<float>(i) * dis(gen) };

      const float_type sub_result_zero = flt_x -= flt_x;

      BOOST_TEST(result_is_ok = ((fpclassify(sub_result_zero) == FP_ZERO) && result_is_ok));
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(8)); ++i)
    {
      static_cast<void>(i);

      float_type flt_x { float_type { (std::numeric_limits<signed long long>::max)() } * static_cast<float>((static_cast<int>(i) + 1) * 2) * dis(gen) };

      const signed long long conversion_result_max { static_cast<signed long long>(flt_x) };

      BOOST_TEST(result_is_ok = ((conversion_result_max == (std::numeric_limits<signed long long>::max)()) && result_is_ok));
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(8)); ++i)
    {
      static_cast<void>(i);

      float_type flt_x { float_type { (std::numeric_limits<signed long long>::min)() } * static_cast<float>((static_cast<int>(i) + 1) * 2) * dis(gen) };

      const signed long long conversion_result_min { static_cast<signed long long>(flt_x) };

      BOOST_TEST(result_is_ok = ((conversion_result_min == (std::numeric_limits<signed long long>::min)()) && result_is_ok));
    }

    return result_is_ok;
  }

  template<class FloatType>
  bool test_edges_extra()
  {
    using float_type = FloatType;

    std::mt19937_64 gen;

    gen.seed(time_point<typename std::mt19937_64::result_type>());

    auto dis =
      std::uniform_real_distribution<float>
      {
        static_cast<float>(1.01F),
        static_cast<float>(1.04F)
      };

    bool result_is_ok { true };

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(8)); ++i)
    {
      static_cast<void>(i);

      float_type flt_x { float_type { (std::numeric_limits<unsigned long long>::max)() } * static_cast<float>((static_cast<int>(i) + 1) * 2) * dis(gen) };

      const unsigned long long conversion_result_max { static_cast<unsigned long long>(flt_x) };

      BOOST_TEST(result_is_ok = ((conversion_result_max == (std::numeric_limits<unsigned long long>::max)()) && result_is_ok));
    }

    return result_is_ok;
  }

  template<typename FloatType>
  auto test_sqrt_edge() -> bool
  {
    using float_type = FloatType;

    std::mt19937_64 gen;

    gen.seed(time_point<typename std::mt19937_64::result_type>());

    std::uniform_real_distribution<float>
      dist
      (
        static_cast<float>(1.01L),
        static_cast<float>(1.04L)
      );

    auto result_is_ok = true;

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(4)); ++i)
    {
      static_cast<void>(i);

      const auto val_nan = sqrt(-std::numeric_limits<float_type>::quiet_NaN() * static_cast<float_type>(dist(gen)));

      const auto result_val_nan_is_ok = isnan(val_nan);

      BOOST_TEST(result_val_nan_is_ok);

      result_is_ok = (result_val_nan_is_ok && result_is_ok);
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(4)); ++i)
    {
      static_cast<void>(i);

      const auto val_inf_pos = sqrt(std::numeric_limits<float_type>::infinity() * static_cast<float_type>(dist(gen)));

      const auto result_val_inf_pos_is_ok = (isinf(val_inf_pos) && (!signbit(val_inf_pos)));

      BOOST_TEST(result_val_inf_pos_is_ok);

      result_is_ok = (result_val_inf_pos_is_ok && result_is_ok);
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(4)); ++i)
    {
      static_cast<void>(i);

      const auto val_one = sqrt(::my_one<float_type>());

      const auto result_val_one_is_ok = (val_one == ::my_one<float_type>());

      BOOST_TEST(result_val_one_is_ok);

      result_is_ok = (result_val_one_is_ok && result_is_ok);
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(4)); ++i)
    {
      static_cast<void>(i);

      const auto val_zero = sqrt(::my_zero<float_type>());

      const auto result_val_zero_is_ok = ((val_zero == ::my_zero<float_type>()) && (!signbit(val_zero)));

      BOOST_TEST(result_val_zero_is_ok);

      result_is_ok = (result_val_zero_is_ok && result_is_ok);
    }

    return result_is_ok;
  }

  template<typename FloatType>
  auto test_exp_edge() -> bool
  {
    using float_type = FloatType;

    std::mt19937_64 gen;

    gen.seed(time_point<typename std::mt19937_64::result_type>());

    std::uniform_real_distribution<float>
      dist
      (
        static_cast<float>(1.01L),
        static_cast<float>(1.08L)
      );

    auto result_is_ok = true;

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
    {
      static_cast<void>(i);

      const float_type val_nan { exp(std::numeric_limits<float_type>::quiet_NaN() * static_cast<float_type>(dist(gen))) };

      const bool result_val_nan_is_ok { isnan(val_nan) };

      BOOST_TEST(result_val_nan_is_ok);

      result_is_ok = (result_val_nan_is_ok && result_is_ok);
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
    {
      static_cast<void>(i);

      const float_type arg_inf { std::numeric_limits<float_type>::infinity() * static_cast<float_type>(dist(gen)) };

      const float_type val_inf_pos { exp(arg_inf) };

      const bool result_val_inf_pos_is_ok { (fpclassify(val_inf_pos) == FP_INFINITE) };

      BOOST_TEST(result_val_inf_pos_is_ok);

      result_is_ok = (result_val_inf_pos_is_ok && result_is_ok);
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
    {
      static_cast<void>(i);

      const float_type val_inf_neg { exp(-std::numeric_limits<float_type>::infinity() * static_cast<float_type>(dist(gen))) };

      const bool result_val_inf_neg_is_ok { (val_inf_neg == ::my_zero<float_type>()) };

      BOOST_TEST(result_val_inf_neg_is_ok);

      result_is_ok = (result_val_inf_neg_is_ok && result_is_ok);
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
    {
      static_cast<void>(i);

      const float_type val_zero { exp(::my_zero<float_type>() * static_cast<float_type>(dist(gen))) };

      const bool result_val_zero_is_ok { (val_zero == ::my_one<float_type>()) };

      BOOST_TEST(result_val_zero_is_ok);

      result_is_ok = (result_val_zero_is_ok && result_is_ok);
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
    {
      static_cast<void>(i);

      const float_type arg_large { ldexp(float_type { 3.14F } * static_cast<float_type>(dist(gen)), static_cast<int>(static_cast<float>(std::numeric_limits<float_type>::max_exponent) * 0.8)) };

      const float_type result_exp_large { exp(arg_large) };

      const bool result_exp_large_is_ok { isinf(result_exp_large) };

      BOOST_TEST(result_exp_large_is_ok);

      result_is_ok = (result_exp_large_is_ok && result_is_ok);
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
    {
      static_cast<void>(i);

      const float_type arg_small { ldexp(float_type { -3.14F } * static_cast<float_type>(dist(gen)), static_cast<int>(static_cast<float>(std::numeric_limits<float_type>::max_exponent) * 0.8)) };

      const float_type result_exp_small { exp(arg_small) };

      const bool result_exp_small_is_ok { (fpclassify(result_exp_small) == FP_ZERO) };

      BOOST_TEST(result_exp_small_is_ok);

      result_is_ok = (result_exp_small_is_ok && result_is_ok);
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
    {
      static_cast<void>(i);

      const float_type one { 1 };

      const float_type
        arg_near_one
        {
            one
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
        };

      using ctrl_type  = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100>, boost::multiprecision::et_off>;

      const float_type result_exp_n { exp(float_type { static_cast<int>(arg_near_one) }) };
      const ctrl_type result_ctrl   { exp(ctrl_type { static_cast<int>(arg_near_one) }) }; 

      bool result_exp_n_is_ok
      {
        local::is_close_fraction
        (
          result_exp_n,
          float_type { result_ctrl },
          std::numeric_limits<float_type>::epsilon() * 512
        )
      };

      if(static_cast<int>(arg_near_one) == 1)
      {
        const bool result_exp_n_at_one_is_ok
        {
          local::is_close_fraction
          (
            result_exp_n,
            ::my_exp1<float_type>(),
            std::numeric_limits<float_type>::epsilon() * 16
          )
        };

        result_exp_n_is_ok = (result_exp_n_at_one_is_ok && result_exp_n_is_ok);
      }

      BOOST_TEST(result_exp_n_is_ok);

      result_is_ok = (result_exp_n_is_ok && result_is_ok);
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
    {
      static_cast<void>(i);

      const float_type one { 1 };

      const float_type
        arg_near_one
        {
            one
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
          * static_cast<float_type>(dist(gen))
        };

      using ctrl_type  = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100>, boost::multiprecision::et_off>;

      const float_type result_exp_n { exp(float_type { static_cast<int>(-arg_near_one) }) };
      const ctrl_type result_ctrl   { exp(ctrl_type { static_cast<int>(-arg_near_one) }) }; 

      bool result_exp_n_is_ok
      {
        local::is_close_fraction
        (
          result_exp_n,
          float_type { result_ctrl },
          std::numeric_limits<float_type>::epsilon() * 512
        )
      };

      if(static_cast<int>(-arg_near_one) == -1)
      {
        const bool result_exp_n_at_one_is_ok
        {
          local::is_close_fraction
          (
            result_exp_n,
            1 / ::my_exp1<float_type>(),
            std::numeric_limits<float_type>::epsilon() * 16
          )
        };

        result_exp_n_is_ok = (result_exp_n_at_one_is_ok && result_exp_n_is_ok);
      }

      BOOST_TEST(result_exp_n_is_ok);

      result_is_ok = (result_exp_n_is_ok && result_is_ok);
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
    {
      static_cast<void>(i);

      const float_type arg_huge { sqrt((std::numeric_limits<float_type>::max)()) * static_cast<float_type>(dist(gen)) };

      const float_type result_exp_huge { exp(arg_huge) };

      const bool result_exp_huge_is_ok { (fpclassify(result_exp_huge) == FP_INFINITE) };

      BOOST_TEST(result_exp_huge_is_ok);

      result_is_ok = (result_exp_huge_is_ok && result_is_ok);
    }

    for(auto i = static_cast<unsigned>(UINT8_C(0)); i < static_cast<unsigned>(UINT8_C(16)); ++i)
    {
      static_cast<void>(i);

      const float_type arg_tiny { -sqrt((std::numeric_limits<float_type>::max)()) * static_cast<float_type>(dist(gen)) };

      const float_type result_exp_tiny { exp(arg_tiny) };

      const bool result_exp_tiny_is_ok { (fpclassify(result_exp_tiny) == FP_ZERO) };

      BOOST_TEST(result_exp_tiny_is_ok);

      result_is_ok = (result_exp_tiny_is_ok && result_is_ok);
    }

    return result_is_ok;
  }

  template<typename FloatType>
  auto test_log_edge() -> bool
  {
    using float_type = FloatType;

    std::mt19937_64 gen;

    gen.seed(time_point<typename std::mt19937_64::result_type>());

    std::uniform_real_distribution<float>
      dist
      (
        static_cast<float>(1.01L),
        static_cast<float>(1.04L)
      );

    auto result_is_ok = true;

    for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(8)); ++index)
    {
      static_cast<void>(index);

      const float_type arg_zero = ::my_zero<float_type>() * static_cast<float_type>(dist(gen));

      const auto log_zero = log(arg_zero);

      const volatile bool result_log_zero_is_ok
      {
           (fpclassify(arg_zero) == FP_ZERO)
        && isinf(log_zero)
        && signbit(log_zero)
      };

      BOOST_TEST(result_log_zero_is_ok);

      result_is_ok = (result_log_zero_is_ok && result_is_ok);
    }

    for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(8)); ++index)
    {
      static_cast<void>(index);

      float_type arg_one =
        static_cast<float_type>
        (
          static_cast<int>(::my_one<float_type>() * static_cast<float_type>(dist(gen)))
        );

      const auto log_one = log(arg_one);

      const volatile auto result_log_one_is_ok = ((arg_one == ::my_one<float_type>()) && (log_one == ::my_zero<float_type>()));

      BOOST_TEST(result_log_one_is_ok);

      result_is_ok = (result_log_one_is_ok && result_is_ok);
    }

    for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(8)); ++index)
    {
      static_cast<void>(index);

      float_type arg_one_minus =
        static_cast<float_type>
        (
          -static_cast<int>(::my_one<float_type>() * static_cast<float_type>(dist(gen)))
        );

      const auto log_one_minus = log(arg_one_minus);

      const volatile auto result_log_one_minus_is_ok = ((-arg_one_minus == ::my_one<float_type>()) &&  isnan(log_one_minus));

      BOOST_TEST(result_log_one_minus_is_ok);

      result_is_ok = (result_log_one_minus_is_ok && result_is_ok);
    }

    for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(8)); ++index)
    {
      static_cast<void>(index);

      const auto log_inf = log(::my_inf<float_type>() * static_cast<float_type>(dist(gen)));

      const volatile auto result_log_inf_is_ok = isinf(log_inf);

      BOOST_TEST(result_log_inf_is_ok);

      result_is_ok = (result_log_inf_is_ok && result_is_ok);
    }

    for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(8)); ++index)
    {
      static_cast<void>(index);

      const auto log_inf_minus = log(-::my_inf<float_type>() * static_cast<float_type>(dist(gen)));

      const volatile auto result_log_inf_minus_is_ok = isnan(log_inf_minus);

      BOOST_TEST(result_log_inf_minus_is_ok);

      result_is_ok = (result_log_inf_minus_is_ok && result_is_ok);
    }

    for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(8)); ++index)
    {
      static_cast<void>(index);

      const float_type arg_nan = ::my_nan<float_type>() * static_cast<float_type>(dist(gen));

      const float_type log_nan = log(arg_nan);

      const volatile auto result_log_nan_is_ok = (isnan(arg_nan) && isnan(log_nan));

      BOOST_TEST(result_log_nan_is_ok);

      result_is_ok = (result_log_nan_is_ok && result_is_ok);
    }

    return result_is_ok;
  }

} // namespace local

auto main() -> int
{
  {
    using float_type = boost::multiprecision::cpp_double_float;

    static_cast<void>(local::test_edges<float_type>());
    static_cast<void>(local::test_edges_extra<float_type>());
    local::test_sqrt_edge<float_type>();
    local::test_exp_edge<float_type>();
    local::test_log_edge<float_type>();
  }

  {
    using float_type = boost::multiprecision::cpp_double_double;

    static_cast<void>(local::test_edges<float_type>());
    static_cast<void>(local::test_edges_extra<float_type>());
    local::test_sqrt_edge<float_type>();
    local::test_exp_edge<float_type>();
    local::test_log_edge<float_type>();
  }

  {
    using float_type = boost::multiprecision::cpp_double_long_double;

    static_cast<void>(local::test_edges<float_type>());
    static_cast<void>(local::test_edges_extra<float_type>());
    local::test_sqrt_edge<float_type>();
    local::test_exp_edge<float_type>();
    local::test_log_edge<float_type>();
  }

  {
    using float_backend_type = boost::multiprecision::cpp_bin_float<106, boost::multiprecision::digit_base_2, void, int, -1021, +1024>;

    using float_type = boost::multiprecision::number<float_backend_type, boost::multiprecision::et_off>;

    static_cast<void>(local::test_edges<float_type>());
    local::test_sqrt_edge<float_type>();
    local::test_exp_edge<float_type>();
    local::test_log_edge<float_type>();
  }

  return boost::report_errors();
}

template<typename FloatType> auto my_zero() noexcept -> FloatType& { using float_type = FloatType; static float_type val_zero { 0 }; return val_zero; }
template<typename FloatType> auto my_one () noexcept -> FloatType& { using float_type = FloatType; static float_type val_one  { 1 }; return val_one; }
template<typename FloatType> auto my_inf () noexcept -> FloatType& { using float_type = FloatType; static float_type val_inf  { std::numeric_limits<float_type>::infinity() }; return val_inf; }
template<typename FloatType> auto my_nan () noexcept -> FloatType& { using float_type = FloatType; static float_type val_nan  { std::numeric_limits<float_type>::quiet_NaN() }; return val_nan; }
template<typename FloatType> auto my_exp1() noexcept -> FloatType& { using float_type = FloatType; static float_type val_exp1 { "2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178525166427427466391932003059921817413596629043572900334295260595630738132328627943490763233829880753195251019011573834187930702154089149934884167509244761460668082265" }; return val_exp1; }
