///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#include <boost/detail/lightweight_test.hpp>
#include <boost/math/special_functions/pow.hpp>

#if !defined(TEST_MPF50) && !defined(TEST_MPF) && !defined(TEST_BACKEND) && !defined(TEST_MPZ) && !defined(TEST_E_FLOAT) && !defined(TEST_MPFR) && !defined(TEST_MPFR_50)
#  define TEST_MPF50
#  define TEST_MPF
#  define TEST_BACKEND
#  define TEST_MPZ
#  define TEST_MPFR
#  define TEST_MPFR_50
#  define TEST_E_FLOAT

#ifdef _MSC_VER
#pragma message("CAUTION!!: No backend type specified so testing everything.... this will take some time!!")
#endif
#ifdef __GNUC__
#pragma warning "CAUTION!!: No backend type specified so testing everything.... this will take some time!!"
#endif

#endif

#if defined(TEST_MPF50) || defined(TEST_MPF) || defined(TEST_MPZ)
#include <boost/math/big_number/gmp.hpp>
#endif
#ifdef TEST_BACKEND
#include <boost/math/concepts/big_number_architypes.hpp>
#endif
#ifdef TEST_E_FLOAT
#include <boost/math/big_number/e_float.hpp>
#endif
#if defined(TEST_MPFR) || defined(TEST_MPFR_50)
#include <boost/math/big_number/mpfr.hpp>
#endif

#define BOOST_TEST_THROW(x, EX)\
   try { x;  BOOST_ERROR("Expected exception not thrown"); } \
   catch(const EX&){}\
   catch(...){ BOOST_ERROR("Incorrect exception type thrown"); }

#undef BOOST_TEST
#define BOOST_TEST(x)\
   try {\
      if(x){}else{ BOOST_ERROR("Value was zero: "); }\
   }\
   catch(const std::exception& e){  \
      BOOST_ERROR("Unexpected exception: ");\
      BOOST_LIGHTWEIGHT_TEST_OSTREAM << e.what() << std::endl;\
   }

template <class Real>
void test_integer_ops(const boost::mpl::false_&){}

