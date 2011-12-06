
// Copyright Christopher Kormanyos 2002 - 2011.
// Copyright 2011 John Maddock. Distributed under the Boost
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// This work is based on an earlier work:
// "Algorithm 910: A Portable C++ Multiple-Precision System for Special-Function Calculations",
// in ACM TOMS, {VOL 37, ISSUE 4, (February 2011)} (C) ACM, 2011. http://doi.acm.org/10.1145/1916461.1916469
//
// This file has no include guards or namespaces - it's expanded inline inside default_ops.hpp
// 

template <class T>
void hyp0F1(T& result, const T& b, const T& x)
{
   typedef typename boost::multiprecision::detail::canonical<boost::int32_t, T>::type si_type;
   typedef typename boost::multiprecision::detail::canonical<boost::uint32_t, T>::type ui_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
   typedef typename mpl::front<typename T::float_types>::type fp_type;

   // Compute the series representation of Hypergeometric0F1 taken from
   // http://functions.wolfram.com/HypergeometricFunctions/Hypergeometric0F1/06/01/01/
   // There are no checks on input range or parameter boundaries.

   T x_pow_n_div_n_fact(x);
   T pochham_b         (b);
   T bp                (b);

   divide(result, x_pow_n_div_n_fact, pochham_b);
   add(result, ui_type(1));

   si_type n;

   T tol;
   tol = ui_type(1);
   eval_ldexp(tol, tol, 1 - boost::multiprecision::detail::digits2<mp_number<T> >::value);
   multiply(tol, result);
   if(get_sign(tol) < 0)
      tol.negate();
   T term;

   // Series expansion of hyperg_0f1(; b; x).
   for(n = 2; n < 300; ++n)
   {
      multiply(x_pow_n_div_n_fact, x);
      divide(x_pow_n_div_n_fact, n);
      increment(bp);
      multiply(pochham_b, bp);

      divide(term, x_pow_n_div_n_fact, pochham_b);
      add(result, term);

      bool neg_term = get_sign(term) < 0;
      if(neg_term)
         term.negate();
      if(term.compare(tol) <= 0)
         break;
   }

   if(n >= 300)
      BOOST_THROW_EXCEPTION(std::runtime_error("H0F1 Failed to Converge"));
}


