///////////////////////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_BIG_NUM_BASE_HPP
#define BOOST_MATH_BIG_NUM_BASE_HPP

#include <limits>

namespace boost{ namespace math{

template <class Backend>
class big_number;

namespace detail{

// Forward-declare an expression wrapper
template<typename Expr>
struct big_number_exp;
//
// Declare our grammars:
//
struct integer_terminal : public
proto::and_<
    proto::terminal< proto::_ >,
    proto::if_ < boost::is_integral< proto::_value >() >
  >
{};

struct big_number_grammar;

template<typename T>
struct is_big_number_ptr : mpl::false_ {};

template<typename Backend>
struct is_big_number_ptr<big_number<Backend>*> : mpl::true_ {};


struct big_number_grammar_cases
{
   // The primary template matches nothing:
   template<typename Tag>
   struct case_
      : proto::not_<proto::_>
   {};
};

template<>
struct big_number_grammar_cases::case_<proto::tag::terminal>
  : proto::and_<
      proto::terminal<proto::_ >,
      proto::or_<
         proto::if_ < is_big_number_ptr< proto::_value >() >,
         proto::if_ < is_arithmetic< proto::_value >() >,
         proto::if_ < is_same< proto::_value, std::string>() >,
         proto::if_ < is_convertible< proto::_value, const char*>() >
      >
  >
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::plus>
  : proto::plus< big_number_grammar, big_number_grammar >
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::multiplies>
  : proto::multiplies< big_number_grammar, big_number_grammar >
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::minus>
  : proto::minus< big_number_grammar, big_number_grammar >
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::divides>
  : proto::divides< big_number_grammar, big_number_grammar >
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::unary_plus>
  : proto::unary_plus< big_number_grammar>
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::negate>
  : proto::negate< big_number_grammar>
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::modulus>
  : proto::modulus< big_number_grammar, big_number_grammar >
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::bitwise_and>
  : proto::bitwise_and< big_number_grammar, big_number_grammar >
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::bitwise_or>
  : proto::bitwise_or< big_number_grammar, big_number_grammar >
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::bitwise_xor>
  : proto::bitwise_xor< big_number_grammar, big_number_grammar >
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::shift_left>
  : proto::shift_left< big_number_grammar, integer_terminal >
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::shift_right>
  : proto::shift_right< big_number_grammar, integer_terminal >
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::complement>
  : proto::complement<big_number_grammar>
{};

template<>
struct big_number_grammar_cases::case_<proto::tag::function>
  : proto::or_<
      proto::function< proto::_, big_number_grammar >,
      proto::function< proto::_, big_number_grammar, proto::_ >
  >
{};

struct big_number_grammar : proto::switch_<big_number_grammar_cases>{};

// Define a calculator domain. Expression within
// the calculator domain will be wrapped in the
// calculator<> expression wrapper.
struct big_number_domain
  : proto::domain< proto::generator<big_number_exp>, big_number_grammar>
{};

struct CalcDepth
  : proto::or_<
        proto::when< proto::terminal<proto::_>,
            mpl::int_<0>()
        >
      , proto::when< proto::unary_expr<proto::_, CalcDepth>,
            CalcDepth(proto::_child)
        >
      , proto::when< proto::binary_expr<proto::_, CalcDepth, CalcDepth>,
            mpl::plus<mpl::max<CalcDepth(proto::_left),
                     CalcDepth(proto::_right)>, mpl::int_<1> >()
        >
    >
{};

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

template <class Exp, class tag>
struct assign_and_eval_imp
{
   typedef tag type;
};

struct add_immediates{};
struct add_and_negate_immediates{};
struct subtract_immediates{};
struct subtract_and_negate_immediates{};
struct multiply_immediates{};
struct multiply_and_negate_immediates{};
struct divide_immediates{};
struct divide_and_negate_immediates{};
struct modulus_immediates{};
struct bitwise_and_immediates{};
struct bitwise_or_immediates{};
struct bitwise_xor_immediates{};
struct complement_immediates{};

struct immediate{};
struct negative_immediate{};

template <class Exp, class tag>
struct immediate_type
{
   typedef tag type;
};
template <class Exp>
struct immediate_type<Exp, proto::tag::terminal>
{
   typedef immediate type;
};
template <class Exp>
struct immediate_type<Exp, proto::tag::unary_plus>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::tag_of<left_type>::type left_tag;
   typedef typename mpl::if_<
      is_same<left_tag, proto::tag::terminal>,
      immediate,
      left_tag
      >::type type;
};
template <class Exp>
struct immediate_type<Exp, proto::tag::negate>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::tag_of<left_type>::type left_tag;
   typedef typename immediate_type<left_type, left_tag>::type tag;
   typedef typename mpl::if_<
      is_same<immediate, tag>,
      negative_immediate,
      tag
   >::type type;
};

template <class Exp>
struct assign_and_eval_imp<Exp, proto::tag::plus>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::result_of::right<Exp>::type right_type;
   typedef typename proto::tag_of<left_type>::type left_tag;
   typedef typename proto::tag_of<right_type>::type right_tag;
   typedef typename immediate_type<left_type, left_tag>::type left_imm;
   typedef typename immediate_type<right_type, right_tag>::type right_imm;
   typedef typename mpl::if_<
      mpl::and_<is_same<left_imm, immediate>, is_same<right_imm, immediate> >,
      add_immediates,
      typename mpl::if_<
         mpl::and_<is_same<left_imm, immediate>, is_same<right_imm, negative_immediate> >,
         subtract_immediates,
         typename mpl::if_<
            mpl::and_<is_same<left_imm, negative_immediate>, is_same<right_imm, immediate> >,
            subtract_and_negate_immediates,
            typename mpl::if_<
               mpl::and_<is_same<left_imm, negative_immediate>, is_same<right_imm, negative_immediate> >,
               add_and_negate_immediates,
               proto::tag::plus
            >::type
         >::type
      >::type
   >::type type;
};

