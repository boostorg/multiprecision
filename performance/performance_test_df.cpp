///////////////////////////////////////////////////////////////
//  Copyright John Maddock 2019.
//  Copyright Christopher Kormanyos 2021 - 2024.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

// This version of "performance_test_df.cpp" is based almost entirely on
// the original "performance_test.cpp". But it has been specifically
// adapted to test various backends in small precision ranges in order
// to check the performance comparison with cpp_double_double.

#include <boost/config.hpp>

#if !defined(TEST_CPP_DOUBLE_FLOAT)
#define TEST_CPP_DOUBLE_FLOAT
#endif

#if !defined(TEST_CPP_BIN_FLOAT)
#define TEST_CPP_BIN_FLOAT
#endif

#if !defined(TEST_CPP_DEC_FLOAT)
#define TEST_CPP_DEC_FLOAT
#endif

#if defined(BOOST_HAS_FLOAT128)
#if !defined(TEST_FLOAT128)
#define TEST_FLOAT128
#endif
#endif

#include <boost/version.hpp>

#if defined(TEST_CPP_DOUBLE_FLOAT)
#include <boost/multiprecision/cpp_double_fp.hpp>
#endif

#if defined(TEST_CPP_BIN_FLOAT)
#include <boost/multiprecision/cpp_bin_float.hpp>
#endif

#if defined(TEST_CPP_BIN_FLOAT)
#include <boost/multiprecision/cpp_dec_float.hpp>
#endif

#if defined(TEST_FLOAT128)
#include <boost/multiprecision/float128.hpp>
#endif

#include "performance_test_df.hpp"

// cd /mnt/c/Users/ckorm/Documents/Ks/PC_Software/Test
// g++ -std=gnu++14 -Wall -Wextra -O3 -I/mnt/c/ChrisGitRepos/boost_gsoc2021/multiprecision/performance -I/mnt/c/ChrisGitRepos/boost_gsoc2021/multiprecision/include -I/mnt/c/boost/boost_1_87_0 ./test.cpp -lquadmath -o test

//
// Keys in order are:
// Category
// Operator
// Type
// Precision
// Time
//
std::map<std::string, std::map<std::string, std::map<std::string, std::map<int, double> > > > result_table;

unsigned bits_wanted; // for integer types

void quickbook_results()
{
   //
   // Keys in order are:
   // Category
   // Operator
   // Type
   // Precision
   // Time
   //
   typedef std::map<std::string, std::map<std::string, std::map<std::string, std::map<int, double> > > >::const_iterator category_iterator;
   typedef std::map<std::string, std::map<std::string, std::map<int, double> > >::const_iterator                         operator_iterator;
   typedef std::map<std::string, std::map<int, double> >::const_iterator                                                 type_iterator;
   typedef std::map<int, double>::const_iterator                                                                         precision_iterator;

   for (category_iterator i = result_table.begin(); i != result_table.end(); ++i)
   {
      std::string cat = i->first;
      cat[0]          = (char)std::toupper((char)cat[0]);
      std::cout << "[section:" << i->first << "_performance " << cat << " Type Perfomance]" << std::endl;

      for (operator_iterator j = i->second.begin(); j != i->second.end(); ++j)
      {
         std::string op = j->first;
         std::cout << "[table Operator " << op << std::endl;
         std::cout << "[[Backend]";

         for (precision_iterator k = j->second.begin()->second.begin(); k != j->second.begin()->second.end(); ++k)
         {
            std::cout << "[" << k->first << " Bits]";
         }
         std::cout << "]\n";

         std::vector<double> best_times(j->second.begin()->second.size(), (std::numeric_limits<double>::max)());
         for (unsigned m = 0; m < j->second.begin()->second.size(); ++m)
         {
            for (type_iterator k = j->second.begin(); k != j->second.end(); ++k)
            {
               if (m < k->second.size())
               {
                  precision_iterator l = k->second.begin();
                  std::advance(l, m);
                  if (best_times[m] > l->second)
                     best_times[m] = l->second ? l->second : best_times[m];
               }
            }
         }

         for (type_iterator k = j->second.begin(); k != j->second.end(); ++k)
         {
            std::cout << "[[" << k->first << "]";

            unsigned m = 0;
            for (precision_iterator l = k->second.begin(); l != k->second.end(); ++l)
            {
               double rel_time = l->second / best_times[m];
               if (rel_time == 1)
                  std::cout << "[[*" << rel_time << "]";
               else
                  std::cout << "[" << rel_time;
               std::cout << " (" << l->second << "s)]";
               ++m;
            }

            std::cout << "]\n";
         }

         std::cout << "]\n";
      }

      std::cout << "[endsect]" << std::endl;
   }
}

