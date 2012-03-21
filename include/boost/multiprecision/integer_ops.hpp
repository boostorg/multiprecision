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
inline void divide_qr(const mp_number<Backend, ExpressionTemplates>& x, const mp_number<Backend, ExpressionTemplates>& y,
   mp_number<Backend, ExpressionTemplates>& q, mp_number<Backend, ExpressionTemplates>& r)
{
   using default_ops::eval_qr;
   eval_qr(x.backend(), y.backend(), q.backend(), r.backend());
}

template <class Backend, bool ExpressionTemplates, class Integer>
inline typename enable_if<is_integral<Integer>, Integer>::type integer_modulus(const mp_number<Backend, ExpressionTemplates>& x, Integer val)
{
   using default_ops::eval_integer_modulus;
   return eval_integer_modulus(x.backend(), val);
}

}} //namespaces

#endif


