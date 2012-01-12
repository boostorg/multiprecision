///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#define BOOST_CHRONO_HEADER_ONLY

#ifdef _MSC_VER
#  define _SCL_SECURE_NO_WARNINGS
#endif

#if !defined(TEST_MPF) && !defined(TEST_MPZ) && \
   !defined(TEST_CPP_FLOAT) && !defined(TEST_MPFR) && !defined(TEST_MPQ) \
   && !defined(TEST_TOMMATH) && !defined(TEST_TOMMATH_BOOST_RATIONAL) && !defined(TEST_MPZ_BOOST_RATIONAL)\
   && !defined(TEST_FIXED_INT)
#  define TEST_MPF
#  define TEST_MPZ
#  define TEST_MPFR
#  define TEST_CPP_FLOAT
#  define TEST_MPQ
#  define TEST_TOMMATH
#  define TEST_FIXED_INT

#ifdef _MSC_VER
#pragma message("CAUTION!!: No backend type specified so testing everything.... this will take some time!!")
#endif
#ifdef __GNUC__
#pragma warning "CAUTION!!: No backend type specified so testing everything.... this will take some time!!"
#endif

#endif

#if defined(TEST_MPF) || defined(TEST_MPZ) || defined(TEST_MPQ) || defined(TEST_MPZ_BOOST_RATIONAL)
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/rational_adapter.hpp>
#endif
#ifdef TEST_CPP_FLOAT
#include <boost/multiprecision/cpp_float.hpp>
#endif
#if defined(TEST_MPFR)
#include <boost/multiprecision/mpfr.hpp>
#endif
#if defined(TEST_TOMMATH) || defined(TEST_TOMMATH_BOOST_RATIONAL)
#include <boost/multiprecision/tommath.hpp>
#include <boost/multiprecision/rational_adapter.hpp>
#endif
#if defined(TEST_FIXED_INT)
#include <boost/multiprecision/fixed_int.hpp>
#endif

#include <boost/chrono.hpp>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>

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

unsigned bits_wanted; // for integer types

template <class T, int Type>
struct tester
{
   tester()
   {
      a.assign(500, 0);
      for(int i = 0; i < 500; ++i)
      {
         b.push_back(generate_random());
         c.push_back(generate_random());
         small.push_back(gen());
      }
   }
   double test_add()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] + c[i];
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_subtract()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] - c[i];
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_add_int()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] + 1;
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_subtract_int()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] - 1;
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_multiply()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] * c[i];
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_divide()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] / c[i] + b[i] / small[i];
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_str()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < b.size(); ++i)
         a[i] = b[i].str();
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   //
   // The following tests only work for ineteger types:
   //
   double test_mod()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] % c[i] + b[i] % small[i];
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_or()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] | c[i];
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_and()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] & c[i];
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_xor()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] ^ c[i];
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_complement()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = ~b[i];
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_left_shift()
   {
      int shift = std::numeric_limits<T>::is_bounded ? std::numeric_limits<T>::digits : bits_wanted;
      shift /= 2;
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] << shift;
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_right_shift()
   {
      int shift = std::numeric_limits<T>::is_bounded ? std::numeric_limits<T>::digits : bits_wanted;
      shift /= 2;
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] >> shift;
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
private:
   T generate_random()
   {
      return generate_random(boost::mpl::int_<Type>());
   }
   T generate_random(const boost::mpl::int_<boost::multiprecision::number_kind_floating_point>&)
   {
      T val = gen();
      T prev_val = -1;
      while(val != prev_val)
      {
         val *= (gen.max)();
         prev_val = val;
         val += gen();
      }
      int e;
      val = frexp(val, &e);

      typedef typename T::backend_type::exponent_type e_type;
      static boost::random::uniform_int_distribution<e_type> ui(0, std::numeric_limits<T>::max_exponent - 10);
      return ldexp(val, ui(gen));
   }
   T generate_random(const boost::mpl::int_<boost::multiprecision::number_kind_integer>&)
   {
      typedef boost::random::mt19937::result_type random_type;

      T max_val;
      unsigned digits;
      if(std::numeric_limits<T>::is_bounded)
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
   std::vector<T> a, b, c, small;
   static boost::random::mt19937 gen;
};

