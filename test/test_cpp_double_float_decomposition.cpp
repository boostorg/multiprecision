///////////////////////////////////////////////////////////////////////////////
//  Copyright 2021 Fahad Syed.
//  Copyright 2021 Christopher Kormanyos.
//  Copyright 2021 Janek Kozicki.
//  Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
//  || copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Test for binary rebuilding of a number from constituent bits.


#include <boost/config.hpp>
#include <boost/multiprecision/number.hpp>
#ifdef BOOST_MATH_USE_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif
#include <boost/multiprecision/cpp_double_float.hpp>
#include <boost/multiprecision/cpp_quad_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/math/special_functions/next.hpp>
#include <boost/core/demangle.hpp>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

class LoopError{};

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

template <typename Rr> void print_compound_number(const std::string& prefix, const Rr& arg) {};
template <typename Rr> void print_compound_number(const std::string& prefix, const boost::multiprecision::backends::cpp_double_float<Rr>& arg);
template <typename Rr> void print_compound_number(const std::string& prefix, const boost::multiprecision::backends::cpp_quad_float<Rr>& arg);

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

      // allow extra room for guard bits https://github.com/BoostGSoC21/multiprecision/commit/766899bb2b05e8f47832d58b99d166913fb496d1#commitcomment-54355724
      // let's allow one byte extra.
      int guard_bits = boost::multiprecision::backends::detail::is_floating_point_or_float128<Rr>::value ? 0 : 8;
      bits.resize(std::numeric_limits<Rr>::digits + guard_bits, 0);

      while ((norm != Rr(0)) && ((pos - ex) < bits.size())) {
         pos -= ex;
         if(!((ex <= 0) && (pos < int(bits.size())) && (pos >= 0))) throw LoopError{};
         bits[pos] = 1;
         norm -= static_cast<Rr>(0.5);
         norm = frexp(norm, &ex);

// Prints while decomposing the number, nice for debugging double float because it prints all the components.
// for quad flaot simply write another specialization of print_compound_number

//       print_compound_number("norm",norm);

      };
   }
   DecomposedReal(const std::string& bit_string, int exp_arg, int sig_arg)
   {
      bits.resize(bit_string.size() , 0);
      for(size_t i = 0 ; i < std::min(bits.size() , bit_string.size()) ; ++i ) {
         bits[i] = bit_string[i] - '0';
      }
      exp = exp_arg;
      sig = sig_arg;
   }
   template <typename Rr> Rr rebuild()
   {
      Rr  ret = 0;
      int i   = 0;
      for (auto c : bits) {
         if(!((c==0) || (c==1))) {
            std::cerr << "bad bits" << std::endl;
            exit(1);
         }
         if (c != 0) {
            using std::pow;
            ret += pow(static_cast<Rr>(2), exp - i);
         }
         ++i;
      }
      return ret * static_cast<Rr>(sig);
   }
   template <typename Rr> void print()
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
   int short_print_shifted(int max_exp = 0, bool skip_exp = true)
   {
      std::string st = bit_str();
      auto len = st.size();
      if(skip_exp) {
         std::cout << "exp : " << std::setw(3) << exp << ", bits : "  << std::right <<                               (sig==1?"+":"-") << bit_str() << std::endl;
      } else {
      // The std::setw makes sure that the bits are printed in properly shifted form - in columns aligned with the first part.
         std::cout << "exp : " << std::setw(3) << exp << ", bits : "  << std::right << std::setw(max_exp - exp+1) << (sig==1?"+":"-") << bit_str() << std::endl;
      }
      return exp;
   }
};

template <typename Rr> void print_compound_number(const std::string& prefix, const boost::multiprecision::backends::cpp_double_float<Rr>& arg) {
   std::cout    << std::left << std::setw(15) << prefix+".first "  << " = " << std::right; auto ex1 = DecomposedReal(arg.crep().first).short_print_shifted();
   if(arg.crep().second != 0) {
      std::cout << std::left << std::setw(15) << prefix+".second " << " = " << std::right;            DecomposedReal(arg.crep().second).short_print_shifted(ex1,false);
   }
}

