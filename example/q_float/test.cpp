#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include "q_float/q_float_qf.h"

// cd /mnt/c/Users/User/Documents/Ks/PC_Software/NumericalPrograms/ExtendedNumberTypes/q_float
// g++-10 -finline-functions -finline-limit=32 -march=native -mtune=native -O3 -Wall -Wextra -std=c++11 -I. -I/mnt/c/boost/boost_1_76_0 q_float/q_float.cpp q_float/q_float_gamma.cpp q_float/q_float_math.cpp q_float/q_float_z_math.cpp test.cpp -o q_float.exe

namespace local
{
  void test_spot()
  {
    // Set output precision and flags.
    std::cout.precision(std::numeric_limits<q_float>::digits10);
    std::cout.setf(std::ios::showpos | std::ios::uppercase);

    // Test some simple operations on real-valued q_float.
    std::cout << std::endl << "Test some simple operations on real-valued q_float:" << std::endl << std::endl;

    std::cout << q_float(0.0) << std::endl;

    std::cout << qf::pi() << std::endl;

    q_float u_start(123.456);

    q_float u(123.456);

    std::cout << u << std::endl;

    u *= u_start;
    u *= u_start;
    u *= u_start;
    u *= u_start;

    std::cout << u << std::endl;

    u /= u_start;
    u /= u_start;
    u /= u_start;
    u /= u_start;

    std::cout << u << std::endl;

    q_float v(2);

    v.sqrt();

    q_float s(q_float::sqrt2());

    std::cout << s << std::endl;

    const q_float Q(12.345678);

    const q_float E = qf::exp(Q);
    const q_float L = qf::log(E);

    std::cout << E << std::endl;
    std::cout << L << std::endl;

    const q_float S = qf::sin(qf::one());
    const q_float C = qf::cos(qf::one());

    std::cout << S << std::endl;
    std::cout << C << std::endl;
    std::cout << qf::asin(qf::half())      << std::endl;
    std::cout << qf::acos(qf::half())      << std::endl;
    std::cout << qf::atan(qf::half())      << std::endl;
    std::cout << qf::cbrt(qf::half())      << std::endl;
    std::cout << qf::rootn(q_float(16), 4) << std::endl;

    // Test some numeric limits of q_float.
    std::cout << std::endl << "Test some numeric limits of q_float:" << std::endl << std::endl;

    std::cout << qf::zero()                                                        << std::endl;
    std::cout << qf::one()                                                         << std::endl;
    std::cout << std::numeric_limits<q_float>::epsilon()                           << std::endl;
    std::cout << qf::one() + std::numeric_limits<q_float>::epsilon()               << std::endl;
    std::cout << qf::one() - std::numeric_limits<q_float>::epsilon()               << std::endl;
    std::cout <<  (std::numeric_limits<q_float>::max)()                            << std::endl;
    std::cout <<  (std::numeric_limits<q_float>::min)()                            << std::endl;
    std::cout <<  (std::numeric_limits<q_float>::min)() * q_float(1E17)            << std::endl;
    std::cout <<  (std::numeric_limits<q_float>::max)() / q_float(1E17)            << std::endl;
    std::cout << ((std::numeric_limits<q_float>::max)() / q_float(3)) * q_float(2) << std::endl;

    // Test some simple operations on complex q_float.
    std::cout << std::endl << "Test some simple operations on complex q_float:" << std::endl << std::endl;

    std::complex<q_float> z(" ( 1.23456 , 7.77888999 )");

    std::cout << z               << std::endl;
    std::cout << qfz::log(z)     << std::endl;
    std::cout << qfz::exp(z)     << std::endl;
    std::cout << qfz::sin(z)     << std::endl;
    std::cout << qfz::cos(z)     << std::endl;
    std::cout << qfz::tan(z)     << std::endl;
    std::cout << z / qfz::exp(z) << std::endl;

    // Test q_float dump operations.
    std::cout << std::endl << "Test q_float dump operations:" << std::endl << std::endl;

    std::string str;

    if(q_float::dump_digits(qfz::tan(z).real(), str))
    {
      std::cout << str << std::endl;
    }

    qfz::tan(z).real().dump();

    std::cout.unsetf(std::ios::dec);
    std::cout.setf(std::ios::hex);

    qfz::tan(z).real().dump();

    std::cout << std::endl << "Test q_float exceptions:" << std::endl << std::endl;

    try
    {
      q_float n(12345678);
      q_float bad = n / qf::zero();
      (void) bad;
    }
    catch(const qf::exception& ex)
    {
      std::cout << ex.what() << std::endl;
    }

    try
    {
      q_float n(1E200);
      q_float bad = n * n;
      (void) bad;
    }
    catch(const qf::exception& ex)
    {
      std::cout << ex.what() << std::endl;
    }
  
    std::cout.unsetf(std::ios::hex);
    std::cout.setf(std::ios::dec);

    // Test the gamma function which uses q_float.
    std::cout << std::endl << "Test the gamma function:" << std::endl << std::endl;

    std::cout.precision(std::numeric_limits<q_float>::digits10);
    std::cout.setf(std::ios::scientific);

    std::cout << qf::gamma(q_float(10.3)) << std::endl;
    std::cout << qf::gamma(q_float(101))  << std::endl;
    std::cout << qf::gamma(qf::one() / 2) << std::endl;
    std::cout << qf::gamma(qf::one() / 3) << std::endl;
    std::cout << qf::gamma(qf::one() / 4) << std::endl;
    std::cout << qf::gamma(qf::one() / 5) << std::endl;
    std::cout << qf::gamma(qf::one() / 6) << std::endl;
    std::cout << qf::gamma(-qf::half())   << std::endl;

    for(UINT32 i = 0; i < 10; i++)
    {
      std::cout << qfz::gamma(std::complex<q_float>(qf::pi() + i, qf::e() + i)) << std::endl;
    }
  }

