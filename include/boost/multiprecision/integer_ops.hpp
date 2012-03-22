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

template <class Backend, bool ExpressionTemplates, class tag, class A1, class A2, class A3>
inline typename enable_if_c<number_category<Backend>::value == number_kind_integer>::type 
   divide_qr(const mp_number<Backend, ExpressionTemplates>& x, const multiprecision::detail::mp_exp<tag, A1, A2, A3>& y,
   mp_number<Backend, ExpressionTemplates>& q, mp_number<Backend, ExpressionTemplates>& r)
{
   divide_qr(x, mp_number<Backend, ExpressionTemplates>(y), q, r);
}

template <class tag, class A1, class A2, class A3, class Backend, bool ExpressionTemplates>
inline typename enable_if_c<number_category<Backend>::value == number_kind_integer>::type 
   divide_qr(const multiprecision::detail::mp_exp<tag, A1, A2, A3>& x, const mp_number<Backend, ExpressionTemplates>& y,
   mp_number<Backend, ExpressionTemplates>& q, mp_number<Backend, ExpressionTemplates>& r)
{
   divide_qr(mp_number<Backend, ExpressionTemplates>(x), y, q, r);
}

template <class tag, class A1, class A2, class A3, class tagb, class A1b, class A2b, class A3b, class Backend, bool ExpressionTemplates>
inline typename enable_if_c<number_category<Backend>::value == number_kind_integer>::type 
   divide_qr(const multiprecision::detail::mp_exp<tag, A1, A2, A3>& x, const multiprecision::detail::mp_exp<tagb, A1b, A2b, A3b>& y,
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

template <class tag, class A1, class A2, class A3, class Integer>
inline typename enable_if<mpl::and_<is_integral<Integer>, mpl::bool_<number_category<typename multiprecision::detail::mp_exp<tag, A1, A2, A3>::result_type>::value == number_kind_integer> >, Integer>::type 
   integer_modulus(const multiprecision::detail::mp_exp<tag, A1, A2, A3>& x, Integer val)
{
   typedef typename multiprecision::detail::mp_exp<tag, A1, A2, A3>::result_type result_type;
   return integer_modulus(result_type(x), val);
}

}} //namespaces

#endif