template <class T>
void eval_sin(T& result, const T& x)
{
   if(&result == &x)
   {
      T temp;
      eval_sin(temp, x);
      result = temp;
      return;
   }

   typedef typename boost::multiprecision::detail::canonical<boost::int32_t, T>::type si_type;
   typedef typename boost::multiprecision::detail::canonical<boost::uint32_t, T>::type ui_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
   typedef typename mpl::front<typename T::float_types>::type fp_type;

   switch(eval_fpclassify(x))
   {
   case FP_INFINITE:
   case FP_NAN:
      result = std::numeric_limits<mp_number<T> >::quiet_NaN().backend();
      return;
   case FP_ZERO:
      result = ui_type(0);
      return;
   default: ;
   }

   // Local copy of the argument
   T xx = x;

   // Analyze and prepare the phase of the argument.
   // Make a local, positive copy of the argument, xx.
   // The argument xx will be reduced to 0 <= xx <= pi/2.
   bool b_negate_sin = false;

   if(get_sign(x) < 0)
   {
      xx.negate();
      b_negate_sin = !b_negate_sin;
   }

   T n_pi, t;
   // Remove even multiples of pi.
   if(xx.compare(get_constant_pi<T>()) > 0)
   {
      divide(n_pi, xx, get_constant_pi<T>());
      eval_trunc(n_pi, n_pi);
      t = ui_type(2);
      eval_fmod(t, n_pi, t);
      const bool b_n_pi_is_even = get_sign(t) == 0;
      multiply(n_pi, get_constant_pi<T>());
      subtract(xx, n_pi);

      BOOST_MATH_INSTRUMENT_CODE(xx.str(0, std::ios_base::scientific));
      BOOST_MATH_INSTRUMENT_CODE(n_pi.str(0, std::ios_base::scientific));

      // Adjust signs if the multiple of pi is not even.
      if(!b_n_pi_is_even)
      {
         b_negate_sin = !b_negate_sin;
      }
   }

   // Reduce the argument to 0 <= xx <= pi/2.
   eval_ldexp(t, get_constant_pi<T>(), -1);
   if(xx.compare(t) > 0)
   {
      subtract(xx, get_constant_pi<T>(), xx);
      BOOST_MATH_INSTRUMENT_CODE(xx.str(0, std::ios_base::scientific));
   }

   subtract(t, xx);
   const bool b_zero    = get_sign(xx) == 0;
   const bool b_pi_half = get_sign(t) == 0;

   // Check if the reduced argument is very close to 0 or pi/2.
   const bool    b_near_zero    = xx.compare(fp_type(1e-1)) < 0;
   const bool    b_near_pi_half = t.compare(fp_type(1e-1)) < 0;;

   if(b_zero)
   {
      result = ui_type(0);
   }
   else if(b_pi_half)
   {
      result = ui_type(1);
   }
   else if(b_near_zero)
   {
      multiply(t, xx, xx);
      divide(t, si_type(-4));
      T t2;
      t2 = fp_type(1.5);
      hyp0F1(result, t2, t);
      BOOST_MATH_INSTRUMENT_CODE(result.str(0, std::ios_base::scientific));
      multiply(result, xx);
   }
   else if(b_near_pi_half)
   {
      multiply(t, t);
      divide(t, si_type(-4));
      T t2;
      t2 = fp_type(0.5);
      hyp0F1(result, t2, t);
      BOOST_MATH_INSTRUMENT_CODE(result.str(0, std::ios_base::scientific));
   }
   else
   {
      // Scale to a small argument for an efficient Taylor series,
      // implemented as a hypergeometric function. Use a standard
      // divide by three identity a certain number of times.
      // Here we use division by 3^9 --> (19683 = 3^9).

      static const si_type n_scale = 9;
      static const si_type n_three_pow_scale = static_cast<si_type>(19683L);

      divide(xx, n_three_pow_scale);

      // Now with small arguments, we are ready for a series expansion.
      multiply(t, xx, xx);
      divide(t, si_type(-4));
      T t2;
      t2 = fp_type(1.5);
      hyp0F1(result, t2, t);
      BOOST_MATH_INSTRUMENT_CODE(result.str(0, std::ios_base::scientific));
      multiply(result, xx);

      // Convert back using multiple angle identity.
      for(boost::int32_t k = static_cast<boost::int32_t>(0); k < n_scale; k++)
      {
         // Rescale the cosine value using the multiple angle identity.
         multiply(t2, result, ui_type(3));
         multiply(t, result, result);
         multiply(t, result);
         multiply(t, ui_type(4));
         subtract(result, t2, t);
      }
   }

   if(b_negate_sin)
      result.negate();
}