template <class Real>
void test_integer_ops(const boost::mpl::true_&)
{
   Real a(20);
   Real b(7);
   Real c;
   BOOST_TEST(a % b == 20 % 7);
   BOOST_TEST(a % 7 == 20 % 7);
   BOOST_TEST(a % 7u == 20 % 7);
   BOOST_TEST(-a % b == -20 % 7);
   BOOST_TEST(-a % -b == -20 % -7);
   BOOST_TEST(a % -b == 20 % -7);
   BOOST_TEST(-a % 7 == -20 % 7);
   BOOST_TEST(-a % -7 == -20 % -7);
   BOOST_TEST(a % -7 == 20 % -7);
   BOOST_TEST(-a % 7u == -20 % 7);

   b = -b;
   BOOST_TEST(a % b == 20 % -7);
   a = -a;
   BOOST_TEST(a % b == -20 % -7);
   BOOST_TEST(a % -7 == -20 % -7);
   b = 7;
   BOOST_TEST(a % b == -20 % 7);
   BOOST_TEST(a % 7 == -20 % 7);
   BOOST_TEST(a % 7u == -20 % 7);

   a = 20;
   a %= b;
   BOOST_TEST(a == 20 % 7);
   a = -20;
   a %= b;
   BOOST_TEST(a == -20 % 7);
   a = 20;
   a %= -b;
   BOOST_TEST(a == 20 % -7);
   a = -20;
   a %= -b;
   BOOST_TEST(a == -20 % -7);

   a = 20;
   a %= 7;
   BOOST_TEST(a == 20 % 7);
   a = -20;
   a %= 7;
   BOOST_TEST(a == -20 % 7);
   a = 20;
   a %= -7;
   BOOST_TEST(a == 20 % -7);
   a = -20;
   a %= -7;
   BOOST_TEST(a == -20 % -7);
#ifndef BOOST_NO_LONG_LONG
   a = 20;
   a %= 7uLL;
   BOOST_TEST(a == 20 % 7);
   a = -20;
   a %= 7uLL;
   BOOST_TEST(a == -20 % 7);
   a = 20;
   a %= -7LL;
   BOOST_TEST(a == 20 % -7);
   a = -20;
   a %= -7LL;
   BOOST_TEST(a == -20 % -7);
#endif
   a = 20;
   BOOST_TEST(++a == 21);
   BOOST_TEST(--a == 20);
   BOOST_TEST(a++ == 20);
   BOOST_TEST(a == 21);
   BOOST_TEST(a-- == 21);
   BOOST_TEST(a == 20);
   a = 2000;
   a <<= 20;
   BOOST_TEST(a == 2000L << 20);
   a >>= 20;
   BOOST_TEST(a == 2000);
   a <<= 20u;
   BOOST_TEST(a == 2000L << 20);
   a >>= 20u;
   BOOST_TEST(a == 2000);
   BOOST_TEST_THROW(a <<= -20, std::out_of_range);
   BOOST_TEST_THROW(a >>= -20, std::out_of_range);
#ifndef BOOST_NO_LONG_LONG
   if(sizeof(long long) > sizeof(std::size_t))
   {
      // extreme values should trigger an exception:
      BOOST_TEST_THROW(a >>= (1uLL << (sizeof(long long) * CHAR_BIT - 2)), std::out_of_range);
      BOOST_TEST_THROW(a <<= (1uLL << (sizeof(long long) * CHAR_BIT - 2)), std::out_of_range);
   }
#endif
   a = 20;
   b = a << 20;
   BOOST_TEST(b == (20 << 20));
   b = a >> 2;
   BOOST_TEST(b == (20 >> 2));
   b = (a + 2) << 10;
   BOOST_TEST(b == (22 << 10));
   b = (a + 3) >> 3;
   BOOST_TEST(b == (23 >> 3));
   //
   // Bit fiddling:
   //
   int i = 1020304;
   int j = 56789123;
   int k = 4523187;
   a = i;
   b = j;
   c = a;
   c &= b;
   BOOST_TEST(c == (i & j));
   c = a;
   c &= j;
   BOOST_TEST(c == (i & j));
   c = a;
   c &= a + b;
   BOOST_TEST(c == (i & (i + j)));
   BOOST_TEST((a & b) == (i & j));
   c = k;
   a = a & (b + k);
   BOOST_TEST(a == (i & (j + k)));
   a = i;
   a = (b + k) & a;
   BOOST_TEST(a == (i & (j + k)));
   a = i;
   c = a & b & k;
   BOOST_TEST(c == (i&j&k));

   a = i;
   b = j;
   c = a;
   c |= b;
   BOOST_TEST(c == (i | j));
   c = a;
   c |= j;
   BOOST_TEST(c == (i | j));
   c = a;
   c |= a + b;
   BOOST_TEST(c == (i | (i + j)));
   BOOST_TEST((a | b) == (i | j));
   c = k;
   a = a | (b + k);
   BOOST_TEST(a == (i | (j + k)));
   a = i;
   a = (b + k) | a;
   BOOST_TEST(a == (i | (j + k)));
   a = i;
   c = a | b | k;
   BOOST_TEST(c == (i|j|k));

   a = i;
   b = j;
   c = a;
   c ^= b;
   BOOST_TEST(c == (i ^ j));
   c = a;
   c ^= j;
   BOOST_TEST(c == (i ^ j));
   c = a;
   c ^= a + b;
   BOOST_TEST(c == (i ^ (i + j)));
   BOOST_TEST((a ^ b) == (i ^ j));
   c = k;
   a = a ^ (b + k);
   BOOST_TEST(a == (i ^ (j + k)));
   a = i;
   a = (b + k) ^ a;
   BOOST_TEST(a == (i ^ (j + k)));
   a = i;
   c = a ^ b ^ k;
   BOOST_TEST(c == (i^j^k));
   a = i;
   b = j;
   c = k;
   BOOST_TEST(~a == ~i);
   c = a & ~b;
   BOOST_TEST(c == (i & ~j));
   c = ~(a | b);
   BOOST_TEST(c == ~(i | j));

   //
   // Non-member functions:
   //
   a = -20;
   BOOST_TEST(abs(a) == 20);
   BOOST_TEST(abs(-a) == 20);
   BOOST_TEST(abs(+a) == 20);
   a = 20;
   BOOST_TEST(abs(a) == 20);
   BOOST_TEST(abs(-a) == 20);
   BOOST_TEST(abs(+a) == 20);
}

template <class Real>
void test_real_ops(const boost::mpl::false_&){}

