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

namespace std {

// Since there are no longer inner specializations of
// numeric_limits for standalone cpp_double_fp_backend<T>
// and cpp_quad_fp_backend<T> template types, we provide our own
// local specializations patched here.

template <typename FloatingPointType>
class numeric_limits<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType> >
    : public std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_double_fp_backend<FloatingPointType> > >
{
};

template <typename FloatingPointType>
class numeric_limits<boost::multiprecision::backends::cpp_quad_fp_backend<FloatingPointType> >
    : public std::numeric_limits<boost::multiprecision::number<boost::multiprecision::backends::cpp_quad_fp_backend<FloatingPointType> > >
{
};

} // namespace std

// Reference data comes in a tuple of four elements:
// 0. string  : binary representation of the number to be tested
// 1. int     : binary exponent, a power of two.
// 2. int     : sign of the number
// 3. string  : exacly the same number but in decimal representation

// The numbers picked here are sort of hand-crafted to pick the worst scenarios. 
// TODO: but a loop on some random numbers could be also useful.
static const boost::array<std::tuple<const char*, int, int, const char*>, 34u /* 37u */> data =
{{
  { "11111100001010000010111000010111011110110001001100001100110101011010000011101001010100111100001110111101010001101010111111101101010111111100110000000000110000000001110110100110010111110011101111011100100001011001000101010111001000110101001110000111100001000101011000011101111011001101100010110101001100110111111000001010010001011100011110100001011001001111101110101010100111000000010111011001100001100001110101010001100001110001000110011110010011000111101101001010111011110101011000010000100010001111", -4 , 1  , "0.1231235123554"}
 ,{ "11111111100011011111111110001100000011111111111111111000111000001111111111110000000000011111111110000000001111111110000001111"   , 65 , 1 , "73658621713667056515.99902391387240466018304640982705677743069827556610107421875"}
 ,{ "101111111111010"                                                                                                                 , -1 , 1 , "0.74981689453125" }
 ,{ "100000000000000000000001100000000000000000000001"                                                                                , 0 , 1  , "1.00000017881394143159923260100185871124267578125"}
 ,{ "10000000000000000000000000000000000000000000100000"                                                                              , 0 , 1  , "1.00000000000005684341886080801486968994140625"}
 ,{ "100000000000000000000000000000000000000000000100000"                                                                             , 0 , 1  , "1.000000000000028421709430404007434844970703125"}
 ,{ "1000000000000000000000000000000000000000000000100000"                                                                            , 0 , 1  , "1.0000000000000142108547152020037174224853515625"}
 ,{ "10000000000000000000000000000000000000000000000100000"                                                                           , 0 , 1  , "1.00000000000000710542735760100185871124267578125"}
 ,{ "100000000000000000000000000000000000000000000000100000"                                                                          , 0 , 1  , "1.000000000000003552713678800500929355621337890625"}
 ,{ "1000000000000000000000000000000000000000000000000100000"                                                                         , 0 , 1  , "1.0000000000000017763568394002504646778106689453125"}
 ,{ "1000000000000000000000000000000000000000000000000000110000000000000000000000000000000000000000000000000001"                      , 0 , 1  , "1.000000000000000333066907387546986778992788583865012866517665087069677287701097156968899071216583251953125"}
 ,{ "10000000000000000000000000000000000000000000000000000000000000011000000000000000000000000000000000000000000000000000000000000001", 0 , 1  ,"1.0000000000000000001626303258728256651069953918846026942537108701861112283890933277838604376075437585313920862972736358642578125" }
 ,{ "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001", 0 , 1  ,"1.000000000000000000000000000000000288889491658085377958396691387739116326746000669415157944667127073181392521417643788955012265195294587906700478146794987008" }
 ,{ "1000000000000000000000011000000000000000000001"      /* These are a bunch of very nasty cases for float */                       , 0 , 1  ,"1.000000178813962747881305404007434844970703125" }
 ,{ "10000000000000000000000110000000000000000000011"                                                                                 , 0 , 1  ,"1.0000001788139769587360206060111522674560546875" }
 ,{ "100000000000000000000001100000000000000000000111"                                                                                , 0 , 1  ,"1.00000017881398406416337820701301097869873046875" }
 ,{ "1000000000000000000000011000000000000000000001111"                                                                               , 0 , 1  ,"1.000000178813987616877057007513940334320068359375" }
 ,{ "10000000000000000000000110000000000000000000011111"                                                                              , 0 , 1  ,"1.0000001788139893932338964077644050121307373046875" }
 ,{ "100000000000000000000001100000000000000000000111111"                                                                             , 0 , 1  ,"1.00000017881399028141231610788963735103607177734375" }
 ,{ "1000000000000000000000010000000000000000000001"                                                                                  , 0 , 1  ,"1.000000119209317972490680404007434844970703125" }
 ,{ "10000000000000000000000100000000000000000000011"                                                                                 , 0 , 1  ,"1.0000001192093321833453956060111522674560546875" }
 ,{ "100000000000000000000001000000000000000000000111"                                                                                , 0 , 1  ,"1.00000011920933928877275320701301097869873046875" }
 ,{ "1000000000000000000000010000000000000000000001111"                                                                               , 0 , 1  ,"1.000000119209342841486432007513940334320068359375" }
 ,{ "10000000000000000000000100000000000000000000011111"                                                                              , 0 , 1  ,"1.0000001192093446178432714077644050121307373046875" }
 ,{ "100000000000000000000001000000000000000000000111111"                                                                             , 0 , 1  ,"1.00000011920934550602169110788963735103607177734375" }
 ,{ "1000000000000000000000010000000000000000000001"                                                                                  , 0 , 1  ,"1.000000119209317972490680404007434844970703125" }
 ,{ "10000000000000000000000100000000000000000000001"                                                                                 , 0 , 1  ,"1.0000001192093037616359652020037174224853515625" }
 ,{ "100000000000000000000001000000000000000000000001"                                                                                , 0 , 1  ,"1.00000011920929665620860760100185871124267578125" }
 ,{ "1000000000000000000000010000000000000000000000001"                                                                               , 0 , 1  ,"1.000000119209293103494928800500929355621337890625" }
 ,{ "10000000000000000000000100000000000000000000000001"                                                                              , 0 , 1  ,"1.0000001192092913271380894002504646778106689453125" }
 ,{ "100000000000000000000001000000000000000000000000001"                                                                             , 0 , 1  ,"1.00000011920929043895966970012523233890533447265625" }
 ,{ "1000000000000000000000000000000000000000000000000000110000000000000000000000000000000000000000000000000111"                      , 0 , 1  ,"1.000000000000000333066907387547134690412517523578527565623655609487741013907680098782293498516082763671875" }
 ,{ "10000000000000000000000000000000000000000000000000000000000000011000000000000000000000000000000000000000000000000000000000000111", 0 , 1  ,"1.0000000000000000001626303258728256651422602224092713194927729663027785987236532944870230632528063097197446040809154510498046875" }
 ,{ "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000110000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111", 0 , 1  ,"1.000000000000000000000000000000000288889491658085377958396691387739227602930521292056729896724973502366793548361006522685085856367062115346903347027564905997" }
// TODO: These should be passing once infities are handled properly in the rest of the code. Or we need to reason why these shouldn't be tested.
// ,{ "11111111100011011111111110001100000011111111111111111000111000001111111111110000000000011111111110000000001111111110000001111"   ,1407,1  , "7.07095004791213209137407618364459278413421454874042247410492385622373956879713960311588804604245728321440648803023224236513586176837484939909893244653903501e+423"}
// ,{ "10110110001000110100010000000011110011010001011100000010110110001000011011111110111001101101001111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111101000000110101011111110001100101110100101110000110111000000111111101111011100110000110110000000100101100011011111001110001001100010010110000010110011000010010110101110100101000111001000000110000111011111110101111011101100000110100011000111010101101001111", 1407 , 1  , "5.0395749966458598419365441242084052981209828021829231181382593274122924204e+423" }
}};

