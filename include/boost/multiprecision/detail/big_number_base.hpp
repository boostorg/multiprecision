///////////////////////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_BIG_NUM_BASE_HPP
#define BOOST_MATH_BIG_NUM_BASE_HPP

#include <limits>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_convertible.hpp>

namespace boost{ namespace math{

template <class Backend>
class mp_number;

namespace detail{

// Forward-declare an expression wrapper
template<class tag, class Arg1 = void, class Arg2 = void, class Arg3 = void>
struct mp_exp;

template <int b>
struct has_enough_bits
{
   template <class T>
   struct type : public mpl::bool_<std::numeric_limits<T>::digits >= b>{};
};

template <class Val, class Backend, class Tag>
struct canonical_imp
{
   typedef Val type;
};
template <class Val, class Backend>
struct canonical_imp<Val, Backend, mpl::int_<0> >
{
   typedef typename has_enough_bits<std::numeric_limits<Val>::digits>::template type<mpl::_> pred_type;
   typedef typename mpl::find_if<
      typename Backend::signed_types,
      pred_type
   >::type iter_type;
   typedef typename mpl::deref<iter_type>::type type;
};
template <class Val, class Backend>
struct canonical_imp<Val, Backend, mpl::int_<1> >
{
   typedef typename has_enough_bits<std::numeric_limits<Val>::digits>::template type<mpl::_> pred_type;
   typedef typename mpl::find_if<
      typename Backend::unsigned_types,
      pred_type
   >::type iter_type;
   typedef typename mpl::deref<iter_type>::type type;
};
template <class Val, class Backend>
struct canonical_imp<Val, Backend, mpl::int_<2> >
{
   typedef typename has_enough_bits<std::numeric_limits<Val>::digits>::template type<mpl::_> pred_type;
   typedef typename mpl::find_if<
      typename Backend::real_types,
      pred_type
   >::type iter_type;
   typedef typename mpl::deref<iter_type>::type type;
};
template <class Val, class Backend>
struct canonical_imp<Val, Backend, mpl::int_<3> >
{
   typedef const char* type;
};

template <class Val, class Backend>
struct canonical
{
   typedef typename mpl::if_<
      is_signed<Val>,
      mpl::int_<0>,
      typename mpl::if_<
         is_unsigned<Val>,
         mpl::int_<1>,
         typename mpl::if_<
            is_floating_point<Val>,
            mpl::int_<2>,
            typename mpl::if_<
               mpl::or_<
                  is_convertible<Val, const char*>,
                  is_same<Val, std::string>
               >,
               mpl::int_<3>,
               mpl::int_<4>
            >::type
         >::type
      >::type
   >::type tag_type;

