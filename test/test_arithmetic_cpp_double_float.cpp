#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <boost/multiprecision/cpp_double_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#ifndef _MSC_VER
#include <boost/multiprecision/float128.hpp>
#endif

namespace local {
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
} // namespace local

template <typename T>
bool test()
{
   constexpr auto digs           = std::numeric_limits<T>::digits10;
   using naked_double_float_type = boost::multiprecision::backends::cpp_double_float<T>;
   using control_float_type      = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<digs * 2 + 2>, boost::multiprecision::et_off>;
   bool all_ok                   = true;
   for (unsigned i = 0U; i < 16U; ++i)
   {
      std::string str_a;
      std::string str_b;

      local::get_random_fixed_string<digs * 2 + 1>(str_a);
      local::get_random_fixed_string<digs * 2 + 1>(str_b);

      std::cout << "Testing " << str_a << " / " << str_b << std::endl;

      const naked_double_float_type df_a(str_a);
      const naked_double_float_type df_b(str_b);

      const control_float_type ctrl_a(str_a);
      const control_float_type ctrl_b(str_b);

      naked_double_float_type df_c   = df_a / df_b;
      control_float_type      ctrl_c = ctrl_a / ctrl_b;

      std::cout << "df_c  : " << std::setprecision(digs * 2 + 2) << df_c << " (" << df_c.get_raw_str() << ")" << std::endl;
      std::cout << "ctrl_c: " << std::setprecision(digs * 2 + 2) << ctrl_c << std::endl;

      bool result_is_ok;

      {
         std::stringstream strm;

         strm << std::setprecision(digs * 4 + 2) << df_c; // digs * 4 because I/O problems cause loss of precision

         const std::string str_df_c = strm.str();

         result_is_ok = (fabs(1 - fabs(ctrl_c / control_float_type(str_df_c))) < std::numeric_limits<control_float_type>::epsilon() * 1000000UL);
      }

      std::cout << "result_is_ok: " << std::boolalpha << result_is_ok << std::endl;
      all_ok = all_ok && result_is_ok;
   }
   return all_ok;
}

//int main()
//{
//   bool ok = true;
//   ok      = ok && test<double>();
//   ok      = ok && test<long double>();
//   ok      = ok && test<float>();
//   //ok      = ok && test<boost::multiprecision::float128>();
//   //ok      = ok && test<boost::multiprecision::number<boost::multiprecision::cpp_bin_float<34>, boost::multiprecision::et_off> >();
//   //ok = ok && test<boost::multiprecision::number<boost::multiprecision::cpp_bin_float<40>, boost::multiprecision::et_off>>(); // error because 1ULL - we could use std::conditional there.
//   //ok = ok && test<boost::multiprecision::number<boost::multiprecision::cpp_dec_float<90>, boost::multiprecision::et_off> >();
//   std::cout << "OK: " << std::boolalpha << ok << std::endl;
//}
