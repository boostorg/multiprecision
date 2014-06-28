// Copyright John Maddock 2013.

// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef _MSC_VER
#  define _SCL_SECURE_NO_WARNINGS
#endif

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/math/special_functions/next.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/chrono.hpp>
#include "test.hpp"
#include <boost/array.hpp>
#include <iostream>
#include <iomanip>

#ifdef BOOST_MSVC
#pragma warning(disable:4127)
#endif

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

template <class T>
struct exponent_type 
{
   typedef int type;
};
template <class T, boost::multiprecision::expression_template_option ET>
struct exponent_type<boost::multiprecision::number<T, ET> >
{
   typedef typename T::exponent_type type;
};

template <class T>
T generate_random_float()
{
   BOOST_MATH_STD_USING
   typedef typename exponent_type<T>::type e_type;
   static boost::random::mt19937 gen;
   T val = gen();
   T prev_val = -1;
   while(val != prev_val)
   {
      val *= (gen.max)();
      prev_val = val;
      val += gen();
   }
   e_type e;
   val = frexp(val, &e);

   static boost::random::uniform_int_distribution<e_type> ui(0, std::numeric_limits<T>::max_exponent - std::numeric_limits<T>::digits - 20);
   return ldexp(val, ui(gen));
}

template <class T>
void do_round_trip(const T& val)
{
   boost::multiprecision::cpp_rational rat(val);
   T new_f(rat);
   BOOST_CHECK_EQUAL(val, new_f);
   //
   // Try adding or subtracting an insignificant amount
   // (0.25ulp) from rat and check that it rounds to the same value:
   //
   typename exponent_type<T>::type e;
   frexp(val, &e);
   e -= std::numeric_limits<T>::digits + 2;
   boost::multiprecision::cpp_rational delta, rounded;
   boost::multiprecision::cpp_int i(1);
   i <<= (e < 0 ? -e : e);
   if(e > 0)
      delta.assign(i);
   else
      delta = boost::multiprecision::cpp_rational(boost::multiprecision::cpp_int(1), i);
   rounded = rat + delta;
   new_f = static_cast<T>(rounded);
   BOOST_CHECK_EQUAL(val, new_f);
   rounded = rat - delta;
   new_f = static_cast<T>(rounded);
   BOOST_CHECK_EQUAL(val, new_f);

   delta /= 2;
   rounded = rat + delta;
   new_f = static_cast<T>(rounded);
   BOOST_CHECK_EQUAL(val, new_f);
   rounded = rat - delta;
   new_f = static_cast<T>(rounded);
   BOOST_CHECK_EQUAL(val, new_f);

   delta /= 2;
   rounded = rat + delta;
   new_f = static_cast<T>(rounded);
   BOOST_CHECK_EQUAL(val, new_f);
   rounded = rat - delta;
   new_f = static_cast<T>(rounded);
   BOOST_CHECK_EQUAL(val, new_f);
}

template <class T>
void test_round_trip()
{
   std::cout << "Testing type " << typeid(T).name() << std::endl;
   std::cout << "digits = " << std::numeric_limits<T>::digits << std::endl;
   std::cout << "digits10 = " << std::numeric_limits<T>::digits10 << std::endl;
   std::cout << "max_digits10 = " << std::numeric_limits<T>::max_digits10 << std::endl;

   stopwatch<boost::chrono::high_resolution_clock> w;

   int count = 0;

   while(boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count() < 200)
   {
      T val = generate_random_float<T>();
      do_round_trip(val);
      do_round_trip(T(-val));
      do_round_trip(T(1/val));
      do_round_trip(T(-1/val));
      count += 4;
   }

   std::cout << "Execution time = " << boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count() << "s" << std::endl;
   std::cout << "Total values tested: " << count << std::endl;
}

boost::multiprecision::cpp_int generate_random_int()
{
   static boost::random::mt19937 gen;
   static boost::random::uniform_int_distribution<boost::random::mt19937::result_type> d(1, 20);

   int lim;
   boost::multiprecision::cpp_int cppi(0);

   lim = d(gen);

   for(int i = 0; i < lim; ++i)
   {
      cppi *= (gen.max)();
      cppi += gen();
   }
   return cppi;
}

template <class Float>
void test_random_rationals()
{
   stopwatch<boost::chrono::high_resolution_clock> w;

   int count = 0;

   while(boost::chrono::duration_cast<boost::chrono::duration<double>>(w.elapsed()).count() < 200)
   {
      boost::multiprecision::cpp_rational rat(generate_random_int(), generate_random_int());
      Float f(rat);
      boost::multiprecision::cpp_rational new_rat(f); // rounded value
      int c = new_rat.compare(rat);
      if(c < 0)
      {
         // If f was rounded down, next float up must be above the original value:
         f = boost::math::float_next(f);
         new_rat.assign(f);
         BOOST_CHECK(new_rat >= rat);
      }
      else if(c > 0)
      {
         // If f was rounded up, next float down must be below the original value:
         f = boost::math::float_prior(f);
         new_rat.assign(f);
         BOOST_CHECK(new_rat <= rat);
      }
      else
      {
         // Values were equal... nothing to test.
      }
   }

   std::cout << "Execution time = " << boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count() << "s" << std::endl;
   std::cout << "Total values tested: " << count << std::endl;
}

int main()
{
   using namespace boost::multiprecision;
#if defined(TEST1) && !defined(BOOST_MSVC)
   test_round_trip<number<cpp_bin_float<113, digit_base_2, void, boost::int16_t> > >();
#elif defined(TEST2)
   test_round_trip<double>();
#elif defined(TEST3) && !defined(BOOST_MSVC)
   test_random_rationals<number<cpp_bin_float<113, digit_base_2, void, boost::int16_t> > >();
#elif defined(TEST4)
   test_random_rationals<double>();
#endif
   return boost::report_errors();
}