// not tested
template <typename Rr> void print_compound_number(const std::string& prefix, const boost::multiprecision::backends::cpp_quad_float<Rr>& arg) {
   std::cout    << std::left << std::setw(15) << prefix+".first "  << " = " << std::right; auto ex1 = DecomposedReal(get<0>(arg.crep())).short_print_shifted();
   if(get<1>(arg.crep()) != 0) {
      std::cout << std::left << std::setw(15) << prefix+".second " << " = " << std::right;            DecomposedReal(get<1>(arg.crep())).short_print_shifted(ex1,false);
   }
   if(get<2>(arg.crep()) != 0) {
      std::cout << std::left << std::setw(15) << prefix+".third  " << " = " << std::right;            DecomposedReal(get<2>(arg.crep())).short_print_shifted(ex1,false);
   }
   if(get<3>(arg.crep()) != 0) {
      std::cout << std::left << std::setw(15) << prefix+".fourth " << " = " << std::right;            DecomposedReal(get<3>(arg.crep())).short_print_shifted(ex1,false);
   }
}

template <typename Rr> int print_number(const Rr& arg)
{
   int errors = 0;
   DecomposedReal d(arg);
   auto rebuilt = d.rebuild<Rr>();

   // FIXME: should these normalize calls be here, ot at the end of some functions inside cpp_double_float.hpp ?
   Rr::arithmetic::normalize(rebuilt.rep());
   Rr::arithmetic::extra_normalize(rebuilt.rep());

   auto diff = (arg - rebuilt);

   static_assert(std::is_same<decltype(diff   ), Rr >::value,"");
   static_assert(std::is_same<decltype(rebuilt), Rr >::value,"");

   std::cout << "original bits   = "; d.short_print();
   print_compound_number("arg",arg);
   print_compound_number("rebuilt",rebuilt);

   if (diff != decltype(diff)(0))
   {
      errors++;
      std::cout << "** ERROR in diff **" << std::endl;
   }
   if (arg != rebuilt)
   {
      errors++;
      std::cout << "** ERROR in rebuilt **" << std::endl;
   }
   if (arg > rebuilt)
   {
      errors++;
      std::cout << "** ERROR in operator > **" << std::endl;
   }
   if (arg < rebuilt)
   {
      errors++;
      std::cout << "** ERROR in operator < **" << std::endl;
   }
   return errors;
};
}}}
//////////////////////////////

template<typename R> R fromBits(const std::string& bit_string, int exp_arg, int sig_arg) {
   return boost::multiprecision::backends::DecomposedReal(bit_string, exp_arg, sig_arg).rebuild<R>();
}

template<typename R, typename Arg>
int try_number(R& prev_number, Arg str) {
   auto z = R(str);
   std::cout  << std::endl << std::setprecision(100000) << "Testing number  : " << str << std::endl;
   std::cout << "With type " << boost::core::demangle(typeid(decltype(z)).name()) << std::endl;

   int errors = 0;
   try {
      // FIXME: should these normalize calls be here, ot at the end of some functions inside cpp_double_float.hpp ?
      R::arithmetic::normalize(z.rep());
      R::arithmetic::extra_normalize(z.rep());

      errors += print_number(z);
   } catch(const LoopError&) {
      std::cout << "** ERROR in decomposition **" << std::endl;
      errors++;
   }

   // try_number is called in pairs: first this number is constructed from bits, then from string.
   // both numbers should be the same. This if determines which call it is:
   if(std::is_same<R, typename std::decay<Arg>::type>::value) {
      // 1. stores in prev_number the z if Arg was constructed from bits
      prev_number = z;

   } else {
      // 2. compares z with prev_number if Arg was string. We allow 1 ULP error, though I might not like such tolerance. / Janek

      // FIXME, this code could be turned into a single call to
      //    std::abs(boost::math::float_distance(z , prev_number); but that does not compile.
      // So I calculate ULP manually.
      R   str_to_bin_error = z - prev_number;
      int exp              = 0;
      str_to_bin_error    = frexp(str_to_bin_error , &exp);

      // 1 ULP will be either 0.5 or 0. :
      if(! (str_to_bin_error == R(-0.5) || str_to_bin_error == R(0.5) || str_to_bin_error == R(0.0)))
      // FIXME, this isn't working yet:
      //if(std::abs(boost::math::float_distance(z , prev_number)) > 1)
      {
         errors++;
         std::cout << "** ERROR between string ↔ binary **" << str_to_bin_error << std::endl;
      }
   }

   return errors;
}

