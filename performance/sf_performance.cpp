///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#define BOOST_MATH_MAX_ROOT_ITERATION_POLICY 500
#define BOOST_MATH_PROMOTE_DOUBLE_POLICY false

#if !defined(TEST_MPFR) && !defined(TEST_MPREAL) && !defined(TEST_MPF) && !defined(TEST_MPREAL) \
   && !defined(TEST_CPP_DEC_FLOAT) && !defined(TEST_MPFR_CLASS) && !defined(TEST_FLOAT)
#  define TEST_MPFR
#  define TEST_MPF
#  define TEST_CPP_DEC_FLOAT
#  define TEST_MPFR_CLASS
#  define TEST_MPREAL
#  define TEST_FLOAT
#endif

#ifdef TEST_FLOAT
#include "arithmetic_backend.hpp"
#endif
#ifdef TEST_MPFR_CLASS
#include <boost/math/bindings/mpfr.hpp>
#endif
#ifdef TEST_MPFR
#include <boost/multiprecision/mpfr.hpp>
#endif
#ifdef TEST_MPREAL
#include <boost/math/bindings/mpreal.hpp>
#endif
#ifdef TEST_MPF
#include <boost/multiprecision/gmp.hpp>
#endif
#ifdef TEST_CPP_DEC_FLOAT
#include <boost/multiprecision/cpp_dec_float.hpp>
#endif
#include <boost/math/special_functions/bessel.hpp>
#include <boost/math/tools/rational.hpp>
#include <boost/math/distributions/non_central_t.hpp>
#include <libs/math/test/table_type.hpp>
#include <boost/chrono.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>

template <class Real>
Real test_bessel();

template <class Clock>
struct stopwatch
{
   typedef typename Clock::duration duration;
   stopwatch()
   {
      m_start = Clock::now();
   }
   duration elapsed()
   {
      return Clock::now() - m_start;
   }
   void reset()
   {
      m_start = Clock::now();
   }

private:
   typename Clock::time_point m_start;
};

template <class Real>
Real test_bessel()
{
   try{
#  define T double
#  define SC_(x) x
#  include "libs/math/test/bessel_i_int_data.ipp"
#  include "libs/math/test/bessel_i_data.ipp"

      Real r;

      for(unsigned i = 0; i < bessel_i_int_data.size(); ++i)
      {
         r += boost::math::cyl_bessel_i(Real(bessel_i_int_data[i][0]), Real(bessel_i_int_data[i][1]));
      }
      for(unsigned i = 0; i < bessel_i_data.size(); ++i)
      {
         r += boost::math::cyl_bessel_i(Real(bessel_i_data[i][0]), Real(bessel_i_data[i][1]));
      }

#include "libs/math/test/bessel_j_int_data.ipp"
      for(unsigned i = 0; i < bessel_j_int_data.size(); ++i)
      {
         r += boost::math::cyl_bessel_j(Real(bessel_j_int_data[i][0]), Real(bessel_j_int_data[i][1]));
      }

#include "libs/math/test/bessel_j_data.ipp"
      for(unsigned i = 0; i < bessel_j_data.size(); ++i)
      {
         r += boost::math::cyl_bessel_j(Real(bessel_j_data[i][0]), Real(bessel_j_data[i][1]));
      }

#include "libs/math/test/bessel_j_large_data.ipp"
      for(unsigned i = 0; i < bessel_j_large_data.size(); ++i)
      {
         r += boost::math::cyl_bessel_j(Real(bessel_j_large_data[i][0]), Real(bessel_j_large_data[i][1]));
      }

#include "libs/math/test/sph_bessel_data.ipp"
      for(unsigned i = 0; i < sph_bessel_data.size(); ++i)
      {
         r += boost::math::sph_bessel(static_cast<unsigned>(sph_bessel_data[i][0]), Real(sph_bessel_data[i][1]));
      }

      return r;
   }
   catch(const std::exception& e)
   {
      std::cout << e.what() << std::endl;
   }
   return 0;
}

template <class Real>
Real test_polynomial()
{
   static const unsigned t[] = {
      2, 3, 4, 5, 6, 7, 8 };
   Real result = 0;
   for(Real k = 2; k < 1000; ++k)
      result += boost::math::tools::evaluate_polynomial(t, k);

   return result;
}