   typedef typename canonical_imp<Val, Backend, tag_type>::type type;
};

struct terminal{};
struct negate{};
struct plus{};
struct minus{};
struct multiplies{};
struct divides{};
struct modulus{};
struct shift_left{};
struct shift_right{};
struct bitwise_and{};
struct bitwise_or{};
struct bitwise_xor{};
struct bitwise_complement{};
struct add_immediates{};
struct subtract_immediates{};
struct multiply_immediates{};
struct divide_immediates{};
struct modulus_immediates{};
struct bitwise_and_immediates{};
struct bitwise_or_immediates{};
struct bitwise_xor_immediates{};
struct complement_immediates{};
struct function{};

template <class T>
struct backend_type;

template <class T>
struct backend_type<mp_number<T> >
{
   typedef T type;
};

template <class tag, class A1, class A2, class A3>
struct backend_type<mp_exp<tag, A1, A2, A3> >
{
   typedef typename backend_type<typename mp_exp<tag, A1, A2, A3>::result_type>::type type;
};


template <class T>
struct is_big_number : public mpl::false_{};
template <class T>
struct is_big_number<boost::math::mp_number<T> > : public mpl::true_{};
template <class T>
struct is_big_number_exp : public mpl::false_{};
template <class Tag, class Arg1, class Arg2, class Arg3>
struct is_big_number_exp<boost::math::detail::mp_exp<Tag, Arg1, Arg2, Arg3> > : public mpl::true_{};

template <class T1, class T2>
struct combine_expression;

template <class T1, class T2>
struct combine_expression<mp_number<T1>, T2>
{
   typedef mp_number<T1> type;
};

template <class T1, class T2>
struct combine_expression<T1, mp_number<T2> >
{
   typedef mp_number<T2> type;
};

template <class T>
struct combine_expression<mp_number<T>, mp_number<T> >
{
   typedef mp_number<T> type;
};

template <class T>
struct arg_type
{
   typedef mp_exp<terminal, T> type;
};

template <class Tag, class Arg1, class Arg2, class Arg3>
struct arg_type<mp_exp<Tag, Arg1, Arg2, Arg3> >
{
   typedef mp_exp<Tag, Arg1, Arg2, Arg3> type;
};

template <class T>
struct unmentionable
{
   static void proc(){}
};

typedef void (*unmentionable_type)();

template <class T>
struct big_number_exp_storage
{
   typedef const T& type;
};

template <class T>
struct big_number_exp_storage<T*>
{
   typedef T* type;
};

template <class T>
struct big_number_exp_storage<const T*>
{
   typedef const T* type;
};

template <class tag, class A1, class A2, class A3>
struct big_number_exp_storage<mp_exp<tag, A1, A2, A3> >
{
   typedef mp_exp<tag, A1, A2, A3> type;
};

template<class tag, class Arg1>
struct mp_exp<tag, Arg1, void, void>
{
   typedef mpl::int_<1> arity;
   typedef typename arg_type<Arg1>::type left_type;
   typedef typename left_type::result_type result_type;
   typedef tag tag_type;

   mp_exp(const Arg1& a) : arg(a) {}

   left_type left()const { return arg; }

   const Arg1& left_ref()const{ return arg; }

   static const unsigned depth = left_type::depth + 1;

   operator unmentionable_type()const
   {
      result_type r(*this);
      return r ? &unmentionable<void>::proc : 0;
   }

private:
   typename big_number_exp_storage<Arg1>::type arg;
};

template<class Arg1>
struct mp_exp<terminal, Arg1, void, void>
{
   typedef mpl::int_<0> arity;
   typedef Arg1 result_type;
   typedef terminal tag_type;

   mp_exp(const Arg1& a) : arg(a) {}

   const Arg1& value()const { return arg; }

   static const unsigned depth = 0;

   operator unmentionable_type()const
   {
      return arg ? &unmentionable<void>::proc : 0;
   }

private:
   typename big_number_exp_storage<Arg1>::type arg;
};

template <class tag, class Arg1, class Arg2>
struct mp_exp<tag, Arg1, Arg2, void>
{
   typedef mpl::int_<2> arity;
   typedef typename arg_type<Arg1>::type left_type;
   typedef typename arg_type<Arg2>::type right_type;
   typedef typename left_type::result_type left_result_type;
   typedef typename right_type::result_type right_result_type;
   typedef typename combine_expression<left_result_type, right_result_type>::type result_type;
   typedef tag tag_type;

   mp_exp(const Arg1& a1, const Arg2& a2) : arg1(a1), arg2(a2) {}

   left_type left()const { return arg1; }
   right_type right()const { return arg2; }
   const Arg1& left_ref()const{ return arg1; }
   const Arg2& right_ref()const{ return arg2; }

   operator unmentionable_type()const
   {
      result_type r(*this);
      return r ? &unmentionable<void>::proc : 0;
   }

   static const unsigned left_depth = left_type::depth + 1;
   static const unsigned right_depth = right_type::depth + 1;
   static const unsigned depth = left_depth > right_depth ? left_depth : right_depth;
private:
   typename big_number_exp_storage<Arg1>::type arg1;
   typename big_number_exp_storage<Arg2>::type arg2;
};

template <class tag, class Arg1, class Arg2, class Arg3>
struct mp_exp
{
   typedef mpl::int_<3> arity;
   typedef typename arg_type<Arg1>::type left_type;
   typedef typename arg_type<Arg2>::type middle_type;
   typedef typename arg_type<Arg3>::type right_type;
   typedef typename left_type::result_type left_result_type;
   typedef typename middle_type::result_type middle_result_type;
   typedef typename right_type::result_type right_result_type;
   typedef typename combine_expression<
      left_result_type, 
      typename combine_expression<right_result_type, middle_result_type>::type
   >::type result_type;
   typedef tag tag_type;

