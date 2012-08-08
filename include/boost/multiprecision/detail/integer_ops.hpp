///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MP_INT_FUNC_HPP
#define BOOST_MP_INT_FUNC_HPP

#include <boost/multiprecision/mp_number.hpp>

namespace boost{ namespace multiprecision{

namespace default_ops
{

template <class Backend>
inline void eval_qr(const Backend& x, const Backend& y, Backend& q, Backend& r)
{
   eval_divide(q, x, y);
   eval_modulus(r, x, y);
}

template <class Integer>
inline typename enable_if<is_signed<Integer>, Integer>::type maybe_abs(Integer i) { return std::abs(i); }
template <class Integer>
inline typename enable_if<is_unsigned<Integer>, Integer>::type maybe_abs(Integer i) { return i; }

template <class Backend, class Integer>
inline Integer eval_integer_modulus(const Backend& x, Integer val)
{
   using default_ops::eval_modulus;
   using default_ops::eval_convert_to;
   typedef typename boost::multiprecision::detail::canonical<Integer, Backend>::type int_type;
   Backend t;
   eval_modulus(t, x, static_cast<int_type>(val));
   Integer result;
   eval_convert_to(&result, t);
   return maybe_abs(result);
}

}

template <class Backend, bool ExpressionTemplates>
inline typename enable_if_c<number_category<Backend>::value == number_kind_integer>::type 
   divide_qr(const mp_number<Backend, ExpressionTemplates>& x, const mp_number<Backend, ExpressionTemplates>& y,
   mp_number<Backend, ExpressionTemplates>& q, mp_number<Backend, ExpressionTemplates>& r)
{
   using default_ops::eval_qr;
   eval_qr(x.backend(), y.backend(), q.backend(), r.backend());
}

template <class Backend, bool ExpressionTemplates, class tag, class A1, class A2, class A3, class A4>
inline typename enable_if_c<number_category<Backend>::value == number_kind_integer>::type 
   divide_qr(const mp_number<Backend, ExpressionTemplates>& x, const multiprecision::detail::mp_exp<tag, A1, A2, A3, A4>& y,
   mp_number<Backend, ExpressionTemplates>& q, mp_number<Backend, ExpressionTemplates>& r)
{
   divide_qr(x, mp_number<Backend, ExpressionTemplates>(y), q, r);
}

template <class tag, class A1, class A2, class A3, class A4, class Backend, bool ExpressionTemplates>
inline typename enable_if_c<number_category<Backend>::value == number_kind_integer>::type 
   divide_qr(const multiprecision::detail::mp_exp<tag, A1, A2, A3, A4>& x, const mp_number<Backend, ExpressionTemplates>& y,
   mp_number<Backend, ExpressionTemplates>& q, mp_number<Backend, ExpressionTemplates>& r)
{
   divide_qr(mp_number<Backend, ExpressionTemplates>(x), y, q, r);
}

template <class tag, class A1, class A2, class A3, class A4, class tagb, class A1b, class A2b, class A3b, class A4b, class Backend, bool ExpressionTemplates>
inline typename enable_if_c<number_category<Backend>::value == number_kind_integer>::type 
   divide_qr(const multiprecision::detail::mp_exp<tag, A1, A2, A3, A4>& x, const multiprecision::detail::mp_exp<tagb, A1b, A2b, A3b, A4b>& y,
   mp_number<Backend, ExpressionTemplates>& q, mp_number<Backend, ExpressionTemplates>& r)
{
   divide_qr(mp_number<Backend, ExpressionTemplates>(x), mp_number<Backend, ExpressionTemplates>(y), q, r);
}

template <class Backend, bool ExpressionTemplates, class Integer>
inline typename enable_if<mpl::and_<is_integral<Integer>, mpl::bool_<number_category<Backend>::value == number_kind_integer> >, Integer>::type 
   integer_modulus(const mp_number<Backend, ExpressionTemplates>& x, Integer val)
{
   using default_ops::eval_integer_modulus;
   return eval_integer_modulus(x.backend(), val);
}

template <class tag, class A1, class A2, class A3, class A4, class Integer>
inline typename enable_if<mpl::and_<is_integral<Integer>, mpl::bool_<number_category<typename multiprecision::detail::mp_exp<tag, A1, A2, A3, A4>::result_type>::value == number_kind_integer> >, Integer>::type 
   integer_modulus(const multiprecision::detail::mp_exp<tag, A1, A2, A3, A4>& x, Integer val)
{
   typedef typename multiprecision::detail::mp_exp<tag, A1, A2, A3, A4>::result_type result_type;
   return integer_modulus(result_type(x), val);
}

template <class Backend, bool ExpressionTemplates>
inline typename enable_if_c<number_category<Backend>::value == number_kind_integer, unsigned>::type 
   lsb(const mp_number<Backend, ExpressionTemplates>& x)
{
   using default_ops::eval_lsb;
   return eval_lsb(x.backend());
}

template <class tag, class A1, class A2, class A3, class A4>
inline typename enable_if_c<number_category<typename multiprecision::detail::mp_exp<tag, A1, A2, A3, A4>::result_type>::value == number_kind_integer, unsigned>::type 
   lsb(const multiprecision::detail::mp_exp<tag, A1, A2, A3, A4>& x)
{
   typedef typename multiprecision::detail::mp_exp<tag, A1, A2, A3, A4>::result_type number_type;
   number_type n(x);
   using default_ops::eval_lsb;
   return eval_lsb(n.backend());
}

template <class Backend, bool ExpressionTemplates>
inline typename enable_if_c<number_category<Backend>::value == number_kind_integer, bool>::type 
   bit_test(const mp_number<Backend, ExpressionTemplates>& x, unsigned index)
{
   using default_ops::eval_bit_test;
   return eval_bit_test(x.backend(), index);
}

template <class tag, class A1, class A2, class A3, class A4>
inline typename enable_if_c<number_category<typename multiprecision::detail::mp_exp<tag, A1, A2, A3, A4>::result_type>::value == number_kind_integer, bool>::type 
   bit_test(const multiprecision::detail::mp_exp<tag, A1, A2, A3, A4>& x, unsigned index)
{
   typedef typename multiprecision::detail::mp_exp<tag, A1, A2, A3, A4>::result_type number_type;
   number_type n(x);
   using default_ops::eval_bit_test;
   return eval_bit_test(n.backend(), index);
}

template <class Backend, bool ExpressionTemplates>
inline typename enable_if_c<number_category<Backend>::value == number_kind_integer, mp_number<Backend, ExpressionTemplates>&>::type 
   bit_set(mp_number<Backend, ExpressionTemplates>& x, unsigned index)
{
   using default_ops::eval_bit_set;
   eval_bit_set(x.backend(), index);
   return x;
}

template <class Backend, bool ExpressionTemplates>
inline typename enable_if_c<number_category<Backend>::value == number_kind_integer, mp_number<Backend, ExpressionTemplates>&>::type 
   bit_unset(mp_number<Backend, ExpressionTemplates>& x, unsigned index)
{
   using default_ops::eval_bit_unset;
   eval_bit_unset(x.backend(), index);
   return x;
}

template <class Backend, bool ExpressionTemplates>
inline typename enable_if_c<number_category<Backend>::value == number_kind_integer, mp_number<Backend, ExpressionTemplates>&>::type 
   bit_flip(mp_number<Backend, ExpressionTemplates>& x, unsigned index)
{
   using default_ops::eval_bit_flip;
   eval_bit_flip(x.backend(), index);
   return x;
}

namespace detail{

//
// Calculate (a^p)%c:
//
template <class Backend>
void eval_powm(Backend& result, const Backend& a, const Backend& p, const Backend& c)
{
   using default_ops::eval_bit_test;
   using default_ops::eval_get_sign;
   using default_ops::eval_multiply;
   using default_ops::eval_modulus;
   using default_ops::eval_right_shift;

   typedef typename canonical<unsigned char, Backend>::type ui_type;
   
   Backend x, y(a), b(p);
   x = ui_type(1u);
   while(eval_get_sign(b) > 0)
   {
      if(eval_bit_test(b, 0))
      {
         eval_multiply(result, x, y);
         eval_modulus(x, result, c);
      }
      eval_multiply(result, y, y);
      eval_modulus(y, result, c);
      eval_right_shift(b, ui_type(1));
   }
   eval_modulus(result, x, c);
}

template <class Backend, class Integer>
void eval_powm(Backend& result, const Backend& a, const Backend& p, Integer c)
{
   typedef typename canonical<unsigned char, Backend>::type ui_type;
   typedef typename canonical<Integer, Backend>::type i_type;

   using default_ops::eval_bit_test;
   using default_ops::eval_get_sign;
   using default_ops::eval_multiply;
   using default_ops::eval_modulus;
   using default_ops::eval_right_shift;

   if(eval_get_sign(p) < 0)
   {
      BOOST_THROW_EXCEPTION(std::runtime_error("powm requires a positive exponent."));
   }

   Backend x, y(a), b(p);
   x = ui_type(1u);
   while(eval_get_sign(b) > 0)
   {
      if(eval_bit_test(b, 0))
      {
         eval_multiply(result, x, y);
         eval_modulus(x, result, static_cast<i_type>(c));
      }
      eval_multiply(result, y, y);
      eval_modulus(y, result, static_cast<i_type>(c));
      eval_right_shift(b, ui_type(1));
   }
   eval_modulus(result, x, static_cast<i_type>(c));
}

template <class Backend, class Integer>
void eval_powm(Backend& result, const Backend& a, Integer b, const Backend& c)
{
   typedef typename canonical<unsigned char, Backend>::type ui_type;
   typedef typename canonical<Integer, Backend>::type i_type;

   using default_ops::eval_bit_test;
   using default_ops::eval_get_sign;
   using default_ops::eval_multiply;
   using default_ops::eval_modulus;
   using default_ops::eval_right_shift;

   if(b < 0)
   {
      BOOST_THROW_EXCEPTION(std::runtime_error("powm requires a positive exponent."));
   }

   Backend x, y(a);
   x = ui_type(1u);
   while(b > 0)
   {
      if(b & 1)
      {
         eval_multiply(result, x, y);
         eval_modulus(x, result, c);
      }
      eval_multiply(result, y, y);
      eval_modulus(y, result, c);
      b >>= 1;
   }
   eval_modulus(result, x, c);
}

template <class Backend, class Integer1, class Integer2>
void eval_powm(Backend& result, const Backend& a, Integer1 b, Integer2 c)
{
   typedef typename canonical<unsigned char, Backend>::type ui_type;
   typedef typename canonical<Integer1, Backend>::type i1_type;
   typedef typename canonical<Integer2, Backend>::type i2_type;

   using default_ops::eval_bit_test;
   using default_ops::eval_get_sign;
   using default_ops::eval_multiply;
   using default_ops::eval_modulus;
   using default_ops::eval_right_shift;

   if(b < 0)
   {
      BOOST_THROW_EXCEPTION(std::runtime_error("powm requires a positive exponent."));
   }

   Backend x, y(a);
   x = ui_type(1u);
   while(b > 0)
   {
      if(b & 1)
      {
         eval_multiply(result, x, y);
         eval_modulus(x, result, static_cast<i2_type>(c));
      }
      eval_multiply(result, y, y);
      eval_modulus(y, result, static_cast<i2_type>(c));
      b >>= 1;
   }
   eval_modulus(result, x, static_cast<i2_type>(c));
}

struct powm_func
{
   template <class T, class U, class V>
   void operator()(T& result, const T& b, const U& p, const V& m)const
   {
      eval_powm(result, b, p, m);
   }
};

}

template <class T, class U, class V>
inline typename enable_if<
   mpl::and_<
      mpl::bool_<number_category<T>::value == number_kind_integer>, 
      mpl::or_<
         is_mp_number<T>,
         is_mp_number_expression<T>
      >,
      mpl::or_<
         is_mp_number<U>,
         is_mp_number_expression<U>,
         is_integral<U>
      >,
      mpl::or_<
         is_mp_number<V>,
         is_mp_number_expression<V>,
         is_integral<V>
      >
   >,
   detail::mp_exp<detail::function, detail::powm_func, T, U, V> >::type 
   powm(const T& b, const U& p, const V& mod)
{
   return detail::mp_exp<detail::function, detail::powm_func, T, U, V>(
      detail::powm_func(), b, p, mod);
}

}} //namespaces

#endif