template<typename R>
int test() {
   int errors = 0;

   // this ref number is used for comparison between number constructed from string, and same number constructed from bits.
   R ref = 0;


// FIXME: the numbers picked here are sort of hand-crafted to pick the worst scenarios.
// but a loop on some random numbers could be also useful.

// FIXME: Infinite loop somewhere. Lockup. Most likely due to mishandling infinities.
// errors += try_number<R>(fromBits<R>("11111111100011011111111110001100000011111111111111111000111000001111111111110000000000011111111110000000001111111110000001111", 1407 , 1 ));
// errors += try_number<R>("7.07095004791213209137407618364459278413421454874042247410492385622373956879713960311588804604245728321440648803023224236513586176837484939909893244653903501e+423");
// errors += try_number<R>("5.0395749966458598419365441242084052981209828021829231181382593274122924204e+423");

   errors += try_number<R>(ref,fromBits<R>("11111111100011011111111110001100000011111111111111111000111000001111111111110000000000011111111110000000001111111110000001111", 65 , 1 ));
   errors += try_number<R>(ref,"73658621713667056515.99902391387240466018304640982705677743069827556610107421875");

   errors += try_number<R>(ref,fromBits<R>("101111111111010", -1 , 1 ));
   errors += try_number<R>(ref,"0.74981689453125");
   errors += try_number<R>(ref,fromBits<R>("11111100001010000010111000010111011110110001001100001100110101011010000011101001010100111100001110111101010001101010111111101101010111111100110000000000110000000001110110100110010111110011101111011100100001011001000101010111001000110101001110000111100001000101011000011101111011001101100010110101001100110111111000001010010001011100011110100001011001001111101110101010100111000000010111011001100001100001110101010001100001110001000110011110010011000111101101001010111011110101011000010000100010001111", -4 , 1 ));
   errors += try_number<R>(ref,"0.1231235123554");

// A series od edge cases. Both are the same number. One is in binary form other is in decimal form.
   std::cout << std::endl << "→→ for float, put '1' in sensitive places:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("100000000000000000000001100000000000000000000001", 0 , 1 ));
   errors += try_number<R>(ref,"1.00000017881394143159923260100185871124267578125");

   std::cout << std::endl << "→→ for float, put '1' at 45 sensitive place:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("10000000000000000000000000000000000000000000100000", 0 , 1 ));
   errors += try_number<R>(ref,"1.00000000000005684341886080801486968994140625");

   std::cout << std::endl << "→→ for float, put '1' at 46 sensitive place:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("100000000000000000000000000000000000000000000100000", 0 , 1 ));
   errors += try_number<R>(ref,"1.000000000000028421709430404007434844970703125");

   std::cout << std::endl << "→→ for float, put '1' at 47 sensitive place:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("1000000000000000000000000000000000000000000000100000", 0 , 1 ));
   errors += try_number<R>(ref,"1.0000000000000142108547152020037174224853515625");

   std::cout << std::endl << "→→ for float, put '1' at 48 sensitive place:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("10000000000000000000000000000000000000000000000100000", 0 , 1 ));
   errors += try_number<R>(ref,"1.00000000000000710542735760100185871124267578125");

   std::cout << std::endl << "→→ for float, put '1' at 49 sensitive place:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("100000000000000000000000000000000000000000000000100000", 0 , 1 ));
   errors += try_number<R>(ref,"1.000000000000003552713678800500929355621337890625");

   std::cout << std::endl << "→→ for float, put '1' at 50 sensitive place:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("1000000000000000000000000000000000000000000000000100000", 0 , 1 ));
   errors += try_number<R>(ref,"1.0000000000000017763568394002504646778106689453125");

   std::cout << std::endl << "→→ for double, put '1' in sensitive places:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("1000000000000000000000000000000000000000000000000000110000000000000000000000000000000000000000000000000001", 0 , 1 ));
   errors += try_number<R>(ref,"1.000000000000000333066907387546986778992788583865012866517665087069677287701097156968899071216583251953125");

   std::cout << std::endl << "→→ for long double, put '1' in sensitive places:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("10000000000000000000000000000000000000000000000000000000000000011000000000000000000000000000000000000000000000000000000000000001", 0 , 1 ));
   errors += try_number<R>(ref,"1.0000000000000000001626303258728256651069953918846026942537108701861112283890933277838604376075437585313920862972736358642578125");

   std::cout << std::endl << "→→ for float128, put '1' in sensitive places:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001", 0 , 1 ));
   errors += try_number<R>(ref,"1.000000000000000000000000000000000288889491658085377958396691387739116326746000669415157944667127073181392521417643788955012265195294587906700478146794987008");