#if defined(__HAS_INCLUDE)
#if __has_include(<sys/utsname.h>)
#define HAS_UTSNAME
#include <sys/utsname.h>
#endif
#endif
#ifdef _WIN32
#include <windows.h>
#endif

void quickbook_platform_details()
{
   std::cout << "[table:platform Platform Details\n[[Platform][";
#ifdef HAS_UTSNAME
   utsname name;
   uname(&name);
   std::cout << name.sysname << " " << name.release << ", version " << name.version << ", " << name.machine << "]]\n";
#elif defined(_WIN32)
   std::cout << "Windows ";
#ifdef _M_AMD64
   std::cout << "x64";
#elif defined(_M_IX86)
   std::cout << "x86";
#endif
   std::cout << "]]\n";
#endif
   std::cout << "[[Compiler][" << BOOST_COMPILER << "]]\n";
   std::cout << "[[Boost][" << BOOST_VERSION << "]]\n";
   std::cout << "[[Run date][" << __DATE__ << "]]\n";
   std::cout << "]\n\n";
}

namespace local {

void test29();
void test32();
void test52();
void test53();

#ifdef TEST_CPP_DOUBLE_FLOAT
template<class T>
constexpr inline auto (max)(T a, T b) noexcept -> T { return a > b ? a : b; }
#endif

} // namespace local

int main()
{
   quickbook_platform_details();

   local::test29();
   local::test32();
   local::test52();
   #if defined(BOOST_HAS_FLOAT128)
   local::test53();
   #endif

   quickbook_results();
}

#ifdef TEST_CPP_DOUBLE_FLOAT
using double_float_of_double_type = boost::multiprecision::cpp_double_double;

constexpr auto digits10_for_performance_test = (local::max)(std::numeric_limits<double_float_of_double_type>::digits10, 32);
#else
constexpr auto digits10_for_performance_test = 32;
#endif

void local::test29()
{
#ifdef TEST_CPP_DEC_FLOAT
   static_assert(digits10_for_performance_test >= 32, "Error: Too few digits for performance comparison");

   using cpp_dec_float_type =
      boost::multiprecision::number<boost::multiprecision::backends::cpp_dec_float<digits10_for_performance_test>,
                                    boost::multiprecision::et_off>;

   test<cpp_dec_float_type>("cpp_dec_float", digits10_for_performance_test);
#endif
}

void local::test32()
{
#ifdef TEST_CPP_BIN_FLOAT
   static_assert(digits10_for_performance_test >= 32, "Error: Too few digits for performance comparison");

   using cpp_bin_float_type =
      boost::multiprecision::number<boost::multiprecision::backends::cpp_bin_float<digits10_for_performance_test>,
                                    boost::multiprecision::et_off>;

   test<cpp_bin_float_type>("cpp_bin_float", digits10_for_performance_test);
#endif
}

void local::test52()
{
#ifdef TEST_CPP_DOUBLE_FLOAT
   test<double_float_of_double_type>("cpp_double_fp_backend<double>", digits10_for_performance_test);
#endif
}

void local::test53()
{
#if defined(BOOST_HAS_FLOAT128)
#ifdef TEST_FLOAT128
   using boost::multiprecision::float128;

   test<float128>("boost::multiprecision::float128(at)", std::numeric_limits<boost::multiprecision::float128>::digits10);
#endif
#endif
}


#if 0

See also: https://github.com/BoostGSoC21/multiprecision/issues/178#issuecomment-2580157139

## section:float_performance Float Type Perfomance

