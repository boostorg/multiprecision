///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Ttimed multiplication cpp_quad_float<double> versuc cpp_bin_float

#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <boost/config.hpp>
#include <boost/multiprecision/number.hpp>
#ifdef BOOST_MATH_USE_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/cpp_quad_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/multiprecision/traits/max_digits10.hpp>
#include <boost/core/demangle.hpp>

#if defined(__clang__)
  #if defined __has_feature && (__has_feature(thread_sanitizer) || __has_feature(address_sanitizer))
  #define CPP_DOUBLE_FLOAT_REDUCE_TEST_DEPTH
  #endif
#elif defined(__GNUC__)
  #if defined(__SANITIZE_THREAD__) || defined(__SANITIZE_ADDRESS__)
  #define CPP_DOUBLE_FLOAT_REDUCE_TEST_DEPTH
  #endif
#endif

namespace local
{
  std::mt19937       engine_man;
  std::ranlux24_base engine_sgn;

  template<typename FloatingPointConstituentType>
  struct control
  {
    using float_type = FloatingPointConstituentType;

    static constexpr int digits       = 4 * std::numeric_limits<float_type>::digits;
    static constexpr int digits10     = boost::multiprecision::detail::calc_digits10<digits>::value;
    static constexpr int max_digits10 = boost::multiprecision::detail::calc_max_digits10<digits>::value;

    static unsigned seed_prescaler;

    using quad_float_type    = boost::multiprecision::number<boost::multiprecision::backends::cpp_quad_float<float_type>, boost::multiprecision::et_off>;
    using control_float_type = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<(2 * std::numeric_limits<quad_float_type>::digits10) + 1>, boost::multiprecision::et_off>;

    static_assert( digits       == std::numeric_limits<quad_float_type>::digits       , "Discrepancy in limts." );
    static_assert( digits10     == std::numeric_limits<quad_float_type>::digits10     , "Discrepancy in limts." );
    static_assert( max_digits10 == std::numeric_limits<quad_float_type>::max_digits10 , "Discrepancy in limts." );

    template<const std::size_t DigitsToGet = digits10>
    static void get_random_fixed_string(std::string& str, const bool is_unsigned = false)
    {
      // This string generator creates strings of the form
      // 0.458279387.... E+5
      // -0.7182937534953.... E-126
      // etc., where the string can be either positive only
      // (positive only via setting is_unsigned to true)
      // or mixed positive/negative.

      // Re-seed the random engine each approx. 65k calls
      // of this string generator.

      if((seed_prescaler % 0x10000U) == 0U)
      {
        const std::clock_t seed_time_stamp = std::clock();

        engine_man.seed(static_cast<typename std::mt19937::result_type>      (seed_time_stamp));
        engine_sgn.seed(static_cast<typename std::ranlux24_base::result_type>(seed_time_stamp));
      }

      ++seed_prescaler;

      static std::uniform_int_distribution<unsigned>
      dist_sgn
      (
        0,
        1
      );

      static std::uniform_int_distribution<unsigned>
      dist_first
      (
        1,
        9
      );

      static std::uniform_int_distribution<unsigned>
      dist_following
      (
        0,
        9
      );

      const bool is_neg = ((is_unsigned == false) && (dist_sgn(engine_sgn) != 0));

      // Use DigitsToGet + 2, where +2 represents the lenth of "0.".
      std::string::size_type len = static_cast<std::string::size_type>(DigitsToGet + 2);

      std::string::size_type pos = 0U;

      if(is_neg)
      {
        ++len;

        str.resize(len);

        str.at(pos) = char('-');

        ++pos;
      }
      else
      {
        str.resize(len);
      }

      str.at(pos) = static_cast<char>('0');
      ++pos;

      str.at(pos) = static_cast<char>('.');
      ++pos;

      str.at(pos) = static_cast<char>(dist_first(engine_man) + 0x30U);
      ++pos;

      while(pos < str.length())
      {
          str.at(pos) = static_cast<char>(dist_following(engine_man) + 0x30U);
          ++pos;
      }

      const bool exp_is_neg = (dist_sgn(engine_sgn) != 0);

      // TBD: Use even more extreme base-10 exponents if desired/possible
      // and base these on the actual range of the exponent10 member of limits.
      // The use of the digits member here is a strange workaround that
      // still needs to be investigated on GCC's 10-bit x86 long double.
      using local_exp10_float_type =
         typename std::conditional<(std::is_same<float_type, long double>::value == true), double, float_type>::type;

      static std::uniform_int_distribution<unsigned>
      dist_exp
      (
        0,
          ((std::numeric_limits<local_exp10_float_type>::max_exponent10 > 1000) ? 1183
        : ((std::numeric_limits<local_exp10_float_type>::max_exponent10 >  200) ?   83
        : ((std::numeric_limits<local_exp10_float_type>::max_exponent10 >   20) ?   13 : 1)))
      );

      std::string str_exp = ((exp_is_neg == false) ? "E+" :  "E-");

      {
        std::stringstream strm;

        strm << dist_exp(engine_man);

        str_exp += strm.str();
      }

      str += str_exp;
    }