template <class Real>
Real test_nct()
{
#define T double
#include "libs/math/test/nct.ipp"

   Real result = 0;
   for(unsigned i = 0; i < nct.size(); ++i)
   {
      try{
         result += quantile(boost::math::non_central_t_distribution<Real>(nct[i][0], nct[i][1]), nct[i][3]);
      }
      catch(const std::exception&)
      {}
      result += cdf(boost::math::non_central_t_distribution<Real>(nct[i][0], nct[i][1]), nct[i][2]);
   }
   return result;
}

unsigned allocation_count = 0;

void *(*alloc_func_ptr) (size_t);
void *(*realloc_func_ptr) (void *, size_t, size_t);
void (*free_func_ptr) (void *, size_t);

void *alloc_func(size_t n)
{
   ++allocation_count;
   return (*alloc_func_ptr)(n);
}

void free_func(void * p, size_t n)
{
   (*free_func_ptr)(p, n);
}

void * realloc_func(void * p, size_t old, size_t n)
{
   ++allocation_count;
   return (*realloc_func_ptr)(p, old, n);
}

template <class Real>
void basic_allocation_test(const char* name, Real x)
{
   static const unsigned a[] = { 2, 3, 4, 5, 6, 7, 8 };
   allocation_count = 0;
   Real result = (((((a[6] * x + a[5]) * x + a[4]) * x + a[3]) * x + a[2]) * x + a[1]) * x + a[0];
   std::cout << "Allocation count for type " << name << " = " << allocation_count << std::endl;
}

template <class Real>
void poly_allocation_test(const char* name, Real x)
{
   static const unsigned a[] = { 2, 3, 4, 5, 6, 7, 8 };
   allocation_count = 0;
   Real result = boost::math::tools::evaluate_polynomial(a, x);
   std::cout << "Allocation count for type " << name << " = " << allocation_count << std::endl;
}

template <class Real>
void time_proc(const char* name, Real (*proc)(), unsigned threads = 1)
{
   try{
      static Real total = 0;
      allocation_count = 0;
      boost::chrono::duration<double> time;
      stopwatch<boost::chrono::high_resolution_clock> c;
      total += proc();
      time = c.elapsed();
      std::cout << "Time for " << name << " = " << time << std::endl;
      std::cout << "Total allocations for " << name << " = " << allocation_count << std::endl;

      for(unsigned thread_count = 1; thread_count < threads; ++thread_count)
      {
         c.reset();
         boost::thread_group g;
         for(unsigned i = 0; i <= thread_count; ++i)
            g.create_thread(proc);
         g.join_all();
         time = c.elapsed();
         std::cout << "Time for " << name << " (" << (thread_count + 1) << " threads) = " << time << std::endl;
         std::cout << "Total allocations for " << name << " = " << allocation_count << std::endl;
      }
   }
   catch(const std::exception& e)
   {
      std::cout << e.what() << std::endl;
   }
}

