///////////////////////////////////////////////////////////////
//  Copyright 2026 Nick Thompson.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt
//
// Tests that fma(a, b, c) for cpp_bin_float rounds only once.
//

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/detail/lightweight_test.hpp>

#include <boost/math/special_functions/next.hpp>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cstdint>
#include <limits>
#include <random>

using namespace boost::multiprecision;

// With expression templates a * b + c and a * b - c are evaluated with eval_multiply_add and
// eval_multiply_subtract, which must also be fused.
template <class B>
void test_expression_templates(const number<B, et_on>*)
{
   using T = number<B, et_on>;
   const T eps = std::numeric_limits<T>::epsilon();
   T       x   = 1 + eps;
   T       z   = -(1 + 2 * eps);
   T       r   = x * x + z;
   BOOST_TEST_EQ(r, eps * eps);
   T       w   = 1 + 2 * eps;
   r = x * x - w;
   BOOST_TEST_EQ(r, eps * eps);
   r = w - x * x;
   BOOST_TEST_EQ(r, -(eps * eps));
   r = x;
   r = r * r + z;
   BOOST_TEST_EQ(r, eps * eps);
   r = x;
   r = r * r - w;
   BOOST_TEST_EQ(r, eps * eps);
   r = z;
   r += x * x;
   BOOST_TEST_EQ(r, eps * eps);
   r = w;
   r -= x * x;
   BOOST_TEST_EQ(r, -(eps * eps));
}
template <class B>
void test_expression_templates(const number<B, et_off>*)
{
}

template <class T>
void test_issue_case()
{
   test_expression_templates(static_cast<const T*>(nullptr));
   // x * x = 1 + 2 eps + eps^2 exactly, so the fused result is eps^2.
   const T eps = std::numeric_limits<T>::epsilon();
   T       x   = 1 + eps;
   T       z   = -(1 + 2 * eps);
   BOOST_TEST_EQ(fma(x, x, z), eps * eps);
   BOOST_TEST_EQ(fma(x, x, z) / eps, eps);
   // Negative operands:
   BOOST_TEST_EQ(fma(-x, x, -z), -(eps * eps));
   BOOST_TEST_EQ(fma(x, -x, -z), -(eps * eps));
   BOOST_TEST_EQ(fma(-x, -x, z), eps * eps);
   BOOST_TEST_EQ(fma(x, x, z) + 1, T(1));
   // TwoProduct:
   T p = x * x;
   T e = fma(x, x, -p);
   BOOST_TEST_EQ(e, eps * eps);
   // Exact cancellation gives +0:
   T zero = fma(x, T(1), -x);
   BOOST_TEST_EQ(zero, T(0));
   BOOST_TEST(!signbit(zero));
   zero = fma(-x, T(1), x);
   BOOST_TEST_EQ(zero, T(0));
   BOOST_TEST(!signbit(zero));
   // c == 0, or a zero factor:
   BOOST_TEST_EQ(fma(x, x, T(0)), x * x);
   BOOST_TEST_EQ(fma(T(0), x, z), z);
   BOOST_TEST_EQ(fma(x, T(0), z), z);
   BOOST_TEST_EQ(fma(T(0), T(0), T(0)), T(0));
   // Exactly representable results:
   BOOST_TEST_EQ(fma(T(3), T(5), T(7)), T(22));
   BOOST_TEST_EQ(fma(T(3), T(5), T(-15)), T(0));
   // Non-finite:
   const T inf = std::numeric_limits<T>::infinity();
   BOOST_TEST((isinf)(fma(inf, x, z)));
   BOOST_TEST((isinf)(fma(x, x, inf)));
   BOOST_TEST((isnan)(fma(inf, T(0), z)));
   BOOST_TEST((isnan)(fma(inf, x, -inf)));
   BOOST_TEST((isnan)(fma(x, x, std::numeric_limits<T>::quiet_NaN())));
   // Aliasing of the result with each of the arguments:
   T a = x, b = x, c = z;
   a = fma(a, b, c);
   BOOST_TEST_EQ(a, eps * eps);
   a = x; b = x; c = z;
   b = fma(a, b, c);
   BOOST_TEST_EQ(b, eps * eps);
   a = x; b = x; c = z;
   c = fma(a, b, c);
   BOOST_TEST_EQ(c, eps * eps);
   a = x;
   a = fma(a, a, -p);
   BOOST_TEST_EQ(a, eps * eps);
}

