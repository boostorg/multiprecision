///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Test for binary rebuilding of a number from constituent bits.


#include <boost/config.hpp>
#include <boost/multiprecision/number.hpp>
#ifdef BOOST_MATH_USE_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/cpp_double_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/core/demangle.hpp>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace boost { namespace multiprecision {
namespace backends {
template <typename T> int sign(T val) { return (T(0) < val) - (val < T(0)); }

template <typename Rr>
inline cpp_double_float<Rr> pow(const cpp_double_float<Rr>& a, int exp)
{
   return a.pown(a,exp);
}

template <typename Rr, typename Exp>
inline cpp_double_float<Rr> frexp(const cpp_double_float<Rr>& a, Exp* b)
{
   cpp_double_float<Rr> ret(0);
   eval_frexp(ret, a, b);
   return ret;
}

template <typename Rr> void print_double_number(const std::string& prefix, const Rr& arg) {};
template <typename Rr> void print_double_number(const std::string& prefix, const boost::multiprecision::backends::cpp_double_float<Rr>& arg);

class DecomposedReal {
private:
   int                        sig;
   int                        exp;
   std::vector<unsigned char> bits;

public:
   template <typename Rr> DecomposedReal(Rr x)
   {
      using std::frexp;
      int ex   = 0;
      Rr  norm = frexp(x > Rr(0) ? x : -x, &ex);
      sig      = sign(x);
      exp      = ex - 1;
      ex       = 0;
      int pos  = 0;
      bits.resize(std::numeric_limits<Rr>::digits, 0);
      while (
         norm != Rr(0)              // correct condition
      // pos-ex < int(bits.size())  //
      ) {
         pos -= ex;
/*
std::cout << "norm        = " << norm        << ", ";
std::cout << "ex          = " << ex          << ", ";
std::cout << "pos         = " << pos         << ", ";
std::cout << "bits.size() = " << bits.size() << std::endl;
      for (auto c : bits)
         std::cout << int(c);
std::cout << std::endl;
*/
         BOOST_ASSERT((ex <= 0) and (pos < int(bits.size())) and (pos >= 0));
         bits[pos] = 1;
         norm -= static_cast<Rr>(0.5);
         norm = frexp(norm, &ex);
         print_double_number("norm",norm);
      };
   }
   template <typename Rr> Rr rebuild()
   {
      Rr  ret = 0;
      int i   = 0;
      for (auto c : bits) {
         if (c != 0) {
            using std::pow;
            ret += pow(static_cast<Rr>(2), /*static_cast<Rr>*/(exp - i));
         }
         ++i;
      }
      return ret * static_cast<Rr>(sig);
   }
   template <typename Rr = double> void print()
   {
      std::cout << "sign : " << sig << std::endl;
      std::cout << "exp  : " << exp << std::endl;
      std::cout << "bits : " << bit_str() << std::endl;
      std::cout << "reconstructed number: " << rebuild<Rr>() << std::endl << std::endl;
   }
   std::string bit_str() {
      std::stringstream s{};
      for (auto c : bits)
         s << int(c);
      return s.str();
   }
   void short_print()
   {
      std::cout << "exp : " << std::setw(3) << exp << ", bits : " << (sig==1?"+":"-") << bit_str() << std::endl;
   }
   int short_print_shifted(int max_exp=0)
   {
      std::string st = bit_str();
      auto len = st.size();
      if(max_exp == 0) {
         std::cout << "exp : " << std::setw(3) << exp << ", bits : " <<                               (sig==1?"+":"-") << bit_str() << std::endl;
      } else {
         std::cout << "exp : " << std::setw(3) << exp << ", bits : " << std::setw(max_exp - exp+1) << (sig==1?"+":"-") << bit_str() << std::endl;
      }
      return exp;
   }
};

template <typename Rr> void print_double_number(const std::string& prefix, const boost::multiprecision::backends::cpp_double_float<Rr>& arg) {
   std::cout << std::left << std::setw(15) << prefix+".first "  << " = " << std::right; auto ex1 = DecomposedReal(arg.crep().first).short_print_shifted();
   if(arg.crep().second != 0) {
      std::cout << std::left << std::setw(15) << prefix+".second " << " = " << std::right;            DecomposedReal(arg.crep().second).short_print_shifted(ex1);
   }
}

template <typename Rr> void print_number(const Rr& arg)
{
   std::cout << std::setprecision(std::numeric_limits<Rr>::digits10 + 3);
   std::cout << std::endl;
   DecomposedReal d(arg);
   std::cout << std::endl;

   auto rebuilt = d.rebuild<Rr>();
   auto diff = (arg - rebuilt);

   std::cout << "original bits   = "; d.short_print();

   print_double_number("arg",arg);
   print_double_number("rebuilt",rebuilt);

   std::string diff_name = boost::core::demangle(typeid(decltype(diff   )).name());
   std::string arg_name  = boost::core::demangle(typeid(decltype(arg    )).name());
   std::string rebu_name = boost::core::demangle(typeid(decltype(rebuilt)).name());

   BOOST_ASSERT(diff     == decltype(diff)(0));
   BOOST_ASSERT(arg_name == rebu_name        );
   BOOST_ASSERT(arg_name == diff_name        );

   // FIXME : but this check fails !
   BOOST_ASSERT(arg      == rebuilt          );
};
}}}
//////////////////////////////

template<typename R>
void try_number(std::string str) {
   std::cout << std::setprecision(100000);
   std::cout << "Testing number  : " << str << std::endl;
   auto z=boost::multiprecision::backends::cpp_double_float<R>(str);
   std::cout << "With type " << boost::core::demangle(typeid(decltype(z)).name()) << std::endl << std::endl;

   int  ex = 0;
   //if(isfinite(z) and isfinite(z2)) { // ← FIXME
      print_number(z);
   //}
}

template<typename R>
void test() {
// binary representation of this number:
//                11111111100011011111111110001100000011111111111111111000111000001111111111110000000000011111111110000000001111111110000001111 * 2^1407
// try_number<R>("7.07095004791213209137407618364459278413421454874042247410492385622373956879713960311588804604245728321440648803023224236513586176837484939909893244653903501e+423");
// binary representation of this number:
//                11111111100011011111111110001100000011111111111111111000111000001111111111110000000000011111111110000000001111111110000001111 * 2^65
   try_number<R>("73658621713667056515.99902391387240466018304640982705677743069827556610107421875");
// try_number<R>("0.74981689453125");
// try_number<R>("0.1231235123554");
// try_number<R>("5.0395749966458598419365441242084052981209828021829231181382593274122924204e+423");
}

int main()
{
// test<float>();
   test<double>();
// test<long double>();
// test<boost::multiprecision::float128>();
}