template <class T>
void eval_cos(T& result, const T& x)
{
   if(&result == &x)
   {
      T temp;
      eval_sin(temp, x);
      result = temp;
      return;
   }

   typedef typename boost::multiprecision::detail::canonical<boost::int32_t, T>::type si_type;
   typedef typename boost::multiprecision::detail::canonical<boost::uint32_t, T>::type ui_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
   typedef typename mpl::front<typename T::float_types>::type fp_type;

   switch(eval_fpclassify(x))
   {
   case FP_INFINITE:
   case FP_NAN:
      result = std::numeric_limits<mp_number<T> >::quiet_NaN().backend();
      return;
   case FP_ZERO:
      result = ui_type(1);
      return;
   default: ;
   }

   // Local copy of the argument
   T xx = x;

   // Analyze and prepare the phase of the argument.
   // Make a local, positive copy of the argument, xx.
   // The argument xx will be reduced to 0 <= xx <= pi/2.
   bool b_negate_cos = false;

   if(get_sign(x) < 0)
   {
      xx.negate();
   }

   T n_pi, t;
   // Remove even multiples of pi.
   if(xx.compare(get_constant_pi<T>()) > 0)
   {
      divide(t, xx, get_constant_pi<T>());
      eval_trunc(n_pi, t);
      BOOST_MATH_INSTRUMENT_CODE(n_pi.str(0, std::ios_base::scientific));
      multiply(t, n_pi, get_constant_pi<T>());
      BOOST_MATH_INSTRUMENT_CODE(t.str(0, std::ios_base::scientific));
      subtract(xx, t);
      BOOST_MATH_INSTRUMENT_CODE(xx.str(0, std::ios_base::scientific));

      // Adjust signs if the multiple of pi is not even.
      t = ui_type(2);
      eval_fmod(t, n_pi, t);
      const bool b_n_pi_is_even = get_sign(t) == 0;

      if(!b_n_pi_is_even)
      {
         b_negate_cos = !b_negate_cos;
      }
   }

   // Reduce the argument to 0 <= xx <= pi/2.
   eval_ldexp(t, get_constant_pi<T>(), -1);
   int com = xx.compare(t);
   if(com > 0)
   {
      subtract(xx, get_constant_pi<T>(), xx);
      b_negate_cos = !b_negate_cos;
      BOOST_MATH_INSTRUMENT_CODE(xx.str(0, std::ios_base::scientific));
   }

   const bool b_zero    = get_sign(xx) == 0;
   const bool b_pi_half = com == 0;

   // Check if the reduced argument is very close to 0 or pi/2.
   const bool    b_near_zero    = xx.compare(fp_type(1e-4)) < 0;
   subtract(t, xx);
   const bool    b_near_pi_half = t.compare(fp_type(1e-4)) < 0;

   if(b_zero)
   {
      result = si_type(1);
   }
   else if(b_pi_half)
   {
      result = si_type(0);
   }
   else if(b_near_zero)
   {
      multiply(t, xx, xx);
      divide(t, si_type(-4));
      n_pi = fp_type(0.5f);
      hyp0F1(result, n_pi, t);
      BOOST_MATH_INSTRUMENT_CODE(result.str(0, std::ios_base::scientific));
   }
   else if(b_near_pi_half)
   {
      T t2(t);
      multiply(t, t);
      divide(t, si_type(-4));
      n_pi = fp_type(1.5f);
      hyp0F1(result, n_pi, t);
      multiply(result, t2);
      BOOST_MATH_INSTRUMENT_CODE(result.str(0, std::ios_base::scientific));
   }
   else
   {
      // Scale to a small argument for an efficient Taylor series,
      // implemented as a hypergeometric function. Use a standard
      // divide by three identity a certain number of times.
      // Here we use division by 3^9 --> (19683 = 3^9).

      static const ui_type n_scale           = 9;
      static const ui_type n_three_pow_scale = 19683;
      divide(xx, n_three_pow_scale);

      multiply(t, xx, xx);
      divide(t, si_type(-4));
      n_pi = fp_type(0.5f);

      // Now with small arguments, we are ready for a series expansion.
      hyp0F1(result, n_pi, t);
      BOOST_MATH_INSTRUMENT_CODE(result.str(0, std::ios_base::scientific));

      // Convert back using multiple angle identity.
      for(ui_type k = 0; k < n_scale; k++)
      {
         multiply(t, result, result);
         multiply(t, result);
         multiply(t, ui_type(4));
         multiply(result, si_type(-3));
         add(result, t);
      }
   }
   if(b_negate_cos)
      result.negate();
}

template <class T>
void eval_tan(T& result, const T& x)
{
   T t;
   eval_sin(result, x);
   eval_cos(t, x);
   divide(result, t);
}

template <class T>
void hyp2F1(T& result, const T& a, const T& b, const T& c, const T& x)
{
  // Compute the series representation of hyperg_2f1 taken from
  // Abramowitz and Stegun 15.1.1.
  // There are no checks on input range or parameter boundaries.

   typedef typename boost::multiprecision::detail::canonical<boost::int32_t, T>::type si_type;
   typedef typename boost::multiprecision::detail::canonical<boost::uint32_t, T>::type ui_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
   typedef typename mpl::front<typename T::float_types>::type fp_type;

   T x_pow_n_div_n_fact(x);
   T pochham_a         (a);
   T pochham_b         (b);
   T pochham_c         (c);
   T ap                (a);
   T bp                (b);
   T cp                (c);

   multiply(result, pochham_a, pochham_b);
   divide(result, pochham_c);
   multiply(result, x_pow_n_div_n_fact);
   add(result, ui_type(1));

   T lim;
   eval_ldexp(lim, result, 1 - boost::multiprecision::detail::digits2<mp_number<T> >::value);

   if(get_sign(lim) < 0)
      lim.negate();

   ui_type n;
   T term;

   // Series expansion of hyperg_2f1(a, b; c; x).
   for(n = 2; n < 300; ++n)
   {
      multiply(x_pow_n_div_n_fact, x);
      divide(x_pow_n_div_n_fact, n);

      increment(ap);
      multiply(pochham_a, ap);
      increment(bp);
      multiply(pochham_b, bp);
      increment(cp);
      multiply(pochham_c, cp);

      multiply(term, pochham_a, pochham_b);
      divide(term, pochham_c);
      multiply(term, x_pow_n_div_n_fact);
      add(result, term);

      if(get_sign(term) < 0)
         term.negate();
      if(lim.compare(term) >= 0)
         break;
   }
   if(n > 300)
      BOOST_THROW_EXCEPTION(std::runtime_error("H2F1 failed to converge."));
}

