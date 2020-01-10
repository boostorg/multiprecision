//---------------------------------------------------------------------------//
// Copyright (c) 2019 Nil Foundation AG
// Copyright (c) 2019 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

//#include <boost/test/unit_test.hpp>
//#include <boost/test/data/test_case.hpp>

#include <iostream>
#include <vector>

#include <boost/multiprecision/modular/modular_adaptor.hpp>
#include <boost/multiprecision/modular/modular_params.hpp>

#ifdef TEST_CPP_INT
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_modular.hpp>
#endif

#ifdef TEST_GMP
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/gmp_modular.hpp>
#endif

#ifdef TEST_TOMMATH
#include <boost/multiprecision/tommath.hpp>
#include <boost/multiprecision/modular/modular_params_tommath.hpp>
#endif

#include "test.hpp"

static const std::string even_mod[] = {"8", "64", "31232", "3242343244", "32444223445648", "2342345878437578", "39083287423897423784987234876"};
static const std::string odd_mod[]  = {"7", "23", "23133", "384828945", "938423847897893", "58976234672378752477", "345943598934088945908589591"};

static const std::string big_numbers[] = {"3777078316807535813265472776245476795571913941214974396352",
                                          "7730650700335662967298805496078834074815880969722197781616",
                                          "76666273003804943755065306388850182474417757120635316375"};

static const size_t numbers_size[] = {8, 16, 20, 25, 30, 35, 40};

using namespace boost::multiprecision;

template <typename Backend>
void base_operators()
{
   typedef number<modular_adaptor<Backend> > modular_number;
   typedef modular_params<Backend>           params_number;
   typedef number<Backend>                   standard_number;

   for (size_t current_number_size = 0; current_number_size < 7; ++current_number_size)
   {
      for (size_t parity = 0; parity <= 1; ++parity)
      {
         std::string a_string = big_numbers[0].substr(0, numbers_size[current_number_size]);
         std::string b_string = big_numbers[1].substr(0, numbers_size[current_number_size]);
         std::string mod_string;
         if (parity)
            mod_string = odd_mod[current_number_size];
         else
            mod_string = even_mod[current_number_size];

         standard_number a_s(a_string), b_s(b_string), mod_s(mod_string), result_s(0);

         params_number  mod(mod_s);
         modular_number a(a_s, mod), b(b_s, mod), result(0, mod);
         result   = a + b;
         result_s = result.template convert_to<standard_number>();
         BOOST_CHECK_EQUAL(result_s, (a_s + b_s) % mod_s);

         result   = a - b;
         result_s = result.template convert_to<standard_number>();
         if (a_s < b_s)
         {
            BOOST_CHECK_EQUAL(result_s, (a_s - b_s) % mod_s + mod_s);
         }
         else
         {
            BOOST_CHECK_EQUAL(result_s, (a_s - b_s) % mod_s);
         }

         result   = a * b;
         result_s = result.template convert_to<standard_number>();
         BOOST_CHECK_EQUAL(result_s, (a_s * b_s) % mod_s);

         result   = a / b;
         result_s = result.template convert_to<standard_number>();
         BOOST_CHECK_EQUAL(result_s, ((a_s % mod_s) / (b_s % mod_s)));

         result   = a % b;
         result_s = result.template convert_to<standard_number>();
         BOOST_CHECK_EQUAL(result_s, ((a_s % mod_s) % (b_s % mod_s)));
      }
   }
}

