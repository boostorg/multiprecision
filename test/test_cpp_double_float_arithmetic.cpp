#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <boost/multiprecision/cpp_double_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#if 0
// TBD: Quadmath support when available
#include <boost/multiprecision/float128.hpp>
#endif

// cd /mnt/c/Users/User/Documents/Ks/PC_Software/Test
// g++ -Wall -Wextra -O3 -std=gnu++11 -I/mnt/c/MyGitRepos/BoostGSoC21_multiprecision/include -I/mnt/c/boost/boost_1_76_0 test.cpp -o test.exe

namespace test_arithmetic_cpp_double_float {

std::mt19937                                                         engine_man;
std::ranlux24_base                                                   engine_sgn;
std::linear_congruential_engine<std::uint32_t, 48271, 0, 2147483647> engine_dec_pt;

template <const std::size_t DigitsToGet>
void get_random_fixed_string(std::string& str)
{
   static std::uniform_int_distribution<unsigned>
       dist_sgn(
           0,
           1);

   static std::uniform_int_distribution<unsigned>
       dist_dec_pt(
           1,
           (int)(std::max)(std::ptrdiff_t(2), std::ptrdiff_t(std::ptrdiff_t(DigitsToGet) - 4)));

   static std::uniform_int_distribution<unsigned>
       dist_first(
           1,
           9);

   static std::uniform_int_distribution<unsigned>
       dist_following(
           0,
           9);

   const bool is_neg = (dist_sgn(engine_sgn) != 0);

   std::string::size_type len = static_cast<std::string::size_type>(DigitsToGet);

   std::string::size_type pos = 0U;

   if (is_neg)
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

   while (pos < str.length())
   {
      if (pos == pos_dec_pt)
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

const unsigned int ErrorThreshold = 1000U;
template <typename FloatingPointType>
int test_add(const unsigned count = 10000U)
{
   using naked_double_float_type = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;
   using control_float_type      = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<naked_double_float_type>::digits10 + 2>, boost::multiprecision::et_off>;

   std::cout << "testing operator+...";
   bool result_is_ok = true;

  for(unsigned i = 0U; i < count; ++i)
  {
    std::string str_a;
    std::string str_b;

    test_arithmetic_cpp_double_float::get_random_fixed_string<std::numeric_limits<naked_double_float_type>::digits10>(str_a);
    test_arithmetic_cpp_double_float::get_random_fixed_string<std::numeric_limits<naked_double_float_type>::digits10>(str_b);

    const naked_double_float_type df_a  (str_a);
    const naked_double_float_type df_b  (str_b);

    const control_float_type      ctrl_a(str_a);
    const control_float_type      ctrl_b(str_b);

    naked_double_float_type df_c    = df_a   + df_b;
    control_float_type      ctrl_c  = ctrl_a + ctrl_b;

    std::stringstream strm;

    strm << std::setprecision(std::numeric_limits<naked_double_float_type>::digits10) << df_c;

    const std::string str_df_c = strm.str();

    const bool b_ok =
      (fabs(1 - fabs(ctrl_c / control_float_type(str_df_c))) < std::numeric_limits<control_float_type>::epsilon() * ErrorThreshold);

    if (!b_ok)
    {
       std::cerr << " [FAILED] while performing '" << str_a << "' + '" << str_b << "'" << std::endl;
       return -1;
    }
  }

  std::cout << " ok" << std::endl;
  return 0;
}

template <typename FloatingPointType>
int test_sub(const unsigned count = 10000U)
{
   using naked_double_float_type = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;
   using control_float_type      = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<naked_double_float_type>::digits10 + 2>, boost::multiprecision::et_off>;

   std::cout << "testing operator-...";
   bool result_is_ok = true;

   for (unsigned i = 0U; i < count; ++i)
   {
      std::string str_a;
      std::string str_b;

      test_arithmetic_cpp_double_float::get_random_fixed_string<std::numeric_limits<naked_double_float_type>::digits10>(str_a);
      test_arithmetic_cpp_double_float::get_random_fixed_string<std::numeric_limits<naked_double_float_type>::digits10>(str_b);

      const naked_double_float_type df_a(str_a);
      const naked_double_float_type df_b(str_b);

      const control_float_type ctrl_a(str_a);
      const control_float_type ctrl_b(str_b);

      naked_double_float_type df_c   = df_a - df_b;
      control_float_type      ctrl_c = ctrl_a - ctrl_b;

      std::stringstream strm;

      strm << std::setprecision(std::numeric_limits<naked_double_float_type>::digits10) << df_c;

      const std::string str_df_c = strm.str();

      const bool b_ok =
          (fabs(1 - fabs(ctrl_c / control_float_type(str_df_c))) < std::numeric_limits<control_float_type>::epsilon() * ErrorThreshold);

      if (!b_ok)
      {
         std::cerr << " [FAILED] while performing '" << str_a << "' - '" << str_b << "'" << std::endl;
         return -1;
      }
   }

   std::cout << " ok" << std::endl;
   return 0;
}


template <typename FloatingPointType>
int test_mul(const unsigned count = 10000U)
{
   using naked_double_float_type = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;
   using control_float_type      = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<naked_double_float_type>::digits10 + 2>, boost::multiprecision::et_off>;

   std::cout << "testing operator*...";
   bool result_is_ok = true;

   for (unsigned i = 0U; i < count; ++i)
   {
      std::string str_a;
      std::string str_b;

      test_arithmetic_cpp_double_float::get_random_fixed_string<std::numeric_limits<naked_double_float_type>::digits10>(str_a);
      test_arithmetic_cpp_double_float::get_random_fixed_string<std::numeric_limits<naked_double_float_type>::digits10>(str_b);

      const naked_double_float_type df_a(str_a);
      const naked_double_float_type df_b(str_b);

      const control_float_type ctrl_a(str_a);
      const control_float_type ctrl_b(str_b);

      naked_double_float_type df_c   = df_a * df_b;
      control_float_type      ctrl_c = ctrl_a * ctrl_b;

      std::stringstream strm;

      strm << std::setprecision(std::numeric_limits<naked_double_float_type>::digits10) << df_c;

      const std::string str_df_c = strm.str();

      const bool b_ok =
          (fabs(1 - fabs(ctrl_c / control_float_type(str_df_c))) < std::numeric_limits<control_float_type>::epsilon() * ErrorThreshold);

      if (!b_ok)
      {
         std::cerr << " [FAILED] while performing '" << str_a << "' * '" << str_b << "'" << std::endl;
         return -1;
      }
   }

   std::cout << " ok" << std::endl;
   return 0;
}


template <typename FloatingPointType>
int test_div(const unsigned count = 10000U)
{
   using naked_double_float_type = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;
   using control_float_type      = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<std::numeric_limits<naked_double_float_type>::digits10 + 2>, boost::multiprecision::et_off>;

   std::cout << "testing operator/...";
   bool result_is_ok = true;

   for (unsigned i = 0U; i < count; ++i)
   {
      std::string str_a;
      std::string str_b;

      test_arithmetic_cpp_double_float::get_random_fixed_string<std::numeric_limits<naked_double_float_type>::digits10>(str_a);
      test_arithmetic_cpp_double_float::get_random_fixed_string<std::numeric_limits<naked_double_float_type>::digits10>(str_b);

      const naked_double_float_type df_a(str_a);
      const naked_double_float_type df_b(str_b);

      const control_float_type ctrl_a(str_a);
      const control_float_type ctrl_b(str_b);

      naked_double_float_type df_c   = df_a / df_b;
      control_float_type      ctrl_c = ctrl_a / ctrl_b;

      std::stringstream strm;

      strm << std::setprecision(std::numeric_limits<naked_double_float_type>::digits10) << df_c;

      const std::string str_df_c = strm.str();

      const bool b_ok =
          (fabs(1 - fabs(ctrl_c / control_float_type(str_df_c))) < std::numeric_limits<control_float_type>::epsilon() * ErrorThreshold);

      if (!b_ok)
      {
         std::cerr << " [FAILED] while performing '" << str_a << "' / '" << str_b << "'" << std::endl;
         return -1;
      }
   }

   std::cout << " ok" << std::endl;
   return 0;
}

template <typename T>
bool test_basic()
{
   std::cout << "Testing basic arithmetic for cpp_double_float<" << typeid(T).name() << ">" << std::endl;

   int e = 0;
   e += test_add<T>();
   e += test_sub<T>();
   e += test_mul<T>();
   e += test_div<T>();

   std::cout << std::endl;
   return e;
}

} // namespace test_arithmetic_cpp_double_float


int main()
{
   test_arithmetic_cpp_double_float::test_basic<float>();
   test_arithmetic_cpp_double_float::test_basic<double>();
}