template <class T>
void eval_asin(T& result, const T& x)
{
   typedef typename boost::multiprecision::detail::canonical<boost::int32_t, T>::type si_type;
   typedef typename boost::multiprecision::detail::canonical<boost::uint32_t, T>::type ui_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
   typedef typename mpl::front<typename T::float_types>::type fp_type;

   if(&result == &x)
   {
      T t(x);
      eval_asin(result, t);
      return;
   }

   switch(eval_fpclassify(x))
   {
   case FP_NAN:
   case FP_INFINITE:
      result = std::numeric_limits<mp_number<T> >::quiet_NaN().backend();
      return;
   case FP_ZERO:
      result = ui_type(0);
      return;
   default: ;
   }

   const bool b_neg = get_sign(x) < 0;

   T xx(x);
   if(b_neg)
      xx.negate();

   int c = xx.compare(ui_type(1));
   if(c > 0)
   {
      result = std::numeric_limits<mp_number<T> >::quiet_NaN().backend();
      return;
   }
   else if(c == 0)
   {
      result = get_constant_pi<T>();
      eval_ldexp(result, result, -1);
      if(b_neg)
         result.negate();
      return;
   }

   if(xx.compare(fp_type(1e-4)) < 0)
   {
      // http://functions.wolfram.com/ElementaryFunctions/ArcSin/26/01/01/
      multiply(xx, xx);
      T t1, t2;
      t1 = fp_type(0.5f);
      t2 = fp_type(1.5f);
      hyp2F1(result, t1, t1, t2, xx);
      multiply(result, x);
      return;
   }
   else if(xx.compare(fp_type(1 - 1e-4f)) > 0)
   {
      T dx1;
      T t1, t2;
      subtract(dx1, ui_type(1), xx);
      t1 = fp_type(0.5f);
      t2 = fp_type(1.5f);
      eval_ldexp(dx1, dx1, -1);
      hyp2F1(result, t1, t1, t2, dx1);
      eval_ldexp(dx1, dx1, 2);
      eval_sqrt(t1, dx1);
      multiply(result, t1);
      eval_ldexp(t1, get_constant_pi<T>(), -1);
      result.negate();
      add(result, t1);
      if(b_neg)
         result.negate();
      return;
   }

   // Get initial estimate using standard math function asin.
   double dd;
   convert_to(&dd, xx);

   result = fp_type(std::asin(dd));

   // Newton-Raphson iteration
   while(true)
   {
      T s, c;
      eval_sin(s, result);
      eval_cos(c, result);
      subtract(s, xx);
      divide(s, c);
      subtract(result, s);

      T lim;
      eval_ldexp(lim, result, 1 - boost::multiprecision::detail::digits2<mp_number<T> >::value);
      if(get_sign(s) < 0)
         s.negate();
      if(get_sign(lim) < 0)
         lim.negate();
      if(lim.compare(s) >= 0)
         break;
   }
   if(b_neg)
      result.negate();
}

template <class T>
inline void eval_acos(T& result, const T& x)
{
   typedef typename boost::multiprecision::detail::canonical<boost::uint32_t, T>::type ui_type;

   switch(eval_fpclassify(x))
   {
   case FP_NAN:
   case FP_INFINITE:
      result = std::numeric_limits<mp_number<T> >::quiet_NaN().backend();
      return;
   case FP_ZERO:
      result = get_constant_pi<T>();
      eval_ldexp(result, result, -1); // divide by two.
      return;
   }

   eval_abs(result, x);
   int c = result.compare(ui_type(1));

   if(c > 0)
   {
      result = std::numeric_limits<mp_number<T> >::quiet_NaN().backend();
      return;
   }
   else if(c == 0)
   {
      if(get_sign(x) < 0)
         result = get_constant_pi<T>();
      else
         result = ui_type(0);
      return;
   }

   eval_asin(result, x);
   T t;
   eval_ldexp(t, get_constant_pi<T>(), -1);
   subtract(result, t);
   result.negate();
}

