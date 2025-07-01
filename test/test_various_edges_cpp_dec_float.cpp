// Copyright 2023 - 2025 Matt Borland
// Copyright 2023 - 2025 Christopher Kormanyos
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Some parts of this test file have been taken from the Boost.Decimal project.

#include <boost/lexical_cast.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include <test_traits.hpp> // Note: include this AFTER the test-backends are defined

#include <chrono>
#include <limits>
#include <random>
#include <sstream>
#include <string>

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
  bool test_convert()
  {
    using float_type = FloatType;

    std::mt19937_64 gen { time_point<typename std::mt19937_64::result_type>() };

    auto dis =
      std::uniform_real_distribution<float>
      {
        static_cast<float>(1.01F),
        static_cast<float>(1.04F)
      };

    bool result_is_ok { true };

    using std::isinf;
    using std::isnan;
    using std::signbit;

    {
      for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(16)); ++index)
      {
        static_cast<void>(index);

        float_type flt_non_finite { };
        long double ldbl_non_finite { };
        bool result_non_finite_is_ok { };

        flt_non_finite = std::numeric_limits<float_type>::quiet_NaN() * dis(gen);
        ldbl_non_finite = static_cast<long double>(flt_non_finite);
        result_non_finite_is_ok = isnan(ldbl_non_finite);
        BOOST_TEST(result_non_finite_is_ok);
        result_is_ok = (result_non_finite_is_ok && result_is_ok);

        flt_non_finite = std::numeric_limits<float_type>::infinity() * dis(gen);
        ldbl_non_finite = static_cast<long double>(flt_non_finite);
        result_non_finite_is_ok = isinf(ldbl_non_finite);
        BOOST_TEST(result_non_finite_is_ok);
        result_is_ok = (result_non_finite_is_ok && result_is_ok);

        flt_non_finite = std::numeric_limits<float_type>::infinity() * -dis(gen);
        ldbl_non_finite = static_cast<long double>(flt_non_finite);
        result_non_finite_is_ok = (isinf(ldbl_non_finite) && signbit(ldbl_non_finite));
        BOOST_TEST(result_non_finite_is_ok);
        result_is_ok = (result_non_finite_is_ok && result_is_ok);

        flt_non_finite =   float_type { (std::numeric_limits<long double>::max)() }
                         * float_type { (std::numeric_limits<long double>::max)() }
                         * dis(gen);
        ldbl_non_finite = static_cast<long double>(flt_non_finite);
        result_non_finite_is_ok = isinf(ldbl_non_finite);
        BOOST_TEST(result_non_finite_is_ok);
        result_is_ok = (result_non_finite_is_ok && result_is_ok);

        flt_non_finite =   float_type { (std::numeric_limits<long double>::max)() }
                         * float_type { (std::numeric_limits<long double>::max)() }
                         * -dis(gen);
        ldbl_non_finite = static_cast<long double>(flt_non_finite);
        result_non_finite_is_ok = (isinf(ldbl_non_finite) && signbit(ldbl_non_finite));
        BOOST_TEST(result_non_finite_is_ok);
        result_is_ok = (result_non_finite_is_ok && result_is_ok);
      }
    }

    #ifdef BOOST_HAS_INT128
    {
      for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(64)); ++index)
      {
        float_type
          flt_n128
          {
              float_type { (std::numeric_limits<signed long long>::max)() }
            * float_type { dis(gen) * 12345.0F }
            * float_type { dis(gen) * dis(gen) }
          };

        const bool is_neg { ((index & 1U) != 0U) };

        if(is_neg)
        {
          flt_n128 = -flt_n128;
        }

        boost::int128_type val_n128 { static_cast<boost::int128_type>(flt_n128) };

        bool result_val_n128_is_ok { };

        const std::string str_n128 { boost::lexical_cast<std::string>(val_n128) };

        const std::string
          str_ctrl
          {
            [&flt_n128]()
            {
              std::stringstream strm { };

              strm << std::fixed << flt_n128;

              std::string str_local { strm.str() };

              str_local = str_local.substr(std::size_t { UINT8_C(0) }, str_local.find('.'));

              return str_local;
            }()
          };

        result_val_n128_is_ok =
          (
               (str_n128 == str_ctrl)
            && (
                 (!is_neg) ? (val_n128 > static_cast<boost::int128_type>((std::numeric_limits<signed long long>::max)()))
                           : (val_n128 < static_cast<boost::int128_type>((std::numeric_limits<signed long long>::max)()))
               )
          );

        BOOST_TEST(result_val_n128_is_ok);

        result_is_ok = (result_val_n128_is_ok && result_is_ok);
      }
    }

    {
      for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(32)); ++index)
      {
        constexpr boost::int128_type my_max_val_n128 = (((static_cast<boost::int128_type>(1) << (sizeof(boost::int128_type) * CHAR_BIT - 2)) - 1) << 1) + 1;
        constexpr boost::int128_type my_min_val_n128 = static_cast<boost::int128_type>(-my_max_val_n128 - 1);

        const float_type
          flt_n128
          {
              float_type { -my_max_val_n128 }
              - ::my_one<float_type>()
              + float_type { ::my_zero<float_type>() + float_type { dis(gen) } }
              - ::my_one<float_type>()
              - ::my_one<float_type>()
          };

        const boost::int128_type val_n128 { static_cast<boost::int128_type>(flt_n128) };

        const bool result_val_n128_is_ok { (val_n128 == my_min_val_n128) };

        BOOST_TEST(result_val_n128_is_ok);

        result_is_ok = (result_val_n128_is_ok && result_is_ok);
      }
    }
    #endif

    return result_is_ok;
  }

} // namespace local

auto main() -> int
{
  {
    using float_backend_type = boost::multiprecision::cpp_dec_float<50>;

    using float_type = boost::multiprecision::number<float_backend_type, boost::multiprecision::et_off>;

    std::cout << "Testing type: " << typeid(float_type).name() << std::endl;

    static_cast<void>(local::test_convert<float_type>());
  }

  return boost::report_errors();
}

template<typename FloatType> auto my_zero() noexcept -> FloatType& { using float_type = FloatType; static float_type val_zero { 0 }; return val_zero; }
template<typename FloatType> auto my_one () noexcept -> FloatType& { using float_type = FloatType; static float_type val_one  { 1 }; return val_one; }
template<typename FloatType> auto my_inf () noexcept -> FloatType& { using float_type = FloatType; static float_type val_inf  { std::numeric_limits<float_type>::infinity() }; return val_inf; }
template<typename FloatType> auto my_nan () noexcept -> FloatType& { using float_type = FloatType; static float_type val_nan  { std::numeric_limits<float_type>::quiet_NaN() }; return val_nan; }

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif
