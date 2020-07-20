// Copyright Paul A. Bristow 1998-2006, 2020. pbristow@hetp.u-net.com

// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// Test to show any errors in round-tripping of loopback, writing out a binary value
// as decimal digit string and reading back into binary format,
// using a variety of floating-point types, including Boost.Multiprecision types.

// All tests use std::numeric_limimts<>::max_digits10 decimal digits,
// enough to ensure that all possibly significant digits are used.
// See for details:
// http://www2.open-std.org/JTC1/SC22/WG21/docs/papers/2005/n1822.pdf
// New code for Boost.Multiprecision 17Jun2020 @jzmaddock with one less digit than previously for all multipreciison types.
// https://github.com/boostorg/multiprecision/pull/249
//
// This code is derived from code used to discover faults in round-trip conversions
// https://www.exploringbinary.com/incorrect-round-trip-conversions-in-visual-c-plus-plus/
// Riock Regan Incorrect Round-Trip Conversions in Visual C++ May 2013
// https://www.exploringbinary.com/gcc-avoids-double-rounding-errors-with-round-to-odd/#more-487

// The original value causing trouble with MSVC using serialization was 0.00019075645054089487;
// wrote 0.0019075645054089487, and read back as 0.0019075645054089489
// a increase of just 1 bit.
// This discrepancy was discovered because the hash of a serialized database
// did not match the hash of restored data.
// The wrong inputs were only found in a very narrow range of values:
// approximately 0.0001 to 0.004, with exponent values of 3f2 to 3f6
// and probably every third value of significand (tested using function nextafter),
// but all C++ compilers standard libraries MSVC, GCC and Clang now produce
// correctly round-trip the fundamental floating-point types and float128.
// The MSVC bug was fixed in 2018 by using the Ryu algorithm and now works correctly.
//
// GCC was recently fixed so that its decimal to floating - point conversions are done correctly;
// it now calls the MPFR function mpfr_strtofr() instead of using its own algorithm.
// Decimal to floating-point conversions that are inexact are rounded,
// using round-to-nearest (ties-to-even) rounding.
// But two successive roundings to nearest can result in a double rounding error.
// Specifically, this happens when rounding to the intermediate precision
// creates a midpoint (a tie) between two floating-point numbers in the target precision.

// https://github.com/ulfjack/ryu  Ryu & Ryu Printf, Ulf Adams
// The Ryu project contains routines to convert IEEE-754 floating-point numbers to decimal strings
// using shortest, fixed %f, and scientific %e formatting.
// Ryu generates the shortest decimal representation of a
// floating-point binary number that maintains round-trip safety.
// Reassuringly, the PLDI'18 paper includes a complete correctness proof of the algorithm:
// https://dl.acm.org/citation.cfm?doid=3296979.3192369

// Previous faulty algorithms caused trouble using Boost.Lexical_cast and Boost.Serialization,
// two very widely used features of Boost, both within Boost, and very commonly in user code.

// The crude first test below creates a random selection of values of type T
// by using Boost.Random to fill both the significand and exponent.
// Only positive, finite, normalized values are used.

// A second test allowed estimation of the range and frequency of errors
// using nextafter, showing that about 1/3 third of significand values
// in the range 0.0001 to 0.003894 were input wrongly.
// The sample output below was with MSVC 8.0 and is similar in release and debug.

// Murphy's law, of course, applies as numbers in this range are very probable!

// It is, of course, not possible to test ALL values even in the range of doubles
// within the entire lifetime of most in computing ;-)
// However a prolonged round-trip test using random values gives
// good reassurance that the algorithms are extremely probably correct.

// (Unlike floats where an exhaustive test is possible - see
// http://lab.msdn.microsoft.com/productfeedback/viewfeedback.aspx?feedbackid=7bf2f26d-171f-41fe-be05-4169a54eef9e
// aka http://tinyurl.com/mpk72
// happily for MSVC.net 8.0 final, and later, ALL possible float values are now correct).

// 15Jun2020 Original works OK with VS 16.7.0 Preview 3,
// but is not portable to other compilers because using _finite and _nextafter.

// This 2020 version of loopback.cpp avoids this by using Boost.Math and Boost.Multiprecision functions,
// and allows testing of some multiprecision types as well as the three fundamental floating-point types.
// C++11 or higher is required.
// Tests so far indicate that the latest version of three compilers MSVC, GCC and Clang,
// and their respective standard libraries are correct.

// #define BOOST_MATH_ROUNDTRIP_SHOW to enable diagnostic show of values.
// Caution this may produce voluminous output if the count is big.
//  #define BOOST_MATH_ROUNDTRIP_SHOW

#include <boost/math/special_functions/next.hpp>  // for float_next and float_distance.
#include <boost/multiprecision/cpp_bin_float.hpp> // For cpp_bin_float_quad.
#include <boost/multiprecision/cpp_int.hpp>       // For cpp_int.
#include <boost/multiprecision/detail/number_base.hpp>
// For boost::multiprecision::number_category<cpp_bin_float_quad>::value == boost::multiprecision::number_kind_floating_point.
#include <boost/random/mersenne_twister.hpp> // For boost::random::mt19937.
#include <boost/random/uniform_int.hpp>      // For uniform_int_distribution.

#include <iostream>
//using std::cout;
//using std::endl;
//using std::ends;
//using std::hex;
//using std::dec;
//using std::showpoint;
//using std::scientific;
//using std::std::ios_base;
#include <string>
//using std::string;
#include <sstream> // std::stringstream
#include <iomanip>
//using std::setw;
// using std::setprecision;
#include <limits>
// using std::numeric_limits;
#include <type_traits> // for type_traits.
#include <typeinfo>    // for typeid to get a name of the type
#include <random>      // For random_device for seeder.

#ifdef BOOST_HAS_FLOAT128
// Use native 128 quad (software).
BOOST_PRAGMA_MESSAGE("Using native 128-bit quad type.")
#include <boost/multiprecision/float128.hpp> // Native 128-bit float not available for MSVC or Clang (yet).
using boost::multiprecision::float128;
#endif

// Get a name of a type as a C string, if possible.
template <class T>
inline const char* name_of()
{
#ifndef BOOST_NO_RTTI
   return typeid(T).name();
#else
   return "unknown. ";
#endif
} // name_of