template <class T>
void eval_atan(T& result, const T& x)
{
   typedef typename boost::multiprecision::detail::canonical<boost::int32_t, T>::type si_type;
   typedef typename boost::multiprecision::detail::canonical<boost::uint32_t, T>::type ui_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
   typedef typename mpl::front<typename T::float_types>::type fp_type;

   switch(eval_fpclassify(x))
   {
   case FP_NAN:
      result = std::numeric_limits<mp_number<T> >::quiet_NaN().backend();
      return;
   case FP_ZERO:
      result = ui_type(0);
      return;
   case FP_INFINITE:
      if(get_sign(x) < 0)
      {
         eval_ldexp(result, get_constant_pi<T>(), -1);
         result.negate();
      }
      else
         eval_ldexp(result, get_constant_pi<T>(), -1);
      return;
   default: ;
   }

   const bool b_neg = get_sign(x) < 0;

   T xx(x);
   if(b_neg)
      xx.negate();

   if(xx.compare(fp_type(0.1)) < 0)
   {
      T t1, t2, t3;
      t1 = ui_type(1);
      t2 = fp_type(0.5f);
      t3 = fp_type(1.5f);
      multiply(xx, xx);
      xx.negate();
      hyp2F1(result, t1, t2, t3, xx);
      multiply(result, x);
      return;
   }

   if(xx.compare(fp_type(10)) > 0)
   {
      T t1, t2, t3;
      t1 = fp_type(0.5f);
      t2 = ui_type(1u);
      t3 = fp_type(1.5f);
      multiply(xx, xx);
      divide(xx, si_type(-1), xx);
      hyp2F1(result, t1, t2, t3, xx);
      divide(result, x);
      if(!b_neg)
         result.negate();
      eval_ldexp(t1, get_constant_pi<T>(), -1);
      add(result, t1);
      if(b_neg)
         result.negate();
      return;
   }


   // Get initial estimate using standard math function atan.
   fp_type d;
   convert_to(&d, xx);
   result = fp_type(std::atan(d));

   // Newton-Raphson iteration
   static const boost::int32_t double_digits10_minus_one = std::numeric_limits<double>::digits10 - 1;

   T s, c, t;
   for(boost::int32_t digits = double_digits10_minus_one; digits <= std::numeric_limits<mp_number<T> >::digits10; digits *= 2)
   {
      eval_sin(s, result);
      eval_cos(c, result);
      multiply(t, xx, c);
      subtract(t, s);
      multiply(s, t, c);
      add(result, s);
   }
   if(b_neg)
      result.negate();
}

template <class T>
void eval_atan2(T& result, const T& y, const T& x)
{
   if(&result == &y)
   {
      T temp(y);
      eval_atan2(result, temp, x);
      return;
   }
   else if(&result == &x)
   {
      T temp(x);
      eval_atan2(result, y, temp);
      return;
   }

   typedef typename boost::multiprecision::detail::canonical<boost::int32_t, T>::type si_type;
   typedef typename boost::multiprecision::detail::canonical<boost::uint32_t, T>::type ui_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
   typedef typename mpl::front<typename T::float_types>::type fp_type;

   switch(eval_fpclassify(y))
   {
   case FP_NAN:
      result = y;
      return;
   case FP_ZERO:
      {
         int c = get_sign(x);
         if(c < 0)
            result = get_constant_pi<T>();
         else if(c >= 0)
            result = ui_type(0); // Note we allow atan2(0,0) to be zero, even though it's mathematically undefined
         return;
      }
   case FP_INFINITE:
      {
         if(eval_fpclassify(x) == FP_INFINITE)
         {
            result = std::numeric_limits<mp_number<T> >::quiet_NaN().backend();
         }
         else
         {
            eval_ldexp(result, get_constant_pi<T>(), -1);
            if(get_sign(y) < 0)
               result.negate();
         }
         return;
      }
   }

   switch(eval_fpclassify(x))
   {
   case FP_NAN:
      result = x;
      return;
   case FP_ZERO:
      {
         eval_ldexp(result, get_constant_pi<T>(), -1);
         if(get_sign(y) < 0)
            result.negate();
         return;
      }
   case FP_INFINITE:
      if(get_sign(x) > 0)
         result = ui_type(0);
      else
         result = get_constant_pi<T>();
      if(get_sign(y) < 0)
         result.negate();
      return;
   }

   T xx;
   divide(xx, y, x);
   if(get_sign(xx) < 0)
      xx.negate();

   eval_atan(result, xx);

   // Determine quadrant (sign) based on signs of x, y
   const bool y_neg = get_sign(y) < 0;
   const bool x_neg = get_sign(x) < 0;

   if(y_neg != x_neg)
      result.negate();

   if(x_neg)
   {
      if(y_neg)
         subtract(result, get_constant_pi<T>());
      else
         add(result, get_constant_pi<T>());
   }
}

template <class T, class Arithmetic>
typename disable_if<is_same<T, Arithmetic> >::type eval_atan2(T& result, const T& a, const Arithmetic& b)
{
   T x;
   x = static_cast<typename boost::multiprecision::detail::canonical<Arithmetic, T>::type>(b);
   eval_atan2(result, a, x);
}