int main()
{
   using namespace boost::multiprecision;
#if defined(TEST_MPFR) || defined(TEST_MPFR_CLASS) || defined(TEST_MPREAL) || defined(TEST_MPF)
   mp_get_memory_functions(&alloc_func_ptr, &realloc_func_ptr, &free_func_ptr);
   mp_set_memory_functions(&alloc_func, &realloc_func, &free_func);
#endif

   std::cout << "Allocation Counts for Horner Evaluation:\n";
#ifdef TEST_MPFR
   basic_allocation_test("mpfr_float_50", mpfr_float_50(2));
   basic_allocation_test("mpfr_float_50 - no expression templates", mp_number<mpfr_float_backend<50>, false>(2));
#endif
#ifdef TEST_MPFR_CLASS
   basic_allocation_test("mpfr_class", mpfr_class(2));
#endif
#ifdef TEST_MPREAL
   basic_allocation_test("mpfr::mpreal", mpfr::mpreal(2));
#endif

   std::cout << "Allocation Counts for boost::math::tools::evaluate_polynomial:\n";
#ifdef TEST_MPFR
   poly_allocation_test("mpfr_float_50", mpfr_float_50(2));
   poly_allocation_test("mpfr_float_50 - no expression templates", mp_number<mpfr_float_backend<50>, false>(2));
#endif
#ifdef TEST_MPFR_CLASS
   poly_allocation_test("mpfr_class", mpfr_class(2));
#endif
#ifdef TEST_MPREAL
   poly_allocation_test("mpfr::mpreal", mpfr::mpreal(2));
#endif

   //
   // Comparison for builtin floats:
   //
#ifdef TEST_FLOAT
   time_proc("Bessel Functions - double", test_bessel<double>);
   time_proc("Bessel Functions - real_concept", test_bessel<boost::math::concepts::real_concept>);
   time_proc("Bessel Functions - arithmetic_backend<double>", test_bessel<mp_number<arithmetic_backend<double> > >);
   time_proc("Bessel Functions - arithmetic_backend<double> - no expression templates", test_bessel<mp_number<arithmetic_backend<double>, false> >);

   time_proc("Non-central T - double", test_nct<double>);
   time_proc("Non-central T - real_concept", test_nct<boost::math::concepts::real_concept>);
   time_proc("Non-central T - arithmetic_backend<double>", test_nct<mp_number<arithmetic_backend<double> > >);
   time_proc("Non-central T - arithmetic_backend<double> - no expression templates", test_nct<mp_number<arithmetic_backend<double>, false> >);
#endif

   //
   // 50 digits first:
   //
   std::cout << "Testing Bessel Functions at 50 digits....." << std::endl;
#if defined(TEST_MPFR) || defined(TEST_MPFR_CLASS)
   mpfr_set_default_prec(50 * 1000L / 301L);
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal::set_default_prec(50 * 1000L / 301L);
#endif
#ifdef TEST_MPFR
#if MPFR_VERSION<MPFR_VERSION_NUM(3,0,0)
   time_proc("mpfr_float_50", test_bessel<boost::multiprecision::mpfr_float_50>, 1);
#else
   time_proc("mpfr_float_50", test_bessel<boost::multiprecision::mpfr_float_50>, mpfr_buildopt_tls_p() ? 3 : 1);
#endif
#endif
#ifdef TEST_MPF
   time_proc("mpf_float_50", test_bessel<mpf_float_50>, 3);
   time_proc("mpf_float_50 (no expression templates", test_bessel<mp_number<gmp_float<50>, false> >, 3);
#endif
#ifdef TEST_CPP_DEC_FLOAT
   time_proc("cpp_dec_float_50", test_bessel<cpp_dec_float_50>, 3);
#endif
#ifdef TEST_MPFR_CLASS
   time_proc("mpfr_class", test_bessel<mpfr_class>, mpfr_buildopt_tls_p() ? 3 : 1);
#endif
#ifdef TEST_MPREAL
   time_proc("mpfr::mpreal", test_bessel<mpfr::mpreal>, mpfr_buildopt_tls_p() ? 3 : 1);
#endif
   //
   // Then 100 digits:
   //
   std::cout << "Testing Bessel Functions at 100 digits....." << std::endl;
#if defined(TEST_MPFR) || defined(TEST_MPFR_CLASS)
   mpfr_set_default_prec(100 * 1000L / 301L);
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal::set_default_prec(100 * 1000L / 301L);
#endif
#ifdef TEST_MPFR
   time_proc("mpfr_float_100", test_bessel<mpfr_float_100>);
#endif
#ifdef TEST_MPF
   time_proc("mpf_float_100", test_bessel<mpf_float_100>);
   time_proc("mpf_float_100 (no expression templates", test_bessel<mp_number<gmp_float<100>, false> >);
#endif
#ifdef TEST_CPP_DEC_FLOAT
   time_proc("cpp_dec_float_100", test_bessel<cpp_dec_float_100>);
#endif
#ifdef TEST_MPFR_CLASS
   time_proc("mpfr_class", test_bessel<mpfr_class>);
#endif
#ifdef TEST_MPREAL
   time_proc("mpfr::mpreal", test_bessel<mpfr::mpreal>);
#endif

   //
   // 50 digits first:
   //
   std::cout << "Testing Polynomial Evaluation at 50 digits....." << std::endl;
#if defined(TEST_MPFR) || defined(TEST_MPFR_CLASS)
   mpfr_set_default_prec(50 * 1000L / 301L);
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal::set_default_prec(50 * 1000L / 301L);
#endif
#ifdef TEST_MPFR
   time_proc("mpfr_float_50", test_polynomial<mpfr_float_50>);
#endif
#ifdef TEST_MPF
   time_proc("mpf_float_50", test_polynomial<mpf_float_50>);
   time_proc("mpf_float_50 (no expression templates", test_polynomial<mp_number<gmp_float<50>, false> >);
#endif
#ifdef TEST_CPP_DEC_FLOAT
   time_proc("cpp_dec_float_50", test_polynomial<cpp_dec_float_50>);
#endif
#ifdef TEST_MPFR_CLASS
   time_proc("mpfr_class", test_polynomial<mpfr_class>);
#endif
#ifdef TEST_MPREAL
   time_proc("mpfr::mpreal", test_polynomial<mpfr::mpreal>);
#endif
   //
   // Then 100 digits:
   //
   std::cout << "Testing Polynomial Evaluation at 100 digits....." << std::endl;
#ifdef TEST_MPFR_CLASS
   mpfr_set_default_prec(100 * 1000L / 301L);
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal::set_default_prec(100 * 1000L / 301L);
#endif
#ifdef TEST_MPFR
   time_proc("mpfr_float_100", test_polynomial<mpfr_float_100>);
#endif
#ifdef TEST_MPF
   time_proc("mpf_float_100", test_polynomial<mpf_float_100>);
   time_proc("mpf_float_100 (no expression templates", test_polynomial<mp_number<gmp_float<100>, false> >);
#endif
#ifdef TEST_CPP_DEC_FLOAT
   time_proc("cpp_dec_float_100", test_polynomial<cpp_dec_float_100>);
#endif
#ifdef TEST_MPFR_CLASS
   time_proc("mpfr_class", test_polynomial<mpfr_class>);
#endif
#ifdef TEST_MPREAL
   time_proc("mpfr::mpreal", test_polynomial<mpfr::mpreal>);
#endif
   //
   // 50 digits first:
   //
   std::cout << "Testing Non-Central T at 50 digits....." << std::endl;
#ifdef TEST_MPFR_CLASS
   mpfr_set_default_prec(50 * 1000L / 301L);
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal::set_default_prec(50 * 1000L / 301L);
#endif
#ifdef TEST_MPFR
   time_proc("mpfr_float_50", test_nct<mpfr_float_50>);
#endif
#ifdef TEST_MPF
   time_proc("mpf_float_50", test_nct<mpf_float_50>);
   time_proc("mpf_float_50 (no expression templates", test_nct<mp_number<gmp_float<50>, false> >);
#endif
#ifdef TEST_CPP_DEC_FLOAT
   time_proc("cpp_dec_float_50", test_nct<cpp_dec_float_50>);
#endif
#ifdef TEST_MPFR_CLASS
   time_proc("mpfr_class", test_nct<mpfr_class>);
#endif
#ifdef TEST_MPREAL
   time_proc("mpfr::mpreal", test_nct<mpfr::mpreal>);
#endif
   //
   // Then 100 digits:
   //
   std::cout << "Testing Non-Central T at 100 digits....." << std::endl;
#ifdef TEST_MPFR_CLASS
   mpfr_set_default_prec(100 * 1000L / 301L);
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal::set_default_prec(100 * 1000L / 301L);
#endif
#ifdef TEST_MPFR
   time_proc("mpfr_float_100", test_nct<mpfr_float_100>);
#endif
#ifdef TEST_MPF
   time_proc("mpf_float_100", test_nct<mpf_float_100>);
   time_proc("mpf_float_100 (no expression templates", test_nct<mp_number<gmp_float<100>, false> >);
#endif
#ifdef TEST_CPP_DEC_FLOAT
   time_proc("cpp_dec_float_100", test_nct<cpp_dec_float_100>);
#endif
#ifdef TEST_MPFR_CLASS
   time_proc("mpfr_class", test_nct<mpfr_class>);
#endif
#ifdef TEST_MPREAL
   time_proc("mpfr::mpreal", test_nct<mpfr::mpreal>);
#endif
}