template <class Exp>
struct assign_and_eval_imp<Exp, proto::tag::minus>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::result_of::right<Exp>::type right_type;
   typedef typename proto::tag_of<left_type>::type left_tag;
   typedef typename proto::tag_of<right_type>::type right_tag;
   typedef typename immediate_type<left_type, left_tag>::type left_imm;
   typedef typename immediate_type<right_type, right_tag>::type right_imm;
   typedef typename mpl::if_<
      mpl::and_<is_same<left_imm, immediate>, is_same<right_imm, immediate> >,
      subtract_immediates,
      typename mpl::if_<
         mpl::and_<is_same<left_imm, immediate>, is_same<right_imm, negative_immediate> >,
         add_immediates,
         typename mpl::if_<
            mpl::and_<is_same<left_imm, negative_immediate>, is_same<right_imm, immediate> >,
            add_and_negate_immediates,
            typename mpl::if_<
               mpl::and_<is_same<left_imm, negative_immediate>, is_same<right_imm, negative_immediate> >,
               subtract_and_negate_immediates,
               proto::tag::minus
            >::type
         >::type
      >::type
   >::type type;
};

template <class Exp>
struct assign_and_eval_imp<Exp, proto::tag::multiplies>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::result_of::right<Exp>::type right_type;
   typedef typename proto::tag_of<left_type>::type left_tag;
   typedef typename proto::tag_of<right_type>::type right_tag;
   typedef typename immediate_type<left_type, left_tag>::type left_imm;
   typedef typename immediate_type<right_type, right_tag>::type right_imm;
   typedef typename mpl::if_<
      mpl::and_<is_same<left_imm, immediate>, is_same<right_imm, immediate> >,
      multiply_immediates,
      typename mpl::if_<
         mpl::and_<is_same<left_imm, immediate>, is_same<right_imm, negative_immediate> >,
         multiply_and_negate_immediates,
         typename mpl::if_<
            mpl::and_<is_same<left_imm, negative_immediate>, is_same<right_imm, immediate> >,
            multiply_and_negate_immediates,
            typename mpl::if_<
               mpl::and_<is_same<left_imm, negative_immediate>, is_same<right_imm, negative_immediate> >,
               multiply_immediates,
               proto::tag::multiplies
            >::type
         >::type
      >::type
   >::type type;
};

template <class Exp>
struct assign_and_eval_imp<Exp, proto::tag::divides>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::result_of::right<Exp>::type right_type;
   typedef typename proto::tag_of<left_type>::type left_tag;
   typedef typename proto::tag_of<right_type>::type right_tag;
   typedef typename immediate_type<left_type, left_tag>::type left_imm;
   typedef typename immediate_type<right_type, right_tag>::type right_imm;
   typedef typename mpl::if_<
      mpl::and_<is_same<left_imm, immediate>, is_same<right_imm, immediate> >,
      divide_immediates,
      typename mpl::if_<
         mpl::and_<is_same<left_imm, immediate>, is_same<right_imm, negative_immediate> >,
         divide_and_negate_immediates,
         typename mpl::if_<
            mpl::and_<is_same<left_imm, negative_immediate>, is_same<right_imm, immediate> >,
            divide_and_negate_immediates,
            typename mpl::if_<
               mpl::and_<is_same<left_imm, negative_immediate>, is_same<right_imm, negative_immediate> >,
               divide_immediates,
               proto::tag::divides
            >::type
         >::type
      >::type
   >::type type;
};