| Operation                      | `float128`             |`cpp_bin_float<32>`    | `cpp_dec_float<32>`    | `cpp_double_fp_backend<double>` |
|--------------------------------|------------------------|-----------------------|------------------------|---------------------------------|
|`*`                             |  1.29655 (0.0482737s)  | 1.56069 (0.0581082s)  | 4.36508 (0.162522s)    |  [*1] (0.0372323s)              |
|`*(int)`                        |  1.67657 (0.0391687s)  | 2.22565 (0.0519966s)  | 6.523 (0.152393s)      |  [*1] (0.0233624s)              |
|`*(unsigned long long)`         |  1.75129 (0.0840601s)  | 1.11573 (0.053554s)   | 3.88301 (0.18638s)     |  [*1] (0.0479989s)              |
|`*=(unsigned long long)`        |  1.79306 (0.0852996s)  | 1.13341 (0.0539183s)  | 3.89232 (0.185166s)    |  [*1] (0.047572s)               |
|`+`                             |  2.47333 (0.0424143s)  | 5.53293 (0.0948825s)  | 3.15139 (0.0540422s)   |  [*1] (0.0171487s)              |
|`+(int)`                        |  2.90697 (0.0356041s)  | 7.90829 (0.0968595s)  | 5.98834 (0.0733442s)   |  [*1] (0.0122478s)              |
|`+(unsigned long long)`         |  3.33903 (0.0725708s)  | 5.27504 (0.114648s)   | 4.36764 (0.0949269s)   |  [*1] (0.0217341s)              |
|`+=(unsigned long long)`        |  2.61742 (0.0752542s)  | 3.90945 (0.112402s)   | 3.10689 (0.0893271s)   |  [*1] (0.0287513s)              |
|`-`                             |  2.16641 (0.0385902s)  | 5.50957 (0.0981418s)  | 3.44314 (0.0613326s)   |  [*1] (0.017813s)               |
|`-(int)`                        |  3.09675 (0.0373687s)  | 8.30948 (0.100271s)   | 7.72051 (0.0931639s)   |  [*1] (0.0120671s)              |
|`-(unsigned long long)`         |  3.43252 (0.07464s)    | 5.38368 (0.117068s)   | 4.84276 (0.105306s)    |  [*1] (0.0217449s)              |
|`-=(unsigned long long)`        |  3.46923 (0.0758863s)  | 4.91925 (0.107604s)   | 4.36939 (0.0955765s)   |  [*1] (0.0218741s)              |
|`/`                             |  1.54905 (0.147974s)   | 7.93385 (0.757886s)   | 26.1974 (2.50253s)     |  [*1] (0.0955256s)              |
|`/(unsigned long long)`         |  2.45133 (0.0978809s)  | 9.63926 (0.384893s)   | 31.7676 (1.26847s)     |  [*1] (0.0399297s)              |
|`/=(unsigned long long)`        |  2.45656 (0.0983785s)  | 9.50848 (0.380789s)   | 31.9545 (1.27969s)     |  [*1] (0.0400473s)              |
|`construct`                     |  [*1] (0.000485036s)   | 7.03755 (0.00341347s) | 14.4582 (0.00701277s)  |  3.67186 (0.00178098s)          |
|`construct(unsigned long long)` |  5.36484 (0.0403896s)  | 2.69439 (0.0202849s)  | 3.09062 (0.023268s)    |  [*1] (0.00752857s)             |
|`construct(unsigned)`           |  23.821 (0.0402518s)   | 12.3039 (0.0207907s)  | 13.7681 (0.0232648s)   |  [*1] (0.00168976s)             |
|`str`                           |  [*1] (0.000392391s)   | 1.48164 (0.000581384s)| 1.31364 (0.000515462s) |  11.8102 (0.0046342s)           |
|`exp`                           | 1.37822 (1.637s)       | 5.17963 (6.1522s)     | 5.24227 (6.22661s)     | [*1] (1.18777s)                 |
|`log`                           | [*1] (2.07022s)        | 9.3833 (19.4255s)     | 25.9987 (53.8229s)     | 1.12041 (2.3195s)               |
|`sqrt`                          | 25.1126 (0.528154s)    | 24.2364 (0.509727s)   | 128.647 (2.70564s)     | [*1] (0.0210315s)               |

#endif