// A series od edge cases. Both are the same number. One is in binary form other is in decimal form.
   std::cout << std::endl << "→→ for float, put '1' in sensitive places:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("100000000000000000000001100000000000000000000111", 0 , 1 ));
   errors += try_number<R>(ref,"1.00000017881398406416337820701301097869873046875");

   std::cout << std::endl << "→→ for double, put '1' in sensitive places:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("1000000000000000000000000000000000000000000000000000110000000000000000000000000000000000000000000000000111", 0 , 1 ));
   errors += try_number<R>(ref,"1.000000000000000333066907387547134690412517523578527565623655609487741013907680098782293498516082763671875");

   std::cout << std::endl << "→→ for long double, put '1' in sensitive places:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("10000000000000000000000000000000000000000000000000000000000000011000000000000000000000000000000000000000000000000000000000000111", 0 , 1 ));
   errors += try_number<R>(ref,"1.0000000000000000001626303258728256651422602224092713194927729663027785987236532944870230632528063097197446040809154510498046875");

   std::cout << std::endl << "→→ for float128, put '1' in sensitive places:" << std::endl;
   errors += try_number<R>(ref,fromBits<R>("1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111", 0 , 1 ));
   errors += try_number<R>(ref,"1.000000000000000000000000000000000288889491658085377958396691387739227602930521292056729896724973502366793548361006522685085856367062115346903347027564905997");

   return errors;
}

template<class R> using double_float = boost::multiprecision::backends::cpp_double_float<R>;
template<class R> using quad_float   = boost::multiprecision::backends::cpp_quad_float<R>;
int main()
{
   int errors = 0;

// NOTE: for extended debugging, try this change in numeric_limits     ↓↓↓↓↓↓↓
//   static constexpr int digits       = 2 * (base_class_type::digits /* -1 */);
// Interesting stuff for double_float<float> && "→→ for float …" case.

   errors += test<double_float<float>>();
   errors += test<double_float<double>>();
   errors += test<double_float<long double>>();
#ifdef BOOST_MATH_USE_FLOAT128
   errors += test<double_float<boost::multiprecision::float128>>();
#endif

/* // FIXME: No pow, no frexp yet...
   // soon we should be able to test quad_float also :)

   errors += test<quad_float<float>>();
   errors += test<quad_float<double>>();
   errors += test<quad_float<long double>>();
#ifdef BOOST_MATH_USE_FLOAT128
   errors += test<quad_float<boost::multiprecision::float128>>();
#endif
*/
   std::cout << "Total number of errors : " << errors << std::endl << std::endl;

#if __cplusplus >= 201700
   boost::multiprecision::backends::cpp_quad_float<double> a(std::make_tuple(0x1.921fb54442d18p+1, 0x1.1a62633145c07p-53, -0x1.f1976b7ed8fbcp-109, 0x1.3b8d3f60d850cp-163));
   boost::multiprecision::backends::cpp_quad_float<double> e(std::make_tuple(0x1.5bf0a8b0ad9b2p+1, -0x1.e86a384b7f304p-53, 0x1.45fe0602f06dbp-107, 0x1.d8cc5979789dep-162));
   auto ae = a-e;
   boost::multiprecision::backends::print_compound_number("a",a);
   boost::multiprecision::backends::print_compound_number("e",e);
   boost::multiprecision::backends::print_compound_number("a-e",ae);
#endif

// std::cin.get(); // In the pipeline I comment it, so we can watch the pipeline. But okay to uncomment locally.

   return (errors != 0);
}

