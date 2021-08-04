#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include "q_float/q_float_qf.h"

// cd /mnt/c/Users/User/Documents/Ks/PC_Software/NumericalPrograms/ExtendedNumberTypes/q_float
// g++-10 -finline-functions -finline-limit=32 -march=native -mtune=native -O3 -Wall -Wextra -std=c++11 -I. -I/mnt/c/boost/boost_1_76_0 q_float/q_float.cpp q_float/q_float_gamma.cpp q_float/q_float_math.cpp q_float/q_float_z_math.cpp test.cpp test_spot.cpp -o q_float.exe

extern void test_spot();

namespace local
{
  std::mt19937       engine_man;
  std::ranlux24_base engine_sgn;

  constexpr int digits     = (2 * std::numeric_limits<double>::digits) - 2;
  constexpr int digits10   = int(float(digits - 1) * 0.301F);

  unsigned seed_prescaler;

  using double_float_type  = q_float;
  using control_float_type = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<(2 * std::numeric_limits<double_float_type>::digits10) + 1>, boost::multiprecision::et_off>;

  template<const std::size_t DigitsToGet = digits10>
  static void get_random_fixed_string(std::string& str, const bool is_unsigned = false)
  {
    if((seed_prescaler % 0x8000U) == 0U)
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

    static std::uniform_int_distribution<unsigned>
    dist_exp
    (
      0,
      105
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
  ConstructionType construct_from(const double_float_type& f)
  {
    return ConstructionType(f.rep_hi()) + ConstructionType(f.rep_lo());
  }

  bool test_add__(const std::uint32_t count)
  {
    bool result_is_ok = true;

    const control_float_type MaxError = ldexp(control_float_type(1), -std::numeric_limits<double_float_type>::digits + 0);

    for(std::uint32_t i = 0U; ((i < count) && result_is_ok); ++i)
    {
      std::string str_a;
      std::string str_b;

      local::get_random_fixed_string<35U>(str_a);
      local::get_random_fixed_string<35U>(str_b);

      const double_float_type  df_a(str_a);
      const double_float_type  df_b(str_b);

      const control_float_type ctrl_a = construct_from<control_float_type>(df_a);
      const control_float_type ctrl_b = construct_from<control_float_type>(df_b);

      double_float_type  df_c    = df_a   + df_b;
      control_float_type ctrl_c  = ctrl_a + ctrl_b;

      const control_float_type delta = fabs(1 - fabs(ctrl_c / construct_from<control_float_type>(df_c)));

      const bool b_ok = (delta < MaxError);

      result_is_ok &= b_ok;
    }

    return result_is_ok;
  }

  bool test_sub__(const std::uint32_t count)
  {
    bool result_is_ok = true;

    const control_float_type MaxError = ldexp(control_float_type(1), -std::numeric_limits<double_float_type>::digits + 0);

    for(std::uint32_t i = 0U; ((i < count) && result_is_ok); ++i)
    {
      std::string str_a;
      std::string str_b;

      local::get_random_fixed_string<35U>(str_a);
      local::get_random_fixed_string<35U>(str_b);

      const double_float_type  df_a(str_a);
      const double_float_type  df_b(str_b);

      const control_float_type ctrl_a = construct_from<control_float_type>(df_a);
      const control_float_type ctrl_b = construct_from<control_float_type>(df_b);

      double_float_type  df_c   = df_a   - df_b;
      control_float_type ctrl_c = ctrl_a - ctrl_b;

      const control_float_type delta = fabs(1 - fabs(ctrl_c / construct_from<control_float_type>(df_c)));

      const bool b_ok = (delta < MaxError);

      result_is_ok &= b_ok;
    }

    return result_is_ok;
  }

  bool test_mul__(const std::uint32_t count)
  {
    bool result_is_ok = true;

    const control_float_type MaxError = ldexp(control_float_type(1), -std::numeric_limits<double_float_type>::digits + 1);

    for(std::uint32_t i = 0U; ((i < count) && result_is_ok); ++i)
    {
      std::string str_a;
      std::string str_b;

      local::get_random_fixed_string<35U>(str_a);
      local::get_random_fixed_string<35U>(str_b);

      const double_float_type df_a(str_a);
      const double_float_type df_b(str_b);

      const control_float_type ctrl_a = construct_from<control_float_type>(df_a);
      const control_float_type ctrl_b = construct_from<control_float_type>(df_b);

      double_float_type  df_c   = df_a   * df_b;
      control_float_type ctrl_c = ctrl_a * ctrl_b;

      const control_float_type delta = fabs(1 - fabs(ctrl_c / construct_from<control_float_type>(df_c)));

      const bool b_ok = (delta < MaxError);

      result_is_ok &= b_ok;
    }

    return result_is_ok;
  }

  bool test_div__(const std::uint32_t count)
  {
    bool result_is_ok = true;

    const control_float_type MaxError = ldexp(control_float_type(1), -std::numeric_limits<double_float_type>::digits + 1);

    for(std::uint32_t i = 0U;((i < count) && result_is_ok); ++i)
    {
      std::string str_a;
      std::string str_b;

      local::get_random_fixed_string<35U>(str_a);
      local::get_random_fixed_string<35U>(str_b);

      const double_float_type  df_a  (str_a);
      const double_float_type  df_b  (str_b);

      const control_float_type ctrl_a = construct_from<control_float_type>(df_a);
      const control_float_type ctrl_b = construct_from<control_float_type>(df_b);

      const double_float_type  df_c   = df_a   / df_b;
      const control_float_type ctrl_c = ctrl_a / ctrl_b;

      const control_float_type delta = fabs(1 - fabs(ctrl_c / construct_from<control_float_type>(df_c)));

      const bool b_ok = (delta < MaxError);

      result_is_ok &= b_ok;
    }

    return result_is_ok;
  }

  bool test_sqrt_(const std::uint32_t count)
  {
    bool result_is_ok = true;

    const control_float_type MaxError = ldexp(control_float_type(1), -std::numeric_limits<double_float_type>::digits + 0);

    for(std::uint32_t i = 0U; ((i < count) && result_is_ok); ++i)
    {
      std::string str_a;
      std::string str_b;

      local::get_random_fixed_string<35U>(str_a, true);

      const double_float_type  df_a(str_a);

      const control_float_type ctrl_a = construct_from<control_float_type>(df_a);

      double_float_type  df_c   = qf::sqrt(df_a);
      control_float_type ctrl_c = sqrt(ctrl_a);

      const control_float_type delta = fabs(1 - fabs(ctrl_c / construct_from<control_float_type>(df_c)));

      const bool b_ok = (delta < (MaxError * 1UL));

      result_is_ok &= b_ok;
    }

    return result_is_ok;
  }
}

int main()
{
  ::test_spot();

  constexpr std::uint32_t count = (std::uint32_t) (0x10000ULL << 3U);

  std::cout << "Testing " << count << " arithmetic cases." << std::endl;

  const bool result_add___is_ok = local::test_add__(count); std::cout << "result_add___is_ok: " << std::boolalpha << result_add___is_ok << std::endl;
  const bool result_sub___is_ok = local::test_sub__(count); std::cout << "result_sub___is_ok: " << std::boolalpha << result_sub___is_ok << std::endl;
  const bool result_mul___is_ok = local::test_mul__(count); std::cout << "result_mul___is_ok: " << std::boolalpha << result_mul___is_ok << std::endl;
  const bool result_div___is_ok = local::test_div__(count); std::cout << "result_div___is_ok: " << std::boolalpha << result_div___is_ok << std::endl;
  const bool result_sqrt__is_ok = local::test_sqrt_(count); std::cout << "result_sqrt__is_ok: " << std::boolalpha << result_sqrt__is_ok << std::endl;

  const bool result_all_is_ok = (   result_add___is_ok
                                 && result_sub___is_ok
                                 && result_mul___is_ok
                                 && result_div___is_ok
                                 && result_sqrt__is_ok);

  return (result_all_is_ok ? 0 : -1);
}
