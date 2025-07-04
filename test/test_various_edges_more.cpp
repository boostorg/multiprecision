// Copyright 2023 - 2025 Matt Borland
// Copyright 2023 - 2025 Christopher Kormanyos
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Some parts of this test file have been taken from the Boost.Decimal project.

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_double_fp.hpp>

#include <test_traits.hpp> // Note: include this AFTER the test-backends are defined

#include <chrono>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#if (defined(__clang__) || defined(__GNUC__))
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

    typename local_clock_type::time_point tp_zero { };

    const auto my_now = local_clock_type::now();

    const auto duration { my_now - tp_zero };

    const std::uintmax_t
      value
      {
        static_cast<std::uintmax_t>
        (
          std::chrono::duration_cast<std::chrono::nanoseconds>(my_now - tp_zero).count()
        )
      };

    return static_cast<local_integral_time_point_type>(value);
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

      result_is_ok = (delta <= tol); // LCOV_EXCL_LINE
    }
    else
    {
      delta = fabs(1 - (a / b));

      result_is_ok = (delta <= tol);
    }

    return result_is_ok;
  }

  template<class AnyFloatType, class OtherFloatType>
  bool test_convert_and_back(const float epsilon_factor)
  {
    using any_float_type = AnyFloatType;
    using other_float_type = OtherFloatType;

    std::mt19937_64 gen { time_point<typename std::mt19937_64::result_type>() };

    auto dis =
      std::uniform_real_distribution<float>
      {
        static_cast<float>(-1.5F),
        static_cast<float>(1.5F)
      };

    bool result_is_ok { true };

    for(int n_loop = static_cast<int>(INT8_C(0)); n_loop < static_cast<int>(INT8_C(64)); ++n_loop)
    {
      static_cast<void>(n_loop);

      using string_data_array_type = std::array<std::string, std::size_t { UINT8_C(5) }>;

      // Table[N[Exp[Pi   EulerGamma*m], 100], {m, 1, 41, 10}]
      // 6.131114182422604828954743181715560166328314533478636289880930665602805209787080979043183175178343525
      // 4.601860472890328982970020344164597095017995523702194983334509372565826509463730150117899613177592949*10^8
      // 3.454041008184686240269652058630805927805404685705614002929256804267342980143530922708514273650048389*10^16
      // 2.592516517287682590319488890844344913780990651072642232646840608443538857001634005575916140667342752*10^24
      // 1.945877850460679347837790546862626024227287564971475316486097406542338497232351222296834286913627889*10^32

      const string_data_array_type
        float_number_strings
        {
          std::string("6.131114182422604828954743181715560166328314533478636289880930665602805209787080979043183175178343525"),
          std::string("4.601860472890328982970020344164597095017995523702194983334509372565826509463730150117899613177592949E8"),
          std::string("3.454041008184686240269652058630805927805404685705614002929256804267342980143530922708514273650048389E16"),
          std::string("2.592516517287682590319488890844344913780990651072642232646840608443538857001634005575916140667342752E24"),
          std::string("1.945877850460679347837790546862626024227287564971475316486097406542338497232351222296834286913627889E32")
        };

      for(std::size_t index { std::size_t { UINT8_C(0) }}; index < std::tuple_size<string_data_array_type>::value; ++index)
      {
        const any_float_type start(any_float_type(float_number_strings[index]) * (dis(gen)* dis(gen)));
        const other_float_type other(start);
        const any_float_type backto(other);

        const bool
          result_of_trip_is_ok
          {
            local::is_close_fraction
            (
              start,
              backto,
              any_float_type(std::numeric_limits<any_float_type>::epsilon() * epsilon_factor)
            )
          };

        BOOST_TEST(result_of_trip_is_ok);

        result_is_ok = (result_of_trip_is_ok && result_is_ok);
      }
    }

    return result_is_ok;
  }

  template<class FloatType>
  bool test_edges()
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
        result_non_finite_is_ok = (isnan)(ldbl_non_finite);
        BOOST_TEST(result_non_finite_is_ok);
        result_is_ok = (result_non_finite_is_ok && result_is_ok);

        flt_non_finite = std::numeric_limits<float_type>::infinity() * dis(gen);
        ldbl_non_finite = static_cast<long double>(flt_non_finite);
        result_non_finite_is_ok = (isinf)(ldbl_non_finite);
        BOOST_TEST(result_non_finite_is_ok);
        result_is_ok = (result_non_finite_is_ok && result_is_ok);

        flt_non_finite = std::numeric_limits<float_type>::infinity() * -dis(gen);
        ldbl_non_finite = static_cast<long double>(flt_non_finite);
        result_non_finite_is_ok = ((isinf)(ldbl_non_finite) && signbit(ldbl_non_finite));
        BOOST_TEST(result_non_finite_is_ok);
        result_is_ok = (result_non_finite_is_ok && result_is_ok);

        flt_non_finite =   float_type { (std::numeric_limits<long double>::max)() }
                         * float_type { (std::numeric_limits<long double>::max)() }
                         * dis(gen);
        ldbl_non_finite = static_cast<long double>(flt_non_finite);
        result_non_finite_is_ok = (isinf)(ldbl_non_finite);
        BOOST_TEST(result_non_finite_is_ok);
        result_is_ok = (result_non_finite_is_ok && result_is_ok);

        flt_non_finite =   float_type { (std::numeric_limits<long double>::max)() }
                         * float_type { (std::numeric_limits<long double>::max)() }
                         * -dis(gen);
        ldbl_non_finite = static_cast<long double>(flt_non_finite);
        result_non_finite_is_ok = ((isinf)(ldbl_non_finite) && signbit(ldbl_non_finite));
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

        const auto
          str128_maker
          {
            [](const float_type& flt)
            {
              std::stringstream strm { };

              strm << std::fixed << flt;

              std::string str_local { strm.str() };

              str_local = str_local.substr(std::size_t { UINT8_C(0) }, str_local.find('.'));

              return str_local;
            }
          };

        const std::string str_ctrl { str128_maker(flt_n128) };
        const std::string str_n128 { str128_maker(static_cast<float_type>(val_n128)) };

        // This test assumes that boost::int128_type is as wide or wider than signed long long.
        // Consider using a a kind of if-constexpr to verify this "up front".

        result_val_n128_is_ok =
          (
               (str_n128 == str_ctrl)
            && (
                 (!is_neg) ? (val_n128 >= static_cast<boost::int128_type>((std::numeric_limits<signed long long>::max)()))
                           : (val_n128 <= static_cast<boost::int128_type>((std::numeric_limits<signed long long>::max)()))
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

    {
      for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(16)); ++index)
      {
        static_cast<void>(index);

        const float_type flt_under_one { float_type { ::my_one<float_type>() / dis(gen) } / dis(gen) };

        const bool
          result_under_one_is_ok
          {
            (static_cast<boost::int128_type>(flt_under_one) == static_cast<boost::int128_type>(INT8_C(0)))
          };

        BOOST_TEST(result_under_one_is_ok);

        result_is_ok = (result_under_one_is_ok && result_is_ok);
      }
    }
    #endif

    {
      const std::initializer_list<std::string>
        funky_strings_list
        {
          std::string("3.14u"),   std::string("3.14l"),   std::string("3.14U"),   std::string("3.14L"),
          std::string("3.14ul"),  std::string("3.14Ul"),  std::string("3.14uL"),  std::string("3.14UL"),
          std::string("3.14ull"), std::string("3.14Ull"), std::string("3.14ULl"), std::string("3.14ULL"), std::string("3.14uLL"),  std::string("3.14ulL")
        };

      const std::vector<std::string> funky_strings(funky_strings_list);

      std::size_t funky_count { };

      for(const auto& str : funky_strings)
      {
        try
        {
          const float_type flt_from_bad_string(str);

          static_cast<void>(flt_from_bad_string);
        }
        catch(const std::runtime_error& excp)
        {
          static_cast<void>(excp.what());

          ++funky_count;
        }
      }

      const bool result_funky_strings_is_ok { (funky_count == funky_strings.size()) };

      BOOST_TEST(result_funky_strings_is_ok);

      result_is_ok = (result_funky_strings_is_ok && result_is_ok);
    }

    {
      for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(16)); ++index)
      {
        static_cast<void>(index);

        float_type flt_nrm { ::my_one<float_type>() * dis(gen) };

        const bool is_neg { ((index & 1U) != 0U) };

        if(is_neg)
        {
          flt_nrm = -flt_nrm;
        }

        const float_type flt_zer { ::my_zero<float_type>() * dis(gen) };

        flt_nrm /= flt_zer;

        bool result_div_zero_is_ok { (boost::multiprecision::isinf)(flt_nrm) };

        if(is_neg)
        {
          result_div_zero_is_ok = ((boost::multiprecision::signbit)(flt_nrm) && result_div_zero_is_ok);
        }

        BOOST_TEST(result_div_zero_is_ok);

        result_is_ok = (result_div_zero_is_ok && result_is_ok);
      }
    }

    {
      for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(16)); ++index)
      {
        static_cast<void>(index);

        float_type flt_nrm { ::my_one<float_type>() + 0.25F };
        float_type flt_one { ::my_one<float_type>() + ::my_zero<float_type>() * dis(gen) };

        const bool is_neg { ((index & 1U) != 0U) };

        if(is_neg)
        {
          flt_one = -flt_one;
        }

        flt_nrm /= flt_one;

        bool result_div_one_is_ok { (is_neg ? (flt_nrm == -1.25F) : (flt_nrm == 1.25F)) };

        BOOST_TEST(result_div_one_is_ok);

        result_is_ok = (result_div_one_is_ok && result_is_ok);
      }
    }

    constexpr unsigned max_index { 1024U };

    {
      using std::ldexp;

      float_type flt_near_min { ldexp((std::numeric_limits<float_type>::min)(), +64) };

      unsigned index { };

      while((index < max_index) && (!((boost::multiprecision::fpclassify)(flt_near_min) == FP_ZERO)))
      {
        flt_near_min /= static_cast<unsigned long long>(0xDEADBEEF);

        ++index;
      }

      const bool result_unf_is_ok { ((index > 1U) && (index < max_index)) };

      BOOST_TEST(result_unf_is_ok);

      result_is_ok = (result_unf_is_ok && result_is_ok);
    }

    {
      using std::ldexp;

      float_type flt_near_min { ldexp((std::numeric_limits<float_type>::min)(), +64) };

      unsigned index { };

      while((index < max_index) && (!((boost::multiprecision::fpclassify)(flt_near_min) == FP_ZERO)))
      {
        flt_near_min /= static_cast<unsigned long long>(100000000ULL - 3ULL);

        ++index;
      }

      const bool result_unf_is_ok { ((index > 1U) && (index < max_index)) };

      BOOST_TEST(result_unf_is_ok);

      result_is_ok = (result_unf_is_ok && result_is_ok);
    }

    {
      using std::ldexp;

      float_type flt_near_max { ldexp((std::numeric_limits<float_type>::max)(), -1) };

      const float_type flt_less_near_max { ldexp((std::numeric_limits<float_type>::max)(), -4) };

      unsigned index { };

      while((index < max_index) && (!(boost::multiprecision::isinf)(flt_near_max)))
      {
        flt_near_max += (flt_less_near_max * dis(gen));

        ++index;
      }

      const bool result_ovf_is_ok { ((index > 1U) && (index < max_index)) };

      BOOST_TEST(result_ovf_is_ok);

      result_is_ok = (result_ovf_is_ok && result_is_ok);
    }

    {
      using std::ldexp;

      float_type flt_near_lowest { -ldexp((std::numeric_limits<float_type>::max)(), -1) };

      const float_type flt_less_near_max {  ldexp((std::numeric_limits<float_type>::max)(), -4) };

      unsigned index { };

      while((index < max_index) && (!(boost::multiprecision::isinf)(flt_near_lowest)))
      {
        flt_near_lowest -= (flt_less_near_max * dis(gen));

        ++index;
      }

      const bool result_ovf_is_ok { ((index > 1U) && (index < max_index)) && signbit(flt_near_lowest) };

      BOOST_TEST(result_ovf_is_ok);

      result_is_ok = (result_ovf_is_ok && result_is_ok);
    }

    for(int n_loop = static_cast<int>(INT8_C(-16)); n_loop < static_cast<int>(INT8_C(-8)); ++n_loop)
    {
      using std::ldexp;
      using std::fabs;

      float_type flt_near_max { ldexp((std::numeric_limits<float_type>::max)(), n_loop) * dis(gen) };

      unsigned index { };

      while((index < max_index) && (!(boost::multiprecision::isinf)(flt_near_max)))
      {
        flt_near_max *= static_cast<unsigned long long>(INT32_C(2));

        ++index;
      }

      const bool result_ovf_is_ok { ((index > 1U) && (index < max_index)) };

      BOOST_TEST(result_ovf_is_ok);

      result_is_ok = (result_ovf_is_ok && result_is_ok);
    }

    {
      for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(16)); ++index)
      {
        static_cast<void>(index);

        const float_type flt_nan_denominator { float_type { std::numeric_limits<float>::quiet_NaN() } * dis(gen) };

        float_type flt_zero_numerator { ::my_zero<float_type>() * dis(gen) };

        const bool
          result_div_zero_by_nan_is_ok
          {
               (boost::multiprecision::isnan)(flt_nan_denominator)
            && (boost::multiprecision::isnan)(flt_zero_numerator /= flt_nan_denominator)
          };

        BOOST_TEST(result_div_zero_by_nan_is_ok);

        result_is_ok = (result_div_zero_by_nan_is_ok && result_is_ok);
      }
    }

    {
      for(auto index = static_cast<unsigned>(UINT8_C(0)); index < static_cast<unsigned>(UINT8_C(16)); ++index)
      {
        static_cast<void>(index);

        float_type flt_zer_numerator { ::my_zero<float_type>() * dis(gen) };
        float_type flt_nrm_numerator { ::my_one<float_type>() * dis(gen) };
        float_type flt_nan_numerator { ::my_nan<float_type>() * dis(gen) };
        float_type flt_inf_numerator { ::my_inf<float_type>() * dis(gen) };

        const float_type flt_inf_result_neg_01 { -flt_nrm_numerator /= static_cast<signed long long>(INT8_C(0)) };
        const float_type flt_inf_result_neg_02 { -flt_inf_numerator *= static_cast<signed long long>(index + 2U) };

        const bool result_edge_00 { (boost::multiprecision::isnan)(flt_zer_numerator /= static_cast<signed long long>(INT8_C(0))) };
        const bool result_edge_01 { (boost::multiprecision::isinf)(flt_nrm_numerator /= static_cast<signed long long>(INT8_C(0))) };
        const bool result_edge_02 { (boost::multiprecision::isinf)(flt_inf_result_neg_01) && (boost::multiprecision::signbit)(flt_inf_result_neg_01) };
        const bool result_edge_03 { (boost::multiprecision::isnan)(flt_nan_numerator /= static_cast<signed long long>(index + 2U)) };
        const bool result_edge_04 { (boost::multiprecision::isnan)(flt_nan_numerator *= static_cast<signed long long>(index + 2U)) };
        const bool result_edge_05 { (boost::multiprecision::isnan)(flt_inf_numerator *= static_cast<signed long long>(0)) };
        const bool result_edge_06 { (boost::multiprecision::isinf)(flt_inf_result_neg_02) && (boost::multiprecision::signbit)(flt_inf_result_neg_02) };

        const bool
          result_divs_are_ok
          {
            (
                 result_edge_00
              && result_edge_01
              && result_edge_02
              && result_edge_03
              && result_edge_04
              && result_edge_05
              && result_edge_06
            )
          };

        BOOST_TEST(result_divs_are_ok);

        result_is_ok = (result_divs_are_ok && result_is_ok);
      }
    }

    return result_is_ok;
  }

  template<class OtherFloatType>
  auto test_convert_and_back_caller(const float epsilon_factor = 0.0F) -> bool
  {
    using other_float_type = OtherFloatType;

    std::cout << "Testing type (in test_convert_and_back): " << typeid(other_float_type).name() << std::endl;

    const bool
      result_of_trip_is_ok
      {
        local::test_convert_and_back<boost::multiprecision::cpp_double_double, other_float_type>(epsilon_factor)
      };

    BOOST_TEST(result_of_trip_is_ok);

    return result_of_trip_is_ok;
  }


  template<class CppDecFloatType>
  auto test_cpp_dec_float_rd_ovf_unf() -> void
  {
    using local_cpp_dec_float_type = CppDecFloatType;

    {
      typename local_cpp_dec_float_type::backend_type::exponent_type max_exp { };

      std::string str_max_pos_01("+1.0E+");
      std::string str_max_pos_02("+2.0E+");
      std::string str_max_pos_03("+3.0E+");
      std::string str_max_pos_04("+0.9E+");

      std::string str_max_neg_01("-1.0E+");
      std::string str_max_neg_02("-2.0E+");
      std::string str_max_neg_03("-3.0E+");
      std::string str_max_neg_04("-0.9E+");

      {
        std::stringstream strm { };

        strm << std::numeric_limits<local_cpp_dec_float_type>::max_exponent10;

        str_max_pos_01 += strm.str();
        str_max_pos_02 += strm.str();
        str_max_pos_03 += strm.str();
        str_max_pos_04 += strm.str();

        str_max_neg_01 += strm.str();
        str_max_neg_02 += strm.str();
        str_max_neg_03 += strm.str();
        str_max_neg_04 += strm.str();
      }

      const local_cpp_dec_float_type max_pos_01(str_max_pos_01);
      const local_cpp_dec_float_type max_pos_02(str_max_pos_02);
      const local_cpp_dec_float_type max_pos_03(str_max_pos_03);
      const local_cpp_dec_float_type max_pos_04(str_max_pos_04);

      const local_cpp_dec_float_type max_neg_01(str_max_neg_01);
      const local_cpp_dec_float_type max_neg_02(str_max_neg_02);
      const local_cpp_dec_float_type max_neg_03(str_max_neg_03);
      const local_cpp_dec_float_type max_neg_04(str_max_neg_04);

      using std::signbit;

      BOOST_TEST(max_pos_01 == (std::numeric_limits<local_cpp_dec_float_type>::max)());
      BOOST_TEST((boost::multiprecision::isinf)(max_pos_02) && (!signbit(max_pos_02)));
      BOOST_TEST((boost::multiprecision::isinf)(max_pos_03) && (!signbit(max_pos_03)));
      BOOST_TEST(((boost::multiprecision::fpclassify)(max_pos_04) == FP_NORMAL) && (!signbit(max_pos_04)));

      BOOST_TEST(max_neg_01 == (std::numeric_limits<local_cpp_dec_float_type>::lowest)());
      BOOST_TEST((boost::multiprecision::isinf)(max_neg_02) && signbit(max_neg_02));
      BOOST_TEST((boost::multiprecision::isinf)(max_neg_03) && signbit(max_neg_03));
      BOOST_TEST(((boost::multiprecision::fpclassify)(max_neg_04) == FP_NORMAL) && signbit(max_neg_04));
    }

    {
      std::string str_min_pos_01("+1.0E");
      std::string str_min_pos_02("+1.1E");
      std::string str_min_pos_03("+0.9E");

      std::string str_min_neg_01("-1.0E");
      std::string str_min_neg_02("-1.1E");
      std::string str_min_neg_03("-0.9E");

      {
        std::stringstream strm { };

        strm << std::numeric_limits<local_cpp_dec_float_type>::min_exponent10;

        str_min_pos_01 += strm.str();
        str_min_pos_02 += strm.str();
        str_min_pos_03 += strm.str();

        str_min_neg_01 += strm.str();
        str_min_neg_02 += strm.str();
        str_min_neg_03 += strm.str();
      }

      const local_cpp_dec_float_type min_pos_01(str_min_pos_01);
      const local_cpp_dec_float_type min_pos_02(str_min_pos_02);
      const local_cpp_dec_float_type min_pos_03(str_min_pos_03);

      const local_cpp_dec_float_type min_neg_01(str_min_neg_01);
      const local_cpp_dec_float_type min_neg_02(str_min_neg_02);
      const local_cpp_dec_float_type min_neg_03(str_min_neg_03);

      BOOST_TEST(min_pos_01 == (std::numeric_limits<local_cpp_dec_float_type>::min)());
      BOOST_TEST(((boost::multiprecision::fpclassify)(min_pos_02) == FP_NORMAL) && (!signbit(min_pos_02)));
      BOOST_TEST(((boost::multiprecision::fpclassify)(min_pos_03) == FP_ZERO) && (!signbit(min_pos_03)));

      BOOST_TEST(min_neg_01 == -(std::numeric_limits<local_cpp_dec_float_type>::min)());
      BOOST_TEST(((boost::multiprecision::fpclassify)(min_neg_02) == FP_NORMAL) && signbit(min_neg_02));
      BOOST_TEST(((boost::multiprecision::fpclassify)(min_neg_03) == FP_ZERO) && (!signbit(min_neg_03)));
    }
  }
} // namespace local