   mp_exp(const Arg1& a1, const Arg2& a2, const Arg3& a3) : arg1(a1), arg2(a2), arg3(a3) {}

   left_type left()const { return arg1; }
   middle_type middle()const { return arg2; }
   right_type right()const { return arg3; }
   const Arg1& left_ref()const{ return arg1; }
   const Arg2& middle_ref()const{ return arg2; }
   const Arg3& right_ref()const{ return arg3; }

   operator unmentionable_type()const
   {
      result_type r(*this);
      return r ? &unmentionable<void>::proc : 0;
   }

   static const unsigned left_depth = left_type::depth + 1;
   static const unsigned middle_depth = middle_type::depth + 1;
   static const unsigned right_depth = right_type::depth + 1;
   static const unsigned depth = left_depth > right_depth ? (left_depth > middle_depth ? left_depth : middle_depth) : (right_depth > middle_depth ? right_depth : middle_depth);
private:
   typename big_number_exp_storage<Arg1>::type arg1;
   typename big_number_exp_storage<Arg2>::type arg2;
   typename big_number_exp_storage<Arg3>::type arg3;
};

} // namespace detail

//
// Non-member operators for mp_number:
//
// Unary operators first:
//
template <class B>
inline const mp_number<B>& operator + (const mp_number<B>& v) { return v; }
template <class tag, class Arg1, class Arg2, class Arg3>
inline const detail::mp_exp<tag, Arg1, Arg2, Arg3>& operator + (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& v) { return v; }
template <class B>
inline detail::mp_exp<detail::negate, mp_number<B> > operator - (const mp_number<B>& v) { return v; }
template <class tag, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::negate, detail::mp_exp<tag, Arg1, Arg2, Arg3> > operator - (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& v) { return v; }
template <class B>
inline detail::mp_exp<detail::complement_immediates, mp_number<B> > operator ~ (const mp_number<B>& v) { return v; }
template <class tag, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::bitwise_complement, detail::mp_exp<tag, Arg1, Arg2, Arg3> > operator ~ (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& v) { return v; }
//
// Then addition:
//
template <class B>
inline detail::mp_exp<detail::add_immediates, mp_number<B>, mp_number<B> >
   operator + (const mp_number<B>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::add_immediates, mp_number<B>, mp_number<B> >(a, b);
}
template <class B, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::add_immediates, mp_number<B>, V > >::type
   operator + (const mp_number<B>& a, const V& b)
{
   return detail::mp_exp<detail::add_immediates, mp_number<B>, V >(a, b);
}
template <class V, class B>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::add_immediates, V, mp_number<B> > >::type
   operator + (const V& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::add_immediates, V, mp_number<B> >(a, b);
}
template <class B, class tag, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::plus, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >
   operator + (const mp_number<B>& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::plus, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class B>
inline detail::mp_exp<detail::plus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >
   operator + (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::plus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class tag2, class Arg1b, class Arg2b, class Arg3b>
inline detail::mp_exp<detail::plus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >
   operator + (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b>& b)
{
   return detail::mp_exp<detail::plus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::plus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V > >::type
   operator + (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const V& b)
{
   return detail::mp_exp<detail::plus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V >(a, b);
}
template <class V, class tag, class Arg1, class Arg2, class Arg3>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::plus, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> > >::type
   operator + (const V& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::plus, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
//
// Repeat operator for negated arguments: propagate the negation to the top level to avoid temporaries:
//
template <class B, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::minus, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type >
   operator + (const mp_number<B>& a, const detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::minus, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type >(a, b.left_ref());
}
template <class Arg1, class Arg2, class Arg3, class B>
inline detail::mp_exp<detail::minus, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type >
   operator + (const detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::minus, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type >(b, a.left_ref());
}
template <class B>
inline detail::mp_exp<detail::subtract_immediates, mp_number<B>, mp_number<B> >
   operator + (const mp_number<B>& a, const detail::mp_exp<detail::negate, mp_number<B> >& b)
{
   return detail::mp_exp<detail::subtract_immediates, mp_number<B>, mp_number<B> >(a, b.left_ref());
}
template <class B>
inline detail::mp_exp<detail::subtract_immediates, mp_number<B>, mp_number<B> >
   operator + (const detail::mp_exp<detail::negate, mp_number<B> >& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::subtract_immediates, mp_number<B>, mp_number<B> >(b, a.left_ref());
}
template <class B, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::subtract_immediates, mp_number<B>, V > >::type
   operator + (const detail::mp_exp<detail::negate, mp_number<B> >& a, const V& b)
{
   return detail::mp_exp<detail::subtract_immediates, V, mp_number<B> >(b, a.left_ref());
}
template <class V, class B>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::subtract_immediates, V, mp_number<B> > >::type
   operator + (const V& a, const detail::mp_exp<detail::negate, mp_number<B> >& b)
{
   return detail::mp_exp<detail::subtract_immediates, mp_number<B>, mp_number<B> >(a, b.left_ref());
}
//
// Subtraction:
//
template <class B>
inline detail::mp_exp<detail::subtract_immediates, mp_number<B>, mp_number<B> >
   operator - (const mp_number<B>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::subtract_immediates, mp_number<B>, mp_number<B> >(a, b);
}
template <class B, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::subtract_immediates, mp_number<B>, V > >::type
   operator - (const mp_number<B>& a, const V& b)
{
   return detail::mp_exp<detail::subtract_immediates, mp_number<B>, V >(a, b);
}
template <class V, class B>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::subtract_immediates, V, mp_number<B> > >::type
   operator - (const V& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::subtract_immediates, V, mp_number<B> >(a, b);
}
template <class B, class tag, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::minus, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >
   operator - (const mp_number<B>& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::minus, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class B>
inline detail::mp_exp<detail::minus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >
   operator - (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::minus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class tag2, class Arg1b, class Arg2b, class Arg3b>
inline detail::mp_exp<detail::minus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >
   operator - (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b>& b)
{
   return detail::mp_exp<detail::minus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::minus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V > >::type
   operator - (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const V& b)
{
   return detail::mp_exp<detail::minus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V >(a, b);
}
template <class V, class tag, class Arg1, class Arg2, class Arg3>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::minus, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> > >::type
   operator - (const V& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::minus, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
//
// Repeat operator for negated arguments: propagate the negation to the top level to avoid temporaries:
//
template <class B, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::plus, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type >
   operator - (const mp_number<B>& a, const detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::plus, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type >(a, b.left_ref());
}
template <class Arg1, class Arg2, class Arg3, class B>
inline detail::mp_exp<detail::negate, detail::mp_exp<detail::plus, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type > >
   operator - (const detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::plus, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type >(b, a.left_ref());
}
template <class B>
inline detail::mp_exp<detail::add_immediates, mp_number<B>, mp_number<B> >
   operator - (const mp_number<B>& a, const detail::mp_exp<detail::negate, mp_number<B> >& b)
{
   return detail::mp_exp<detail::add_immediates, mp_number<B>, mp_number<B> >(a, b.left_ref());
}
template <class B>
inline detail::mp_exp<detail::negate, detail::mp_exp<detail::add_immediates, mp_number<B>, mp_number<B> > >
   operator - (const detail::mp_exp<detail::negate, mp_number<B> >& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::add_immediates, mp_number<B>, mp_number<B> >(b, a.left_ref());
}
template <class B, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::negate, detail::mp_exp<detail::add_immediates, mp_number<B>, V > > >::type
   operator - (const detail::mp_exp<detail::negate, mp_number<B> >& a, const V& b)
{
   return detail::mp_exp<detail::add_immediates, V, mp_number<B> >(b, a.left_ref());
}
template <class V, class B>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::add_immediates, V, mp_number<B> > >::type
   operator - (const V& a, const detail::mp_exp<detail::negate, mp_number<B> >& b)
{
   return detail::mp_exp<detail::add_immediates, V, mp_number<B> >(a, b.left_ref());
}
//
// Multiplication:
//
template <class B>
inline detail::mp_exp<detail::multiply_immediates, mp_number<B>, mp_number<B> >
   operator * (const mp_number<B>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::multiply_immediates, mp_number<B>, mp_number<B> >(a, b);
}
template <class B, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::multiply_immediates, mp_number<B>, V > >::type
   operator * (const mp_number<B>& a, const V& b)
{
   return detail::mp_exp<detail::multiply_immediates, mp_number<B>, V >(a, b);
}
template <class V, class B>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::multiply_immediates, V, mp_number<B> > >::type
   operator * (const V& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::multiply_immediates, V, mp_number<B> >(a, b);
}
template <class B, class tag, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::multiplies, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >
   operator * (const mp_number<B>& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::multiplies, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class B>
inline detail::mp_exp<detail::multiplies, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >
   operator * (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::multiplies, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class tag2, class Arg1b, class Arg2b, class Arg3b>
inline detail::mp_exp<detail::multiplies, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >
   operator * (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b>& b)
{
   return detail::mp_exp<detail::multiplies, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::multiplies, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V > >::type
   operator * (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const V& b)
{
   return detail::mp_exp<detail::multiplies, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V >(a, b);
}
template <class V, class tag, class Arg1, class Arg2, class Arg3>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::multiplies, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> > >::type
   operator * (const V& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::multiplies, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
//
// Repeat operator for negated arguments: propagate the negation to the top level to avoid temporaries:
//
template <class B, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::negate, detail::mp_exp<detail::multiplies, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type > >
   operator * (const mp_number<B>& a, const detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::multiplies, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type > (a, b.left_ref());
}
template <class Arg1, class Arg2, class Arg3, class B>
inline detail::mp_exp<detail::negate, detail::mp_exp<detail::multiplies, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type > >
   operator * (const detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::multiplies, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type >(b, a.left_ref());
}
template <class B>
inline detail::mp_exp<detail::negate, detail::mp_exp<detail::multiply_immediates, mp_number<B>, mp_number<B> > >
   operator * (const mp_number<B>& a, const detail::mp_exp<detail::negate, mp_number<B> >& b)
{
   return detail::mp_exp<detail::multiply_immediates, mp_number<B>, mp_number<B> >(a, b.left_ref());
}
template <class B>
inline detail::mp_exp<detail::negate, detail::mp_exp<detail::multiply_immediates, mp_number<B>, mp_number<B> > >
   operator * (const detail::mp_exp<detail::negate, mp_number<B> >& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::multiply_immediates, mp_number<B>, mp_number<B> >(b, a.left_ref());
}
template <class B, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::negate, detail::mp_exp<detail::multiply_immediates, mp_number<B>, V > > >::type
   operator * (const detail::mp_exp<detail::negate, mp_number<B> >& a, const V& b)
{
   return detail::mp_exp<detail::multiply_immediates, mp_number<B>, V >(a.left_ref(), b);
}
template <class V, class B>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::negate, detail::mp_exp<detail::multiply_immediates, mp_number<B>, V > > >::type
   operator * (const V& a, const detail::mp_exp<detail::negate, mp_number<B> >& b)
{
   return detail::mp_exp<detail::multiply_immediates, mp_number<B>, V >(b.left_ref(), a);
}
//
// Division:
//
template <class B>
inline detail::mp_exp<detail::divide_immediates, mp_number<B>, mp_number<B> >
   operator / (const mp_number<B>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::divide_immediates, mp_number<B>, mp_number<B> >(a, b);
}
template <class B, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::divide_immediates, mp_number<B>, V > >::type
   operator / (const mp_number<B>& a, const V& b)
{
   return detail::mp_exp<detail::divide_immediates, mp_number<B>, V >(a, b);
}
template <class V, class B>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::divide_immediates, V, mp_number<B> > >::type
   operator / (const V& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::divide_immediates, V, mp_number<B> >(a, b);
}
template <class B, class tag, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::divides, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >
   operator / (const mp_number<B>& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::divides, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class B>
inline detail::mp_exp<detail::divides, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >
   operator / (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::divides, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class tag2, class Arg1b, class Arg2b, class Arg3b>
inline detail::mp_exp<detail::divides, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >
   operator / (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b>& b)
{
   return detail::mp_exp<detail::divides, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::divides, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V > >::type
   operator / (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const V& b)
{
   return detail::mp_exp<detail::divides, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V >(a, b);
}
template <class V, class tag, class Arg1, class Arg2, class Arg3>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::divides, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> > >::type
   operator / (const V& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::divides, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
//
// Repeat operator for negated arguments: propagate the negation to the top level to avoid temporaries:
//
template <class B, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::negate, detail::mp_exp<detail::divides, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type > >
   operator / (const mp_number<B>& a, const detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::divides, mp_number<B>, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type >(a, b.left_ref());
}
template <class Arg1, class Arg2, class Arg3, class B>
inline detail::mp_exp<detail::negate, detail::mp_exp<detail::divides, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type, mp_number<B> > >
   operator / (const detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::divides, typename detail::mp_exp<detail::negate, Arg1, Arg2, Arg3>::left_type, mp_number<B> >(a.left_ref(), b);
}
template <class B>
inline detail::mp_exp<detail::negate, detail::mp_exp<detail::divide_immediates, mp_number<B>, mp_number<B> > >
   operator / (const mp_number<B>& a, const detail::mp_exp<detail::negate, mp_number<B> >& b)
{
   return detail::mp_exp<detail::divide_immediates, mp_number<B>, mp_number<B> >(a, b.left_ref());
}
template <class B>
inline detail::mp_exp<detail::negate, detail::mp_exp<detail::divide_immediates, mp_number<B>, mp_number<B> > >
   operator / (const detail::mp_exp<detail::negate, mp_number<B> >& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::divide_immediates, mp_number<B>, mp_number<B> >(a.left_ref(), b);
}
template <class B, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::negate, detail::mp_exp<detail::divide_immediates, mp_number<B>, V > > >::type
   operator / (const detail::mp_exp<detail::negate, mp_number<B> >& a, const V& b)
{
   return detail::mp_exp<detail::divide_immediates, mp_number<B>, V>(a.left_ref(), b);
}
template <class V, class B>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::negate, detail::mp_exp<detail::divide_immediates, V, mp_number<B> > > >::type
   operator / (const V& a, const detail::mp_exp<detail::negate, mp_number<B> >& b)
{
   return detail::mp_exp<detail::divide_immediates, V, mp_number<B> >(a, b.left_ref());
}
//
// Modulus:
//
template <class B>
inline detail::mp_exp<detail::modulus_immediates, mp_number<B>, mp_number<B> >
   operator % (const mp_number<B>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::modulus_immediates, mp_number<B>, mp_number<B> >(a, b);
}
template <class B, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::modulus_immediates, mp_number<B>, V > >::type
   operator % (const mp_number<B>& a, const V& b)
{
   return detail::mp_exp<detail::modulus_immediates, mp_number<B>, V >(a, b);
}
template <class V, class B>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::modulus_immediates, V, mp_number<B> > >::type
   operator % (const V& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::modulus_immediates, V, mp_number<B> >(a, b);
}
template <class B, class tag, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::modulus, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >
   operator % (const mp_number<B>& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::modulus, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class B>
inline detail::mp_exp<detail::modulus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >
   operator % (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::modulus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class tag2, class Arg1b, class Arg2b, class Arg3b>
inline detail::mp_exp<detail::modulus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >
   operator % (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b>& b)
{
   return detail::mp_exp<detail::modulus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::modulus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V > >::type
   operator % (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const V& b)
{
   return detail::mp_exp<detail::modulus, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V >(a, b);
}
template <class V, class tag, class Arg1, class Arg2, class Arg3>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::modulus, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> > >::type
   operator % (const V& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::modulus, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
//
// Left shift:
//
template <class B, class I>
inline typename enable_if<is_integral<I>, detail::mp_exp<detail::shift_left, mp_number<B>, I > >::type
   operator << (const mp_number<B>& a, const I& b)
{
   return detail::mp_exp<detail::shift_left, mp_number<B>, I>(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class I>
inline typename enable_if<is_integral<I>, detail::mp_exp<detail::shift_left, detail::mp_exp<tag, Arg1, Arg2, Arg3>, I> >::type
   operator << (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const I& b)
{
   return detail::mp_exp<detail::shift_left, detail::mp_exp<tag, Arg1, Arg2, Arg3>, I>(a, b);
}
//
// Right shift:
//
template <class B, class I>
inline typename enable_if<is_integral<I>, detail::mp_exp<detail::shift_right, mp_number<B>, I > >::type
   operator >> (const mp_number<B>& a, const I& b)
{
   return detail::mp_exp<detail::shift_right, mp_number<B>, I>(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class I>
inline typename enable_if<is_integral<I>, detail::mp_exp<detail::shift_right, detail::mp_exp<tag, Arg1, Arg2, Arg3>, I> >::type
   operator >> (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const I& b)
{
   return detail::mp_exp<detail::shift_right, detail::mp_exp<tag, Arg1, Arg2, Arg3>, I>(a, b);
}
//
// Bitwise AND:
//
template <class B>
inline detail::mp_exp<detail::bitwise_and_immediates, mp_number<B>, mp_number<B> >
   operator & (const mp_number<B>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::bitwise_and_immediates, mp_number<B>, mp_number<B> >(a, b);
}
template <class B, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::bitwise_and_immediates, mp_number<B>, V > >::type
   operator & (const mp_number<B>& a, const V& b)
{
   return detail::mp_exp<detail::bitwise_and_immediates, mp_number<B>, V >(a, b);
}
template <class V, class B>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::bitwise_and_immediates, V, mp_number<B> > >::type
   operator & (const V& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::bitwise_and_immediates, V, mp_number<B> >(a, b);
}
template <class B, class tag, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::bitwise_and, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >
   operator & (const mp_number<B>& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::bitwise_and, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class B>
inline detail::mp_exp<detail::bitwise_and, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >
   operator & (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::bitwise_and, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class tag2, class Arg1b, class Arg2b, class Arg3b>
inline detail::mp_exp<detail::bitwise_and, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >
   operator & (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b>& b)
{
   return detail::mp_exp<detail::bitwise_and, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::bitwise_and, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V > >::type
   operator & (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const V& b)
{
   return detail::mp_exp<detail::bitwise_and, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V >(a, b);
}
template <class V, class tag, class Arg1, class Arg2, class Arg3>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::bitwise_and, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> > >::type
   operator & (const V& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::bitwise_and, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
//
// Bitwise OR:
//
template <class B>
inline detail::mp_exp<detail::bitwise_or_immediates, mp_number<B>, mp_number<B> >
   operator| (const mp_number<B>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::bitwise_or_immediates, mp_number<B>, mp_number<B> >(a, b);
}
template <class B, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::bitwise_or_immediates, mp_number<B>, V > >::type
   operator| (const mp_number<B>& a, const V& b)
{
   return detail::mp_exp<detail::bitwise_or_immediates, mp_number<B>, V >(a, b);
}
template <class V, class B>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::bitwise_or_immediates, V, mp_number<B> > >::type
   operator| (const V& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::bitwise_or_immediates, V, mp_number<B> >(a, b);
}
template <class B, class tag, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::bitwise_or, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >
   operator| (const mp_number<B>& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::bitwise_or, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class B>
inline detail::mp_exp<detail::bitwise_or, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >
   operator| (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::bitwise_or, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class tag2, class Arg1b, class Arg2b, class Arg3b>
inline detail::mp_exp<detail::bitwise_or, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >
   operator| (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b>& b)
{
   return detail::mp_exp<detail::bitwise_or, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::bitwise_or, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V > >::type
   operator| (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const V& b)
{
   return detail::mp_exp<detail::bitwise_or, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V >(a, b);
}
template <class V, class tag, class Arg1, class Arg2, class Arg3>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::bitwise_or, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> > >::type
   operator| (const V& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::bitwise_or, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
//
// Bitwise XOR:
//
template <class B>
inline detail::mp_exp<detail::bitwise_xor_immediates, mp_number<B>, mp_number<B> >
   operator^ (const mp_number<B>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::bitwise_xor_immediates, mp_number<B>, mp_number<B> >(a, b);
}
template <class B, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::bitwise_xor_immediates, mp_number<B>, V > >::type
   operator^ (const mp_number<B>& a, const V& b)
{
   return detail::mp_exp<detail::bitwise_xor_immediates, mp_number<B>, V >(a, b);
}
template <class V, class B>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::bitwise_xor_immediates, V, mp_number<B> > >::type
   operator^ (const V& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::bitwise_xor_immediates, V, mp_number<B> >(a, b);
}
template <class B, class tag, class Arg1, class Arg2, class Arg3>
inline detail::mp_exp<detail::bitwise_xor, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >
   operator^ (const mp_number<B>& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::bitwise_xor, mp_number<B>, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class B>
inline detail::mp_exp<detail::bitwise_xor, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >
   operator^ (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const mp_number<B>& b)
{
   return detail::mp_exp<detail::bitwise_xor, detail::mp_exp<tag, Arg1, Arg2, Arg3>, mp_number<B> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class tag2, class Arg1b, class Arg2b, class Arg3b>
inline detail::mp_exp<detail::bitwise_xor, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >
   operator^ (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b>& b)
{
   return detail::mp_exp<detail::bitwise_xor, detail::mp_exp<tag, Arg1, Arg2, Arg3>, detail::mp_exp<tag2, Arg1b, Arg2b, Arg3b> >(a, b);
}
template <class tag, class Arg1, class Arg2, class Arg3, class V>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::bitwise_xor, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V > >::type
   operator^ (const detail::mp_exp<tag, Arg1, Arg2, Arg3>& a, const V& b)
{
   return detail::mp_exp<detail::bitwise_xor, detail::mp_exp<tag, Arg1, Arg2, Arg3>, V >(a, b);
}
template <class V, class tag, class Arg1, class Arg2, class Arg3>
inline typename enable_if<is_arithmetic<V>, detail::mp_exp<detail::bitwise_xor, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> > >::type
   operator^ (const V& a, const detail::mp_exp<tag, Arg1, Arg2, Arg3>& b)
{
   return detail::mp_exp<detail::bitwise_xor, V, detail::mp_exp<tag, Arg1, Arg2, Arg3> >(a, b);
}

//
// Traits class, lets us know what kind of number we have, defaults to a floating point type:
//
enum number_category_type
{
   number_kind_integer = 0,
   number_kind_floating_point = 1,
   number_kind_rational = 2,
   number_kind_fixed_point = 3
};

template <class Num>
struct number_category : public mpl::int_<number_kind_floating_point> {};
template <class Backend>
struct number_category<mp_number<Backend> > : public number_category<Backend>{};
template <class tag, class A1, class A2, class A3>
struct number_category<detail::mp_exp<tag, A1, A2, A3> > : public number_category<typename detail::mp_exp<tag, A1, A2, A3>::result_type>{};

}} // namespaces

namespace boost{ namespace math{ namespace tools{

template <class T>
struct promote_arg;

template <class tag, class A1, class A2, class A3>
struct promote_arg<boost::math::detail::mp_exp<tag, A1, A2, A3> >
{
   typedef typename boost::math::detail::mp_exp<tag, A1, A2, A3>::result_type type;
};

}}}

#endif // BOOST_MATH_BIG_NUM_BASE_HPP


