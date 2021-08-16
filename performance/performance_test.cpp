///////////////////////////////////////////////////////////////
//  Copyright 2011-9 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#define TEST_CPP_QUAD_FLOAT
//#define TEST_CPP_BIN_FLOAT

#include "performance_test.hpp"

#ifdef TEST_MPZ
#include <gmp.h>
#endif
#ifdef TEST_MPFR
#include <mpfr.h>
#endif
#include <boost/version.hpp>

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
#ifdef TEST_MPZ
   std::cout << "[[GMP][" << gmp_version << "]]\n";
#endif
#ifdef TEST_MPFR
   std::cout << "[[MPFR][" << MPFR_VERSION << "]]\n";
#endif
   std::cout << "[[Boost][" << BOOST_VERSION << "]]\n";
   std::cout << "[[Run date][" << __DATE__ << "]]\n";
   std::cout << "]\n\n";
}

int main()
{
   quickbook_platform_details();

   #if defined(TEST_CPP_QUAD_FLOAT)
   test53();
   #elif defined(TEST_CPP_BIN_FLOAT)
   test33();
   #endif

   quickbook_results();
   return 0;
}

using local_float_constituent_type = double;

#if defined(TEST_CPP_QUAD_FLOAT)

///////////////////////////////////////////////////////////////
//  Copyright 2019 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

//#include "performance_test.hpp"
#if defined(BOOST_MATH_USE_FLOAT128)
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/cpp_quad_float.hpp>

void test53()
{
   using quad_float_of_double_type = boost::multiprecision::number<boost::multiprecision::backends::cpp_quad_float<local_float_constituent_type>, boost::multiprecision::et_off>;

   test<quad_float_of_double_type>("cpp_quad_float", 1024*16);
}

#elif defined(TEST_CPP_BIN_FLOAT)

///////////////////////////////////////////////////////////////
//  Copyright 2019 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#include <boost/multiprecision/cpp_bin_float.hpp>

void test33()
{
   constexpr int my_digits10 = (int) (float((std::numeric_limits<local_float_constituent_type>::digits * 4) - 1) * 0.301F);

   using cpp_bin_float_type = boost::multiprecision::number<boost::multiprecision::cpp_bin_float<my_digits10>, boost::multiprecision::et_off>;

   test<boost::multiprecision::cpp_bin_float_100>("cpp_bin_float", 1024*16);
}

#endif

// Chris PC TEST_CPP_QUAD_FLOAT
//cpp_quad_float 16384     +                                  0.0328359
//cpp_quad_float 16384     -                                  0.0326524
//cpp_quad_float 16384     *                                  0.0516673
//cpp_quad_float 16384     /                                  0.707075
//cpp_quad_float 16384     str                                0.0419498
//cpp_quad_float 16384     +(int)                             0.0267044
//cpp_quad_float 16384     -(int)                             0.0279037
//cpp_quad_float 16384     *(int)                             0.0512275
//cpp_quad_float 16384     /(int)                             0.327655
//cpp_quad_float 16384     construct                          0.0011852
//cpp_quad_float 16384     construct(unsigned)                0.0011655
//cpp_quad_float 16384     construct(unsigned long long)      0.171181
//cpp_quad_float 16384     +(unsigned long long)              0.113957
//cpp_quad_float 16384     -(unsigned long long)              0.115939
//cpp_quad_float 16384     *(unsigned long long)              0.142094
//cpp_quad_float 16384     /(unsigned long long)              0.407529
//cpp_quad_float 16384     +=(unsigned long long)             0.113941
//cpp_quad_float 16384     -=(unsigned long long)             0.114742
//cpp_quad_float 16384     *=(unsigned long long)             0.141193
//cpp_quad_float 16384     /=(unsigned long long)             0.410069


// Chris PC TEST_CPP_BIN_FLOAT
//cpp_bin_float  16384     +                                  0.0433169
//cpp_bin_float  16384     -                                  0.0459543
//cpp_bin_float  16384     *                                  0.102164
//cpp_bin_float  16384     /                                  1.18277
//cpp_bin_float  16384     str                                0.0042434
//cpp_bin_float  16384     +(int)                             0.0486855
//cpp_bin_float  16384     -(int)                             0.0529648
//cpp_bin_float  16384     *(int)                             0.0262129
//cpp_bin_float  16384     /(int)                             0.112234
//cpp_bin_float  16384     construct                          0.0024933
//cpp_bin_float  16384     construct(unsigned)                0.0134788
//cpp_bin_float  16384     construct(unsigned long long)      0.0151112
//cpp_bin_float  16384     +(unsigned long long)              0.0559024
//cpp_bin_float  16384     -(unsigned long long)              0.0570488
//cpp_bin_float  16384     *(unsigned long long)              0.0424473
//cpp_bin_float  16384     /(unsigned long long)              0.431306
//cpp_bin_float  16384     +=(unsigned long long)             0.0551851
//cpp_bin_float  16384     -=(unsigned long long)             0.0525513
//cpp_bin_float  16384     *=(unsigned long long)             0.0417501
//cpp_bin_float  16384     /=(unsigned long long)             0.424533
