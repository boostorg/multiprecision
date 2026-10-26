///////////////////////////////////////////////////////////////////////////////
//  Copyright 2026 Treston Cuzzort.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  https://www.boost.org/LICENSE_1_0.txt)

#if defined(TEST_MPC)
#include <boost/multiprecision/mpc.hpp>
#elif defined(TEST_MPFR)
#include <boost/multiprecision/mpfr.hpp>
#elif defined(TEST_GMP)
#include <boost/multiprecision/gmp.hpp>
#else
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_complex.hpp>
#endif
#include <boost/multiprecision/eigen.hpp>
#include <boost/core/lightweight_test.hpp>
#include <sstream>

template <class Num>
void test_output(const Num& value)
{
   using std::abs;
   using Real = typename Eigen::NumTraits<Num>::Real;
   BOOST_TEST_EQ(Eigen::NumTraits<decltype(value + value)>::max_digits10(), Eigen::NumTraits<Num>::max_digits10());
   Eigen::Matrix<Num, 1, 1> matrix;
   matrix(0, 0) = value;

   // Even the default stream format needs max_digits10 in Eigen 5.
   std::ostringstream ordinary;
   ordinary << matrix;
   BOOST_TEST(!ordinary.str().empty());

   // FullPrecision should preserve the value and restore the stream precision.
   std::ostringstream output;
   output.precision(8);
   output << matrix.format(Eigen::IOFormat(Eigen::FullPrecision, Eigen::DontAlignCols));
   BOOST_TEST_EQ(output.precision(), 8);
#if !EIGEN_VERSION_AT_LEAST(3, 4, 90)
   // Eigen 3 uses digits10 for FullPrecision. Request round-trip precision explicitly.
   output.str("");
   output << matrix.format(Eigen::IOFormat(Eigen::NumTraits<Num>::max_digits10(), Eigen::DontAlignCols));
   BOOST_TEST_EQ(output.precision(), 8);
#endif
   Num restored;
   std::istringstream input(output.str());
   input >> restored;
   BOOST_TEST(!input.fail());
   BOOST_TEST(abs(Num(restored - value)) <= Eigen::NumTraits<Num>::epsilon() * Real(abs(value)));
}

template <class Num>
void test_fixed()
{
   using Real = typename Eigen::NumTraits<Num>::Real;
   BOOST_TEST_EQ(Eigen::NumTraits<Num>::max_digits10(), std::numeric_limits<Real>::max_digits10);
   test_output(Num(Num(1) / 7));
   test_output(Num(Num(-1) / 3));
}

#if defined(TEST_GMP) || defined(TEST_MPFR) || defined(TEST_MPC)
template <class Num>
void test_variable()
{
   using Real = typename Eigen::NumTraits<Num>::Real;
   const unsigned old_precision = Num::thread_default_precision();
   const unsigned old_real_precision = Real::thread_default_precision();
   int previous = 0;
   for (unsigned precision : {20u, 50u, 100u})
   {
      Num::thread_default_precision(precision);
      Real::thread_default_precision(precision);
      const int digits = Eigen::NumTraits<Num>::max_digits10();
      BOOST_TEST_GT(digits, static_cast<int>(precision));
      BOOST_TEST_LT(digits, static_cast<int>(precision + 30));
      BOOST_TEST_GT(digits, previous);
      previous = digits;
      test_output(Num(Num(1) / 7));
      test_output(Num(Num(-1) / 3));
   }
   Real::thread_default_precision(old_real_precision);
   Num::thread_default_precision(old_precision);
}
#endif

int main()
{
   using namespace boost::multiprecision;
#if defined(TEST_MPC)
   test_fixed<mpc_complex_50>();
   test_variable<mpc_complex>();
   test_output(mpc_complex_50(mpfr_float_50(1) / 7, -mpfr_float_50(1) / 3));
#elif defined(TEST_MPFR)
   test_fixed<mpfr_float_50>();
   test_variable<mpfr_float>();
   test_variable<number<mpfr_float_backend<0>, et_off> >();
#elif defined(TEST_GMP)
   test_fixed<mpf_float_50>();
   test_variable<mpf_float>();
   test_variable<number<gmp_float<0>, et_off> >();
#else
   test_fixed<cpp_bin_float_50>();
   test_fixed<cpp_bin_float_double>();
   test_fixed<cpp_dec_float_50>();
   test_fixed<cpp_complex_50>();
   test_output(cpp_complex_50(cpp_bin_float_50(1) / 7, -cpp_bin_float_50(1) / 3));
   test_fixed<number<cpp_bin_float<50>, et_on> >();
   test_fixed<number<cpp_dec_float<50>, et_off> >();
   test_output(cpp_int("123456789012345678901234567890"));
   test_output(cpp_rational(cpp_rational(1) / 7));
#endif
   return boost::report_errors();
}