template <class Real>
void test_real_ops(const boost::mpl::true_&)
{
#if defined(TEST_MPF) || defined(TEST_MPF_50) || defined(TEST_BACKEND)
   BOOST_TEST(abs(Real(2)) == 2);
   BOOST_TEST(abs(Real(-2)) == 2);
   BOOST_TEST(fabs(Real(2)) == 2);
   BOOST_TEST(fabs(Real(-2)) == 2);
   BOOST_TEST(floor(Real(5) / 2) == 2);
   BOOST_TEST(ceil(Real(5) / 2) == 3);
   BOOST_TEST(floor(Real(-5) / 2) == -3);
   BOOST_TEST(ceil(Real(-5) / 2) == -2);
   BOOST_TEST(trunc(Real(5) / 2) == 2);
   BOOST_TEST(trunc(Real(-5) / 2) == -2);
   //
   // ldexp and frexp, these pretty much have to implemented by each backend:
   //
   BOOST_TEST(ldexp(Real(2), 5) == 64);
   BOOST_TEST(ldexp(Real(2), -5) == Real(2) / 32);
   Real v(512);
   int exp;
   Real r = frexp(v, &exp);
   BOOST_TEST(r == 0.5);
   BOOST_TEST(exp == 10);
   BOOST_TEST(v == 512);
   v = 1 / v;
   r = frexp(v, &exp);
   BOOST_TEST(r == 0.5);
   BOOST_TEST(exp == -8);
   //
   // pow and exp:
   //
   v = 3.25;
   r = pow(v, 0);
   BOOST_TEST(r == 1);
   r = pow(v, 1);
   BOOST_TEST(r == 3.25);
   r = pow(v, 2);
   BOOST_TEST(r == boost::math::pow<2>(3.25));
   r = pow(v, 3);
   BOOST_TEST(r == boost::math::pow<3>(3.25));
   r = pow(v, 4);
   BOOST_TEST(r == boost::math::pow<4>(3.25));
   r = pow(v, 5);
   BOOST_TEST(r == boost::math::pow<5>(3.25));
   r = pow(v, 6);
   BOOST_TEST(r == boost::math::pow<6>(3.25));
   r = pow(v, 25);
   BOOST_TEST(r == boost::math::pow<25>(Real(3.25)));
   /*
   r = pow(v, 56);
   BOOST_TEST(r == boost::math::pow<56>(Real(3.25)));
   */
#endif
}

#ifdef TEST_E_FLOAT

template <class T>
struct lexical_cast_target_type
{
   typedef long double type;
};

#else

template <class T>
struct lexical_cast_target_type
{
   typedef typename boost::mpl::if_<
      boost::is_signed<T>,
      boost::intmax_t,
      typename boost::mpl::if_<
         boost::is_unsigned<T>,
         boost::uintmax_t,
         T
      >::type
   >::type type;
};

#endif