template <class N, int V>
boost::random::mt19937 tester<N, V>::gen;

template <class Number, int N>
void test_int_ops(tester<Number, N>& t, const char* type, unsigned precision, const boost::mpl::int_<boost::multiprecision::number_kind_integer>&)
{
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "%" << t.test_mod() << std::endl;
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "|" << t.test_or() << std::endl;
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "&" << t.test_and() << std::endl;
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "^" << t.test_xor() << std::endl;
   //std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "~" << t.test_complement() << std::endl;
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "<<" << t.test_left_shift() << std::endl;
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << ">>" << t.test_right_shift() << std::endl;
}
template <class Number, int N, class U>
void test_int_ops(tester<Number, N>& t, const char* type, unsigned precision, const U&)
{
}

template <class Number>
void test(const char* type, unsigned precision)
{
   bits_wanted = precision;
   tester<Number, boost::multiprecision::number_category<Number>::value> t;
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "+" << t.test_add() << std::endl;
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "-" << t.test_subtract() << std::endl;
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "+ (int)" << t.test_add_int() << std::endl;
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "- (int)" << t.test_subtract_int() << std::endl;
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "*" << t.test_multiply() << std::endl;
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "/" << t.test_divide() << std::endl;
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << "str" << t.test_str() << std::endl;

   test_int_ops(t, type, precision, typename boost::multiprecision::number_category<Number>::type());
}


int main()
{
#ifdef TEST_MPF
   test<boost::multiprecision::mpf_float_50>("gmp_float", 50);
   test<boost::multiprecision::mpf_float_100>("gmp_float", 100);
   test<boost::multiprecision::mpf_float_500>("gmp_float", 500);
#endif
#ifdef TEST_MPZ
   test<boost::multiprecision::mpz_int>("gmp_int", 64);
   test<boost::multiprecision::mpz_int>("gmp_int", 128);
   test<boost::multiprecision::mpz_int>("gmp_int", 256);
   test<boost::multiprecision::mpz_int>("gmp_int", 512);
   test<boost::multiprecision::mpz_int>("gmp_int", 1024);
#endif
#ifdef TEST_TOMMATH
   test<boost::multiprecision::mp_int>("tommath_int", 64);
   test<boost::multiprecision::mp_int>("tommath_int", 128);
   test<boost::multiprecision::mp_int>("tommath_int", 256);
   test<boost::multiprecision::mp_int>("tommath_int", 512);
   test<boost::multiprecision::mp_int>("tommath_int", 1024);
#endif
#ifdef TEST_FIXED_INT
   test<boost::multiprecision::mp_number<boost::multiprecision::fixed_int<64, true> > >("mp_int64_t", 64);
   test<boost::multiprecision::mp_int128_t>("mp_int128_t", 128);
   test<boost::multiprecision::mp_int256_t>("mp_int256_t", 256);
   test<boost::multiprecision::mp_int512_t>("mp_int512_t", 512);
   test<boost::multiprecision::mp_number<boost::multiprecision::fixed_int<1024, true> > >("mp_int1024_t", 1024);
#endif
#ifdef TEST_CPP_FLOAT
   test<boost::multiprecision::cpp_float_50>("cpp_float", 50);
   test<boost::multiprecision::cpp_float_100>("cpp_float", 100);
   test<boost::multiprecision::mp_number<boost::multiprecision::cpp_float<500> > >("cpp_float", 500);
#endif
#ifdef TEST_MPFR
   test<boost::multiprecision::mpfr_float_50>("mpfr_float", 50);
   test<boost::multiprecision::mpfr_float_100>("mpfr_float", 100);
   test<boost::multiprecision::mpfr_float_500>("mpfr_float", 500);
#endif
   return 0;
}

