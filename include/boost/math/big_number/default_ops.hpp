///////////////////////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_BIG_NUM_DEF_OPS
#define BOOST_MATH_BIG_NUM_DEF_OPS

#include <boost/math/policies/error_handling.hpp>
#include <boost/math/big_number/big_number_base.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/lexical_cast.hpp>

namespace boost{ namespace math{ namespace big_num_default_ops{

//
// Default versions of mixed arithmetic, these just construct a temporary
// from the arithmetic value and then do the arithmetic on that:
//
template <class T, class V>
inline typename enable_if<mpl::or_<is_arithmetic<V>, is_convertible<V, const char*>, is_same<V, std::string> > >::type 
   add(T& result, V const& v)
{
   T t;
   t = v;
   add(result, t);
}
template <class T, class V>
inline typename enable_if<mpl::or_<is_arithmetic<V>, is_convertible<V, const char*>, is_same<V, std::string> > >::type
   subtract(T& result, V const& v)
{
   T t;
   t = v;
   subtract(result, t);
}
template <class T, class V>
inline typename enable_if<mpl::or_<is_arithmetic<V>, is_convertible<V, const char*>, is_same<V, std::string> > >::type
   multiply(T& result, V const& v)
{
   T t;
   t = v;
   multiply(result, t);
}
template <class T, class V>
inline typename enable_if<mpl::or_<is_arithmetic<V>, is_convertible<V, const char*>, is_same<V, std::string> > >::type
   divide(T& result, V const& v)
{
   T t;
   t = v;
   divide(result, t);
}
template <class T, class V>
inline typename enable_if<mpl::or_<is_arithmetic<V>, is_convertible<V, const char*>, is_same<V, std::string> > >::type
   modulus(T& result, V const& v)
{
   T t;
   t = v;
   modulus(result, t);
}
template <class T, class V>
inline typename enable_if<mpl::or_<is_arithmetic<V>, is_convertible<V, const char*>, is_same<V, std::string> > >::type
   bitwise_and(T& result, V const& v)
{
   T t;
   t = v;
   bitwise_and(result, t);
}
template <class T, class V>
inline typename enable_if<mpl::or_<is_arithmetic<V>, is_convertible<V, const char*>, is_same<V, std::string> > >::type
   bitwise_or(T& result, V const& v)
{
   T t;
   t = v;
   bitwise_or(result, t);
}
template <class T, class V>
inline typename enable_if<mpl::or_<is_arithmetic<V>, is_convertible<V, const char*>, is_same<V, std::string> > >::type
   bitwise_xor(T& result, V const& v)
{
   T t;
   t = v;
   bitwise_xor(result, t);
}

template <class T, class V>
inline typename enable_if<mpl::or_<is_arithmetic<V>, is_convertible<V, const char*>, is_same<V, std::string> > >::type
   complement(T& result, V const& v)
{
   T t;
   t = v;
   complement(result, t);
}

template <class T>
inline bool is_same_object(const T& u, const T&v)
{  return &u == &v;  }
template <class T, class U>
inline bool is_same_object(const T& u, const U&v)
{  return false;  }

//
// Default versions of 3-arg arithmetic functions, these just forward to the 2 arg versions:
//
template <class T, class U, class V>
inline void add(T& t, const U& u, const V& v)
{
   if(is_same_object(t, v))
   {
      add(t, u);
   }
   else if(is_same_object(t, u))
   {
      add(t, v);
   }
   else
   {
      t = u;
      add(t, v);
   }
}
template <class T, class U, class V>
inline void subtract(T& t, const U& u, const V& v)
{
   if(is_same_object(t, u))
      subtract(t, v);
   else if(is_same_object(t, v))
   {
      subtract(t, u);
      t.negate();
   }
   else
   {
      t = u;
      subtract(t, v);
   }
}
template <class T, class U, class V>
inline void multiply(T& t, const U& u, const V& v)
{
   if(is_same_object(t, u))
      multiply(t, v);
   else if(is_same_object(t, v))
      multiply(t, u);
   else
   {
      t = u;
      multiply(t, v);
   }
}
template <class T, class U, class V>
inline void divide(T& t, const U& u, const V& v)
{
   if(is_same_object(t, u))
      divide(t, v);
   else if(is_same_object(t, v))
   {
      T temp = t;
      divide(temp, u, v);
      temp.swap(t);
   }
   else
   {
      t = u;
      divide(t, v);
   }
}
template <class T, class U, class V>
inline void modulus(T& t, const U& u, const V& v)
{
   if(is_same_object(t, u))
      modulus(t, v);
   else if(is_same_object(t, v))
   {
      T temp;
      modulus(temp, u, v);
      temp.swap(t);
   }
   else
   {
      t = u;
      modulus(t, v);
   }
}
template <class T, class U, class V>
inline void bitwise_and(T& t, const U& u, const V& v)
{
   if(is_same_object(t, u))
      bitwise_and(t, v);
   else if(is_same_object(t, v))
      bitwise_and(t, u);
   else
   {
      t = u;
      bitwise_and(t, v);
   }
}

template <class T, class U, class V>
inline void bitwise_or(T& t, const U& u, const V& v)
{
   if(is_same_object(t, u))
      bitwise_or(t, v);
   else if(is_same_object(t, v))
      bitwise_or(t, u);
   else
   {
      t = u;
      bitwise_or(t, v);
   }
}

template <class T, class U, class V>
inline void bitwise_xor(T& t, const U& u, const V& v)
{
   if(is_same_object(t, u))
      bitwise_xor(t, v);
   else if(is_same_object(t, v))
      bitwise_xor(t, u);
   else
   {
      t = u;
      bitwise_xor(t, v);
   }
}

template <class T>
inline void increment(T& val)
{
   typedef typename mpl::front<typename T::unsigned_types>::type ui_type;
   add(val, static_cast<ui_type>(1u));
}

template <class T>
inline void decrement(T& val)
{
   typedef typename mpl::front<typename T::unsigned_types>::type ui_type;
   subtract(val, static_cast<ui_type>(1u));
}

template <class T, class V>
inline void left_shift(T& result, const T& arg, const V val)
{
   result = arg;
   left_shift(result, val);
}

template <class T, class V>
inline void right_shift(T& result, const T& arg, const V val)
{
   result = arg;
   right_shift(result, val);
}

template <class T>
inline bool is_zero(const T& val)
{
   typedef typename mpl::front<typename T::unsigned_types>::type ui_type;
   return val.compare(static_cast<ui_type>(0)) == 0;
}
template <class T>
inline int get_sign(const T& val)
{
   typedef typename mpl::front<typename T::unsigned_types>::type ui_type;
   return val.compare(static_cast<ui_type>(0));
}

template <class R, int b>
struct has_enough_bits
{
   template <class T>
   struct type : public mpl::and_<mpl::not_<is_same<R, T> >, mpl::bool_<std::numeric_limits<T>::digits >= b> >{};
};

template <class R>
struct terminal
{
   terminal(const R& v) : value(v){}
   terminal(){}
   terminal& operator = (R val) {  value = val;  }
   R value;
   operator R()const {  return value;  }
};

template<class R, class B>
struct calculate_next_larger_type
{
   typedef typename mpl::if_<
      is_signed<R>,
      typename B::signed_types,
      typename mpl::if_<
         is_unsigned<R>,
         typename B::unsigned_types,
         typename B::real_types
      >::type
   >::type list_type;
   typedef typename has_enough_bits<R, std::numeric_limits<R>::digits>::template type<mpl::_> pred_type;
   typedef typename mpl::find_if<
      list_type,
      pred_type
   >::type iter_type;
   typedef typename mpl::eval_if<
      is_same<typename mpl::end<list_type>::type, iter_type>,
      mpl::identity<terminal<R> >,
      mpl::deref<iter_type>
      >::type type;
};

template <class R, class B>
inline void convert_to(R* result, const B& backend)
{
   typedef typename calculate_next_larger_type<R, B>::type next_type;
   next_type n;
   convert_to(&n, backend);
   *result = static_cast<R>(n);
}

template <class R, class B>
inline void convert_to(terminal<R>* result, const B& backend)
{
   //
   // We ran out of types to try for the convertion, try
   // a lexical_cast and hope for the best:
   //
   result->value = boost::lexical_cast<R>(backend.str(0, false));
}

//
// Functions:
//
template <class T>
void eval_abs(T* result, const T& arg)
{
   typedef typename T::signed_types type_list;
   typedef typename mpl::front<type_list>::type front;
   *result = arg;
   if(arg.compare(front(0)) < 0)
      result->negate();
}
template <class T>
void eval_fabs(T* result, const T& arg)
{
   typedef typename T::signed_types type_list;
   typedef typename mpl::front<type_list>::type front;
   *result = arg;
   if(arg.compare(front(0)) < 0)
      result->negate();
}

template <class Backend>
inline int eval_fpclassify(const Backend& arg)
{
   return is_zero(arg) ? FP_ZERO : FP_NORMAL;
}
//
// These have to implemented by the backend, declared here so that our macro generated code compiles OK.
//
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_floor();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_ceil();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_trunc();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_sqrt();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_ldexp();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_frexp();
//
// TODO: implement default versions of these:
//
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_exp();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_log();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_sin();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_cos();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_tan();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_asin();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_acos();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_atan();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_sinh();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_cosh();
template <class T>
typename enable_if_c<sizeof(T) == 0>::type eval_tanh();
//
// These functions are implemented in separate files, but expanded inline here:
//
#include <boost/math/big_number/functions/pow.hpp>

}


template <class Backend>
class big_number;

namespace detail{

template<typename Expr>
struct big_number_exp;

}

//
// Default versions of floating point classification routines:
//
template <class Backend>
inline int fpclassify BOOST_PREVENT_MACRO_SUBSTITUTION(const big_number<Backend>& arg)
{
   using big_num_default_ops::eval_fpclassify;
   return eval_fpclassify(arg.backend());
}
template <class Exp>
inline int fpclassify BOOST_PREVENT_MACRO_SUBSTITUTION(const detail::big_number_exp<Exp>& arg)
{
   typedef typename detail::expression_type<Exp>::type value_type;
   return fpclassify(value_type(arg));
}
template <class Backend>
inline bool isfinite BOOST_PREVENT_MACRO_SUBSTITUTION(const big_number<Backend>& arg)
{
   int v = fpclassify(arg);
   return (v != FP_INFINITE) && (v != FP_NAN);
}
template <class Exp>
inline bool isfinite BOOST_PREVENT_MACRO_SUBSTITUTION(const detail::big_number_exp<Exp>& arg)
{
   typedef typename detail::expression_type<Exp>::type value_type;
   return isfinite(value_type(arg));
}
template <class Backend>
inline bool isnan BOOST_PREVENT_MACRO_SUBSTITUTION(const big_number<Backend>& arg)
{
   return fpclassify(arg) == FP_NAN;
}
template <class Exp>
inline bool isnan BOOST_PREVENT_MACRO_SUBSTITUTION(const detail::big_number_exp<Exp>& arg)
{
   typedef typename detail::expression_type<Exp>::type value_type;
   return isnan(value_type(arg));
}
template <class Backend>
inline bool isinf BOOST_PREVENT_MACRO_SUBSTITUTION(const big_number<Backend>& arg)
{
   return fpclassify(arg) == FP_INFINITE;
}
template <class Exp>
inline bool isinf BOOST_PREVENT_MACRO_SUBSTITUTION(const detail::big_number_exp<Exp>& arg)
{
   typedef typename detail::expression_type<Exp>::type value_type;
   return isinf(value_type(arg));
}
template <class Backend>
inline bool isnormal BOOST_PREVENT_MACRO_SUBSTITUTION(const big_number<Backend>& arg)
{
   return fpclassify(arg) == FP_NORMAL;
}
template <class Exp>
inline bool isnormal BOOST_PREVENT_MACRO_SUBSTITUTION(const detail::big_number_exp<Exp>& arg)
{
   typedef typename detail::expression_type<Exp>::type value_type;
   return isnormal(value_type(arg));
}

template <class Exp, class Policy>
inline int itrunc(const detail::big_number_exp<Exp>& v, const Policy& pol)
{
   typedef typename detail::expression_type<Exp>::type number_type;
   number_type r = trunc(v, pol);
   if(fabs(r) > (std::numeric_limits<int>::max)())
      return policies::raise_rounding_error("boost::math::itrunc<%1%>(%1%)", 0, v, 0, pol).template convert_to<int>();
   return r.template convert_to<int>();
}
template <class Backend, class Policy>
inline int itrunc(const big_number<Backend>& v, const Policy& pol)
{
   big_number<Backend> r = trunc(v, pol);
   if(fabs(r) > (std::numeric_limits<int>::max)())
      return policies::raise_rounding_error("boost::math::itrunc<%1%>(%1%)", 0, v, 0, pol).template convert_to<int>();
   return r.template convert_to<int>();
}
template <class T, class Policy>
inline long ltrunc(const detail::big_number_exp<T>& v, const Policy& pol)
{
   typedef typename detail::expression_type<T>::type number_type;
   number_type r = trunc(v, pol);
   if(fabs(r) > (std::numeric_limits<long>::max)())
      return policies::raise_rounding_error("boost::math::ltrunc<%1%>(%1%)", 0, v, 0L, pol).template convert_to<long>();
   return r.template convert_to<long>();
}
template <class T, class Policy>
inline long ltrunc(const big_number<T>& v, const Policy& pol)
{
   big_number<T> r = trunc(v, pol);
   if(fabs(r) > (std::numeric_limits<long>::max)())
      return policies::raise_rounding_error("boost::math::ltrunc<%1%>(%1%)", 0, v, 0L, pol).template convert_to<long>();
   return r.template convert_to<long>();
}
#ifndef BOOST_NO_LONG_LONG
template <class T, class Policy>
inline long long lltrunc(const detail::big_number_exp<T>& v, const Policy& pol)
{
   typedef typename detail::expression_type<T>::type number_type;
   number_type r = trunc(v, pol);
   if(fabs(r) > (std::numeric_limits<long long>::max)())
      return policies::raise_rounding_error("boost::math::lltrunc<%1%>(%1%)", 0, v, 0LL, pol).template convert_to<long long>();
   return r.template convert_to<long long>();
}
template <class T, class Policy>
inline long long lltrunc(const big_number<T>& v, const Policy& pol)
{
   big_number<T> r = trunc(v, pol);
   if(fabs(r) > (std::numeric_limits<long long>::max)())
      return policies::raise_rounding_error("boost::math::lltrunc<%1%>(%1%)", 0, v, 0LL, pol).template convert_to<long long>();
   return r.template convert_to<long long>();
}
#endif
template <class T, class Policy>
inline int iround(const detail::big_number_exp<T>& v, const Policy& pol)
{
   typedef typename detail::expression_type<T>::type number_type;
   number_type r = round(v, pol);
   if(fabs(r) > (std::numeric_limits<int>::max)())
      return policies::raise_rounding_error("boost::math::iround<%1%>(%1%)", 0, v, 0, pol).template convert_to<int>();
   return r.template convert_to<int>();
}
template <class T, class Policy>
inline int iround(const big_number<T>& v, const Policy& pol)
{
   big_number<T> r = round(v, pol);
   if(fabs(r) > (std::numeric_limits<int>::max)())
      return policies::raise_rounding_error("boost::math::iround<%1%>(%1%)", 0, v, 0, pol).template convert_to<int>();
   return r.template convert_to<int>();
}
template <class T, class Policy>
inline long lround(const detail::big_number_exp<T>& v, const Policy& pol)
{
   typedef typename detail::expression_type<T>::type number_type;
   number_type r = round(v, pol);
   if(fabs(r) > (std::numeric_limits<long>::max)())
      return policies::raise_rounding_error("boost::math::lround<%1%>(%1%)", 0, v, 0L, pol).template convert_to<long>();
   return r.template convert_to<long>();
}
template <class T, class Policy>
inline long lround(const big_number<T>& v, const Policy& pol)
{
   big_number<T> r = round(v, pol);
   if(fabs(r) > (std::numeric_limits<long>::max)())
      return policies::raise_rounding_error("boost::math::lround<%1%>(%1%)", 0, v, 0L, pol).template convert_to<long>();
   return r.template convert_to<long>();
}
#ifndef BOOST_NO_LONG_LONG
template <class T, class Policy>
inline long long llround(const detail::big_number_exp<T>& v, const Policy& pol)
{
   typedef typename detail::expression_type<T>::type number_type;
   number_type r = round(v, pol);
   if(fabs(r) > (std::numeric_limits<long long>::max)())
      return policies::raise_rounding_error("boost::math::iround<%1%>(%1%)", 0, v, 0LL, pol).template convert_to<long long>();
   return r.template convert_to<long long>();
}
template <class T, class Policy>
inline long long llround(const big_number<T>& v, const Policy& pol)
{
   big_number<T> r = round(v, pol);
   if(fabs(r) > (std::numeric_limits<long long>::max)())
      return policies::raise_rounding_error("boost::math::iround<%1%>(%1%)", 0, v, 0LL, pol).template convert_to<long long>();
   return r.template convert_to<long long>();
}
#endif
//
// Overload of Boost.Math functions that find the wrong overload when used with big_number:
//
namespace detail{
   template <class T> T sinc_pi_imp(T);
   template <class T> T sinhc_pi_imp(T);
}
template <class Backend>
inline big_number<Backend> sinc_pi(const big_number<Backend>& x)
{
   return detail::sinc_pi_imp(x);
}

template <class Backend, class Policy>
inline big_number<Backend> sinc_pi(const big_number<Backend>& x, const Policy&)
{
   return detail::sinc_pi_imp(x);
}

template <class Backend>
inline big_number<Backend> sinhc_pi(const big_number<Backend>& x)
{
   return detail::sinhc_pi_imp(x);
}

template <class Backend, class Policy>
inline big_number<Backend> sinhc_pi(const big_number<Backend>& x, const Policy&)
{
   return boost::math::sinhc_pi(x);
}

#define UNARY_OP_FUNCTOR(func)\
namespace detail{\
template <class Backend> \
struct BOOST_JOIN(func, _funct)\
{\
   void operator()(Backend& result, const Backend& arg)const\
   {\
      using big_num_default_ops::BOOST_JOIN(eval_,func);\
      BOOST_JOIN(eval_,func)(result, arg);\
   }\
};\
\
}\
\
template <class Exp> \
typename proto::result_of::make_expr<\
    proto::tag::function\
  , detail::BOOST_JOIN(func, _funct)<typename detail::backend_type<Exp>::type> \
  , detail::big_number_exp<Exp> \
>::type const \
func(const detail::big_number_exp<Exp>& arg)\
{\
    return proto::make_expr<proto::tag::function>(\
        detail::BOOST_JOIN(func, _funct)<typename detail::backend_type<Exp>::type>() \
      , arg   \
    );\
}\
template <class Backend> \
typename proto::result_of::make_expr<\
    proto::tag::function\
  , detail::BOOST_JOIN(func, _funct)<Backend> \
  , detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type> \
>::type const \
func(const big_number<Backend>& arg)\
{\
    return proto::make_expr<proto::tag::function>(\
        detail::BOOST_JOIN(func, _funct)<Backend>() \
      , static_cast<const detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type>&>(arg)   \
    );\
}

#define BINARY_OP_FUNCTOR(func)\
namespace detail{\
template <class Backend> \
struct BOOST_JOIN(func, _funct)\
{\
   void operator()(Backend& result, const Backend& arg, const Backend& a)const\
   {\
      using big_num_default_ops:: BOOST_JOIN(eval_,func);\
      BOOST_JOIN(eval_,func)(result, arg, a);\
   }\
   template <class Arithmetic> \
   void operator()(Backend& result, const Backend& arg, const Arithmetic& a)const\
   {\
      using big_num_default_ops:: BOOST_JOIN(eval_,func);\
      BOOST_JOIN(eval_,func)(result, arg, a);\
   }\
};\
\
}\
template <class Backend> \
typename proto::result_of::make_expr<\
    proto::tag::function\
  , detail::BOOST_JOIN(func, _funct)<Backend> \
  , detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type> \
  , detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type> \
>::type const \
func(const big_number<Backend>& arg, const big_number<Backend>& a)\
{\
    return proto::make_expr<proto::tag::function>(\
        detail::BOOST_JOIN(func, _funct)<Backend>() \
      , static_cast<const detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type>&>(arg),\
      static_cast<const detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type>&>(a)\
    );\
}\
template <class Backend, class Exp> \
typename proto::result_of::make_expr<\
    proto::tag::function\
  , detail::BOOST_JOIN(func, _funct)<Backend> \
  , detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type> \
  , detail::big_number_exp<Exp> \
>::type const \
func(const big_number<Backend>& arg, const detail::big_number_exp<Exp>& a)\
{\
    return proto::make_expr<proto::tag::function>(\
        detail::BOOST_JOIN(func, _funct)<Backend>() \
      , static_cast<const detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type>&>(arg),\
      a\
    );\
}\
template <class Exp, class Backend> \
typename proto::result_of::make_expr<\
    proto::tag::function\
  , detail::BOOST_JOIN(func, _funct)<Backend> \
  , detail::big_number_exp<Exp> \
  , detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type > \
>::type const \
func(const detail::big_number_exp<Exp>& arg, const big_number<Backend>& a)\
{\
    return proto::make_expr<proto::tag::function>(\
        detail::BOOST_JOIN(func, _funct)<Backend>() \
      , arg,\
      static_cast<const detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type>&>(a)\
    );\
}\
template <class Exp1, class Exp2> \
typename proto::result_of::make_expr<\
    proto::tag::function\
  , detail::BOOST_JOIN(func, _funct)<typename detail::backend_type<Exp1>::type> \
  , detail::big_number_exp<Exp1> \
  , detail::big_number_exp<Exp2> \
>::type const \
func(const detail::big_number_exp<Exp1>& arg, const detail::big_number_exp<Exp2>& a)\
{\
    return proto::make_expr<proto::tag::function>(\
        detail::BOOST_JOIN(func, _funct)<typename detail::backend_type<Exp1>::type>() \
      , arg,\
      a\
    );\
}\
template <class Backend, class Arithmetic> \
typename enable_if<\
   is_arithmetic<Arithmetic>,\
   typename proto::result_of::make_expr<\
    proto::tag::function\
  , detail::BOOST_JOIN(func, _funct)<Backend> \
  , detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type> \
  , typename proto::result_of::as_child<const Arithmetic&>::type\
>::type>::type const \
func(const big_number<Backend>& arg, const Arithmetic& a)\
{\
    return proto::make_expr<proto::tag::function>(\
        detail::BOOST_JOIN(func, _funct)<Backend>() \
      , static_cast<const detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type>&>(arg),\
      proto::as_child(a)\
    );\
}\
template <class Exp, class Arithmetic> \
typename enable_if<\
   is_arithmetic<Arithmetic>,\
   typename proto::result_of::make_expr<\
    proto::tag::function\
  , detail::BOOST_JOIN(func, _funct)<typename detail::backend_type<Exp>::type> \
  , detail::big_number_exp<Exp> \
  , typename proto::result_of::as_child<const Arithmetic&>::type\
>::type>::type const \
func(const detail::big_number_exp<Exp>& arg, const Arithmetic& a)\
{\
    return proto::make_expr<proto::tag::function>(\
        detail::BOOST_JOIN(func, _funct)<typename detail::backend_type<Exp>::type>() \
      , arg,\
      proto::as_child(a)\
    );\
}\


#define HETERO_BINARY_OP_FUNCTOR(func, Arg2)\
namespace detail{\
template <class Backend> \
struct BOOST_JOIN(func, _funct)\
{\
   void operator()(Backend& result, Backend const& arg, Arg2 a)const\
   {\
      using big_num_default_ops:: BOOST_JOIN(eval_,func);\
      BOOST_JOIN(eval_,func)(result, arg, a);\
   }\
};\
\
}\
\
template <class Exp> \
typename proto::result_of::make_expr<\
    proto::tag::function\
  , detail::BOOST_JOIN(func, _funct)<typename detail::backend_type<Exp>::type> \
  , detail::big_number_exp<Exp> \
  , typename proto::result_of::as_child<Arg2>::type \
>::type const \
func(const detail::big_number_exp<Exp>& arg, Arg2 const& a)\
{\
    return proto::make_expr<proto::tag::function>(\
        detail::BOOST_JOIN(func, _funct)<typename detail::backend_type<Exp>::type>() \
      , arg, proto::as_child(a)   \
    );\
}\
template <class Backend> \
typename proto::result_of::make_expr<\
    proto::tag::function\
  , detail::BOOST_JOIN(func, _funct)<Backend> \
  , detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type> \
  , typename proto::result_of::as_child<Arg2 const>::type \
>::type const \
func(const big_number<Backend>& arg, Arg2 const& a)\
{\
    return proto::make_expr<proto::tag::function>(\
        detail::BOOST_JOIN(func, _funct)<Backend>() \
      , static_cast<const detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type>&>(arg),\
      proto::as_child(a)\
    );\
}\

UNARY_OP_FUNCTOR(abs)
UNARY_OP_FUNCTOR(fabs)
UNARY_OP_FUNCTOR(sqrt)
UNARY_OP_FUNCTOR(floor)
UNARY_OP_FUNCTOR(ceil)
UNARY_OP_FUNCTOR(trunc)
UNARY_OP_FUNCTOR(exp)
UNARY_OP_FUNCTOR(log)
UNARY_OP_FUNCTOR(cos)
UNARY_OP_FUNCTOR(sin)
UNARY_OP_FUNCTOR(tan)
UNARY_OP_FUNCTOR(asin)
UNARY_OP_FUNCTOR(acos)
UNARY_OP_FUNCTOR(atan)
UNARY_OP_FUNCTOR(cosh)
UNARY_OP_FUNCTOR(sinh)
UNARY_OP_FUNCTOR(tanh)

HETERO_BINARY_OP_FUNCTOR(ldexp, int)
HETERO_BINARY_OP_FUNCTOR(frexp, int*)
BINARY_OP_FUNCTOR(pow)

#undef BINARY_OP_FUNCTOR
#undef UNARY_OP_FUNCTOR

}} // namespaces

#endif