template <class Real, class Num>
void test_negative_mixed(boost::mpl::true_ const&)
{
   typedef typename lexical_cast_target_type<Num>::type target_type;
   std::cout << "Testing mixed arithmetic with type: " << typeid(Real).name()  << " and " << typeid(Num).name() << std::endl;
   Num n1 = -static_cast<Num>(1uLL << (std::numeric_limits<Num>::digits - 1));
   Num n2 = -1;
   Num n3 = 0;
   Num n4 = -20;
   Num n5 = -8;
   // Default construct:
   BOOST_TEST(Real(n1) == n1);
   BOOST_TEST(Real(n2) == n2);
   BOOST_TEST(Real(n3) == n3);
   BOOST_TEST(Real(n4) == n4);
   BOOST_TEST(n1 == Real(n1));
   BOOST_TEST(n2 == Real(n2));
   BOOST_TEST(n3 == Real(n3));
   BOOST_TEST(n4 == Real(n4));
   BOOST_TEST(n1 == boost::lexical_cast<target_type>(Real(n1).str(0, boost::is_floating_point<Num>::value)));
   BOOST_TEST(n2 == boost::lexical_cast<target_type>(Real(n2).str(0, boost::is_floating_point<Num>::value)));
   BOOST_TEST(n3 == boost::lexical_cast<target_type>(Real(n3).str(0, boost::is_floating_point<Num>::value)));
   BOOST_TEST(n4 == boost::lexical_cast<target_type>(Real(n4).str(0, boost::is_floating_point<Num>::value)));
   // Assignment:
   Real r(0);
   BOOST_TEST(r != n1);
   r = n1;
   BOOST_TEST(r == n1);
   r = n2;
   BOOST_TEST(r == n2);
   r = n3;
   BOOST_TEST(r == n3);
   r = n4;
   BOOST_TEST(r == n4);
   // Addition:
   r = n2;
   BOOST_TEST(r + n4 == n2 + n4);
   BOOST_TEST(Real(r + n4) == n2 + n4);
   r += n4;
   BOOST_TEST(r == n2 + n4);
   // subtraction:
   r = n4;
   BOOST_TEST(r - n5 == n4 - n5);
   BOOST_TEST(Real(r - n5) == n4 - n5);
   r -= n5;
   BOOST_TEST(r == n4 - n5);
   // Multiplication:
   r = n2;
   BOOST_TEST(r * n4 == n2 * n4);
   BOOST_TEST(Real(r * n4) == n2 * n4);
   r *= n4;
   BOOST_TEST(r == n2 * n4);
   // Division:
   r = n1;
   BOOST_TEST(r / n5 == n1 / n5);
   BOOST_TEST(Real(r / n5) == n1 / n5);
   r /= n5;
   BOOST_TEST(r == n1 / n5);
   //
   // Extra cases for full coverage:
   //
   r = Real(n4) + n5;
   BOOST_TEST(r == n4 + n5);
   r = n4 + Real(n5);
   BOOST_TEST(r == n4 + n5);
   r = Real(n4) - n5;
   BOOST_TEST(r == n4 - n5);
   r = n4 - Real(n5);
   BOOST_TEST(r == n4 - n5);
   r = n4 * Real(n5);
   BOOST_TEST(r == n4 * n5);
   r = (4 * n4) / Real(4);
   BOOST_TEST(r == n4);
}

template <class Real, class Num>
void test_negative_mixed(boost::mpl::false_ const&)
{
}

template <class Real, class Num>
void test_mixed()
{
   typedef typename lexical_cast_target_type<Num>::type target_type;
   std::cout << "Testing mixed arithmetic with type: " << typeid(Real).name()  << " and " << typeid(Num).name() << std::endl;
   Num n1 = static_cast<Num>(1uLL << (std::numeric_limits<Num>::digits - 1));
   Num n2 = 1;
   Num n3 = 0;
   Num n4 = 20;
   Num n5 = 8;
   // Default construct:
   BOOST_TEST(Real(n1) == n1);
   BOOST_TEST(Real(n2) == n2);
   BOOST_TEST(Real(n3) == n3);
   BOOST_TEST(Real(n4) == n4);
   BOOST_TEST(Real(n1).template convert_to<Num>() == n1);
   BOOST_TEST(Real(n2).template convert_to<Num>() == n2);
   BOOST_TEST(Real(n3).template convert_to<Num>() == n3);
   BOOST_TEST(Real(n4).template convert_to<Num>() == n4);
   BOOST_TEST(n1 == Real(n1));
   BOOST_TEST(n2 == Real(n2));
   BOOST_TEST(n3 == Real(n3));
   BOOST_TEST(n4 == Real(n4));
   BOOST_TEST(n1 == boost::lexical_cast<target_type>(Real(n1).str(0, boost::is_floating_point<Num>::value)));
   BOOST_TEST(n2 == boost::lexical_cast<target_type>(Real(n2).str(0, boost::is_floating_point<Num>::value)));
   BOOST_TEST(n3 == boost::lexical_cast<target_type>(Real(n3).str(0, boost::is_floating_point<Num>::value)));
   BOOST_TEST(n4 == boost::lexical_cast<target_type>(Real(n4).str(0, boost::is_floating_point<Num>::value)));
   // Assignment:
   Real r(0);
   BOOST_TEST(r != n1);
   r = n1;
   BOOST_TEST(r == n1);
   r = n2;
   BOOST_TEST(r == n2);
   r = n3;
   BOOST_TEST(r == n3);
   r = n4;
   BOOST_TEST(r == n4);
   // Addition:
   r = n2;
   BOOST_TEST(r + n4 == n2 + n4);
   BOOST_TEST(Real(r + n4) == n2 + n4);
   r += n4;
   BOOST_TEST(r == n2 + n4);
   // subtraction:
   r = n4;
   BOOST_TEST(r - n5 == n4 - n5);
   BOOST_TEST(Real(r - n5) == n4 - n5);
   r -= n5;
   BOOST_TEST(r == n4 - n5);
   // Multiplication:
   r = n2;
   BOOST_TEST(r * n4 == n2 * n4);
   BOOST_TEST(Real(r * n4) == n2 * n4);
   r *= n4;
   BOOST_TEST(r == n2 * n4);
   // Division:
   r = n1;
   BOOST_TEST(r / n5 == n1 / n5);
   BOOST_TEST(Real(r / n5) == n1 / n5);
   r /= n5;
   BOOST_TEST(r == n1 / n5);

   //
   // special cases for full coverage:
   //
   r = n5 + Real(n4);
   BOOST_TEST(r == n4 + n5);
   r = n4 - Real(n5);
   BOOST_TEST(r == n4 - n5);
   r = n4 * Real(n5);
   BOOST_TEST(r == n4 * n5);
   r = (4 * n4) / Real(4);
   BOOST_TEST(r == n4);

   test_negative_mixed<Real, Num>(boost::mpl::bool_<std::numeric_limits<Num>::is_signed>());
}