template <class Exp>
struct assign_and_eval_imp<Exp, proto::tag::modulus>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::result_of::right<Exp>::type right_type;
   typedef typename proto::tag_of<left_type>::type left_tag;
   typedef typename proto::tag_of<right_type>::type right_tag;
   typedef typename immediate_type<left_type, left_tag>::type left_imm;
   typedef typename immediate_type<right_type, right_tag>::type right_imm;
   typedef typename mpl::if_<
      mpl::and_<is_same<left_imm, immediate>, is_same<right_imm, immediate> >,
      modulus_immediates,
      proto::tag::modulus
   >::type type;
};

template <class Exp>
struct assign_and_eval_imp<Exp, proto::tag::bitwise_and>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::result_of::right<Exp>::type right_type;
   typedef typename proto::tag_of<left_type>::type left_tag;
   typedef typename proto::tag_of<right_type>::type right_tag;
   typedef typename immediate_type<left_type, left_tag>::type left_imm;
   typedef typename immediate_type<right_type, right_tag>::type right_imm;
   typedef typename mpl::if_<
      mpl::and_<is_same<left_imm, immediate>, is_same<right_imm, immediate> >,
      bitwise_and_immediates,
      proto::tag::bitwise_and
   >::type type;
};

template <class Exp>
struct assign_and_eval_imp<Exp, proto::tag::bitwise_or>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::result_of::right<Exp>::type right_type;
   typedef typename proto::tag_of<left_type>::type left_tag;
   typedef typename proto::tag_of<right_type>::type right_tag;
   typedef typename immediate_type<left_type, left_tag>::type left_imm;
   typedef typename immediate_type<right_type, right_tag>::type right_imm;
   typedef typename mpl::if_<
      mpl::and_<is_same<left_imm, immediate>, is_same<right_imm, immediate> >,
      bitwise_or_immediates,
      proto::tag::bitwise_or
   >::type type;
};

template <class Exp>
struct assign_and_eval_imp<Exp, proto::tag::bitwise_xor>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::result_of::right<Exp>::type right_type;
   typedef typename proto::tag_of<left_type>::type left_tag;
   typedef typename proto::tag_of<right_type>::type right_tag;
   typedef typename immediate_type<left_type, left_tag>::type left_imm;
   typedef typename immediate_type<right_type, right_tag>::type right_imm;
   typedef typename mpl::if_<
      mpl::and_<is_same<left_imm, immediate>, is_same<right_imm, immediate> >,
      bitwise_xor_immediates,
      proto::tag::bitwise_xor
   >::type type;
};

template <class Exp>
struct assign_and_eval_imp<Exp, proto::tag::complement>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::tag_of<left_type>::type left_tag;
   typedef typename immediate_type<left_type, left_tag>::type left_imm;
   typedef typename mpl::if_<
      is_same<left_imm, immediate>,
      complement_immediates,
      proto::tag::complement
   >::type type;
};

template <class Exp>
struct assign_and_eval
{
   typedef typename proto::tag_of<Exp>::type tag_type;
   typedef typename assign_and_eval_imp<Exp, tag_type>::type type;
};

template <class Exp, class tag>
struct underlying_result_imp
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::tag_of<left_type>::type tag_type;
   typedef typename underlying_result_imp<left_type, tag_type>::type type;
};

template <class Exp>
struct underlying_result_imp<Exp, proto::tag::terminal>
{
   typedef typename proto::result_of::value<Exp const &>::type type;
};

template <class Exp>
struct underlying_result
{
   typedef typename proto::tag_of<Exp>::type tag_type;
   typedef typename underlying_result_imp<Exp, tag_type>::type type;
};

template <class Exp1, class Exp2>
struct combine_expression_type
{
   typedef void type;
};

template <class Backend>
struct combine_expression_type<boost::math::big_number<Backend>, boost::math::big_number<Backend> >
{
   typedef boost::math::big_number<Backend> type;
};

template <class Backend, class Exp>
struct combine_expression_type<boost::math::big_number<Backend>, Exp>
{
   typedef boost::math::big_number<Backend> type;
};

template <class Backend, class Exp>
struct combine_expression_type<Exp, boost::math::big_number<Backend> >
{
   typedef boost::math::big_number<Backend> type;
};