class LoopError{};

namespace boost { namespace multiprecision {
namespace backends {
template <typename T> int sign(T val) { return (T(0) < val) - (val < T(0)); }

template <typename Rr>
inline cpp_double_fp_backend<Rr> pow(const cpp_double_fp_backend<Rr>& a, int exp)
{
   return a.pown(a,exp);
}

template <typename Rr, typename Exp>
inline cpp_double_fp_backend<Rr> frexp(const cpp_double_fp_backend<Rr>& a, Exp* b)
{
   cpp_double_fp_backend<Rr> ret(0);
   eval_frexp(ret, a, b);
   return ret;
}

template <typename Rr> void print_compound_number(const std::string& prefix, const Rr& arg) {};
template <typename Rr> void print_compound_number(const std::string& prefix, const cpp_double_fp_backend<Rr>& arg);
template <typename Rr> void print_compound_number(const std::string& prefix, const cpp_quad_fp_backend<Rr>& arg);

template<typename R>
struct double_or_quad_traits {
   static constexpr auto guard_bits                = 0;
   static constexpr auto double_or_quad_multiplier = 1;
   using underlying_type                           = R;
};

template<typename R>
struct double_or_quad_traits<cpp_double_fp_backend<R>> {
   static constexpr auto guard_bits                = 3;
   static constexpr auto double_or_quad_multiplier = 2;
   using underlying_type                           = R;
};

template<typename R>
struct double_or_quad_traits<cpp_quad_fp_backend<R>> {
   static constexpr auto guard_bits                = 4;
   static constexpr auto double_or_quad_multiplier = 4;
   using underlying_type                           = R;
};

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
      int guard = double_or_quad_traits<Rr>::guard_bits;
      bits.resize(std::numeric_limits<Rr>::digits + guard, 0);