template <class Real>
void test()
{
#ifndef NO_MIXED_OPS
   test_mixed<Real, unsigned char>();
   test_mixed<Real, signed char>();
   test_mixed<Real, char>();
   test_mixed<Real, short>();
   test_mixed<Real, unsigned short>();
   test_mixed<Real, int>();
   test_mixed<Real, unsigned int>();
   test_mixed<Real, long>();
   test_mixed<Real, unsigned long>();
#ifdef BOOST_HAS_LONG_LONG
   test_mixed<Real, long long>();
   test_mixed<Real, unsigned long long>();
#endif
   test_mixed<Real, float>();
   test_mixed<Real, double>();
   test_mixed<Real, long double>();
#endif
   //
   // Integer only functions:
   //
   test_integer_ops<Real>(boost::math::is_extended_integer<Real>());
   //
   // Real number only functions:
   //
   test_real_ops<Real>(boost::mpl::bool_<false == boost::math::is_extended_integer<Real>::value >());
   //
   // Test basic arithmetic:
   //
   Real a(8);
   Real b(64);
   Real c(500);
   Real d(1024);
   BOOST_TEST(a + b == 72);
   a += b;
   BOOST_TEST(a == 72);
   BOOST_TEST(a - b == 8);
   a -= b;
   BOOST_TEST(a == 8);
   BOOST_TEST(a * b == 8*64L);
   a *= b;
   BOOST_TEST(a == 8*64L);
   BOOST_TEST(a / b == 8);
   a /= b;
   BOOST_TEST(a == 8);

   Real ac(a);
   BOOST_TEST(ac == a);
   BOOST_TEST(-a == -8);

   ac = a * c;
   BOOST_TEST(ac == 8*500L);
   
   ac = ac + b + c;
   BOOST_TEST(ac == 8*500L+64+500);
   ac = a;
   ac = b + c + ac;
   BOOST_TEST(ac == 8+64+500);
   ac = ac - b + c;
   BOOST_TEST(ac == 8+64+500-64+500);
   ac = a;
   ac = b + c - ac;
   BOOST_TEST(ac == -8+64+500);
   ac = a;
   ac = ac * b;
   BOOST_TEST(ac == 8*64);
   ac = a;
   ac *= b * ac;
   BOOST_TEST(ac == 8*8*64);
   ac = b;
   ac = ac / a;
   BOOST_TEST(ac == 64/8);
   ac = b;
   ac /= ac / a;
   BOOST_TEST(ac == 64 / (64/8));
   ac = a;
   ac = b + ac * a;
   BOOST_TEST(ac == 64 * 2);
   ac = a;
   ac = b - ac * a;
   BOOST_TEST(ac == 0);
   ac = a;
   ac = b * (ac + a);
   BOOST_TEST(ac == 64 * (16));
   ac = a;
   ac = b / (ac * 1);
   BOOST_TEST(ac == 64 / 8);
   ac = a;
   ac = ac + b;
   BOOST_TEST(ac == 8 + 64);
   ac = a;
   ac = a + ac;
   BOOST_TEST(ac == 16);
   ac = a;
   ac = ac - b;
   BOOST_TEST(ac == 8 - 64);
   ac = a;
   ac = a - ac;
   BOOST_TEST(ac == 0);
   ac = a;
   ac += a + b;
   BOOST_TEST(ac == 80);
   ac = a;
   ac += b + a;
   BOOST_TEST(ac == 80);
   ac = a;
   ac -= a + b;
   BOOST_TEST(ac == -64);
   ac = a;
   ac -= b - a;
   BOOST_TEST(ac == 16 - 64);
   ac = +a;
   BOOST_TEST(ac == 8);
   ac = -a;
   BOOST_TEST(ac == -8);
   ac = 8;
   ac = a * ac;
   BOOST_TEST(ac == 8*8);
   ac = a;
#ifndef TEST_E_FLOAT
   ac = ac + "8";
   BOOST_TEST(ac == 16);
#endif
   ac = a;
   ac += +a;
   BOOST_TEST(ac == 16);
   ac = a;
   ac += -a;
   BOOST_TEST(ac == 0);
   ac = a;
   ac += b - a;
   BOOST_TEST(ac == 8 + 64-8);
   ac = a;
   ac += b*c;
   BOOST_TEST(ac == 8 + 64 * 500);
   ac = a;
#ifndef TEST_E_FLOAT
   ac = ac - "8";
   BOOST_TEST(ac == 0);
#endif
   ac = a;
   ac -= +a;
   BOOST_TEST(ac == 0);
   ac = a;
   ac -= -a;
   BOOST_TEST(ac == 16);
   ac = a;
   ac -= c - b;
   BOOST_TEST(ac == 8 - (500-64));
   ac = a;
   ac -= b*c;
   BOOST_TEST(ac == 8 - 500*64);
   ac = a;
   ac += ac * b;
   BOOST_TEST(ac == 8 + 8 * 64);
   ac = a;
   ac -= ac * b;
   BOOST_TEST(ac == 8 - 8 * 64);
#ifndef TEST_E_FLOAT
   ac = a * "8";
   BOOST_TEST(ac == 64);
#else
   ac = a * 8;
#endif
   ac *= +a;
   BOOST_TEST(ac == 64 * 8);
   ac = a;
   ac *= -a;
   BOOST_TEST(ac == -64);
   ac = a;
   ac *= b * c;
   BOOST_TEST(ac == 8 * 64 * 500);
   ac = a;
   ac *= b / a;
   BOOST_TEST(ac == 8 * 64 / 8);
   ac = a;
   ac *= b + c;
   BOOST_TEST(ac == 8 * (64 + 500));
#ifndef TEST_E_FLOAT
   ac = b / "8";
   BOOST_TEST(ac == 8);
#endif
   ac = b;
   ac /= +a;
   BOOST_TEST(ac == 8);
   ac = b;
   ac /= -a;
   BOOST_TEST(ac == -8);
   ac = b;
   ac /= b / a;
   BOOST_TEST(ac == 64 / (64/8));
   ac = b;
   ac /= a + Real(0);
   BOOST_TEST(ac == 8);
#ifndef TEST_E_FLOAT
   ac = a + std::string("8");
   BOOST_TEST(ac == 16);
#endif
   //
   // simple tests with immediate values, these calls can be optimised in many backends:
   //
   ac = a + b;
   BOOST_TEST(ac == 72);
   ac = a + +b;
   BOOST_TEST(ac == 72);
   ac = +a + b;
   BOOST_TEST(ac == 72);
   ac = +a + +b;
   BOOST_TEST(ac == 72);
   ac = a + -b;
   BOOST_TEST(ac == 8 - 64);
   ac = -a + b;
   BOOST_TEST(ac == -8+64);
   ac = -a + -b;
   BOOST_TEST(ac == -72);
   ac = a + - + -b; // lots of unary operators!!
   BOOST_TEST(ac == 72);
   ac = a;
   ac = b / ac;
   BOOST_TEST(ac == b / a);
   //
   // Comparisons:
   //
   BOOST_TEST((a == b) == false);
   BOOST_TEST((a != b) == true);
   BOOST_TEST((a <= b) == true);
   BOOST_TEST((a < b) == true);
   BOOST_TEST((a >= b) == false);
   BOOST_TEST((a > b) == false);

   BOOST_TEST((a+b == b) == false);
   BOOST_TEST((a+b != b) == true);
   BOOST_TEST((a+b >= b) == true);
   BOOST_TEST((a+b > b) == true);
   BOOST_TEST((a+b <= b) == false);
   BOOST_TEST((a+b < b) == false);

   BOOST_TEST((a == b+a) == false);
   BOOST_TEST((a != b+a) == true);
   BOOST_TEST((a <= b+a) == true);
   BOOST_TEST((a < b+a) == true);
   BOOST_TEST((a >= b+a) == false);
   BOOST_TEST((a > b+a) == false);

   BOOST_TEST((a+b == b+a) == true);
   BOOST_TEST((a+b != b+a) == false);
   BOOST_TEST((a+b <= b+a) == true);
   BOOST_TEST((a+b < b+a) == false);
   BOOST_TEST((a+b >= b+a) == true);
   BOOST_TEST((a+b > b+a) == false);

   BOOST_TEST((8 == b+a) == false);
   BOOST_TEST((8 != b+a) == true);
   BOOST_TEST((8 <= b+a) == true);
   BOOST_TEST((8 < b+a) == true);
   BOOST_TEST((8 >= b+a) == false);
   BOOST_TEST((8 > b+a) == false);
   BOOST_TEST((800 == b+a) == false);
   BOOST_TEST((800 != b+a) == true);
   BOOST_TEST((800 >= b+a) == true);
   BOOST_TEST((800 > b+a) == true);
   BOOST_TEST((800 <= b+a) == false);
   BOOST_TEST((800 < b+a) == false);
   BOOST_TEST((72 == b+a) == true);
   BOOST_TEST((72 != b+a) == false);
   BOOST_TEST((72 <= b+a) == true);
   BOOST_TEST((72 < b+a) == false);
   BOOST_TEST((72 >= b+a) == true);
   BOOST_TEST((72 > b+a) == false);
   //
   // Test sign and zero functions, plus use in boolian context:
   //
   a = 20;
   BOOST_TEST(a.sign() > 0);
   BOOST_TEST(!a.is_zero());
   a = -20;
   BOOST_TEST(a.sign() < 0);
   BOOST_TEST(!a.is_zero());
   a = 0;
   BOOST_TEST(a.sign() == 0);
   BOOST_TEST(a.is_zero());
   if(a)
   {
      BOOST_ERROR("Unexpected non-zero result");
   }
   if(!a){}
   else
   {
      BOOST_ERROR("Unexpected zero result");
   }
   b = 2;
   if(!b)
   {
      BOOST_ERROR("Unexpected zero result");
   }
   if(b){}
   else
   {
      BOOST_ERROR("Unexpected non-zero result");
   }
   if(a && b)
   {
      BOOST_ERROR("Unexpected zero result");
   }
   if(!(a || b))
   {
      BOOST_ERROR("Unexpected zero result");
   }
   if(a + b){}
   else
   {
      BOOST_ERROR("Unexpected zero result");
   }
   if(b - 2)
   {
      BOOST_ERROR("Unexpected non-zero result");
   }
   //
   // Test iostreams:
   //
   std::stringstream ss;
   a = 20;
   b = 2;
   ss << a;
   ss >> c;
   BOOST_TEST(a == c);
   ss.clear();
   ss << a + b;
   ss >> c;
   BOOST_TEST(c == 22);
   BOOST_TEST(c == a + b);
}


int main()
{
#ifdef TEST_BACKEND
   test<boost::math::big_number<boost::math::concepts::big_number_backend_real_architype> >();
#endif
#ifdef TEST_MPF50
   test<boost::math::mpf_real_50>();
#endif
#ifdef TEST_MPF
   boost::math::mpf_real::default_precision(1000);
   /*
   boost::math::mpf_real r;
   r.precision(50);
   BOOST_TEST(r.precision() >= 50);
   */
   BOOST_TEST(boost::math::mpf_real::default_precision() == 1000);
   test<boost::math::mpf_real>();
#endif
#ifdef TEST_MPZ
   test<boost::math::mpz_int>();
#endif
#ifdef TEST_E_FLOAT
   test<boost::math::e_float>();
#endif
#ifdef TEST_MPFR
   test<boost::math::mpfr_real>();
#endif
#ifdef TEST_MPFR_50
   test<boost::math::mpfr_real_50>();
#endif
   return boost::report_errors();
}