template <class T>
struct is_big_number : public mpl::false_{};
template <class T>
struct is_big_number<boost::math::big_number<T> > : public mpl::true_{};
template <class T>
struct is_big_number_exp : public mpl::false_{};
template <class T>
struct is_big_number_exp<boost::math::detail::big_number_exp<T> > : public mpl::true_{};


template <class Exp, int arity>
struct expression_type_imp;

template <class Exp>
struct expression_type_imp<Exp, 0>
{
   typedef typename remove_pointer<typename proto::result_of::value<Exp>::type>::type type;
};

template <class Exp>
struct expression_type_imp<Exp, 1>
{
   typedef typename proto::result_of::left<Exp>::type nested_type;
   typedef typename expression_type_imp<nested_type, proto::arity_of<nested_type>::value>::type type;
};

template <class Exp>
struct expression_type_imp<Exp, 2>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::result_of::right<Exp>::type right_type;
   typedef typename expression_type_imp<left_type, proto::arity_of<left_type>::value>::type left_result;
   typedef typename expression_type_imp<right_type, proto::arity_of<right_type>::value>::type right_result;
   typedef typename combine_expression_type<left_result, right_result>::type type;
};

template <class Exp>
struct expression_type_imp<Exp, 3>
{
   typedef typename proto::result_of::left<Exp>::type left_type;
   typedef typename proto::result_of::right<Exp>::type right_type;
   typedef typename proto::result_of::child_c<Exp, 2>::type end_type;
   typedef typename expression_type_imp<left_type, proto::arity_of<left_type>::value>::type left_result;
   typedef typename expression_type_imp<right_type, proto::arity_of<right_type>::value>::type right_result;
   typedef typename expression_type_imp<end_type, proto::arity_of<end_type>::value>::type end_result;
   typedef typename combine_expression_type<left_result, typename combine_expression_type<right_result, end_type>::type>::type type;
};

template <class Exp>
struct expression_type
{
   typedef typename expression_type_imp<Exp, proto::arity_of<Exp>::value>::type type;
};

template <class Exp>
struct backend_type
{
   typedef typename expression_type<Exp>::type num_type;
   typedef typename backend_type<num_type>::type type;
};

template <class Backend>
struct backend_type<boost::math::big_number<Backend> >
{
   typedef Backend type;
};

template<typename Expr>
struct big_number_exp
  : proto::extends<Expr, big_number_exp<Expr>, big_number_domain>
{
private:
    typedef          proto::extends<Expr, big_number_exp<Expr>, big_number_domain>      base_type;
    typedef          big_number_exp<Expr>                                               self_type;
    typedef typename remove_reference<typename expression_type<self_type>::type>::type  number_type;
    typedef          void (self_type::*unmentionable_type)();
    void unmentionable_proc(){}
    unmentionable_type boolean_context_from_terminal(const number_type* pval)const
    {
       return pval->is_zero() ? 0 : &self_type::unmentionable_proc;
    }
    unmentionable_type boolean_context_from_terminal(number_type* pval)const
    {
       return pval->is_zero() ? 0 : &self_type::unmentionable_proc;
    }
    template <class Terminal>
    unmentionable_type boolean_context_from_terminal(const Terminal& val)const
    {
       return val ? 0 : &self_type::unmentionable_proc;
    }
    unmentionable_type boolean_context(const proto::tag::terminal&)const
    {
       return boolean_context_from_terminal(proto::value(*this));
    }
    template <class Tag>
    unmentionable_type boolean_context(const Tag&)const
    {
       // we have to evaluate the expression template:
       number_type result(*this);
       return result.is_zero() ? 0 : &self_type::unmentionable_proc;
    }
public:
    big_number_exp(Expr const &expr = Expr())
      : base_type(expr)
    {}
    template <class Other>
    big_number_exp(const Other& o, typename enable_if<is_convertible<Other, base_type> >::type const* = 0)
       : base_type(o)
    {}

    operator unmentionable_type()const
    {
       return boolean_context(typename proto::tag_of<self_type>::type());
    }
};

} // namespace detail

//
// Traits class, lets us know whether a backend is an integer type, otherwise assumed to be a real number type:
//
template <class Num>
struct is_extended_integer : public mpl::false_ {};
template <class Backend>
struct is_extended_integer<big_number<Backend> > : public is_extended_integer<Backend>{};

}} // namespaces

namespace boost{ namespace math{ namespace tools{

template <class T>
struct promote_arg;

template <class Exp>
struct promote_arg<boost::math::detail::big_number_exp<Exp> >
{
  typedef typename boost::math::detail::expression_type<Exp>::type type;
};

}}}

#endif // BOOST_MATH_BIG_NUM_BASE_HPP


