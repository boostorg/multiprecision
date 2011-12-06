
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

namespace detail{

template<typename T, typename U> 
inline T pow_imp(T& result, const T& t, const U& p, const mpl::false_&)
{
   T temp(p);
   eval_pow(result, t, p);
}

template<typename T, typename U> 
inline void pow_imp(T& result, const T& t, const U& p, const mpl::true_&)
{
   // Compute the pure power of typename T t^p. Binary splitting of the power is
   // used. The resulting computational complexity has the order of log2[abs(p)].

   typedef typename mpl::front<typename T::signed_types>::type int_type;

   if(p < 0)
   {
      T temp;
      temp = static_cast<int_type>(1);
      T denom;
      pow_imp(denom, t, -p, mpl::true_());
      divide(result, temp, denom);
      return;
   }

   switch(p)
   {
   case 0:
      result = static_cast<int_type>(1);
      break;
   case 1:
      result = t;
      break;
   case 2:
      multiply(result, t, t);
      break;
   case 3:
      multiply(result, t, t);
      multiply(result, t);
      break;
   case 4:
      multiply(result, t, t);
      multiply(result, result);
      break;
   default:
      {
         result = t;
         U n;

         for(n = static_cast<U>(1); n <= static_cast<U>(p / static_cast<U>(2)); n *= static_cast<U>(2))
         {
            multiply(result, result);
         }

         const U p_minus_n = static_cast<U>(p - n);

         // Call the function recursively for computing the remaining power of n.
         if(p_minus_n)
         {
            T temp;
            pow_imp(temp, t, p_minus_n, mpl::true_());
            multiply(result, temp);
         }
      }
   }
}

} // namespace detail

template<typename T, typename U> 
inline void eval_pow(T& result, const T& t, const U& p)
{
   typedef typename is_integral<U>::type tag_type;
   detail::pow_imp(result, t, p, tag_type());
}

template <class T>
void hyp0F0(T& H0F0, const T& x)
{
   // Compute the series representation of Hypergeometric0F0 taken from
   // http://functions.wolfram.com/HypergeometricFunctions/Hypergeometric0F0/06/01/
   // There are no checks on input range or parameter boundaries.

   typedef typename mpl::front<typename T::unsigned_types>::type ui_type;

   BOOST_ASSERT(&H0F0 != &x);
   long tol = boost::multiprecision::detail::digits2<mp_number<T> >::value;
   T t;

   T x_pow_n_div_n_fact(x);

   add(H0F0, x_pow_n_div_n_fact, ui_type(1));

   T lim;
   eval_ldexp(lim, H0F0, 1 - tol);
   if(get_sign(lim) < 0)
      lim.negate();

   ui_type n;

   // Series expansion of hyperg_0f0(; ; x).
   for(n = 2; n < 300; ++n)
   {
      multiply(x_pow_n_div_n_fact, x);
      divide(x_pow_n_div_n_fact, n);
      add(H0F0, x_pow_n_div_n_fact);
      bool neg = get_sign(x_pow_n_div_n_fact) < 0;
      if(neg)
         x_pow_n_div_n_fact.negate();
      if(lim.compare(x_pow_n_div_n_fact) > 0)
         break;
      if(neg)
         x_pow_n_div_n_fact.negate();
   }
   if(n >= 300)
      BOOST_THROW_EXCEPTION(std::runtime_error("H0F0 failed to converge"));
}

template <class T>
void hyp1F0(T& H1F0, const T& a, const T& x)
{
   // Compute the series representation of Hypergeometric1F0 taken from
   // http://functions.wolfram.com/HypergeometricFunctions/Hypergeometric1F0/06/01/01/
   // and also see the corresponding section for the power function (i.e. x^a).
   // There are no checks on input range or parameter boundaries.

   typedef typename boost::multiprecision::detail::canonical<int, T>::type si_type;
   typedef typename boost::multiprecision::detail::canonical<unsigned, T>::type ui_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
   typedef typename mpl::front<typename T::float_types>::type fp_type;

   BOOST_ASSERT(&H1F0 != &x);
   BOOST_ASSERT(&H1F0 != &a);

   T x_pow_n_div_n_fact(x);
   T pochham_a         (a);
   T ap                (a);

   multiply(H1F0, pochham_a, x_pow_n_div_n_fact);
   add(H1F0, si_type(1));
   T lim;
   eval_ldexp(lim, H1F0, 1 - boost::multiprecision::detail::digits2<mp_number<T> >::value);
   if(get_sign(lim) < 0)
      lim.negate();

   si_type n;
   T term, part;

   // Series expansion of hyperg_1f0(a; ; x).
   for(n = 2; n < boost::multiprecision::detail::digits2<mp_number<T> >::value + 10; n++)
   {
      multiply(x_pow_n_div_n_fact, x);
      divide(x_pow_n_div_n_fact, n);
      increment(ap);
      multiply(pochham_a, ap);
      multiply(term, pochham_a, x_pow_n_div_n_fact);
      add(H1F0, term);
      if(get_sign(term) < 0)
         term.negate();
      if(lim.compare(term) >= 0)
         break;
   }
   if(n >= boost::multiprecision::detail::digits2<mp_number<T> >::value + 10)
      BOOST_THROW_EXCEPTION(std::runtime_error("H1F0 failed to converge"));
}

