///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

//
// Compare arithmetic results using fixed_int to GMP results.
//

#ifdef _MSC_VER
#  define _SCL_SECURE_NO_WARNINGS
#endif

#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/timer.hpp>
#include "test.hpp"

template <class T>
T generate_random(unsigned bits_wanted)
{
   static boost::random::mt19937 gen;
   typedef boost::random::mt19937::result_type random_type;

   T max_val;
   unsigned digits;
   if(std::numeric_limits<T>::is_bounded && (bits_wanted == std::numeric_limits<T>::digits))
   {
      max_val = (std::numeric_limits<T>::max)();
      digits = std::numeric_limits<T>::digits;
   }
   else
   {
      max_val = T(1) << bits_wanted;
      digits = bits_wanted;
   }

   unsigned bits_per_r_val = std::numeric_limits<random_type>::digits - 1;
   while((random_type(1) << bits_per_r_val) > (gen.max)()) --bits_per_r_val;

   unsigned terms_needed = digits / bits_per_r_val + 1;

   T val = 0;
   for(unsigned i = 0; i < terms_needed; ++i)
   {
      val *= (gen.max)();
      val += gen();
   }
   val %= max_val;
   return val;
}

template <class Number>
void test()
{
   using namespace boost::multiprecision;
   typedef Number test_type;
   unsigned last_error_count = 0;
   boost::timer tim;

   for(int i = 0; i < 10000; ++i)
   {
      mpz_int a = generate_random<mpz_int>(1000);
      mpz_int b = generate_random<mpz_int>(512);
      mpz_int c = generate_random<mpz_int>(256);
      mpz_int d = generate_random<mpz_int>(32);

      int si = d.convert_to<int>();
      unsigned ui = si;

      test_type a1 = a.str();
      test_type b1 = b.str();
      test_type c1 = c.str();
      test_type d1 = d.str();

      BOOST_CHECK_EQUAL(a.str(), a1.str());
      BOOST_CHECK_EQUAL(b.str(), b1.str());
      BOOST_CHECK_EQUAL(c.str(), c1.str());
      BOOST_CHECK_EQUAL(d.str(), d1.str());
      BOOST_CHECK_EQUAL(mpz_int(a+b).str(), test_type(a1 + b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)+=b).str(), (test_type(a1) += b1).str());
      BOOST_CHECK_EQUAL((mpz_int(b)+=a).str(), (test_type(b1) += a1).str());
      BOOST_CHECK_EQUAL(mpz_int(a-b).str(), test_type(a1 - b1).str());
      BOOST_CHECK_EQUAL(mpz_int(mpz_int(-a)+b).str(), test_type(test_type(-a1) + b1).str());
      BOOST_CHECK_EQUAL(mpz_int(mpz_int(-a)-b).str(), test_type(test_type(-a1) - b1).str());
      BOOST_CHECK_EQUAL(mpz_int(c * d).str(), test_type(c1 * d1).str());
      BOOST_CHECK_EQUAL((mpz_int(c)*=d).str(), (test_type(c1) *= d1).str());
      BOOST_CHECK_EQUAL((mpz_int(d)*=c).str(), (test_type(d1) *= c1).str());
      BOOST_CHECK_EQUAL(mpz_int(c * -d).str(), test_type(c1 * -d1).str());
      BOOST_CHECK_EQUAL(mpz_int(-c * d).str(), test_type(-c1 * d1).str());
      BOOST_CHECK_EQUAL((mpz_int(c)*=-d).str(), (test_type(c1) *= -d1).str());
      BOOST_CHECK_EQUAL((mpz_int(-d)*=c).str(), (test_type(-d1) *= c1).str());
      BOOST_CHECK_EQUAL(mpz_int(b * c).str(), test_type(b1 * c1).str());
      BOOST_CHECK_EQUAL(mpz_int(a / b).str(), test_type(a1 / b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)/=b).str(), (test_type(a1) /= b1).str());
      BOOST_CHECK_EQUAL(mpz_int(a / -b).str(), test_type(a1 / -b1).str());
      BOOST_CHECK_EQUAL(mpz_int(-a / b).str(), test_type(-a1 / b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)/=-b).str(), (test_type(a1) /= -b1).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)/=b).str(), (test_type(-a1) /= b1).str());
      BOOST_CHECK_EQUAL(mpz_int(a / d).str(), test_type(a1 / d1).str());
      BOOST_CHECK_EQUAL(mpz_int(a % b).str(), test_type(a1 % b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)%=b).str(), (test_type(a1) %= b1).str());
      BOOST_CHECK_EQUAL(mpz_int(a % -b).str(), test_type(a1 % -b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)%=-b).str(), (test_type(a1) %= -b1).str());
      BOOST_CHECK_EQUAL(mpz_int(-a % b).str(), test_type(-a1 % b1).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)%=b).str(), (test_type(-a1) %= b1).str());
      BOOST_CHECK_EQUAL(mpz_int(a % d).str(), test_type(a1 % d1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)%=d).str(), (test_type(a1) %= d1).str());
      // bitwise ops:
      BOOST_CHECK_EQUAL(mpz_int(a|b).str(), test_type(a1 | b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)|=b).str(), (test_type(a1) |= b1).str());
      BOOST_CHECK_EQUAL(mpz_int(-a|b).str(), test_type(-a1 | b1).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)|=b).str(), (test_type(-a1) |= b1).str());
      BOOST_CHECK_EQUAL(mpz_int(a|-b).str(), test_type(a1 | -b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)|=-b).str(), (test_type(a1) |= -b1).str());
      BOOST_CHECK_EQUAL(mpz_int(-a|-b).str(), test_type(-a1 | -b1).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)|=-b).str(), (test_type(-a1) |= -b1).str());
      BOOST_CHECK_EQUAL(mpz_int(a&b).str(), test_type(a1 & b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)&=b).str(), (test_type(a1) &= b1).str());
      BOOST_CHECK_EQUAL(mpz_int(-a&b).str(), test_type(-a1 & b1).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)&=b).str(), (test_type(-a1) &= b1).str());
      BOOST_CHECK_EQUAL(mpz_int(a&-b).str(), test_type(a1 & -b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)&=-b).str(), (test_type(a1) &= -b1).str());
      BOOST_CHECK_EQUAL(mpz_int(-a&-b).str(), test_type(-a1 & -b1).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)&=-b).str(), (test_type(-a1) &= -b1).str());
      BOOST_CHECK_EQUAL(mpz_int(a^b).str(), test_type(a1 ^ b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)^=b).str(), (test_type(a1) ^= b1).str());
      BOOST_CHECK_EQUAL(mpz_int(-a^b).str(), test_type(-a1 ^ b1).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)^=b).str(), (test_type(-a1) ^= b1).str());
      BOOST_CHECK_EQUAL(mpz_int(a^-b).str(), test_type(a1 ^ -b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)^=-b).str(), (test_type(a1) ^= -b1).str());
      BOOST_CHECK_EQUAL(mpz_int(-a^-b).str(), test_type(-a1 ^ -b1).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)^=-b).str(), (test_type(-a1) ^= -b1).str());
      // Shift ops:
      for(unsigned i = 0; i < 128; ++i)
      {
         if(!std::numeric_limits<test_type>::is_bounded)
         {
            BOOST_CHECK_EQUAL(mpz_int(a << i).str(), test_type(a1 << i).str());
         }
         BOOST_CHECK_EQUAL(mpz_int(a >> i).str(), test_type(a1 >> i).str());
      }
      // Now check operations involving signed integers:
      BOOST_CHECK_EQUAL(mpz_int(a + si).str(), test_type(a1 + si).str());
      BOOST_CHECK_EQUAL(mpz_int(a + -si).str(), test_type(a1 + -si).str());
      BOOST_CHECK_EQUAL(mpz_int(-a + si).str(), test_type(-a1 + si).str());
      BOOST_CHECK_EQUAL(mpz_int(si + a).str(), test_type(si + a1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)+=si).str(), (test_type(a1) += si).str());
      BOOST_CHECK_EQUAL((mpz_int(a)+=-si).str(), (test_type(a1) += -si).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)+=si).str(), (test_type(-a1) += si).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)+=-si).str(), (test_type(-a1) += -si).str());
      BOOST_CHECK_EQUAL(mpz_int(a - si).str(), test_type(a1 - si).str());
      BOOST_CHECK_EQUAL(mpz_int(a - -si).str(), test_type(a1 - -si).str());
      BOOST_CHECK_EQUAL(mpz_int(-a - si).str(), test_type(-a1 - si).str());
      BOOST_CHECK_EQUAL(mpz_int(si - a).str(), test_type(si - a1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)-=si).str(), (test_type(a1) -= si).str());
      BOOST_CHECK_EQUAL((mpz_int(a)-=-si).str(), (test_type(a1) -= -si).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)-=si).str(), (test_type(-a1) -= si).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)-=-si).str(), (test_type(-a1) -= -si).str());
      BOOST_CHECK_EQUAL(mpz_int(b * si).str(), test_type(b1 * si).str());
      BOOST_CHECK_EQUAL(mpz_int(b * -si).str(), test_type(b1 * -si).str());
      BOOST_CHECK_EQUAL(mpz_int(-b * si).str(), test_type(-b1 * si).str());
      BOOST_CHECK_EQUAL(mpz_int(si * b).str(), test_type(si * b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)*=si).str(), (test_type(a1) *= si).str());
      BOOST_CHECK_EQUAL((mpz_int(a)*=-si).str(), (test_type(a1) *= -si).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)*=si).str(), (test_type(-a1) *= si).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)*=-si).str(), (test_type(-a1) *= -si).str());
      BOOST_CHECK_EQUAL(mpz_int(a / si).str(), test_type(a1 / si).str());
      BOOST_CHECK_EQUAL(mpz_int(a / -si).str(), test_type(a1 / -si).str());
      BOOST_CHECK_EQUAL(mpz_int(-a / si).str(), test_type(-a1 / si).str());
      BOOST_CHECK_EQUAL((mpz_int(a)/=si).str(), (test_type(a1) /= si).str());
      BOOST_CHECK_EQUAL((mpz_int(a)/=-si).str(), (test_type(a1) /= -si).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)/=si).str(), (test_type(-a1) /= si).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)/=-si).str(), (test_type(-a1) /= -si).str());
      BOOST_CHECK_EQUAL(mpz_int(a % si).str(), test_type(a1 % si).str());
      BOOST_CHECK_EQUAL(mpz_int(a % -si).str(), test_type(a1 % -si).str());
      BOOST_CHECK_EQUAL(mpz_int(-a % si).str(), test_type(-a1 % si).str());
      BOOST_CHECK_EQUAL((mpz_int(a)%=si).str(), (test_type(a1) %= si).str());
      BOOST_CHECK_EQUAL((mpz_int(a)%=-si).str(), (test_type(a1) %= -si).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)%=si).str(), (test_type(-a1) %= si).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)%=-si).str(), (test_type(-a1) %= -si).str());
      BOOST_CHECK_EQUAL(mpz_int(a|si).str(), test_type(a1 | si).str());
      BOOST_CHECK_EQUAL((mpz_int(a)|=si).str(), (test_type(a1) |= si).str());
      BOOST_CHECK_EQUAL(mpz_int(a&si).str(), test_type(a1 & si).str());
      BOOST_CHECK_EQUAL((mpz_int(a)&=si).str(), (test_type(a1) &= si).str());
      BOOST_CHECK_EQUAL(mpz_int(a^si).str(), test_type(a1 ^ si).str());
      BOOST_CHECK_EQUAL((mpz_int(a)^=si).str(), (test_type(a1) ^= si).str());
      BOOST_CHECK_EQUAL(mpz_int(si|a).str(), test_type(si|a1).str());
      BOOST_CHECK_EQUAL(mpz_int(si&a).str(), test_type(si&a1).str());
      BOOST_CHECK_EQUAL(mpz_int(si^a).str(), test_type(si^a1).str());
      BOOST_CHECK_EQUAL(mpz_int(gcd(a, b)).str(), test_type(gcd(a1, b1)).str());
      BOOST_CHECK_EQUAL(mpz_int(lcm(c, d)).str(), test_type(lcm(c1, d1)).str());
      BOOST_CHECK_EQUAL(mpz_int(gcd(-a, b)).str(), test_type(gcd(-a1, b1)).str());
      BOOST_CHECK_EQUAL(mpz_int(lcm(-c, d)).str(), test_type(lcm(-c1, d1)).str());
      BOOST_CHECK_EQUAL(mpz_int(gcd(-a, -b)).str(), test_type(gcd(-a1, -b1)).str());
      BOOST_CHECK_EQUAL(mpz_int(lcm(-c, -d)).str(), test_type(lcm(-c1, -d1)).str());
      BOOST_CHECK_EQUAL(mpz_int(gcd(a, -b)).str(), test_type(gcd(a1, -b1)).str());
      BOOST_CHECK_EQUAL(mpz_int(lcm(c, -d)).str(), test_type(lcm(c1, -d1)).str());
      // Now check operations involving unsigned integers:
      BOOST_CHECK_EQUAL(mpz_int(a + ui).str(), test_type(a1 + ui).str());
      BOOST_CHECK_EQUAL(mpz_int(-a + ui).str(), test_type(-a1 + ui).str());
      BOOST_CHECK_EQUAL(mpz_int(ui + a).str(), test_type(ui + a1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)+=ui).str(), (test_type(a1) += ui).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)+=ui).str(), (test_type(-a1) += ui).str());
      BOOST_CHECK_EQUAL(mpz_int(a - ui).str(), test_type(a1 - ui).str());
      BOOST_CHECK_EQUAL(mpz_int(-a - ui).str(), test_type(-a1 - ui).str());
      BOOST_CHECK_EQUAL(mpz_int(ui - a).str(), test_type(ui - a1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)-=ui).str(), (test_type(a1) -= ui).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)-=ui).str(), (test_type(-a1) -= ui).str());
      BOOST_CHECK_EQUAL(mpz_int(b * ui).str(), test_type(b1 * ui).str());
      BOOST_CHECK_EQUAL(mpz_int(-b * ui).str(), test_type(-b1 * ui).str());
      BOOST_CHECK_EQUAL(mpz_int(ui * b).str(), test_type(ui * b1).str());
      BOOST_CHECK_EQUAL((mpz_int(a)*=ui).str(), (test_type(a1) *= ui).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)*=ui).str(), (test_type(-a1) *= ui).str());
      BOOST_CHECK_EQUAL(mpz_int(a / ui).str(), test_type(a1 / ui).str());
      BOOST_CHECK_EQUAL(mpz_int(-a / ui).str(), test_type(-a1 / ui).str());
      BOOST_CHECK_EQUAL((mpz_int(a)/=ui).str(), (test_type(a1) /= ui).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)/=ui).str(), (test_type(-a1) /= ui).str());
      BOOST_CHECK_EQUAL(mpz_int(a % ui).str(), test_type(a1 % ui).str());
      BOOST_CHECK_EQUAL(mpz_int(-a % ui).str(), test_type(-a1 % ui).str());
      BOOST_CHECK_EQUAL((mpz_int(a)%=ui).str(), (test_type(a1) %= ui).str());
      BOOST_CHECK_EQUAL((mpz_int(-a)%=ui).str(), (test_type(-a1) %= ui).str());
      BOOST_CHECK_EQUAL(mpz_int(a|ui).str(), test_type(a1 | ui).str());
      BOOST_CHECK_EQUAL((mpz_int(a)|=ui).str(), (test_type(a1) |= ui).str());
      BOOST_CHECK_EQUAL(mpz_int(a&ui).str(), test_type(a1 & ui).str());
      BOOST_CHECK_EQUAL((mpz_int(a)&=ui).str(), (test_type(a1) &= ui).str());
      BOOST_CHECK_EQUAL(mpz_int(a^ui).str(), test_type(a1 ^ ui).str());
      BOOST_CHECK_EQUAL((mpz_int(a)^=ui).str(), (test_type(a1) ^= ui).str());
      BOOST_CHECK_EQUAL(mpz_int(ui|a).str(), test_type(ui|a1).str());
      BOOST_CHECK_EQUAL(mpz_int(ui&a).str(), test_type(ui&a1).str());
      BOOST_CHECK_EQUAL(mpz_int(ui^a).str(), test_type(ui^a1).str());
      BOOST_CHECK_EQUAL(mpz_int(gcd(a, b)).str(), test_type(gcd(a1, b1)).str());
      BOOST_CHECK_EQUAL(mpz_int(lcm(c, d)).str(), test_type(lcm(c1, d1)).str());
      BOOST_CHECK_EQUAL(mpz_int(gcd(-a, b)).str(), test_type(gcd(-a1, b1)).str());
      BOOST_CHECK_EQUAL(mpz_int(lcm(-c, d)).str(), test_type(lcm(-c1, d1)).str());
      BOOST_CHECK_EQUAL(mpz_int(gcd(-a, -b)).str(), test_type(gcd(-a1, -b1)).str());
      BOOST_CHECK_EQUAL(mpz_int(lcm(-c, -d)).str(), test_type(lcm(-c1, -d1)).str());
      BOOST_CHECK_EQUAL(mpz_int(gcd(a, -b)).str(), test_type(gcd(a1, -b1)).str());
      BOOST_CHECK_EQUAL(mpz_int(lcm(c, -d)).str(), test_type(lcm(c1, -d1)).str());

      if(std::numeric_limits<test_type>::is_modulo)
      {
         static mpz_int m = mpz_int(1) << std::numeric_limits<test_type>::digits;
         mpz_int t(a);
         test_type t1(a1);
         for(unsigned i = 0; i < 10; ++i)
         {
            t *= a;
            t %= m;
            t += a;
            t %= m;
            t1 *= a1;
            t1 += a1;
         }
         BOOST_CHECK_EQUAL(t.str(), t1.str());
      }

      if(last_error_count != boost::detail::test_errors())
      {
         last_error_count = boost::detail::test_errors();
         std::cout << std::hex << std::showbase;

         std::cout << "a    = " << a << std::endl;
         std::cout << "a1   = " << a1 << std::endl;
         std::cout << "b    = " << b << std::endl;
         std::cout << "b1   = " << b1 << std::endl;
         std::cout << "c    = " << c << std::endl;
         std::cout << "c1   = " << c1 << std::endl;
         std::cout << "d    = " << d << std::endl;
         std::cout << "d1   = " << d1 << std::endl;
         std::cout << "a + b   = " << a+b << std::endl;
         std::cout << "a1 + b1 = " << a1+b1 << std::endl;
         std::cout << std::dec;
         std::cout << "a - b   = " << a-b << std::endl;
         std::cout << "a1 - b1 = " << a1-b1 << std::endl;
         std::cout << "-a + b   = " << mpz_int(-a)+b << std::endl;
         std::cout << "-a1 + b1 = " << test_type(-a1)+b1 << std::endl;
         std::cout << "-a - b   = " << mpz_int(-a)-b << std::endl;
         std::cout << "-a1 - b1 = " << test_type(-a1)-b1 << std::endl;
         std::cout << "c*d    = " << c*d << std::endl;
         std::cout << "c1*d1  = " << c1*d1 << std::endl;
         std::cout << "b*c    = " << b*c << std::endl;
         std::cout << "b1*c1  = " << b1*c1 << std::endl;
         std::cout << "a/b    = " << a/b << std::endl;
         std::cout << "a1/b1  = " << a1/b1 << std::endl;
         std::cout << "a/d    = " << a/d << std::endl;
         std::cout << "a1/d1  = " << a1/d1 << std::endl;
         std::cout << "a%b    = " << a%b << std::endl;
         std::cout << "a1%b1  = " << a1%b1 << std::endl;
         std::cout << "a%d    = " << a%d << std::endl;
         std::cout << "a1%d1  = " << a1%d1 << std::endl;
      }

      //
      // Check to see if test is taking too long.
      // Tests run on the compiler farm time out after 300 seconds, 
      // so don't get too close to that:
      //
      if(tim.elapsed() > 100)
      {
         std::cout << "Timeout reached, aborting tests now....\n";
         break;
      }

   }
}

int main()
{
   using namespace boost::multiprecision;
   test<cpp_int>();
   test<mp_number<cpp_int_backend<2048, true, void> > >();
   return boost::report_errors();
}