    template<typename ConstructionType>
    static ConstructionType construct_from(const quad_float_type& f)
    {
      return   ConstructionType(std::get<0>(quad_float_type::canonical_value(f).crep()))
             + ConstructionType(std::get<1>(quad_float_type::canonical_value(f).crep()))
             + ConstructionType(std::get<2>(quad_float_type::canonical_value(f).crep()))
             + ConstructionType(std::get<3>(quad_float_type::canonical_value(f).crep()))
             ;
    }
  };

  template<typename FloatingPointConstituentType> unsigned control<FloatingPointConstituentType>::seed_prescaler;

  template<typename UnknownFloatType>
  void naked_multiply(const std::vector<UnknownFloatType>& va,
                      const std::vector<UnknownFloatType>& vb,
                            std::vector<UnknownFloatType>& vc)
  {
    for(std::size_t i = 0U; i < va.size(); ++i)
    {
      vc[i] = va[i] * vb[i];
    }
  }
}

int main()
{
  using float_type = double;

  using quad_type = local::control<float_type>::quad_float_type;
  using ctrl_type = local::control<float_type>::control_float_type;

  std::vector<quad_type> quad_a(8192U);
  std::vector<quad_type> quad_b(8192U);
  std::vector<quad_type> quad_c(8192U);
  std::vector<ctrl_type> ctrl_a(8192U);
  std::vector<ctrl_type> ctrl_b(8192U);
  std::vector<ctrl_type> ctrl_c(8192U);

  for(std::size_t i = 0U; i < quad_a.size(); ++i)
  {
    std::string str_a;
    std::string str_b;

    local::control<float_type>::get_random_fixed_string<local::control<float_type>::digits10 + 4>(str_a);
    local::control<float_type>::get_random_fixed_string<local::control<float_type>::digits10 + 4>(str_b);

    quad_a[i] = quad_type(str_a);
    quad_b[i] = quad_type(str_b);

    ctrl_a[i] = local::control<float_type>::construct_from<ctrl_type>(quad_a[i]);
    ctrl_b[i] = local::control<float_type>::construct_from<ctrl_type>(quad_b[i]);
  }

  constexpr unsigned loops = 1000U;

  const std::clock_t quad_start = std::clock();
  for(unsigned i = 0U; i < loops; ++i)
  {
    local::naked_multiply(quad_a, quad_b, quad_c);
  }
  const std::clock_t quad_stop = std::clock();

  const std::clock_t ctrl_start = std::clock();
  for(unsigned i = 0U; i < loops; ++i)
  {
    local::naked_multiply(ctrl_a, ctrl_b, ctrl_c);
  }
  const std::clock_t ctrl_stop = std::clock();

  const float quad_time = (float) (quad_stop - quad_start) / (float) CLOCKS_PER_SEC;
  const float ctrl_time = (float) (ctrl_stop - ctrl_start) / (float) CLOCKS_PER_SEC;

  std::cout << "quad_time: " << quad_time << std::endl;
  std::cout << "ctrl_time: " << ctrl_time << std::endl;

  bool result_is_ok = true;

  const ctrl_type MaxError = ldexp(ctrl_type(1), 6 - std::numeric_limits<quad_type>::digits);

  for(std::size_t i = 0U; i < quad_a.size(); ++i)
  {
    const ctrl_type delta = fabs(1 - fabs(ctrl_c[i] / local::control<float_type>::construct_from<ctrl_type>(quad_c[i])));

    const bool b_ok = (delta < MaxError);

    result_is_ok &= b_ok;
  }

  std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;

  return (result_is_ok ? 0 : -1);
}
