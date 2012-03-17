///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MP_MR_HPP
#define BOOST_MP_MR_HPP

#include <boost/multiprecision/random.hpp>

namespace boost{
namespace multiprecision{
//
// Calculate (a^b)%c:
//
template <class Backend, bool ExpressionTemplates>
void expmod(const mp_number<Backend, ExpressionTemplates>& a, mp_number<Backend, ExpressionTemplates> b, const mp_number<Backend, ExpressionTemplates>& c, mp_number<Backend, ExpressionTemplates>& result)
{
   typedef mp_number<Backend, ExpressionTemplates> number_type;
   number_type x(1), y(a);
   while(b > 0)
   {
      if(b & 1)
      {
         x = (x * y) % c;
      }
      y = (y * y) % c;
      b /= 2;
   }
   result = x % c;
}

template <class Backend, bool ExpressionTemplates, class Engine>
bool miller_rabin_test(const mp_number<Backend, ExpressionTemplates>& n, unsigned trials, Engine& gen)
{
   typedef mp_number<Backend, ExpressionTemplates> number_type;

   static const unsigned small_factors[] = {
      3u, 5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u, 37u, 41u, 43u, 47u, 53u };

   if(n < 2)
      return false;
   if((n & 1) == 0)
      return false;
   //
   // Sanity check for small factors:
   //

   number_type q = (n - 1) >> 1;
   unsigned k = 1;
   while((q & 1) == 0)
   {
      q >>= 1;
      ++k;
   }
   // Declare our random number generator:
   boost::random::uniform_int_distribution<number_type> dist(0, n);
   number_type nm1 = n - 1;
   //
   // Execute the trials:
   //
   for(unsigned i = 0; i < trials; ++i)
   {
      number_type x = dist(gen);
      number_type y;
      expmod(x, q, n, y);
      unsigned j = 0;
      while(true)
      {
         if((y == nm1) || ((y == 1) && (j == 0)))
            break;
         if(y == 1)
            return false; // test failed
         if(++j == k)
            return false; // failed
         y = (y * y) % n;
      }
   }
   return true;  // Yeheh! probably prime.
}

template <class Backend, bool ExpressionTemplates>
bool miller_rabin_test(const mp_number<Backend, ExpressionTemplates>& x, unsigned trials)
{
   static mt19937 gen;
   return miller_rabin_test(x, trials, gen);
}

template <class tag, class Arg1, class Arg2, class Arg3, class Engine>
bool miller_rabin_test(const detail::mp_exp<tag, Arg1, Arg2, Arg3> & n, unsigned trials, Engine& gen)
{
   typedef typename detail::mp_exp<tag, Arg1, Arg2, Arg3>::result_type number_type;
   return miller_rabin_test(number_type(n), trials, gen);
}

template <class tag, class Arg1, class Arg2, class Arg3>
bool miller_rabin_test(const detail::mp_exp<tag, Arg1, Arg2, Arg3> & n, unsigned trials)
{
   typedef typename detail::mp_exp<tag, Arg1, Arg2, Arg3>::result_type number_type;
   return miller_rabin_test(number_type(n), trials);
}

}} // namespaces

#endif