template <typename Backend>
void comparsion_operators()
{
   typedef number<modular_adaptor<Backend> > modular_number;
   typedef modular_params<Backend>           params_number;
   typedef number<Backend>                   standard_number;

   for (size_t current_number_size = 0; current_number_size < 7; ++current_number_size)
   {
      for (size_t parity = 0; parity <= 1; ++parity)
      {
         std::string a_string = big_numbers[0].substr(0, numbers_size[current_number_size]);
         std::string b_string = big_numbers[1].substr(0, numbers_size[current_number_size]);
         std::string mod_string;
         if (parity)
            mod_string = odd_mod[current_number_size];
         else
            mod_string = even_mod[current_number_size];

         standard_number a_s(a_string), b_s(b_string), mod_s(mod_string), result_s(0);
         params_number   mod(mod_s);
         modular_number  a(a_s, mod), b(b_s, mod);

         BOOST_CHECK_EQUAL(a < b, (a_s % mod_s) < (b_s % mod_s));
         BOOST_CHECK_EQUAL(a <= b, (a_s % mod_s) <= (b_s % mod_s));
         BOOST_CHECK_EQUAL(a > b, (a_s % mod_s) > (b_s % mod_s));
         BOOST_CHECK_EQUAL(a >= b, (a_s % mod_s) >= (b_s % mod_s));
         BOOST_CHECK_EQUAL(a == b, (a_s % mod_s) == (b_s % mod_s));
         BOOST_CHECK_EQUAL(a != b, (a_s % mod_s) != (b_s % mod_s));
      }
   }
}

template <typename Backend>
void bitwise_operators()
{
   typedef number<modular_adaptor<Backend> > modular_number;
   typedef modular_params<Backend>           params_number;
   typedef number<Backend>                   standard_number;

   for (size_t current_number_size = 0; current_number_size < 7; ++current_number_size)
   {
      for (size_t parity = 0; parity <= 1; ++parity)
      {
         std::string a_string = big_numbers[0].substr(0, numbers_size[current_number_size]);
         std::string b_string = big_numbers[1].substr(0, numbers_size[current_number_size]);
         std::string mod_string;
         if (parity)
            mod_string = odd_mod[current_number_size];
         else
            mod_string = even_mod[current_number_size];

         standard_number a_s(a_string), b_s(b_string), mod_s(mod_string), result_s(0);
         params_number   mod(mod_s);
         modular_number  a(a_s, mod), b(b_s, mod), result(0, mod);

         BOOST_CHECK_EQUAL((a & b).template convert_to<standard_number>(), (((a_s % mod_s) & (b_s % mod_s))) % mod_s);
         BOOST_CHECK_EQUAL((a | b).template convert_to<standard_number>(), (((a_s % mod_s) | (b_s % mod_s))) % mod_s);
         BOOST_CHECK_EQUAL((a ^ b).template convert_to<standard_number>(), (((a_s % mod_s) ^ (b_s % mod_s))) % mod_s);
      }
   }
}

template <typename Backend>
void pow_test()
{
   typedef number<modular_adaptor<Backend> > modular_number;
   typedef modular_params<Backend>           params_number;
   typedef number<Backend>                   standard_number;

   for (size_t current_number_size = 0; current_number_size < 7; ++current_number_size)
   {
      for (size_t parity = 0; parity <= 1; ++parity)
      {
         std::string a_string = big_numbers[0].substr(0, numbers_size[current_number_size]);
         std::string b_string = big_numbers[1].substr(0, numbers_size[current_number_size]);
         std::string mod_string;
         if (parity)
            mod_string = odd_mod[current_number_size];
         else
            mod_string = even_mod[current_number_size];

         standard_number a_s(a_string), b_s(b_string), mod_s(mod_string), result_s(0);
         params_number   mod(mod_s);
         modular_number  a(a_s, mod), b(b_s, mod), result(0, mod);
         BOOST_CHECK_EQUAL(pow(a, b).template convert_to<standard_number>(), powm(a_s % mod_s, b_s % mod_s, mod_s));
      }
   }
}

template <typename Backend>
void run_tests()
{
   base_operators<Backend>();
   comparsion_operators<Backend>();
   bitwise_operators<Backend>();
   pow_test<Backend>();
}

int main()
{
#ifdef TEST_CPP_INT
     run_tests<cpp_int_backend<> >();
#endif

#ifdef TEST_GMP
   run_tests<gmp_int>();
#endif

#ifdef TEST_TOMMATH
   run_tests<tommath_int>();
#endif
   return boost::report_errors();
}
