// Copyright 2011 John Maddock. Distributed under the Boost
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// This file has no include guards or namespaces - it's expanded inline inside default_ops.hpp
// 

template <class T>
void calc_log2(T& num, unsigned digits)
{
   typedef typename has_enough_bits<unsigned, std::numeric_limits<unsigned>::digits>::template type<mpl::_> pred_type;
   typedef typename mpl::find_if<
      typename T::unsigned_types,
      pred_type
   >::type iter_type;
   typedef typename mpl::deref<iter_type>::type ui_type;
   typedef typename mpl::front<typename T::signed_types>::type si_type;

   num = static_cast<ui_type>(1180509120u);
   T denom, next_term, temp;
   denom = static_cast<ui_type>(1277337600u);
   next_term = static_cast<ui_type>(120u);
   si_type sign = -1;

   ui_type limit = digits / 3 + 1;

   for(ui_type n = 6; n < limit; ++n)
   {
      temp = static_cast<ui_type>(2);
      multiply(temp, ui_type(2 * n));
      multiply(temp, ui_type(2 * n + 1));
      multiply(num, temp);
      multiply(denom, temp);
      sign = -sign;
      multiply(next_term, n);
      multiply(temp, next_term, next_term);
      multiply(temp, sign);
      add(num, temp);
   }
   multiply(denom, ui_type(4));
   multiply(num, ui_type(3));
   INSTRUMENT_BACKEND(denom);
   INSTRUMENT_BACKEND(num);
   divide(num, denom);
   INSTRUMENT_BACKEND(num);
}

template <class T>
void calc_e(T& result, unsigned digits)
{
   typedef typename mpl::front<typename T::unsigned_types>::type ui_type;
   typedef typename mpl::front<typename T::float_types>::type real_type;
   T lim;
   lim = ui_type(1);
   eval_ldexp(lim, lim, digits);

   result = ui_type(2);
   T denom;
   denom = ui_type(1);
   ui_type i = 2;
   do{
      multiply(denom, i);
      multiply(result, i);
      add(result, ui_type(1));
      ++i;
   }while(denom.compare(lim) <= 0);
   divide(result, denom);
}

template <class T>
void calc_pi(T& result, unsigned digits)
{
   typedef typename mpl::front<typename T::unsigned_types>::type ui_type;
   typedef typename mpl::front<typename T::float_types>::type real_type;

   T a;
   a = ui_type(1);
   T b;
   T A(a);
   T B;
   B = real_type(0.5f);
   T D;
   D = real_type(0.25f);

   T lim;
   lim = ui_type(1);
   eval_ldexp(lim, lim, -(int)digits);

   unsigned k = 1;

   do
   {
      add(result, A, B);
      eval_ldexp(result, result, -2);
      eval_sqrt(b, B);
      add(a, b);
      eval_ldexp(a, a, -1);
      multiply(A, a, a);
      subtract(B, A, result);
      eval_ldexp(B, B, 1);
      subtract(result, A, B);
      bool neg = get_sign(result) < 0;
      if(neg)
         result.negate();
      if(result.compare(lim) <= 0)
         break;
      if(neg)
         result.negate();
      eval_ldexp(result, result, k - 1);
      subtract(D, result);
      ++k;
      eval_ldexp(lim, lim, 1);
   }
   while(true);

   divide(result, B, D);
}

template <class T, const T& (*F)(void)>
struct constant_initializer
{
   static void do_nothing()
   {
      init.do_nothing();
   }
private:
   struct initializer
   {
      initializer()
      {
         F();
      }
      void do_nothing()const{}
   };
   static const initializer init;
};

template <class T, const T& (*F)(void)>
typename constant_initializer<T, F>::initializer const constant_initializer<T, F>::init;

template <class T>
const T& get_constant_ln2()
{
   static T result;
   static bool b = false;
   if(!b)
   {
      calc_log2(result, std::numeric_limits<mp_number<T> >::digits);
      b = true;
   }

   constant_initializer<T, &get_constant_ln2<T> >::do_nothing();

   return result;
}

template <class T>
const T& get_constant_e()
{
   static T result;
   static bool b = false;
   if(!b)
   {
      calc_e(result, std::numeric_limits<mp_number<T> >::digits);
      b = true;
   }

   constant_initializer<T, &get_constant_e<T> >::do_nothing();

   return result;
}

template <class T>
const T& get_constant_pi()
{
   static T result;
   static bool b = false;
   if(!b)
   {
      calc_pi(result, std::numeric_limits<mp_number<T> >::digits);
      b = true;
   }

   constant_initializer<T, &get_constant_pi<T> >::do_nothing();

   return result;
}

