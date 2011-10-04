
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

template<typename T> 
inline void eval_pow(T& result, const T& t, const T& p)
{
}

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

   long tol = std::numeric_limits<mp_number<T> >::digits;
   T t;

   T x_pow_n_div_n_fact(x);

   add(H0F0, x_pow_n_div_n_fact, ui_type(1));

   ui_type n;

   // Series expansion of hyperg_0f0(; ; x).
   for(n = 2; n < 300; ++n)
   {
      multiply(x_pow_n_div_n_fact, x);
      divide(x_pow_n_div_n_fact, n);

      if(n > 20)
      {
         eval_ldexp(t, H0F0, -tol);
         if(t.compare(x_pow_n_div_n_fact) >= 0)
         {
            break;
         }
      }

      add(H0F0, x_pow_n_div_n_fact);
   }
   if(n >= 300)
      throw std::runtime_error("H0F0 failed to converge");
}


template <class T>
void eval_exp(T& result, const T& x)
{
   if(&x == &result)
   {
      T temp;
      eval_exp(temp, x);
      result = temp;
   }
   typedef typename boost::multiprecision::detail::canonical<unsigned, T>::type ui_type;
   typedef typename boost::multiprecision::detail::canonical<int, T>::type si_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;
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
   static const canonical_exp_type maximum_arg_for_exp = std::numeric_limits<mp_number<T> >::max_exponent == 0 ? std::numeric_limits<long>::max() : std::numeric_limits<mp_number<T> >::max_exponent;

   if(xx.compare(maximum_arg_for_exp) >= 0)
   {
      // Overflow / underflow
      if(isneg)
         result = ui_type(0);
      else
         result = std::numeric_limits<mp_number<T> >::has_infinity ? std::numeric_limits<mp_number<T> >::infinity().backend() : (std::numeric_limits<mp_number<T> >::max)().backend();
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

   typedef typename mpl::front<typename T::real_types>::type float_type;

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
   }

   typedef typename boost::multiprecision::detail::canonical<int, T>::type si_type;
   typedef typename boost::multiprecision::detail::canonical<unsigned, T>::type ui_type;
   typedef typename T::exponent_type exp_type;
   typedef typename boost::multiprecision::detail::canonical<exp_type, T>::type canonical_exp_type;

   exp_type e;
   T t;
   eval_frexp(t, arg, &e);
   
   multiply(result, get_constant_ln2<T>(), canonical_exp_type(e));
   subtract(t, ui_type(1)); /* -0.5 <= t <= 0 */
   t.negate(); /* 0 <= t <= 0.5 */
   T pow = t;
   T lim;
   T t2;
   multiply(lim, result, std::numeric_limits<mp_number<T> >::epsilon().backend());
   if(get_sign(lim) < 0)
      lim.negate();

   subtract(result, t);

   ui_type k = 1;
   do
   {
      ++k;
      multiply(pow, t);
      divide(t2, pow, k);
      subtract(result, t2);
   }while(lim.compare(t2) <= 0);
}