//
// Random inputs, compared with a hardware fma for float and double.  The mantissas are sometimes short so that
// ties and exact cases are frequent, and c is placed at a random distance from the product so
// that every one of the code paths is exercised.
//
template <class Hw, class Bf>
void test_against_hardware(unsigned count)
{
   std::mt19937_64 gen(12345);
   constexpr int   digits = std::numeric_limits<Hw>::digits;

   auto random_value = [&](int min_exp, int max_exp) -> Hw {
      int      bits = 1 + static_cast<int>(gen() % digits);
      if (gen() % 3 == 0)
         bits = digits;
      std::uint64_t m = gen() & ((std::uint64_t(1) << bits) - 1);
      m |= std::uint64_t(1) << (bits - 1);
      int e = min_exp + static_cast<int>(gen() % static_cast<unsigned>(max_exp - min_exp + 1));
      Hw  r = std::ldexp(static_cast<Hw>(m), e - bits);
      return (gen() & 1) ? -r : r;
   };

   unsigned bad = 0;
   for (unsigned i = 0; i < count; ++i)
   {
      Hw a = random_value(-10, 10);
      Hw b = random_value(-10, 10);
      Hw c;
      switch (gen() % 4)
      {
      case 0:
         // Close to the negated product, lots of cancellation:
         c = -(a * b);
         c = std::nextafter(c, (gen() & 1) ? Hw(1) : Hw(-1)) * ((gen() & 1) ? Hw(1) : Hw(1) + std::numeric_limits<Hw>::epsilon());
         break;
      case 1:
         // Anywhere from far below to far above the product:
         c = random_value(-5 * digits, 5 * digits);
         break;
      case 2:
         // Near the product's last bits (ties):
         c = std::ldexp(random_value(1, 3), -digits - static_cast<int>(gen() % 3) - 1);
         break;
      default:
         c = random_value(-10, 10);
         break;
      }
      Hw expected = std::fma(a, b, c);
      Bf r        = fma(Bf(a), Bf(b), Bf(c));
      Bf s        = Bf(a);
      s           = fma(s, Bf(b), Bf(c));
      Bf nc       = -Bf(c);
      Bf d        = fma(Bf(a), Bf(b), -nc);
      if ((r != Bf(expected)) || (s != r) || (d != Bf(expected)))
      {
         if (++bad < 10)
            std::cout << std::setprecision(20) << "a = " << a << " b = " << b << " c = " << c << " got " << r << " expected " << expected << std::endl;
      }
   }
   BOOST_TEST_EQ(bad, 0u);
}

//
// Compare with an exact computation in a much wider type.  The exponents of the product and c
// differ by well under 10 * digits, so the wide sum is exact.
//
template <class T>
void test_against_wide(unsigned count)
{
   using wide_type = number<backends::cpp_bin_float<12 * (std::numeric_limits<T>::digits + 8), backends::digit_base_2>, et_off>;
   std::mt19937_64 gen(6789);
   const int       digits = std::numeric_limits<T>::digits;

   auto random_value = [&](int min_exp, int max_exp) -> T {
      T r = 1;
      for (int i = 0; i < digits; i += 32)
         r = r * 4294967296.0 + static_cast<T>(static_cast<std::uint32_t>(gen()));
      if (gen() % 4 == 0)
      {
         // Short mantissa:
         int keep = 1 + static_cast<int>(gen() % 16);
         int e;
         r = ldexp(trunc(ldexp(frexp(r, &e), keep)), -keep);
      }
      int e;
      r = frexp(r, &e);
      int ne = min_exp + static_cast<int>(gen() % static_cast<unsigned>(max_exp - min_exp + 1));
      r      = ldexp(r, ne);
      return (gen() & 1) ? T(-r) : r;
   };

   unsigned bad = 0;
   for (unsigned i = 0; i < count; ++i)
   {
      T a = random_value(-20, 20);
      T b = random_value(-20, 20);
      T c;
      switch (gen() % 3)
      {
      case 0:
      {
         T p = a * b;
         c   = -p;
         if (gen() & 1)
            c = c * (1 + std::numeric_limits<T>::epsilon() * static_cast<int>(gen() % 5));
         else
            c = boost::math::float_next(c);
         break;
      }
      case 1:
         c = random_value(-4 * digits, 4 * digits);
         break;
      default:
         c = random_value(-30, 30);
         break;
      }
      wide_type w = wide_type(a) * wide_type(b) + wide_type(c);
      T         expected(w);
      T         r = fma(a, b, c);
      T         m = fma(a, b, -c);
      wide_type w2 = wide_type(a) * wide_type(b) - wide_type(c);
      T         expected2(w2);
      if ((r != expected) || (m != expected2))
      {
         if (++bad < 10)
            std::cout << std::setprecision(40) << "a = " << a << " b = " << b << " c = " << c << " got " << r << " expected " << expected << std::endl;
      }
   }
   BOOST_TEST_EQ(bad, 0u);
}

