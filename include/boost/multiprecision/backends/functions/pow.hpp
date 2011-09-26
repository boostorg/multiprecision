
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