template <class T>
void eval_exp(T& result, const T& x)
{
   if(&x == &result)
   {
      T temp;
      eval_exp(temp, x);
      result = temp;
      return;
   }
   typedef typename boost::multiprecision::detail::canonical<unsigned, T>::type ui_type;
   typedef typename boost::multiprecision::detail::canonical<int, T>::type si_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
   typedef typename boost::multiprecision::detail::canonical<float, T>::type float_type;

   // Handle special arguments.
   int type = eval_fpclassify(x);
   bool isneg = get_sign(x) < 0;
   if(type == FP_NAN)
   {
      result = std::numeric_limits<mp_number<T> >::quiet_NaN().backend();
      return;
   }
   else if(type == FP_INFINITE)
   {
      result = x;
      if(isneg)
         result = ui_type(0u);
      else 
         result = x;
      return;
   }
   else if(type == FP_ZERO)
   {
      result = ui_type(1);
      return;
   }

   // Get local copy of argument and force it to be positive.
   T xx = x;
   T exp_series;
   if(isneg)
      xx.negate();

   // Check the range of the argument.
   static const canonical_exp_type maximum_arg_for_exp = std::numeric_limits<mp_number<T> >::max_exponent == 0 ? (std::numeric_limits<long>::max)() : std::numeric_limits<mp_number<T> >::max_exponent;

   if(xx.compare(maximum_arg_for_exp) >= 0)
   {
      // Overflow / underflow
      if(isneg)
         result = ui_type(0);
      else
         result = std::numeric_limits<mp_number<T> >::has_infinity ? std::numeric_limits<mp_number<T> >::infinity().backend() : (std::numeric_limits<mp_number<T> >::max)().backend();
      return;
   }
   if(xx.compare(si_type(1)) <= 0)
   {
      //
      // Use series for exp(x) - 1:
      //
      T lim = std::numeric_limits<mp_number<T> >::epsilon().backend();
      unsigned k = 2;
      exp_series = xx;
      result = si_type(1);
      if(isneg)
         subtract(result, exp_series);
      else
         add(result, exp_series);
      multiply(exp_series, xx);
      divide(exp_series, ui_type(k));
      add(result, exp_series);
      while(exp_series.compare(lim) > 0)
      {
         ++k;
         multiply(exp_series, xx);
         divide(exp_series, ui_type(k));
         if(isneg && (k&1))
            subtract(result, exp_series);
         else
            add(result, exp_series);
      }
      return;
   }

   // Check for pure-integer arguments which can be either signed or unsigned.
   boost::intmax_t ll;
   eval_trunc(exp_series, x);
   convert_to(&ll, exp_series);
   if(x.compare(ll) == 0)
   {
      detail::pow_imp(result, get_constant_e<T>(), ll, mpl::true_());
      return;
   }

   // The algorithm for exp has been taken from MPFUN.
   // exp(t) = [ (1 + r + r^2/2! + r^3/3! + r^4/4! ...)^p2 ] * 2^n
   // where p2 is a power of 2 such as 2048, r = t_prime / p2, and
   // t_prime = t - n*ln2, with n chosen to minimize the absolute
   // value of t_prime. In the resulting Taylor series, which is
   // implemented as a hypergeometric function, |r| is bounded by
   // ln2 / p2. For small arguments, no scaling is done.

   const bool b_scale = (xx.compare(float_type(1e-4)) > 0);

   // Compute the exponential series of the (possibly) scaled argument.

   if(b_scale)
   {
      divide(result, xx, get_constant_ln2<T>());
      exp_type n;
      convert_to(&n, result);

      // The scaling is 2^11 = 2048.
      static const si_type p2 = static_cast<si_type>(si_type(1) << 11);

      multiply(exp_series, get_constant_ln2<T>(), static_cast<canonical_exp_type>(n));
      subtract(exp_series, xx);
      divide(exp_series, p2);
      exp_series.negate();
      hyp0F0(result, exp_series);

      detail::pow_imp(exp_series, result, p2, mpl::true_());
      result = ui_type(1);
      eval_ldexp(result, result, n);
      multiply(exp_series, result);
   }
   else
   {
      hyp0F0(exp_series, xx);
   }

   if(isneg)
      divide(result, ui_type(1), exp_series);
   else
      result = exp_series;
}