// Write test_count random value to a decimal digit string, and read back into read_value
// and count any when output_value != read_value
//! \returns Number of values for which output_value != read_value.
//! \tparam T Floating-point type to test roundtrip (loopback).
template <typename T>
int roundtrips(int test_count)
{
   BOOST_STATIC_ASSERT_MSG(boost::multiprecision::number_category<T>::value == boost::multiprecision::number_kind_floating_point, "Type is not floating-point.");
   // includes fundamental floating-point types, float, double and long double.
   std::cout << "Type is " << name_of<T>() << std::endl;
   std::cout << "max_digits10 = " << std::numeric_limits<T>::max_digits10 << std::endl;

   using std::random_device;
   random_device seeder;
   // Use seeder to get a different set of random values each time .exe is run.
   static boost::random::mt19937 generator(seeder()); // uint32_t

   // See \boost\libs\multiprecision\test\test_convert_from_cpp_bin_float.cpp
   // Our aim is to fill up the significand/mantissa of T with random bits.
   // The main loop simply multiplies by the largest random value
   // we produce and then adds the next random value.  It stops
   // when the next random value is too small to change the result,
   // and so "val" and "prev_val" end up with the same value.
   //
   // We start with an arbitrary value in "prev_val" - anything you
   // like as long as it's a value that our generator can not produce.
   //
   // At the end we ditch the current exponent,
   // and replace with our own randomly generated exponent.

   typedef int exponent_type;
   // Use std::numeric_limits<T>::max_exponent and min_exponent to cover the full range of exponents -
   // but this means that values near zero are almost untested for multiprecision types.
   //const exponent_type max_exp = std::numeric_limits<T>::max_exponent;
   //const exponent_type min_exp = std::numeric_limits<T>::min_exponent;

   // Or concentrate on the most used range of values centered around zero?
   exponent_type max_exp = +20;
   exponent_type min_exp = -20;
   //   static boost::random::uniform_int_distribution<exponent_type> ui(-20, +20); used by JM.
   // +20 to -20 limits range of randoms to e-6 to e+6  (multiply 20 by log2(10) = 0.301 to get 6)
   // This is also much quicker?
   static boost::random::uniform_int_distribution<exponent_type> uniform_int(min_exp, max_exp); //

   int fail_count = 0;
   for (int i = 0; i < test_count; i++)
   {
      T val      = static_cast<T>(generator());
      T prev_val = -1; // Can't be generated by the chosen generator.
      while (val != prev_val)
      {
         val *= (generator.max)();
         prev_val = val;
         val += generator();
      }
      // return val; // about 1e+18
      typedef int   exponent_type;
      exponent_type e;      // Original exponent discarded and new random exponent used instead.
      val = frexp(val, &e); // val = only significand part.

      exponent_type random_exponent = uniform_int(generator);
#ifdef BOOST_MATH_ROUNDTRIP_SHOW
      std::cout << "Random significand = " << val << ", exponent = " << e << ", random exponent " << random_exponent;
#endif
      T result = ldexp(val, random_exponent); // Add new random exponent to random significand for final random value.
#ifdef BOOST_MATH_ROUNDTRIP_SHOW
      std::cout << ", result " << std::scientific << result << std::defaultfloat << std::endl;
      // Easier to judge randomness of exponent using scientific format.
#endif

      exponent_type result_exponent;
      // Check result_exponent isn't too big or small for the type T.
      // Should not be necessary and no failure detected so far.
      T result_significand = frexp(result, &result_exponent);
      //if ((result_exponent > std::numeric_limits<T>::max_exponent) //
      //   || (result_exponent < std::numeric_limits<T>::min_exponent))
      //{
      //   std::cout << "result_significand = " << result_significand
      //      << ", result_exponent " << result_exponent
      //      << std::endl;
      //}

      T output_value = result;

#ifdef BOOST_MATH_ROUNDTRIP_SHOW
      // Would repeat the output from generate_random.
      //   std::cout << "Random value = " << output_value << std::endl;
#endif // BOOST_MATH_ROUNDTRIP_SHOW
      std::stringstream ss;
      ss.precision(std::numeric_limits<T>::max_digits10); // All potentially significant digits, including
      ss << std::showpoint << std::endl;                  // any potentially significant trailing zeros.
      // For double 17 is the maximum number of decimal digits that are potentially significant.
      // ss << << output_value; // Uses std::defaultfloat format.
      //   ss << std::defaultfloat << output_value; //
      //ss << std::scientific << output_value; // std::scientific also seems to work,
      ss << std::hexfloat << output_value; //  Fails on GCC 10.0.1
      T read_value;
      ss >> read_value;

      using boost::math::float_distance;
      //if (read_value == output_value)
      if (float_distance(read_value, output_value) != 0) // exactly equal for all types.
      {
         std::cout << "Written  : " << std::showpoint << std::defaultfloat << output_value << " == " << std::hexfloat << output_value << std::endl;
         // std::cout << stream.str() << std::endl; == Written.
         std::cout << "Readback : " << std::showpoint << std::defaultfloat << read_value << " == " << std::hexfloat << read_value << std::endl;
         fail_count++;
      }
   } // for test_count
   if (fail_count == 0)
   {
      std::cout << test_count << " values round_trip OK." << std::endl;
   }
   else
   {
      std::cout << fail_count << " values failed out of " << test_count << " values round_tripped." << std::endl;
   }
   return fail_count;
} // template <typename T> int roundtrips(int test_count)

