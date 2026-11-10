// Copyright 2026 loaff123.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)
//
// https://github.com/boostorg/multiprecision/issues/639

#include <boost/multiprecision/float128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cstdlib>
#include <initializer_list>

template <class T>
void check_remainder(const T& actual, const T& expected)
{
   BOOST_TEST_EQ(actual, expected);
   if (expected == 0)
      BOOST_TEST_EQ(signbit(actual), signbit(expected));
}

void check_quotient(int actual, int expected, bool negative)
{
   // remquo only guarantees at least the low three bits of the quotient.
   BOOST_TEST_EQ(std::abs(actual) % 8, std::abs(expected) % 8);
   if (actual != 0)
      BOOST_TEST_EQ(actual < 0, negative);
}

template <class T>
void test()
{
   // x, y, x - n*y, n: n is the nearest integer to x/y, ties to even.
   const int cases[][4] = {
       {180, 360, 180, 0}, {540, 360, -180, 2},
       {900, 360, 180, 2}, {1260, 360, -180, 4},
       {179, 360, 179, 0}, {181, 360, -179, 1},
       {720, 360, 0, 2}, {9000, 360, 0, 25}, {0, 360, 0, 0}};

   for (const auto& c : cases)
   {
      for (int sx : {-1, 1})
      {
         for (int sy : {-1, 1})
         {
            const int ix = sx * c[0];
            const int iy = sy * c[1];
            const T x = ix;
            const T y = iy;
            T expected = sx * c[2];
            if ((expected == 0) && (ix < 0))
               expected = -expected;
            const int expected_quotient = sx * sy * c[3];
            const bool negative_quotient = (ix < 0) != (iy < 0);

            check_remainder(T(remainder(x, y)), expected);
            check_remainder(T(remainder(x, iy)), expected);
            check_remainder(T(remainder(ix, y)), expected);
            check_remainder(T(remainder(x * 1, iy)), expected);
            check_remainder(T(remainder(ix, y * 1)), expected);

            int q = 0;
            check_remainder(T(remquo(x, y, &q)), expected);
            check_quotient(q, expected_quotient, negative_quotient);
            check_remainder(T(remquo(x, iy, &q)), expected);
            check_quotient(q, expected_quotient, negative_quotient);
            check_remainder(T(remquo(ix, y, &q)), expected);
            check_quotient(q, expected_quotient, negative_quotient);
            check_remainder(T(remquo(x * 1, iy, &q)), expected);
            check_quotient(q, expected_quotient, negative_quotient);
            check_remainder(T(remquo(ix, y * 1, &q)), expected);
            check_quotient(q, expected_quotient, negative_quotient);
         }
      }
   }

   // An integer cannot represent negative zero, so test this separately.
   const T negative_zero = -T(0);
   const T divisor = 360;
   for (int sign : {-1, 1})
   {
      int q = 0;
      check_remainder(T(remainder(negative_zero, sign * 360)), negative_zero);
      check_remainder(T(remainder(negative_zero, divisor * sign)), negative_zero);
      check_remainder(T(remquo(negative_zero, sign * 360, &q)), negative_zero);
      check_quotient(q, 0, sign > 0);
   }
}

int main()
{
   using namespace boost::multiprecision;
   test<number<float128_backend, et_off>>();
   test<number<float128_backend, et_on>>();
   return boost::report_errors();
}
