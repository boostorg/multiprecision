///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#define BOOST_MP_GCD_DEBUG

#include "test.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <map>
#include <tuple>
#include <boost/random.hpp>

template <class T>
std::tuple<std::vector<T>, std::vector<T>, std::vector<T> >& get_test_vector(std::size_t bits)
{
   static std::map<std::size_t, std::tuple<std::vector<T>, std::vector<T>, std::vector<T> > > data;

   std::tuple<std::vector<T>, std::vector<T>, std::vector<T> >& result = data[bits];

   if (std::get<0>(result).size() == 0)
   {
      boost::random::mt19937                     mt;
      boost::random::uniform_int_distribution<T> ui(T(1) << (bits - 1), T(1) << bits);

      std::vector<T>& a = std::get<0>(result);
      std::vector<T>& b = std::get<1>(result);
      std::vector<T>& c = std::get<2>(result);

      for (unsigned i = 0; i < 1000; ++i)
      {
         a.push_back(ui(mt));
         b.push_back(ui(mt));
         if (b.back() > a.back())
            b.back().swap(a.back());
         c.push_back(0);
      }
   }
   return result;
}

template <class T>
std::vector<T>& get_test_vector_a(std::size_t bits)
{
   return std::get<0>(get_test_vector<T>(bits));
}
template <class T>
std::vector<T>& get_test_vector_b(std::size_t bits)
{
   return std::get<1>(get_test_vector<T>(bits));
}
template <class T>
std::vector<T>& get_test_vector_c(std::size_t bits)
{
   return std::get<2>(get_test_vector<T>(bits));
}

template <class T>
T gcd_euler(T u, T v)
{
   while (v)
   {
      T t(v);
      v = u % v;
      u = t;
   }
   return u;
}

namespace boost {
namespace multiprecision {
namespace backends {

unsigned total_lehmer_gcd_calls = 0;
unsigned total_lehmer_gcd_bits_saved = 0;
unsigned total_lehmer_gcd_cycles = 0;

}}}


int main()
{
   using boost::multiprecision::cpp_int;
   using boost::multiprecision::backends::total_lehmer_gcd_calls;
   using boost::multiprecision::backends::total_lehmer_gcd_bits_saved;
   using boost::multiprecision::backends::total_lehmer_gcd_cycles;

   unsigned bits = 2048;

   std::vector<cpp_int>& a = get_test_vector_a<cpp_int>(bits);
   std::vector<cpp_int>& b = get_test_vector_b<cpp_int>(bits);
   std::vector<cpp_int>& c = get_test_vector_c<cpp_int>(bits);

   for (unsigned i = 0; i < a.size(); ++i)
   {
      c[i] = gcd(a[i], b[i]);
      cpp_int t = gcd_euler(a[i], b[i]);
      BOOST_CHECK_EQUAL(t, c[i]);
   }

   float average_bits_saved_per_lehmer_call = static_cast<float>(total_lehmer_gcd_bits_saved) / total_lehmer_gcd_calls;
   float average_number_of_euler_cycles_saved = static_cast<float>(total_lehmer_gcd_cycles) / total_lehmer_gcd_calls;

   std::cout << "Average number of bits saved per Lehmer call:           " << average_bits_saved_per_lehmer_call << std::endl;
   std::cout << "Average number of Euclid cycles saved per Lehmer call:  " << average_number_of_euler_cycles_saved << std::endl;

#ifndef BOOST_HAS_INT128
   BOOST_CHECK_GT(average_bits_saved_per_lehmer_call, 30);
#else
   BOOST_CHECK_GT(average_bits_saved_per_lehmer_call, 62);
#endif

   return boost::report_errors();
}