int main()
{

   using boost::multiprecision::cpp_bin_float_double_extended;
   using boost::multiprecision::cpp_bin_float_oct;
   using boost::multiprecision::cpp_bin_float_quad;

   try
   {
      //   std::cout << generate_random<cpp_bin_float_quad>() << std::endl;
      std::cout << std::setprecision(std::numeric_limits<cpp_bin_float_quad>::max_digits10);
      // Show all potentially significant decimal digits max_digits10 (17 for double).
      std::cout << std::showpoint << std::endl; // Also show potentially significant trailing zeros.

      //   int count = 100000000; // execution time : 10270.714 s - a few hours.
      //  int count = 100000; // execution time : 10.339 s
      int count = 100; // execution time : 1 sec

      roundtrips<float>(count);
      roundtrips<double>(count);
      if (std::numeric_limits<long double>::digits > std::numeric_limits<double>::digits)
      { // Worth also checking native long double
         roundtrips<long double>(count);
      }
      else
      { // Emulate 80-bit long double using multiprecision type.
         roundtrips<cpp_bin_float_double_extended>(count);
      }
      roundtrips<cpp_bin_float_quad>(count); // 128-bit
      roundtrips<cpp_bin_float_oct>(count);  // 256-bit
#ifdef BOOST_HAS_FLOAT128                    // Use native 128 quad (software).
      roundtrips<float128>(count);           // 128-bit using GCC or Intel software.
#endif
      return 0;
   }
   catch (std::exception& ex)
   {
      std::cout << "Exception: " << ex.what();
   }

   return 0;
} // int main()

/*

Output:

MSVC 16.7.0 Preview 3

   roundtrip.cpp
   Generating code
   0 of 1771 functions ( 0.0%) were compiled, the rest were copied from previous compilation.
      0 functions were new in current compilation
      0 functions had inline decision re-evaluated but remain unchanged
   Finished generating code
   roundtrip.vcxproj -> I:\Cpp\math\x64\Release\roundtrip.exe
   Autorun "I:\Cpp\math\x64\Release\roundtrip.exe"

   Type is float
   max_digits10 = 9
   100 values round_trip OK.
   Type is double
   max_digits10 = 17
   100 values round_trip OK.
   Type is class boost::multiprecision::number<class boost::multiprecision::backends::cpp_bin_float<64,2,void,short,-16382,16383>,0>
   max_digits10 = 21
   100 values round_trip OK.
   Type is class boost::multiprecision::number<class boost::multiprecision::backends::cpp_bin_float<113,2,void,short,-16382,16383>,0>
   max_digits10 = 36
   100 values round_trip OK.
   Type is class boost::multiprecision::number<class boost::multiprecision::backends::cpp_bin_float<237,2,void,int,-262142,262143>,0>
   max_digits10 = 73
   100 values round_trip OK.


   GCC 10.0.1


Type is f
max_digits10 = 9
100 values round_trip OK.
Type is d
max_digits10 = 17
100 values round_trip OK.
Type is e
max_digits10 = 21
100 values round_trip OK.
Type is N5boost14multiprecision6numberINS0_8backends13cpp_bin_floatILj113ELNS2_15digit_base_typeE2EvsLsn16382ELs16383EEELNS0_26expression_template_optionE0EEE
max_digits10 = 36
100 values round_trip OK.
Type is N5boost14multiprecision6numberINS0_8backends13cpp_bin_floatILj237ELNS2_15digit_base_typeE2EviLin262142ELi262143EEELNS0_26expression_template_optionE0EEE
max_digits10 = 73
100 values round_trip OK.
Type is N5boost14multiprecision6numberINS0_8backends16float128_backendELNS0_26expression_template_optionE0EEE
max_digits10 = 36
100 values round_trip OK.

Process returned 0 (0x0)   execution time : 0.032 s



*/