//
// Products that are exact ties when rounded to the working precision, plus or minus a power of two
// at every distance below the product's last bit: the sign and size of the addend then decides
// the rounding direction.
//
template <class T>
void test_ties(unsigned count)
{
   using wide_type = number<backends::cpp_bin_float<12 * (std::numeric_limits<T>::digits + 8), backends::digit_base_2>, et_off>;
   std::mt19937_64 gen(2468);
   const int       digits = std::numeric_limits<T>::digits;

   // An odd integer with exactly n bits:
   auto random_odd = [&](int n) -> T {
      T r = 1;
      for (int i = 1; i < n; ++i)
         r = r * 2 + static_cast<int>(i == n - 1 ? 1 : gen() & 1);
      return r;
   };

   unsigned bad = 0;
   for (unsigned i = 0; i < count;)
   {
      int h  = 1 + static_cast<int>(gen() % static_cast<unsigned>(digits));
      int h2 = digits + 1 - h;
      T   a  = random_odd(h);
      T   b  = random_odd(h2);
      if (a * b < ldexp(T(1), digits))
         continue; // The product must have digits + 1 bits, so that it is a tie.
      ++i;
      int e  = static_cast<int>(gen() % 41) - 20;
      int j  = 1 + static_cast<int>(gen() % static_cast<unsigned>(3 * digits + 10));
      if (gen() % 3 == 0)
         j = digits - 3 + static_cast<int>(gen() % 8);
      T c = ldexp(T(1), e - j); // The last bit of a * b has weight 2^e
      if (gen() & 1)
         c = -c;
      if (gen() & 1)
         a = -a;
      a = ldexp(a, e / 2);
      b = ldexp(b, e - e / 2);
      wide_type w  = wide_type(a) * wide_type(b) + wide_type(c);
      T         r  = fma(a, b, c);
      T         m  = fma(a, b, -c);
      wide_type w2 = wide_type(a) * wide_type(b) - wide_type(c);
      if ((r != T(w)) || (m != T(w2)))
      {
         if (++bad < 10)
            std::cout << std::setprecision(40) << "a = " << a << " b = " << b << " c = " << c << " got " << r << " expected " << T(w) << std::endl;
      }
   }
   BOOST_TEST_EQ(bad, 0u);
}

//
// Results that are outside the range of the type, but where the product (or the sum) is not.
//
template <class T>
void test_range()
{
   const T max = (std::numeric_limits<T>::max)();
   const T min = (std::numeric_limits<T>::min)();
   const T eps = std::numeric_limits<T>::epsilon();
   BOOST_TEST_EQ(fma(max, T(2), -max), max);
   BOOST_TEST((isinf)(fma(max, max, T(1))));
   BOOST_TEST_EQ(fma(max, max, T(1)), std::numeric_limits<T>::infinity());
   BOOST_TEST_EQ(fma(-max, max, T(1)), -std::numeric_limits<T>::infinity());
   BOOST_TEST_EQ(fma(min, min, T(0)), T(0));
   BOOST_TEST_EQ(fma(min, min, min), min);
   BOOST_TEST_EQ(fma(min, min, -min), -min);
   // The fused residual is far below min:
   const int e = std::numeric_limits<T>::min_exponent / 2;
   T         x = ldexp(1 + eps, e);
   T         z = -ldexp(1 + 2 * eps, 2 * e);
   BOOST_TEST_EQ(fma(x, x, z), T(0));
   BOOST_TEST_EQ(fma(-x, x, -z), T(0));
}

int main()
{
   // cpp_bin_float_quad, 50 decimal digits (digit_base_10) and a few other precisions, with and without expression templates:
   test_issue_case<cpp_bin_float_quad>();
   test_issue_case<cpp_bin_float_50>();
   test_issue_case<cpp_bin_float_100>();
   test_issue_case<number<backends::cpp_bin_float<113, backends::digit_base_2>, et_on> >();
   test_issue_case<number<backends::cpp_bin_float<24, backends::digit_base_2, void, std::int16_t>, et_off> >();
   test_issue_case<number<backends::cpp_bin_float<53, backends::digit_base_2, void, std::int32_t, -1022, 1023>, et_off> >();
   test_issue_case<number<backends::cpp_bin_float<200, backends::digit_base_2, std::allocator<void> >, et_off> >();

   test_range<cpp_bin_float_quad>();

   using f24 = number<backends::cpp_bin_float<24, backends::digit_base_2, void, std::int32_t, -126, 127>, et_off>;
   using f53 = number<backends::cpp_bin_float<53, backends::digit_base_2, void, std::int32_t, -1022, 1023>, et_off>;
   test_against_hardware<float, f24>(20000);
   test_against_hardware<double, f53>(20000);

   test_range<f24>();
   test_range<f53>();
   test_ties<cpp_bin_float_quad>(5000);
   test_ties<cpp_bin_float_50>(2000);
   test_ties<f24>(5000);
   test_ties<f53>(5000);
   test_against_wide<cpp_bin_float_quad>(2000);
   test_against_wide<cpp_bin_float_50>(2000);
   test_against_wide<number<backends::cpp_bin_float<200, backends::digit_base_2, std::allocator<void> >, et_off> >(500);

   return boost::report_errors();
}