template <class T>
void eval_log(T& result, const T& arg)
{
   //
   // We use a variation of http://dlmf.nist.gov/4.45#i
   // using frexp to reduce the argument to x * 2^n,
   // then let y = x - 1 and compute:
   // log(x) = log(2) * n + log1p(1 + y)
   //
   if(&arg == &result)
   {
      T temp;
      eval_log(temp, arg);
      result = temp;
      return;
   }

   typedef typename boost::multiprecision::detail::canonical<int, T>::type si_type;
   typedef typename boost::multiprecision::detail::canonical<unsigned, T>::type ui_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
   typedef typename mpl::front<typename T::float_types>::type fp_type;

   exp_type e;
   T t;
   eval_frexp(t, arg, &e);
   bool alternate = false;

   if(t.compare(fp_type(2) / fp_type(3)) <= 0)
   {
      alternate = true;
      eval_ldexp(t, t, 1);
      --e;
   }
   
   multiply(result, get_constant_ln2<T>(), canonical_exp_type(e));
   INSTRUMENT_BACKEND(result);
   subtract(t, ui_type(1)); /* -0.3 <= t <= 0.3 */
   if(!alternate)
      t.negate(); /* 0 <= t <= 0.33333 */
   T pow = t;
   T lim;
   T t2;

   if(alternate)
      add(result, t);
   else
      subtract(result, t);

   multiply(lim, result, std::numeric_limits<mp_number<T> >::epsilon().backend());
   if(get_sign(lim) < 0)
      lim.negate();
   INSTRUMENT_BACKEND(lim);

   ui_type k = 1;
   do
   {
      ++k;
      multiply(pow, t);
      divide(t2, pow, k);
      INSTRUMENT_BACKEND(t2);
      if(alternate && ((k & 1) != 0))
         add(result, t2);
      else
         subtract(result, t2);
      INSTRUMENT_BACKEND(result);
   }while(lim.compare(t2) < 0);
}

template <class T>
const T& get_constant_log10()
{
   static T result;
   static bool b = false;
   if(!b)
   {
      typedef typename boost::multiprecision::detail::canonical<unsigned, T>::type ui_type;
      T ten;
      ten = ui_type(10u);
      eval_log(result, ten);
   }

   constant_initializer<T, &get_constant_log10<T> >::do_nothing();

   return result;
}

template <class T>
void eval_log10(T& result, const T& arg)
{
   eval_log(result, arg);
   divide(result, get_constant_log10<T>());
}