auto main() -> int
{
  {
    using float_backend_type = boost::multiprecision::cpp_bin_float<50>;

    using float_type = boost::multiprecision::number<float_backend_type, boost::multiprecision::et_off>;

    std::cout << "Testing type: " << typeid(float_type).name() << std::endl;

    static_cast<void>(local::test_edges<float_type>());
  }

  {
    using float_backend_type = boost::multiprecision::cpp_dec_float<50>;

    using float_type = boost::multiprecision::number<float_backend_type, boost::multiprecision::et_off>;

    std::cout << "Testing type: " << typeid(float_type).name() << std::endl;

    static_cast<void>(local::test_edges<float_type>());
    local::test_cpp_dec_float_rd_ovf_unf<float_type>();
  }

  {
    using big_bin_float_backend_type = boost::multiprecision::cpp_bin_float<512>;
    using big_dec_float_backend_type = boost::multiprecision::cpp_dec_float<512>;

    using big_bin_float_type = boost::multiprecision::number<big_bin_float_backend_type, boost::multiprecision::et_off>;
    using big_dec_float_type = boost::multiprecision::number<big_dec_float_backend_type, boost::multiprecision::et_off>;

    static_cast<void>(local::test_convert_and_back_caller<big_bin_float_type>(0.25F));
    static_cast<void>(local::test_convert_and_back_caller<big_dec_float_type>(0.50F));
  }

  return boost::report_errors();
}

template<typename FloatType> auto my_zero() noexcept -> FloatType& { using float_type = FloatType; static float_type val_zero { 0 }; return val_zero; }
template<typename FloatType> auto my_one () noexcept -> FloatType& { using float_type = FloatType; static float_type val_one  { 1 }; return val_one; }
template<typename FloatType> auto my_inf () noexcept -> FloatType& { using float_type = FloatType; static float_type val_inf  { std::numeric_limits<float_type>::infinity() }; return val_inf; }
template<typename FloatType> auto my_nan () noexcept -> FloatType& { using float_type = FloatType; static float_type val_nan  { std::numeric_limits<float_type>::quiet_NaN() }; return val_nan; }

#if (defined(__clang__) || defined(__GNUC__))
#  pragma GCC diagnostic pop
#endif