  using naked_double_float_type = q_float;
  using control_float_type      = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<32U>, boost::multiprecision::et_off>;

  std::mt19937                                                         engine_man;
  std::ranlux24_base                                                   engine_sgn;
  std::linear_congruential_engine<std::uint32_t, 48271, 0, 2147483647> engine_dec_pt;

  template<const std::size_t DigitsToGet>
  void get_random_fixed_string(std::string& str, const bool is_unsigned = false)
  {
    static std::uniform_int_distribution<unsigned>
    dist_sgn
    (
      0,
      1
    );

    static std::uniform_int_distribution<unsigned>
    dist_dec_pt
    (
      1,
      (int) (std::max)(std::ptrdiff_t(2) , std::ptrdiff_t(std::ptrdiff_t(DigitsToGet) - 4))
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

    std::string::size_type len = static_cast<std::string::size_type>(DigitsToGet);

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

    str.at(pos) = static_cast<char>(dist_first(engine_man) + 0x30U);

    ++pos;

    const std::string::size_type pos_dec_pt = pos + std::string::size_type(dist_dec_pt(engine_dec_pt));

    while(pos < str.length())
    {
      if(pos == pos_dec_pt)
      {
        ++len;

        str.resize(len);

        str.at(pos) = char('.');

        ++pos;
      }

      str.at(pos) = static_cast<char>(dist_following(engine_man) + 0x30U);

      ++pos;
    }
  }

  bool test_add__(const unsigned count)
  {
    bool result_is_ok = true;

    for(unsigned i = 0U; i < count; ++i)
    {
      std::string str_a;
      std::string str_b;

      local::get_random_fixed_string<33U>(str_a);
      local::get_random_fixed_string<33U>(str_b);

      const naked_double_float_type df_a  (str_a);
      const naked_double_float_type df_b  (str_b);

      const control_float_type      ctrl_a(str_a);
      const control_float_type      ctrl_b(str_b);

      naked_double_float_type df_c    = df_a   + df_b;
      control_float_type      ctrl_c  = ctrl_a + ctrl_b;

      std::stringstream strm;

      strm << std::setprecision(33) << df_c;

      const std::string str_df_c = strm.str();

      const bool b_ok =
        (fabs(1 - fabs(ctrl_c / control_float_type(str_df_c))) < std::numeric_limits<control_float_type>::epsilon() * 10000UL);

      result_is_ok &= b_ok;
    }

    return result_is_ok;
  }

  bool test_sub__(const unsigned count)
  {
    bool result_is_ok = true;

    for(unsigned i = 0U; i < count; ++i)
    {
      std::string str_a;
      std::string str_b;

      local::get_random_fixed_string<33U>(str_a);
      local::get_random_fixed_string<33U>(str_b);

      const naked_double_float_type df_a  (str_a);
      const naked_double_float_type df_b  (str_b);

      const control_float_type      ctrl_a(str_a);
      const control_float_type      ctrl_b(str_b);

      naked_double_float_type df_c    = df_a   - df_b;
      control_float_type      ctrl_c  = ctrl_a - ctrl_b;

      std::stringstream strm;

      strm << std::setprecision(33) << df_c;

      const std::string str_df_c = strm.str();

      const bool b_ok =
        (fabs(1 - fabs(ctrl_c / control_float_type(str_df_c))) < std::numeric_limits<control_float_type>::epsilon() * 10000UL);

      result_is_ok &= b_ok;
    }

    return result_is_ok;
  }

  bool test_mul__(const unsigned count)
  {
    bool result_is_ok = true;

    for(unsigned i = 0U; i < count; ++i)
    {
      std::string str_a;
      std::string str_b;

      local::get_random_fixed_string<33U>(str_a);
      local::get_random_fixed_string<33U>(str_b);

      const naked_double_float_type df_a  (str_a);
      const naked_double_float_type df_b  (str_b);

      const control_float_type      ctrl_a(str_a);
      const control_float_type      ctrl_b(str_b);

      naked_double_float_type df_c    = df_a   * df_b;
      control_float_type      ctrl_c  = ctrl_a * ctrl_b;

      std::stringstream strm;

      strm << std::setprecision(33) << df_c;

      const std::string str_df_c = strm.str();

      const bool b_ok =
        (fabs(1 - fabs(ctrl_c / control_float_type(str_df_c))) < std::numeric_limits<control_float_type>::epsilon() * 10000UL);

      result_is_ok &= b_ok;
    }

    return result_is_ok;
  }

  bool test_div__(const unsigned count)
  {
    bool result_is_ok = true;

    for(unsigned i = 0U; i < count; ++i)
    {
      std::string str_a;
      std::string str_b;

      local::get_random_fixed_string<33U>(str_a);
      local::get_random_fixed_string<33U>(str_b);

      const naked_double_float_type df_a  (str_a);
      const naked_double_float_type df_b  (str_b);

      const control_float_type      ctrl_a(str_a);
      const control_float_type      ctrl_b(str_b);

      naked_double_float_type df_c    = df_a   / df_b;
      control_float_type      ctrl_c  = ctrl_a / ctrl_b;

      std::stringstream strm;

      strm << std::setprecision(33) << df_c;

      const std::string str_df_c = strm.str();

      const bool b_ok =
        (fabs(1 - fabs(ctrl_c / control_float_type(str_df_c))) < std::numeric_limits<control_float_type>::epsilon() * 10000UL);

      result_is_ok &= b_ok;
    }

    return result_is_ok;
  }

  bool test_sqrt_(const unsigned count)
  {
    bool result_is_ok = true;

    for(unsigned i = 0U; i < count; ++i)
    {
      std::string str_a;
      std::string str_b;

      local::get_random_fixed_string<33U>(str_a, true);

      const naked_double_float_type df_a  (str_a);
      const naked_double_float_type df_b  (str_b);

      const control_float_type      ctrl_a(str_a);

      naked_double_float_type df_c    = qf::sqrt(df_a);
      control_float_type      ctrl_c  = sqrt(ctrl_a);

      std::stringstream strm;

      strm << std::setprecision(33) << df_c;

      const std::string str_df_c = strm.str();

      const bool b_ok =
        (fabs(1 - fabs(ctrl_c / control_float_type(str_df_c))) < std::numeric_limits<control_float_type>::epsilon() * 10000UL);

      result_is_ok &= b_ok;
    }

    return result_is_ok;
  }
}

int main()
{
  local::test_spot();

  const bool result_add___is_ok = local::test_add__(1024U); std::cout << "result_add___is_ok: " << std::boolalpha << result_add___is_ok << std::endl;
  const bool result_sub___is_ok = local::test_sub__(1024U); std::cout << "result_sub___is_ok: " << std::boolalpha << result_sub___is_ok << std::endl;
  const bool result_mul___is_ok = local::test_mul__(1024U); std::cout << "result_mul___is_ok: " << std::boolalpha << result_mul___is_ok << std::endl;
  const bool result_div___is_ok = local::test_div__(1024U); std::cout << "result_div___is_ok: " << std::boolalpha << result_div___is_ok << std::endl;
  const bool result_sqrt__is_ok = local::test_sqrt_(1024U); std::cout << "result_sqrt__is_ok: " << std::boolalpha << result_sqrt__is_ok << std::endl;

  const bool result_all_is_ok = (   result_add___is_ok
                                 && result_sub___is_ok
                                 && result_mul___is_ok
                                 && result_div___is_ok
                                 && result_sqrt__is_ok);

  return (result_all_is_ok ? 0 : -1);
}
