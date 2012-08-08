///////////////////////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_COMPARE_HPP
#define BOOST_MP_COMPARE_HPP

//
// Comparison operators for number.
//

namespace boost{ namespace multiprecision{

namespace default_ops{

template <class B>
inline bool eval_eq(const B& a, const B& b)
{
   return a.compare(b) == 0;
}
template <class B, class A>
inline bool eval_eq(const B& a, const A& b)
{
   typedef typename mpl::if_<
      is_convertible<A, number<B, false> >, 
      number<B, false>, 
      number<B, true> 
   >::type mp_type;
   mp_type t(b);
   return eval_eq(a, t.backend());
}

template <class B>
inline bool eval_lt(const B& a, const B& b)
{
   return a.compare(b) < 0;
}
template <class B, class A>
inline bool eval_lt(const B& a, const A& b)
{
   typedef typename mpl::if_<
      is_convertible<A, number<B, false> >, 
      number<B, false>, 
      number<B, true> 
   >::type mp_type;
   mp_type t(b);
   return eval_lt(a, t.backend());
}

template <class B>
inline bool eval_gt(const B& a, const B& b)
{
   return a.compare(b) > 0;
}
template <class B, class A>
inline bool eval_gt(const B& a, const A& b)
{
   typedef typename mpl::if_<
      is_convertible<A, number<B, false> >, 
      number<B, false>, 
      number<B, true> 
   >::type mp_type;
   mp_type t(b);
   return eval_gt(a, t.backend());
}

}

template <class Backend, bool ExpressionTemplates>
inline bool operator == (const number<Backend, ExpressionTemplates>& a, const number<Backend, ExpressionTemplates>& b)
{
   using default_ops::eval_eq;
   return eval_eq(a.backend(), b.backend());
}
template <class Backend, bool ExpressionTemplates, class Arithmetic>
inline typename enable_if_c<(is_convertible<Arithmetic, number<Backend, ExpressionTemplates> >::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator == (const number<Backend, ExpressionTemplates>& a, const Arithmetic& b)
{
   typedef typename detail::canonical<Arithmetic, Backend>::type ct;
   using default_ops::eval_eq;
   return eval_eq(a.backend(), ct(b));
}
template <class Backend, bool ExpressionTemplates, class Tag, class A1, class A2, class A3, class A4>
inline typename enable_if<is_same<number<Backend, ExpressionTemplates>, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>, bool>::type 
   operator == (const number<Backend, ExpressionTemplates>& a, const detail::expression<Tag, A1, A2, A3, A4>& b)
{
   using default_ops::eval_eq;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(b);
   return eval_eq(a.backend(), t.backend());
}
template <class Arithmetic, class Backend, bool ExpressionTemplates>
inline typename enable_if_c<(is_convertible<Arithmetic, number<Backend, ExpressionTemplates> >::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator == (const Arithmetic& a, const number<Backend, ExpressionTemplates>& b)
{
   typedef typename detail::canonical<Arithmetic, Backend>::type ct;
   using default_ops::eval_eq;
   return eval_eq(b.backend(), ct(a));
}
template <class Arithmetic, class Tag, class A1, class A2, class A3, class A4>
inline typename enable_if_c<(is_convertible<Arithmetic, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator == (const Arithmetic& a, const detail::expression<Tag, A1, A2, A3, A4>& b)
{
   typedef typename detail::expression<Tag, A1, A2, A3, A4>::result_type result_type;
   typedef typename detail::canonical<Arithmetic, typename result_type::backend_type>::type ct;
   using default_ops::eval_eq;
   result_type t(b);
   return eval_eq(t.backend(), ct(a));
}
template <class Tag, class A1, class A2, class A3, class A4, class Backend, bool ExpressionTemplates>
inline typename enable_if<is_same<number<Backend, ExpressionTemplates>, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>, bool>::type 
   operator == (const detail::expression<Tag, A1, A2, A3, A4>& a, const number<Backend, ExpressionTemplates>& b)
{
   using default_ops::eval_eq;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(a);
   return eval_eq(t.backend(), b.backend());
}
template <class Tag, class A1, class A2, class A3, class A4, class Arithmetic>
inline typename enable_if_c<(is_convertible<Arithmetic, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator == (const detail::expression<Tag, A1, A2, A3, A4>& a, const Arithmetic& b)
{
   typedef typename detail::expression<Tag, A1, A2, A3, A4>::result_type result_type;
   typedef typename detail::canonical<Arithmetic, typename result_type::backend_type>::type ct;
   using default_ops::eval_eq;
   result_type t(a);
   return eval_eq(t.backend(), ct(b));
}
template <class Tag, class A1, class A2, class A3, class A4, class Tagb, class A1b, class A2b, class A3b, class A4b>
inline typename enable_if<is_same<typename detail::expression<Tag, A1, A2, A3, A4>::result_type, typename detail::expression<Tagb, A1b, A2b, A3b, A4b>::result_type>, bool>::type 
   operator == (const detail::expression<Tag, A1, A2, A3, A4>& a, const detail::expression<Tagb, A1b, A2b, A3b, A4b>& b)
{
   using default_ops::eval_eq;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(a);
   typename detail::expression<Tagb, A1b, A2b, A3b, A4b>::result_type t2(b);
   return eval_eq(t.backend(), t2.backend());
}

template <class Backend, bool ExpressionTemplates>
inline bool operator != (const number<Backend, ExpressionTemplates>& a, const number<Backend, ExpressionTemplates>& b)
{
   using default_ops::eval_eq;
   return !eval_eq(a.backend(), b.backend());
}
template <class Backend, bool ExpressionTemplates, class Arithmetic>
inline typename enable_if_c<(is_convertible<Arithmetic, number<Backend, ExpressionTemplates> >::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator != (const number<Backend, ExpressionTemplates>& a, const Arithmetic& b)
{
   typedef typename detail::canonical<Arithmetic, Backend>::type ct;
   using default_ops::eval_eq;
   return !eval_eq(a.backend(), ct(b));
}
template <class Backend, bool ExpressionTemplates, class Tag, class A1, class A2, class A3, class A4>
inline typename enable_if<is_same<number<Backend, ExpressionTemplates>, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>, bool>::type 
   operator != (const number<Backend, ExpressionTemplates>& a, const detail::expression<Tag, A1, A2, A3, A4>& b)
{
   using default_ops::eval_eq;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(b);
   return !eval_eq(a.backend(), t.backend());
}
template <class Arithmetic, class Backend, bool ExpressionTemplates>
inline typename enable_if_c<(is_convertible<Arithmetic, number<Backend, ExpressionTemplates> >::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator != (const Arithmetic& a, const number<Backend, ExpressionTemplates>& b)
{
   typedef typename detail::canonical<Arithmetic, Backend>::type ct;
   using default_ops::eval_eq;
   return !eval_eq(b.backend(), ct(a));
}
template <class Arithmetic, class Tag, class A1, class A2, class A3, class A4>
inline typename enable_if_c<(is_convertible<Arithmetic, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator != (const Arithmetic& a, const detail::expression<Tag, A1, A2, A3, A4>& b)
{
   typedef typename detail::expression<Tag, A1, A2, A3, A4>::result_type result_type;
   typedef typename detail::canonical<Arithmetic, typename result_type::backend_type>::type ct;
   using default_ops::eval_eq;
   result_type t(b);
   return !eval_eq(t.backend(), ct(a));
}
template <class Tag, class A1, class A2, class A3, class A4, class Backend, bool ExpressionTemplates>
inline typename enable_if<is_same<number<Backend, ExpressionTemplates>, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>, bool>::type 
   operator != (const detail::expression<Tag, A1, A2, A3, A4>& a, const number<Backend, ExpressionTemplates>& b)
{
   using default_ops::eval_eq;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(a);
   return !eval_eq(t.backend(), b.backend());
}
template <class Tag, class A1, class A2, class A3, class A4, class Arithmetic>
inline typename enable_if_c<(is_convertible<Arithmetic, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator != (const detail::expression<Tag, A1, A2, A3, A4>& a, const Arithmetic& b)
{
   typedef typename detail::expression<Tag, A1, A2, A3, A4>::result_type result_type;
   typedef typename detail::canonical<Arithmetic, typename result_type::backend_type>::type ct;
   using default_ops::eval_eq;
   result_type t(a);
   return !eval_eq(t.backend(), ct(b));
}
template <class Tag, class A1, class A2, class A3, class A4, class Tagb, class A1b, class A2b, class A3b, class A4b>
inline typename enable_if<is_same<typename detail::expression<Tag, A1, A2, A3, A4>::result_type, typename detail::expression<Tagb, A1b, A2b, A3b, A4b>::result_type>, bool>::type 
   operator != (const detail::expression<Tag, A1, A2, A3, A4>& a, const detail::expression<Tagb, A1b, A2b, A3b, A4b>& b)
{
   using default_ops::eval_eq;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(a);
   typename detail::expression<Tagb, A1b, A2b, A3b, A4b>::result_type t2(b);
   return !eval_eq(t.backend(), t2.backend());
}

template <class Backend, bool ExpressionTemplates>
inline bool operator < (const number<Backend, ExpressionTemplates>& a, const number<Backend, ExpressionTemplates>& b)
{
   using default_ops::eval_lt;
   return eval_lt(a.backend(), b.backend());
}
template <class Backend, bool ExpressionTemplates, class Arithmetic>
inline typename enable_if_c<(is_convertible<Arithmetic, number<Backend, ExpressionTemplates> >::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator < (const number<Backend, ExpressionTemplates>& a, const Arithmetic& b)
{
   typedef typename detail::canonical<Arithmetic, Backend>::type ct;
   using default_ops::eval_lt;
   return eval_lt(a.backend(), ct(b));
}
template <class Backend, bool ExpressionTemplates, class Tag, class A1, class A2, class A3, class A4>
inline typename enable_if<is_same<number<Backend, ExpressionTemplates>, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>, bool>::type 
   operator < (const number<Backend, ExpressionTemplates>& a, const detail::expression<Tag, A1, A2, A3, A4>& b)
{
   using default_ops::eval_lt;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(b);
   return eval_lt(a.backend(), t.backend());
}
template <class Arithmetic, class Backend, bool ExpressionTemplates>
inline typename enable_if_c<(is_convertible<Arithmetic, number<Backend, ExpressionTemplates> >::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator < (const Arithmetic& a, const number<Backend, ExpressionTemplates>& b)
{
   typedef typename detail::canonical<Arithmetic, Backend>::type ct;
   using default_ops::eval_gt;
   return eval_gt(b.backend(), ct(a));
}
template <class Arithmetic, class Tag, class A1, class A2, class A3, class A4>
inline typename enable_if_c<(is_convertible<Arithmetic, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator < (const Arithmetic& a, const detail::expression<Tag, A1, A2, A3, A4>& b)
{
   typedef typename detail::expression<Tag, A1, A2, A3, A4>::result_type result_type;
   typedef typename detail::canonical<Arithmetic, typename result_type::backend_type>::type ct;
   using default_ops::eval_gt;
   result_type t(b);
   return eval_gt(t.backend(), ct(a));
}
template <class Tag, class A1, class A2, class A3, class A4, class Backend, bool ExpressionTemplates>
inline typename enable_if<is_same<number<Backend, ExpressionTemplates>, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>, bool>::type 
   operator < (const detail::expression<Tag, A1, A2, A3, A4>& a, const number<Backend, ExpressionTemplates>& b)
{
   using default_ops::eval_lt;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(a);
   return eval_lt(t.backend(), b.backend());
}
template <class Tag, class A1, class A2, class A3, class A4, class Arithmetic>
inline typename enable_if_c<(is_convertible<Arithmetic, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator < (const detail::expression<Tag, A1, A2, A3, A4>& a, const Arithmetic& b)
{
   typedef typename detail::expression<Tag, A1, A2, A3, A4>::result_type result_type;
   typedef typename detail::canonical<Arithmetic, typename result_type::backend_type>::type ct;
   using default_ops::eval_lt;
   result_type t(a);
   return eval_lt(t.backend(), ct(b));
}
template <class Tag, class A1, class A2, class A3, class A4, class Tagb, class A1b, class A2b, class A3b, class A4b>
inline typename enable_if<is_same<typename detail::expression<Tag, A1, A2, A3, A4>::result_type, typename detail::expression<Tagb, A1b, A2b, A3b, A4b>::result_type>, bool>::type 
   operator < (const detail::expression<Tag, A1, A2, A3, A4>& a, const detail::expression<Tagb, A1b, A2b, A3b, A4b>& b)
{
   using default_ops::eval_lt;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(a);
   typename detail::expression<Tagb, A1b, A2b, A3b, A4b>::result_type t2(b);
   return eval_lt(t.backend(), t2.backend());
}

template <class Backend, bool ExpressionTemplates>
inline bool operator > (const number<Backend, ExpressionTemplates>& a, const number<Backend, ExpressionTemplates>& b)
{
   using default_ops::eval_gt;
   return eval_gt(a.backend(), b.backend());
}
template <class Backend, bool ExpressionTemplates, class Arithmetic>
inline typename enable_if_c<(is_convertible<Arithmetic, number<Backend, ExpressionTemplates> >::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator > (const number<Backend, ExpressionTemplates>& a, const Arithmetic& b)
{
   typedef typename detail::canonical<Arithmetic, Backend>::type ct;
   using default_ops::eval_gt;
   return eval_gt(a.backend(), ct(b));
}
template <class Backend, bool ExpressionTemplates, class Tag, class A1, class A2, class A3, class A4>
inline typename enable_if<is_same<number<Backend, ExpressionTemplates>, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>, bool>::type 
   operator > (const number<Backend, ExpressionTemplates>& a, const detail::expression<Tag, A1, A2, A3, A4>& b)
{
   using default_ops::eval_gt;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(b);
   return eval_gt(a.backend(), t.backend());
}
template <class Arithmetic, class Backend, bool ExpressionTemplates>
inline typename enable_if_c<(is_convertible<Arithmetic, number<Backend, ExpressionTemplates> >::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator > (const Arithmetic& a, const number<Backend, ExpressionTemplates>& b)
{
   typedef typename detail::canonical<Arithmetic, Backend>::type ct;
   using default_ops::eval_lt;
   return eval_lt(b.backend(), ct(a));
}
template <class Arithmetic, class Tag, class A1, class A2, class A3, class A4>
inline typename enable_if_c<(is_convertible<Arithmetic, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator > (const Arithmetic& a, const detail::expression<Tag, A1, A2, A3, A4>& b)
{
   typedef typename detail::expression<Tag, A1, A2, A3, A4>::result_type result_type;
   typedef typename detail::canonical<Arithmetic, typename result_type::backend_type>::type ct;
   using default_ops::eval_lt;
   result_type t(b);
   return eval_lt(t.backend(), ct(a));
}
template <class Tag, class A1, class A2, class A3, class A4, class Backend, bool ExpressionTemplates>
inline typename enable_if<is_same<number<Backend, ExpressionTemplates>, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>, bool>::type 
   operator > (const detail::expression<Tag, A1, A2, A3, A4>& a, const number<Backend, ExpressionTemplates>& b)
{
   using default_ops::eval_gt;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(a);
   return eval_gt(t.backend(), b.backend());
}
template <class Tag, class A1, class A2, class A3, class A4, class Arithmetic>
inline typename enable_if_c<(is_convertible<Arithmetic, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator > (const detail::expression<Tag, A1, A2, A3, A4>& a, const Arithmetic& b)
{
   typedef typename detail::expression<Tag, A1, A2, A3, A4>::result_type result_type;
   typedef typename detail::canonical<Arithmetic, typename result_type::backend_type>::type ct;
   using default_ops::eval_gt;
   result_type t(a);
   return eval_gt(t.backend(), ct(b));
}
template <class Tag, class A1, class A2, class A3, class A4, class Tagb, class A1b, class A2b, class A3b, class A4b>
inline typename enable_if<is_same<typename detail::expression<Tag, A1, A2, A3, A4>::result_type, typename detail::expression<Tagb, A1b, A2b, A3b, A4b>::result_type>, bool>::type 
   operator > (const detail::expression<Tag, A1, A2, A3, A4>& a, const detail::expression<Tagb, A1b, A2b, A3b, A4b>& b)
{
   using default_ops::eval_gt;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(a);
   typename detail::expression<Tagb, A1b, A2b, A3b, A4b>::result_type t2(b);
   return eval_gt(t.backend(), t2.backend());
}

template <class Backend, bool ExpressionTemplates>
inline bool operator <= (const number<Backend, ExpressionTemplates>& a, const number<Backend, ExpressionTemplates>& b)
{
   using default_ops::eval_gt;
   return !eval_gt(a.backend(), b.backend());
}
template <class Backend, bool ExpressionTemplates, class Arithmetic>
inline typename enable_if_c<(is_convertible<Arithmetic, number<Backend, ExpressionTemplates> >::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator <= (const number<Backend, ExpressionTemplates>& a, const Arithmetic& b)
{
   typedef typename detail::canonical<Arithmetic, Backend>::type ct;
   using default_ops::eval_gt;
   return !eval_gt(a.backend(), ct(b));
}
template <class Backend, bool ExpressionTemplates, class Tag, class A1, class A2, class A3, class A4>
inline typename enable_if<is_same<number<Backend, ExpressionTemplates>, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>, bool>::type 
   operator <= (const number<Backend, ExpressionTemplates>& a, const detail::expression<Tag, A1, A2, A3, A4>& b)
{
   using default_ops::eval_gt;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(b);
   return !eval_gt(a.backend(), t.backend());
}
template <class Arithmetic, class Backend, bool ExpressionTemplates>
inline typename enable_if_c<(is_convertible<Arithmetic, number<Backend, ExpressionTemplates> >::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator <= (const Arithmetic& a, const number<Backend, ExpressionTemplates>& b)
{
   typedef typename detail::canonical<Arithmetic, Backend>::type ct;
   using default_ops::eval_lt;
   return !eval_lt(b.backend(), ct(a));
}
template <class Arithmetic, class Tag, class A1, class A2, class A3, class A4>
inline typename enable_if_c<(is_convertible<Arithmetic, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator <= (const Arithmetic& a, const detail::expression<Tag, A1, A2, A3, A4>& b)
{
   typedef typename detail::expression<Tag, A1, A2, A3, A4>::result_type result_type;
   typedef typename detail::canonical<Arithmetic, typename result_type::backend_type>::type ct;
   using default_ops::eval_lt;
   result_type t(b);
   return !eval_lt(t.backend(), ct(a));
}
template <class Tag, class A1, class A2, class A3, class A4, class Backend, bool ExpressionTemplates>
inline typename enable_if<is_same<number<Backend, ExpressionTemplates>, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>, bool>::type 
   operator <= (const detail::expression<Tag, A1, A2, A3, A4>& a, const number<Backend, ExpressionTemplates>& b)
{
   using default_ops::eval_gt;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(a);
   return !eval_gt(t.backend(), b.backend());
}
template <class Tag, class A1, class A2, class A3, class A4, class Arithmetic>
inline typename enable_if_c<(is_convertible<Arithmetic, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator <= (const detail::expression<Tag, A1, A2, A3, A4>& a, const Arithmetic& b)
{
   typedef typename detail::expression<Tag, A1, A2, A3, A4>::result_type result_type;
   typedef typename detail::canonical<Arithmetic, typename result_type::backend_type>::type ct;
   using default_ops::eval_gt;
   result_type t(a);
   return !eval_gt(t.backend(), ct(b));
}
template <class Tag, class A1, class A2, class A3, class A4, class Tagb, class A1b, class A2b, class A3b, class A4b>
inline typename enable_if<is_same<typename detail::expression<Tag, A1, A2, A3, A4>::result_type, typename detail::expression<Tagb, A1b, A2b, A3b, A4b>::result_type>, bool>::type 
   operator <= (const detail::expression<Tag, A1, A2, A3, A4>& a, const detail::expression<Tagb, A1b, A2b, A3b, A4b>& b)
{
   using default_ops::eval_gt;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(a);
   typename detail::expression<Tagb, A1b, A2b, A3b, A4b>::result_type t2(b);
   return !eval_gt(t.backend(), t2.backend());
}

template <class Backend, bool ExpressionTemplates>
inline bool operator >= (const number<Backend, ExpressionTemplates>& a, const number<Backend, ExpressionTemplates>& b)
{
   using default_ops::eval_lt;
   return !eval_lt(a.backend(), b.backend());
}
template <class Backend, bool ExpressionTemplates, class Arithmetic>
inline typename enable_if_c<(is_convertible<Arithmetic, number<Backend, ExpressionTemplates> >::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator >= (const number<Backend, ExpressionTemplates>& a, const Arithmetic& b)
{
   typedef typename detail::canonical<Arithmetic, Backend>::type ct;
   using default_ops::eval_lt;
   return !eval_lt(a.backend(), ct(b));
}
template <class Backend, bool ExpressionTemplates, class Tag, class A1, class A2, class A3, class A4>
inline typename enable_if<is_same<number<Backend, ExpressionTemplates>, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>, bool>::type 
   operator >= (const number<Backend, ExpressionTemplates>& a, const detail::expression<Tag, A1, A2, A3, A4>& b)
{
   using default_ops::eval_lt;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(b);
   return !eval_lt(a.backend(), t.backend());
}
template <class Arithmetic, class Backend, bool ExpressionTemplates>
inline typename enable_if_c<(is_convertible<Arithmetic, number<Backend, ExpressionTemplates> >::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator >= (const Arithmetic& a, const number<Backend, ExpressionTemplates>& b)
{
   typedef typename detail::canonical<Arithmetic, Backend>::type ct;
   using default_ops::eval_gt;
   return !eval_gt(b.backend(), ct(a));
}
template <class Arithmetic, class Tag, class A1, class A2, class A3, class A4>
inline typename enable_if_c<(is_convertible<Arithmetic, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator >= (const Arithmetic& a, const detail::expression<Tag, A1, A2, A3, A4>& b)
{
   typedef typename detail::expression<Tag, A1, A2, A3, A4>::result_type result_type;
   typedef typename detail::canonical<Arithmetic, typename result_type::backend_type>::type ct;
   using default_ops::eval_gt;
   result_type t(b);
   return !eval_gt(t.backend(), ct(a));
}
template <class Tag, class A1, class A2, class A3, class A4, class Backend, bool ExpressionTemplates>
inline typename enable_if<is_same<number<Backend, ExpressionTemplates>, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>, bool>::type 
   operator >= (const detail::expression<Tag, A1, A2, A3, A4>& a, const number<Backend, ExpressionTemplates>& b)
{
   using default_ops::eval_lt;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(a);
   return !eval_lt(t.backend(), b.backend());
}
template <class Tag, class A1, class A2, class A3, class A4, class Arithmetic>
inline typename enable_if_c<(is_convertible<Arithmetic, typename detail::expression<Tag, A1, A2, A3, A4>::result_type>::value && !is_mp_number_expression<Arithmetic>::value && !is_mp_number<Arithmetic>::value), bool>::type 
   operator >= (const detail::expression<Tag, A1, A2, A3, A4>& a, const Arithmetic& b)
{
   typedef typename detail::expression<Tag, A1, A2, A3, A4>::result_type result_type;
   typedef typename detail::canonical<Arithmetic, typename result_type::backend_type>::type ct;
   using default_ops::eval_lt;
   result_type t(a);
   return !eval_lt(t.backend(), ct(b));
}
template <class Tag, class A1, class A2, class A3, class A4, class Tagb, class A1b, class A2b, class A3b, class A4b>
inline typename enable_if<is_same<typename detail::expression<Tag, A1, A2, A3, A4>::result_type, typename detail::expression<Tagb, A1b, A2b, A3b, A4b>::result_type>, bool>::type 
   operator >= (const detail::expression<Tag, A1, A2, A3, A4>& a, const detail::expression<Tagb, A1b, A2b, A3b, A4b>& b)
{
   using default_ops::eval_lt;
   typename detail::expression<Tag, A1, A2, A3, A4>::result_type t(a);
   typename detail::expression<Tagb, A1b, A2b, A3b, A4b>::result_type t2(b);
   return !eval_lt(t.backend(), t2.backend());
}


}} // namespaces

#endif // BOOST_MP_COMPARE_HPP

