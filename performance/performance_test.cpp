///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#define BOOST_CHRONO_HEADER_ONLY

#ifdef _MSC_VER
#  define _SCL_SECURE_NO_WARNINGS
#endif

#if !defined(TEST_MPF) && !defined(TEST_MPZ) && \
   !defined(TEST_CPP_DEC_FLOAT) && !defined(TEST_MPFR) && !defined(TEST_MPQ) \
   && !defined(TEST_TOMMATH) && !defined(TEST_TOMMATH_BOOST_RATIONAL) && !defined(TEST_MPZ_BOOST_RATIONAL)\
   && !defined(TEST_FIXED_INT)
#  define TEST_MPF
#  define TEST_MPZ
#  define TEST_MPQ
#  define TEST_MPFR
#  define TEST_CPP_DEC_FLOAT
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
#ifdef TEST_CPP_DEC_FLOAT
#include <boost/multiprecision/cpp_dec_float.hpp>
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
#include <map>
#include <string>
#include <cstring>
#include <cctype>
#include <iostream>
#include <iomanip>
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
         for(unsigned k = 0; k < b.size(); ++k)
            a[k] = b[k] * c[k];
      }
      return boost::chrono::duration_cast<boost::chrono::duration<double> >(w.elapsed()).count();
   }
   double test_multiply_int()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] * 3;
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
   double test_divide_int()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] / 3;
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
   // The following tests only work for integer types:
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
   double test_mod_int()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] % 254;
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
   double test_or_int()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] | 234;
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
   double test_and_int()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] & 234;
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
   double test_xor_int()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = b[i] ^ 234;
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
   double test_gcd()
   {
      stopwatch<boost::chrono::high_resolution_clock> w;
      for(unsigned i = 0; i < 1000; ++i)
      {
         for(unsigned i = 0; i < b.size(); ++i)
            a[i] = gcd(b[i], c[i]);
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
      return ldexp(val, static_cast<int>(ui(gen)));
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
   T generate_random(const boost::mpl::int_<boost::multiprecision::number_kind_rational>&)
   {
      typedef boost::random::mt19937::result_type random_type;
      typedef typename boost::multiprecision::component_type<T>::type IntType;

      IntType max_val;
      unsigned digits;
      if(std::numeric_limits<IntType>::is_bounded)
      {
         max_val = (std::numeric_limits<IntType>::max)();
         digits = std::numeric_limits<IntType>::digits;
      }
      else
      {
         max_val = IntType(1) << bits_wanted;
         digits = bits_wanted;
      }

      unsigned bits_per_r_val = std::numeric_limits<random_type>::digits - 1;
      while((random_type(1) << bits_per_r_val) > (gen.max)()) --bits_per_r_val;

      unsigned terms_needed = digits / bits_per_r_val + 1;

      IntType val = 0;
      IntType denom = 0;
      for(unsigned i = 0; i < terms_needed; ++i)
      {
         val *= (gen.max)();
         val += gen();
      }
      for(unsigned i = 0; i < terms_needed; ++i)
      {
         denom *= (gen.max)();
         denom += gen();
      }
      if(denom == 0)
         denom = 1;
      val %= max_val;
      denom %= max_val;
      return T(val, denom);
   }
   std::vector<T> a, b, c, small;
   static boost::random::mt19937 gen;
};

template <class N, int V>
boost::random::mt19937 tester<N, V>::gen;

const char* category_name(const boost::mpl::int_<boost::multiprecision::number_kind_integer>&)
{
   return "integer";
}
const char* category_name(const boost::mpl::int_<boost::multiprecision::number_kind_floating_point>&)
{
   return "float";
}
const char* category_name(const boost::mpl::int_<boost::multiprecision::number_kind_rational>&)
{
   return "rational";
}

//
// Keys in order are:
// Category
// Operator
// Type
// Precision
// Time
//
std::map<std::string, std::map<std::string, std::map<std::string, std::map<int, double> > > > result_table;

void report_result(const char* cat, const char* type, const char* op, unsigned precision, double time)
{
   std::cout << std::left << std::setw(15) << type << std::setw(10) << precision << std::setw(5) << op << time << std::endl;
   result_table[cat][op][type][precision] = time;
}

template <class Number, int N>
void test_int_ops(tester<Number, N>& t, const char* type, unsigned precision, const boost::mpl::int_<boost::multiprecision::number_kind_integer>&)
{
   const char* cat = "integer";
   report_result(cat, type, "%", precision, t.test_mod());
   report_result(cat, type, "|", precision, t.test_or());
   report_result(cat, type, "&", precision, t.test_and());
   report_result(cat, type, "^", precision, t.test_xor());
   //report_result(cat, type, "~", precision, t.test_complement());
   report_result(cat, type, "<<", precision, t.test_left_shift());
   report_result(cat, type, ">>", precision, t.test_right_shift());
   // integer ops:
   report_result(cat, type, "%(int)", precision, t.test_mod_int());
   report_result(cat, type, "|(int)", precision, t.test_or_int());
   report_result(cat, type, "&(int)", precision, t.test_and_int());
   report_result(cat, type, "^(int)", precision, t.test_xor_int());
   report_result(cat, type, "gcd", precision, t.test_gcd());
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
   const char* cat = category_name(typename boost::multiprecision::number_category<Number>::type());
   //
   // call t.test_multiply() first so that the destination operands are
   // forced to perform whatever memory allocation may be needed.  That way
   // we measure only algorithm performance, and not memory allocation effects.
   //
   t.test_multiply();
   //
   // Now the actual tests:
   //
   report_result(cat, type, "+", precision, t.test_add());
   report_result(cat, type, "-", precision, t.test_subtract());
   report_result(cat, type, "*", precision, t.test_multiply());
   report_result(cat, type, "/", precision, t.test_divide());
   report_result(cat, type, "str", precision, t.test_str());
   // integer ops:
   report_result(cat, type, "+(int)", precision, t.test_add_int());
   report_result(cat, type, "-(int)", precision, t.test_subtract_int());
   report_result(cat, type, "*(int)", precision, t.test_multiply_int());
   report_result(cat, type, "/(int)", precision, t.test_divide_int());
   test_int_ops(t, type, precision, typename boost::multiprecision::number_category<Number>::type());
}

void quickbook_results()
{
   //
   // Keys in order are:
   // Category
   // Operator
   // Type
   // Precision
   // Time
   //
   typedef std::map<std::string, std::map<std::string, std::map<std::string, std::map<int, double> > > >::const_iterator  category_iterator;
   typedef std::map<std::string, std::map<std::string, std::map<int, double> > >::const_iterator                          operator_iterator;
   typedef std::map<std::string, std::map<int, double> >::const_iterator                                                  type_iterator;
   typedef std::map<int, double>::const_iterator                                                                          precision_iterator;

   for(category_iterator i = result_table.begin(); i != result_table.end(); ++i)
   {
      std::string cat = i->first;
      cat[0] = std::toupper(cat[0]);
      std::cout << "[section:" << i->first << "_performance " << cat << " Type Perfomance]" << std::endl;

      for(operator_iterator j = i->second.begin(); j != i->second.end(); ++j)
      {
         std::string op = j->first;
         std::cout << "[table Operator " << op << std::endl;
         std::cout << "[[Backend]";

         for(precision_iterator k = j->second.begin()->second.begin(); k != j->second.begin()->second.end(); ++k)
         {
            std::cout << "[" << k->first << " Bits]";
         }
         std::cout << "]\n";

         std::vector<double> best_times(j->second.begin()->second.size(), (std::numeric_limits<double>::max)());
         for(unsigned m = 0; m < j->second.begin()->second.size(); ++m)
         {
            for(type_iterator k = j->second.begin(); k != j->second.end(); ++k)
            {
               precision_iterator l = k->second.begin();
               std::advance(l, m);
               if(best_times[m] > l->second)
                  best_times[m] = l->second;
            }
         }

         for(type_iterator k = j->second.begin(); k != j->second.end(); ++k)
         {
            std::cout << "[[" << k->first << "]";

            unsigned m = 0;
            for(precision_iterator l = k->second.begin(); l != k->second.end(); ++l)
            {
               double rel_time = l->second / best_times[m];
               if(rel_time == 1)
                  std::cout << "[[*" << rel_time << "]";
               else
                  std::cout << "[" << rel_time;
               std::cout << " (" << l->second << "s)]";
               ++m;
            }

            std::cout << "]\n";
         }

         std::cout << "]\n";
      }

      std::cout << "[endsect]" << std::endl;
   }
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
#ifdef TEST_MPQ
   test<boost::multiprecision::mpq_rational>("mpq_rational", 64);
   test<boost::multiprecision::mpq_rational>("mpq_rational", 128);
   test<boost::multiprecision::mpq_rational>("mpq_rational", 256);
   test<boost::multiprecision::mpq_rational>("mpq_rational", 512);
   test<boost::multiprecision::mpq_rational>("mpq_rational", 1024);
#endif
#ifdef TEST_TOMMATH
   test<boost::multiprecision::mp_int>("tommath_int", 64);
   test<boost::multiprecision::mp_int>("tommath_int", 128);
   test<boost::multiprecision::mp_int>("tommath_int", 256);
   test<boost::multiprecision::mp_int>("tommath_int", 512);
   test<boost::multiprecision::mp_int>("tommath_int", 1024);
   /*
   //
   // These are actually too slow to test!!!
   //
   test<boost::multiprecision::mp_rational>("mp_rational", 64);
   test<boost::multiprecision::mp_rational>("mp_rational", 128);
   test<boost::multiprecision::mp_rational>("mp_rational", 256);
   test<boost::multiprecision::mp_rational>("mp_rational", 512);
   test<boost::multiprecision::mp_rational>("mp_rational", 1024);
   */
#endif
#ifdef TEST_FIXED_INT
   test<boost::multiprecision::mp_number<boost::multiprecision::fixed_int<64, true> > >("fixed_int", 64);
   test<boost::multiprecision::mp_int128_t>("fixed_int", 128);
   test<boost::multiprecision::mp_int256_t>("fixed_int", 256);
   test<boost::multiprecision::mp_int512_t>("fixed_int", 512);
   test<boost::multiprecision::mp_number<boost::multiprecision::fixed_int<1024, true> > >("fixed_int", 1024);
#endif
#ifdef TEST_CPP_DEC_FLOAT
   test<boost::multiprecision::cpp_float_50>("cpp_dec_float", 50);
   test<boost::multiprecision::cpp_float_100>("cpp_dec_float", 100);
   test<boost::multiprecision::mp_number<boost::multiprecision::cpp_dec_float<500> > >("cpp_dec_float", 500);
#endif
#ifdef TEST_MPFR
   test<boost::multiprecision::mpfr_float_50>("mpfr_float", 50);
   test<boost::multiprecision::mpfr_float_100>("mpfr_float", 100);
   test<boost::multiprecision::mpfr_float_500>("mpfr_float", 500);
#endif
   quickbook_results();
   return 0;
}