template<typename T> 
inline void eval_pow(T& result, const T& x, const T& a)
{
   typedef typename boost::multiprecision::detail::canonical<int, T>::type si_type;
   typedef typename boost::multiprecision::detail::canonical<unsigned, T>::type ui_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
   typedef typename mpl::front<typename T::float_types>::type fp_type;

   if((&result == &x) || (&result == &a))
   {
      T t;
      eval_pow(t, x, a);
      result = t;
   }

   if(a.compare(si_type(1)) == 0)
   {
      result = x;
      return;
   }

   int type = eval_fpclassify(x);

   switch(type)
   {
   case FP_INFINITE:
      result = x;
      return;
   case FP_ZERO:
      result = si_type(1);
      return;
   case FP_NAN:
      result = x;
      return;
   default: ;
   }

   if(get_sign(a) == 0)
   {
      result = si_type(1);
      return;
   }

   boost::intmax_t an;
   convert_to(&an, a);
   const bool bo_a_isint = a.compare(an) == 0;

   if((get_sign(x) < 0) && !bo_a_isint)
   {
      result = std::numeric_limits<mp_number<T> >::quiet_NaN().backend();
   }

   T t;
   T da(a);
   subtract(da, an);

   if(a.compare(si_type(-1)) < 0)
   {
      t = a;
      t.negate();
      eval_pow(da, x, t);
      divide(result, si_type(1), da);
      return;
   }
   
   subtract(da, a, an);

   if(bo_a_isint)
   {
      detail::pow_imp(result, x, an, mpl::true_());
      return;
   }

   if((get_sign(x) > 0) && (x.compare(fp_type(0.5)) >= 0) && (x.compare(fp_type(0.9)) < 0))
   {
      if(a.compare(fp_type(1e-5f)) <= 0)
      {
         // Series expansion for small a.
         eval_log(t, x);
         multiply(t, a);
         hyp0F0(result, t);
         return;
      }
      else
      {
         // Series expansion for moderately sized x. Note that for large power of a,
         // the power of the integer part of a is calculated using the pown function.
         if(an)
         {
            da.negate();
            t = si_type(1);
            subtract(t, x);
            hyp1F0(result, da, t);
            detail::pow_imp(t, x, an, mpl::true_());
            multiply(result, t);
         }
         else
         {
            da = a;
            da.negate();
            t = si_type(1);
            subtract(t, x);
            hyp1F0(result, da, t);
         }
      }
   }
   else
   {
      // Series expansion for pow(x, a). Note that for large power of a, the power
      // of the integer part of a is calculated using the pown function.
      if(an)
      {
         eval_log(t, x);
         multiply(t, da);
         eval_exp(result, t);
         detail::pow_imp(t, x, an, mpl::true_());
         multiply(result, t);
      }
      else
      {
         eval_log(t, x);
         multiply(t, a);
         eval_exp(result, t);
      }
   }
}

namespace detail{

   template <class T>
   void small_sinh_series(T x, T& result)
   {
      typedef typename boost::multiprecision::detail::canonical<unsigned, T>::type ui_type;
      bool neg = get_sign(x) < 0;
      if(neg)
         x.negate();
      T p(x);
      T mult(x);
      multiply(mult, x);
      result = x;
      ui_type k = 1;

      T lim(x);
      eval_ldexp(lim, lim, 1 - boost::multiprecision::detail::digits2<mp_number<T> >::value);

      do
      {
         multiply(p, mult);
         divide(p, ++k);
         divide(p, ++k);
         add(result, p);
      }while(p.compare(lim) >= 0);
      if(neg)
         result.negate();
   }

   template <class T>
   void sinhcosh(const T& x, T* p_sinh, T* p_cosh)
   {
      typedef typename boost::multiprecision::detail::canonical<int, T>::type si_type;
      typedef typename boost::multiprecision::detail::canonical<unsigned, T>::type ui_type;
      typedef typename T::exponent_type exp_type;
      typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
      typedef typename mpl::front<typename T::float_types>::type fp_type;

      switch(eval_fpclassify(x))
      {
      case FP_NAN:
      case FP_INFINITE:
         if(p_sinh)
            *p_sinh = x;
         if(p_cosh)
            *p_cosh = x;
         return;
      case FP_ZERO:
         if(p_sinh)
            *p_sinh = x;
         if(p_cosh)
            *p_cosh = ui_type(1);
         return;
      default: ;
      }

      T e_px, e_mx;

      bool small_sinh = get_sign(x) < 0 ? e_px.compare(fp_type(-0.5)) > 0 : e_px.compare(fp_type(0.5)) < 0;

      if(p_cosh || !small_sinh)
      {
         eval_exp(e_px, x);
         divide(e_mx, ui_type(1), e_px);

         if(p_sinh) 
         { 
            if(small_sinh)
            {
               small_sinh_series(x, *p_sinh);
            }
            else
            {
               subtract(*p_sinh, e_px, e_mx);
               divide(*p_sinh, ui_type(2));
            }
         }
         if(p_cosh) 
         { 
            add(*p_cosh, e_px, e_mx);
            divide(*p_cosh, ui_type(2)); 
         }
      }
      else
      {
         small_sinh_series(x, *p_sinh);
      }
   }

} // namespace detail

template <class T>
inline void eval_sinh(T& result, const T& x)
{
   detail::sinhcosh(x, &result, static_cast<T*>(0));
}

template <class T>
inline void eval_cosh(T& result, const T& x)
{
   detail::sinhcosh(x, static_cast<T*>(0), &result);
}

template <class T>
inline void eval_tanh(T& result, const T& x)
{
  T c;
  detail::sinhcosh(x, &result, &c);
  divide(result, c);
}

