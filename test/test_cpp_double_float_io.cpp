///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Basic I/O tests for cpp_double_float<>
// Note that the I/O of cpp_double_float<> is currently extremely underdeveloped

#include <boost/multiprecision/cpp_double_float.hpp>
#include <iostream>
#include <cstdlib>
#include <random>

namespace test_cpp_double_float_io {
double unif_rand()
{
   //static std::random_device                     rd;
   static std::mt19937                           gen;
   static std::uniform_real_distribution<double> dis(0.0, 1.0);

   return dis(gen);
}
double rand_double(int e = 3)
{
   if (unif_rand() > 0.5)
      return std::pow(unif_rand(), e);
   else
      return 1. / std::pow(unif_rand(), e);
}

template <typename FloatingPointType>
void test()
{
   using double_float_t = boost::multiprecision::backends::cpp_double_float<FloatingPointType>;

   bool passed = true;
   int  i;
   for (i = 0; i < 50000; ++i)
   {
      std::stringstream ss1, ss2;
      FloatingPointType d = (FloatingPointType)rand_double(1 + i / 10000);
      double_float_t d_ = d;

      if (unif_rand() > 0.66 && std::log10(d) + 1 < std::numeric_limits<FloatingPointType>::digits10)
         ss1.setf(std::ios::fixed);
      else if (unif_rand() > 0.33)
         ss1.setf(std::ios::scientific);
      else
         ss1.unsetf(std::ios::floatfield);

      auto conditionally_set = [&](std::stringstream& sstream, std::ios::fmtflags flags) {
         if (unif_rand() > 0.5)
            sstream.setf(flags);
         else
            sstream.unsetf(flags);
      };

      conditionally_set(ss1, std::ios::showpoint);
      conditionally_set(ss1, std::ios::showpos);
      conditionally_set(ss1, std::ios::uppercase);

      ss2.flags(ss1.flags());

      int p = static_cast<int>(unif_rand() * std::numeric_limits<FloatingPointType>::digits10);
      if ((ss1.flags() & std::ios::fixed) && d > 0)
      {
         p = std::min(p, std::numeric_limits<FloatingPointType>::digits10 - (int)std::log10(d) - 1);
      }

      ss1.precision(p);
      ss2.precision(p);

      ss1 << d;
      ss2 << d_;

      if (ss1.str() != ss2.str())
      {
         std::cerr.precision(16);
         std::cerr << "FAIL | d=" << std::scientific << d << ", p=" << p << ", ";
         std::cerr << "scientific=" << bool(ss1.flags() & std::ios::scientific) << ", ";
         std::cerr << "fixed=" << bool(ss1.flags() & std::ios::fixed) << ", ";
         std::cerr << "showpoint=" << bool(ss1.flags() & std::ios::showpoint) << ", ";
         std::cerr << "showpos=" << bool(ss1.flags() & std::ios::showpos) << ", ";
         std::cerr << "uppercase=" << bool(ss1.flags() & std::ios::uppercase);
         std::cerr << std::endl;
         std::cerr << "expected: '" << ss1.str() << "'\nactual  : '" << ss2.str() << "'\n" << std::endl;

         std::stringstream ss;
         ss.flags(ss2.flags());
         ss.precision(ss2.precision());
         ss << d_;

         passed = false;
      }
   }

   std::cout << "cpp_double_float<" << typeid(FloatingPointType).name() << ">: "
     << (passed ? "PASS" : "FAIL") << " (" << i << " tests)" << std::endl;
}
} // namespace test_cpp_double_float_io

int main()
{
   test_cpp_double_float_io::test<double>();
   test_cpp_double_float_io::test<float>();

   std::cin.get();
   std::cin.get();
}