      while ((norm != Rr(0)) && ((pos - ex) < (int) bits.size())) {
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
////////////////////// Helper print / debug functions - START (in the moddle of the class, sorry ;) //////////////////////
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
      std::cout << "exp : " << std::setw(6) << exp << ", bits : " << (sig==1?"+":"-") << bit_str() << std::endl;
   }
   int short_print_shifted(int max_exp = 0, bool skip_exp = true)
   {
      std::string st = bit_str();

      const auto len = st.size();
      (void) len;

      if(skip_exp) {
         std::cout << "exp : " << std::setw(6) << exp << ", bits : "  << std::right <<                               (sig==1?"+":"-") << bit_str() << std::endl;
      } else {
      // The std::setw makes sure that the bits are printed in properly shifted form - in columns aligned with the first part.
         std::cout << "exp : " << std::setw(6) << exp << ", bits : "  << std::right << std::setw(max_exp - exp+1) << (sig==1?"+":"-") << bit_str() << std::endl;
      }
      return exp;
   }
};

template <typename Rr> void print_compound_number(const std::string& prefix, const boost::multiprecision::backends::cpp_double_fp_backend<Rr>& arg) {
   std::cout    << std::left << std::setw(15) << prefix+".first "  << " = " << std::right; auto ex1 = DecomposedReal(arg.crep().first).short_print_shifted();
   if(arg.crep().second != 0) {
      std::cout << std::left << std::setw(15) << prefix+".second " << " = " << std::right;            DecomposedReal(arg.crep().second).short_print_shifted(ex1,false);
   }
}

// not tested
template <typename Rr> void print_compound_number(const std::string& prefix, const boost::multiprecision::backends::cpp_quad_fp_backend<Rr>& arg) {
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

void sometimes_print_bit_positions(int digs, int mult)
{
   static int sometimes = 0;
   if(sometimes++ % 2 == 0) {
      std::cout << std::setw(40) << " "; // these spaces are synchronised with short_print_shifted(…) and short_print(…)
      for(int i = 0 ; i < mult ; i++) {
         std::cout << std::setw(digs) << digs*(i+1);
      }
      std::cout << std::endl;
      std::cout << std::setw(40) << " "; // these spaces are synchronised with short_print_shifted(…) and short_print(…)
      for(int i = 0 ; i < mult ; i++) {
         std::cout << std::setw(digs) << "|";
      }
      std::cout << std::endl;
   }
}
////////////////////// Helper print / debug functions - END //////////////////////

template <typename Rr> int test_number_decomposition(const Rr& arg, bool verbose)
{
   int errors = 0;
   DecomposedReal d(arg);
   auto rebuilt = d.rebuild<Rr>();

   // FIXME: should these normalize calls be here, ot at the end of some functions inside cpp_double_fp_backend.hpp ?
   Rr::arithmetic::normalize(rebuilt.rep());
   Rr::arithmetic::extra_normalize(rebuilt.rep());

   auto diff = (arg - rebuilt);

   static_assert(std::is_same<decltype(diff   ), Rr >::value,"");
   static_assert(std::is_same<decltype(rebuilt), Rr >::value,"");

   if(verbose) {
      sometimes_print_bit_positions(std::numeric_limits<typename double_or_quad_traits<Rr>::underlying_type>::digits , double_or_quad_traits<Rr>::double_or_quad_multiplier);
      std::cout << "original bits   = "; d.short_print();
      print_compound_number("arg",arg);
      print_compound_number("rebuilt",rebuilt);
   }

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
int try_number(R& ref, Arg str, bool verbose) {
   auto z = R(str);
   if(verbose) {
      std::cout  << std::endl << std::setprecision(100000) << "Testing number  : " << str << std::endl;
      std::cout << "With type " << boost::core::demangle(typeid(decltype(z)).name()) << std::endl;
   }

   int errors = 0;
   try {
      // FIXME: should these normalize calls be here, ot at the end of some functions inside cpp_double_fp_backend.hpp ?
      R::arithmetic::normalize(z.rep());
      R::arithmetic::extra_normalize(z.rep());

      errors += test_number_decomposition(z, verbose);
   } catch(const LoopError&) {
      std::cout << "** ERROR in decomposition **" << std::endl;
      errors++;
   }

   if(std::is_same<R, typename std::decay<Arg>::type>::value) {
      // 1. stores in ref the number constructed from bits
      ref = z;
   } else {
      // 2. compares with ref if Arg was string.
      R   str_to_bin_error = z - ref;
      int exp1             = 0;
      int exp2             = 0;
      frexp(z , &exp1);
      str_to_bin_error     = frexp(str_to_bin_error , &exp2);

      if(verbose) {
         std::cout << "--> " << str_to_bin_error  << " , exp1 =  " << exp1 << " , exp2 = " << exp2 << std::endl;
      }
      double ulp_error = 0;
      if(str_to_bin_error != R(0)) {
         ulp_error = pow(2.0,-((exp1 - exp2) - std::numeric_limits<R>::digits));
         if(verbose) {
            std::cout << "ULP error is : " << ulp_error  << std::endl;
         }
      }
      if( ulp_error > 2)
/* TODO */ //if(std::abs(boost::math::float_distance(z , ref)) > 2)
      {
         errors++;
         std::cout << "** ERROR between string <--> binary **" << str_to_bin_error  << ", exp1 =" << exp1 << ", exp2 =" << exp2 << std::endl;
      }
   }

   return errors;
}

template<typename R>
int test() {
   int errors = 0;

   // this ref number is used for comparison between number constructed from string, and same number constructed from bits.
   R ref = 0;

   bool verbose = true; // print all stuff only on the first number from data.
   for(const auto& dat : data) {
      using std::get;
      errors += try_number<R>(ref,fromBits<R>( get<0>(dat) , get<1>(dat) , get<2>(dat) ) , verbose);
      errors += try_number<R>(ref, get<3>(dat) , verbose);
      verbose = false;
   }

   return errors;
}

template<class R> using double_float = boost::multiprecision::backends::cpp_double_fp_backend<R>;
template<class R> using quad_float   = boost::multiprecision::backends::cpp_quad_fp_backend<R>;

int main()
{
   int errors = 0;

   errors += test<double_float<float>>();
   errors += test<double_float<double>>();
   errors += test<double_float<long double>>();
#ifdef BOOST_MATH_USE_FLOAT128
   errors += test<double_float<boost::multiprecision::float128>>();
#endif

// TODO: soon we should be able to test quad_float also :)
/*
   errors += test<quad_float<float>>();
   errors += test<quad_float<double>>();
   errors += test<quad_float<long double>>();
#ifdef BOOST_MATH_USE_FLOAT128
   errors += test<quad_float<boost::multiprecision::float128>>();
#endif
*/

   std::cout << "Total number of errors : " << errors << std::endl << std::endl;

   return (errors != 0);
}