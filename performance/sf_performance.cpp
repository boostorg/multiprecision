///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#define BOOST_CHRONO_HEADER_ONLY
#define BOOST_MATH_MAX_ROOT_ITERATION_POLICY 500

#if !defined(TEST_MPFR) && !defined(TEST_MP_REAL) && !defined(TEST_MPF)
#  define TEST_MPFR
#  define TEST_MPF
#  define TEST_MPF
#  define TEST_CPP_FLOAT
#  define TEST_MPFR_CLASS
#endif

#ifdef TEST_MPFR
#include <boost/math/bindings/mpfr.hpp>
#include <boost/multiprecision/mpfr.hpp>
#endif
#ifdef TEST_MP_REAL
#include <boost/math/bindings/mpreal.hpp>
#endif
#ifdef TEST_MPF
#include <boost/multiprecision/gmp.hpp>
#endif
#ifdef TEST_CPP_FLOAT
#include <boost/multiprecision/cpp_float.hpp>
#endif
#include <boost/math/special_functions/bessel.hpp>
#include <boost/math/tools/rational.hpp>
#include <boost/math/distributions/non_central_t.hpp>
#include <libs/math/test/table_type.hpp>
#include <boost/chrono.hpp>
#include <boost/array.hpp>

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

int main()
{
   mp_get_memory_functions(&alloc_func_ptr, &realloc_func_ptr, &free_func_ptr);
   mp_set_memory_functions(&alloc_func, &realloc_func, &free_func);

   static const unsigned a[] = {
      2, 3, 4, 5, 6, 7, 8 };
#ifdef TEST_MPFR
   boost::multiprecision::mpfr_float_50 result, x(2);
   allocation_count = 0;
   result = (((((a[6] * x + a[5]) * x + a[4]) * x + a[3]) * x + a[2]) * x + a[1]) * x + a[0];
   std::cout << allocation_count << std::endl;
#endif
#ifdef TEST_MPFR_CLASS
   mpfr_class r, x2(2);
   allocation_count = 0;
   r = (((((a[6] * x2 + a[5]) * x2 + a[4]) * x2 + a[3]) * x2 + a[2]) * x2 + a[1]) * x2 + a[0];
   std::cout << allocation_count << std::endl;
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal r2, x3(2);
   allocation_count = 0;
   r2 = (((((a[6] * x3 + a[5]) * x3 + a[4]) * x3 + a[3]) * x3 + a[2]) * x3 + a[1]) * x3 + a[0];
   std::cout << allocation_count << std::endl;
#endif

   allocation_count = 0;
#ifdef TEST_MPFR
   result = boost::math::tools::evaluate_polynomial(a, x);
   std::cout << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPFR_CLASS
   r = boost::math::tools::evaluate_polynomial(a, x2);
   std::cout << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPREAL
   r2 = boost::math::tools::evaluate_polynomial(a, x3);
   std::cout << allocation_count << std::endl;
#endif

   boost::chrono::duration<double> time;
   stopwatch<boost::chrono::high_resolution_clock> c;
   //
   // 50 digits first:
   //
   std::cout << "Testing Bessel Functions....." << std::endl;
#if defined(TEST_MPFR) || defined(TEST_MPFR_CLASS)
   mpfr_set_default_prec(50 * 1000L / 301L);
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal::set_default_prec(50 * 1000L / 301L);
#endif
#ifdef TEST_MPFR
   c.reset();
   test_bessel<boost::multiprecision::mpfr_float_50>();
   time = c.elapsed();
   std::cout << "Time for mpfr_float_50 = " << time << std::endl;
   std::cout << "Total allocations for mpfr_float_50 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPF
   c.reset();
   test_bessel<boost::multiprecision::mpf_float_50>();
   time = c.elapsed();
   std::cout << "Time for mpf_float_50 = " << time << std::endl;
   std::cout << "Total allocations for mpf_float_50 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_CPP_FLOAT
   c.reset();
   test_bessel<boost::multiprecision::cpp_float_50>();
   time = c.elapsed();
   std::cout << "Time for cpp_float_50 = " << time << std::endl;
   std::cout << "Total allocations for cpp_float_50 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPFR_CLASS
   c.reset();
   test_bessel<mpfr_class>();
   time = c.elapsed();
   std::cout << "Time for mpfr_class (50 digits) = " << time << std::endl;
   std::cout << "Total allocations for mpfr_class (50 digits) = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MP_REAL
   c.reset();
   test_bessel<mpfr::mpreal>();
   time = c.elapsed();
   std::cout << "Time for mpreal (50 digits) = " << time << std::endl;
   std::cout << "Total allocations for mpreal (50 digits) = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
   //
   // Then 100 digits:
   //
#if defined(TEST_MPFR) || defined(TEST_MPFR_CLASS)
   mpfr_set_default_prec(100 * 1000L / 301L);
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal::set_default_prec(100 * 1000L / 301L);
#endif
#ifdef TEST_MPFR
   c.reset();
   test_bessel<boost::multiprecision::mpfr_float_100>();
   time = c.elapsed();
   std::cout << "Time for mpfr_float_100 = " << time << std::endl;
   std::cout << "Total allocations for mpfr_float_50 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPF
   c.reset();
   test_bessel<boost::multiprecision::mpf_float_100>();
   time = c.elapsed();
   std::cout << "Time for mpf_float_100 = " << time << std::endl;
   std::cout << "Total allocations for mpf_float_100 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_CPP_FLOAT
   c.reset();
   test_bessel<boost::multiprecision::cpp_float_100>();
   time = c.elapsed();
   std::cout << "Time for cpp_float_100 = " << time << std::endl;
   std::cout << "Total allocations for cpp_float_100 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPFR_CLASS
   c.reset();
   test_bessel<mpfr_class>();
   time = c.elapsed();
   std::cout << "Time for mpfr_class (100 digits) = " << time << std::endl;
   std::cout << "Total allocations for mpfr_class (100 digits) = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPREAL
   c.reset();
   test_bessel<mpfr::mpreal>();
   time = c.elapsed();
   std::cout << "Time for mpreal (100 digits) = " << time << std::endl;
   std::cout << "Total allocations for mpreal (100 digits) = " << allocation_count << std::endl;
   allocation_count = 0;
#endif

   //
   // 50 digits first:
   //
   c.reset();
   std::cout << "Testing Polynomial Evaluation....." << std::endl;
#if defined(TEST_MPFR) || defined(TEST_MPFR_CLASS)
   mpfr_set_default_prec(50 * 1000L / 301L);
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal::set_default_prec(50 * 1000L / 301L);
#endif
#ifdef TEST_MPFR
   c.reset();
   test_polynomial<boost::multiprecision::mpfr_float_50>();
   time = c.elapsed();
   std::cout << "Time for mpfr_float_50 = " << time << std::endl;
   std::cout << "Total allocations for mpfr_float_50 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPF
   c.reset();
   test_polynomial<boost::multiprecision::mpf_float_50>();
   time = c.elapsed();
   std::cout << "Time for mpf_float_50 = " << time << std::endl;
   std::cout << "Total allocations for mpf_float_50 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_CPP_FLOAT
   c.reset();
   test_polynomial<boost::multiprecision::cpp_float_50>();
   time = c.elapsed();
   std::cout << "Time for cpp_float_50 = " << time << std::endl;
   std::cout << "Total allocations for cpp_float_50 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPFR_CLASS
   c.reset();
   test_polynomial<mpfr_class>();
   time = c.elapsed();
   std::cout << "Time for mpfr_class (50 digits) = " << time << std::endl;
   std::cout << "Total allocations for mpfr_class (50 digits) = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPREAL
   c.reset();
   test_polynomial<mpfr::mpreal>();
   time = c.elapsed();
   std::cout << "Time for mpreal (50 digits) = " << time << std::endl;
   std::cout << "Total allocations for mpreal (50 digits = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
   //
   // Then 100 digits:
   //
#ifdef TEST_MPFR_CLASS
   mpfr_set_default_prec(100 * 1000L / 301L);
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal::set_default_prec(100 * 1000L / 301L);
#endif
#ifdef TEST_MPFR
   c.reset();
   test_polynomial<boost::multiprecision::mpfr_float_100>();
   time = c.elapsed();
   std::cout << "Time for mpfr_float_100 = " << time << std::endl;
   std::cout << "Total allocations for mpfr_float_100 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPF
   c.reset();
   test_polynomial<boost::multiprecision::mpf_float_100>();
   time = c.elapsed();
   std::cout << "Time for mpf_float_100 = " << time << std::endl;
   std::cout << "Total allocations for mpf_float_100 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_CPP_FLOAT
   c.reset();
   test_polynomial<boost::multiprecision::cpp_float_100>();
   time = c.elapsed();
   std::cout << "Time for cpp_float_100 = " << time << std::endl;
   std::cout << "Total allocations for cpp_float_100 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPFR_CLASS
   c.reset();
   test_polynomial<mpfr_class>();
   time = c.elapsed();
   std::cout << "Time for mpfr_class (100 digits) = " << time << std::endl;
   std::cout << "Total allocations for mpfr_class (100 digits) = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPREAL
   c.reset();
   test_polynomial<mpfr::mpreal>();
   time = c.elapsed();
   std::cout << "Time for mpreal (100 digits) = " << time << std::endl;
   std::cout << "Total allocations for mpreal (100 digits) = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
   //
   // 50 digits first:
   //
   c.reset();
   std::cout << "Testing Non-Central T....." << std::endl;
#ifdef TEST_MPFR_CLASS
   mpfr_set_default_prec(50 * 1000L / 301L);
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal::set_default_prec(50 * 1000L / 301L);
#endif
#ifdef TEST_MPFR
   c.reset();
   test_nct<boost::multiprecision::mpfr_float_50>();
   time = c.elapsed();
   std::cout << "Time for mpfr_float_50 = " << time << std::endl;
   std::cout << "Total allocations for mpfr_float_50 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPF
   c.reset();
   test_nct<boost::multiprecision::mpf_float_50>();
   time = c.elapsed();
   std::cout << "Time for mpf_float_50 = " << time << std::endl;
   std::cout << "Total allocations for mpf_float_50 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_CPP_FLOAT
   c.reset();
   test_nct<boost::multiprecision::cpp_float_50>();
   time = c.elapsed();
   std::cout << "Time for cpp_float_50 = " << time << std::endl;
   std::cout << "Total allocations for cpp_float_50 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPFR_CLASS
   c.reset();
   test_nct<mpfr_class>();
   time = c.elapsed();
   std::cout << "Time for mpfr_class (50 digits) = " << time << std::endl;
   std::cout << "Total allocations for mpfr_class (50 digits) = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MP_REAL
   c.reset();
   test_nct<mpfr::mpreal>();
   time = c.elapsed();
   std::cout << "Time for mpreal (50 digits) = " << time << std::endl;
   std::cout << "Total allocations for mpreal (50 digits) = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
   //
   // Then 100 digits:
   //
#ifdef TEST_MPFR_CLASS
   mpfr_set_default_prec(100 * 1000L / 301L);
#endif
#ifdef TEST_MPREAL
   mpfr::mpreal::set_default_prec(100 * 1000L / 301L);
#endif
#ifdef TEST_MPFR
   c.reset();
   test_nct<boost::multiprecision::mpfr_float_100>();
   time = c.elapsed();
   std::cout << "Time for mpfr_float_100 = " << time << std::endl;
   std::cout << "Total allocations for mpfr_float_100 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPF
   c.reset();
   test_nct<boost::multiprecision::mpf_float_100>();
   time = c.elapsed();
   std::cout << "Time for mpf_float_100 = " << time << std::endl;
   std::cout << "Total allocations for mpf_float_100 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_CPP_FLOAT
   c.reset();
   test_nct<boost::multiprecision::cpp_float_100>();
   time = c.elapsed();
   std::cout << "Time for cpp_float_100 = " << time << std::endl;
   std::cout << "Total allocations for cpp_float_100 = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPFR_CLASS
   c.reset();
   test_nct<mpfr_class>();
   time = c.elapsed();
   std::cout << "Time for mpfr_class (100 digits) = " << time << std::endl;
   std::cout << "Total allocations for mpfr_class (100 digits) = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
#ifdef TEST_MPREAL
   c.reset();
   test_nct<mpfr::mpreal>();
   time = c.elapsed();
   std::cout << "Time for mpreal (100 digits) = " << time << std::endl;
   std::cout << "Total allocations for mpreal (100 digits) = " << allocation_count << std::endl;
   allocation_count = 0;
#endif
}

