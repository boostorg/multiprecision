///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

//
// Compare arithmetic results using fixed_int to GMP results.
//

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

//
// This ensures all our code gets tested, even though it may
// not be the fastest configuration in normal use:
//
#define BOOST_MP_USE_LIMB_SHIFT

#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/timer.hpp>
#include "test.hpp"

#ifdef _MSC_VER
#pragma warning(disable : 4127) //  Conditional expression is constant
#endif

#ifndef TEST
#  define TEST 0
#endif

template <class T>
T generate_random(unsigned bits_wanted)
{
   static boost::random::mt19937               gen;
   typedef boost::random::mt19937::result_type random_type;

   T        max_val;
   unsigned digits;
   if (std::numeric_limits<T>::is_bounded && (bits_wanted == (unsigned)std::numeric_limits<T>::digits))
   {
      max_val = (std::numeric_limits<T>::max)();
      digits  = std::numeric_limits<T>::digits;
   }
   else
   {
      max_val = T(1) << bits_wanted;
      digits  = bits_wanted;
   }

   unsigned bits_per_r_val = std::numeric_limits<random_type>::digits - 1;
   while ((random_type(1) << bits_per_r_val) > (gen.max)())
      --bits_per_r_val;

   unsigned terms_needed = digits / bits_per_r_val + 1;

   T val = 0;
   for (unsigned i = 0; i < terms_needed; ++i)
   {
      val *= (gen.max)();
      val += gen();
   }
   val %= max_val;
   return val;
}

template <class T>
struct is_checked_cpp_int : public boost::mpl::false_
{};
template <unsigned MinBits, unsigned MaxBits, boost::multiprecision::cpp_integer_type SignType, class Allocator, boost::multiprecision::expression_template_option ET>
struct is_checked_cpp_int<boost::multiprecision::number<boost::multiprecision::cpp_int_backend<MinBits, MaxBits, SignType, boost::multiprecision::checked, Allocator>, ET> > : public boost::mpl::true_
{};


template <class N>
typename boost::enable_if_c<boost::multiprecision::backends::is_fixed_precision<typename N::backend_type>::value && !is_checked_cpp_int<N>::value>::type test(const N&)
{
   using namespace boost::multiprecision;

   static unsigned last_error_count = 0;

   boost::timer tim;

   do
   {
      // Test modular arithmetic by filling all the bits of our test type:
      mpz_int f = generate_random<mpz_int>(std::numeric_limits<N>::digits + 2);
      mpz_int g = generate_random<mpz_int>(std::numeric_limits<N>::digits + 2);
      mpz_int mask(1);
      mask <<= std::numeric_limits<N>::digits;
      --mask;
      f &= mask;
      g &= mask;
      mpz_int r = (f * g) & mask;

      N f1(f);
      N g1(g);
      N r1 = f1 * g1;
      BOOST_CHECK_EQUAL(r1.str(), r.str());

      if (last_error_count != (unsigned)boost::detail::test_errors())
      {
         last_error_count = boost::detail::test_errors();
         std::cout << std::hex << std::showbase;
         std::cout << f1 << std::endl;
         std::cout << f << std::endl;
         std::cout << g1 << std::endl;
         std::cout << g << std::endl;
         std::cout << r1 << std::endl;
         std::cout << r << std::endl;
      }

      static boost::random::mt19937             gen;
      boost::random::uniform_int_distribution<> d(12, std::numeric_limits<N>::digits);
      f = generate_random<mpz_int>(d(gen));
      g = generate_random<mpz_int>(d(gen));
      r = (f * g) & mask;

      f1 = N(f);
      g1 = N(g);
      r1 = f1 * g1;
      BOOST_CHECK_EQUAL(r1.str(), r.str());

      if (last_error_count != (unsigned)boost::detail::test_errors())
      {
         last_error_count = boost::detail::test_errors();
         std::cout << std::hex << std::showbase;
         std::cout << f1 << std::endl;
         std::cout << f << std::endl;
         std::cout << g1 << std::endl;
         std::cout << g << std::endl;
         std::cout << r1 << std::endl;
         std::cout << r << std::endl;
      }

      #ifndef CI_SUPPRESS_KNOWN_ISSUES
      if (tim.elapsed() > 200)
#else
      if (tim.elapsed() > 25)
#endif
      {
         std::cout << "Timeout reached, aborting tests now....\n";
         break;
      }

   } while (true);
}
template <class N>
typename boost::disable_if_c<boost::multiprecision::backends::is_fixed_precision<typename N::backend_type>::value && !is_checked_cpp_int<N>::value>::type test(const N&)
{
   using namespace boost::multiprecision;

   static unsigned last_error_count = 0;

   boost::timer tim;

   do
   {
      // Test modular arithmetic by filling all the bits of our test type:
      static boost::random::mt19937             gen;
      boost::random::uniform_int_distribution<> d(12, 10000);
      mpz_int                                   f = generate_random<mpz_int>(d(gen));
      mpz_int                                   g = generate_random<mpz_int>(d(gen));
      mpz_int                                   r = f * g;

      N f1(f);
      N g1(g);
      N r1 = f1 * g1;
      BOOST_CHECK_EQUAL(r1.str(), r.str());

      if (last_error_count != (unsigned)boost::detail::test_errors())
      {
         last_error_count = boost::detail::test_errors();
         std::cout << std::hex << std::showbase;
         std::cout << f1 << std::endl;
         std::cout << f << std::endl;
         std::cout << g1 << std::endl;
         std::cout << g << std::endl;
         std::cout << r1 << std::endl;
         std::cout << r << std::endl;
      }

#ifndef CI_SUPPRESS_KNOWN_ISSUES
      if (tim.elapsed() > 200)
#else
      if (tim.elapsed() > 25)
#endif
      {
         std::cout << "Timeout reached, aborting tests now....\n";
         break;
      }

   } while (true);
}

int main()
{
   using namespace boost::multiprecision;

   #if (TEST == 1) || (TEST == 0)
   test(cpp_int());
   #endif
#if (TEST == 2) || (TEST == 0)
   test(number<cpp_int_backend<8192, 8192, signed_magnitude, unchecked, void> >());
#endif
#if (TEST == 3) || (TEST == 0)
   test(number<cpp_int_backend<8192, 8192, signed_magnitude, unchecked, std::allocator<char> > >());
#endif
#if (TEST == 4) || (TEST == 0)
   test(number<cpp_int_backend<8192, 8192, unsigned_magnitude, checked> >());
#endif
   return boost::report_errors();
}
