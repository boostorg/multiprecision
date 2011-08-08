///////////////////////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_EXTENDED_REAL_HPP
#define BOOST_MATH_EXTENDED_REAL_HPP

#include <boost/proto/proto.hpp>
#include <boost/cstdint.hpp>
#include <boost/mpl/max.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/or.hpp>
#include <boost/assert.hpp>
#include <boost/type_traits/remove_pointer.hpp>

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
struct big_number_grammar
  : proto::or_<
        proto::terminal< proto::_ >
      , proto::plus< big_number_grammar, big_number_grammar >
      , proto::multiplies< big_number_grammar, big_number_grammar >
      , proto::minus< big_number_grammar, big_number_grammar >
      , proto::divides< big_number_grammar, big_number_grammar >
      , proto::unary_plus< big_number_grammar >
      , proto::negate< big_number_grammar >
      , proto::modulus<big_number_grammar, big_number_grammar>
    >
{};

// Define a calculator domain. Expression within
// the calculator domain will be wrapped in the
// calculator<> expression wrapper.
struct big_number_domain
  : proto::domain< proto::generator<big_number_exp>, big_number_grammar>
{};

template<typename Expr>
struct big_number_exp
  : proto::extends<Expr, big_number_exp<Expr>, big_number_domain>
{
    typedef
        proto::extends<Expr, big_number_exp<Expr>, big_number_domain> base_type;

    big_number_exp(Expr const &expr = Expr())
      : base_type(expr)
    {}
    template <class Other>
    big_number_exp(const Other& o)
       : base_type(o)
    {}
};

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

} // namespace detail

//
// Traits class, lets us know whether a backend is an integer type, otherwise assumed to be a real number type:
//
template <class Num>
struct is_extended_integer : public mpl::false_ {};
template <class Backend>
struct is_extended_integer<big_number<Backend> > : public is_extended_integer<Backend>{};

template <class Backend>
class big_number : public detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type >
{
   typedef detail::big_number_exp<typename proto::terminal<big_number<Backend>*>::type > base_type;
   typedef big_number<Backend> self_type;
public:
   big_number()
   {
      proto::value(*this) = this;
      BOOST_ASSERT(proto::value(*this) == this);
   }
   big_number(const big_number& e) : m_backend(e.m_backend)
   {
      proto::value(*this) = this;
      BOOST_ASSERT(proto::value(*this) == this);
   }
   template <class V>
   big_number(V v, typename enable_if<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> > >::type* dummy1 = 0)
   {
      proto::value(*this) = this;
      BOOST_ASSERT(proto::value(*this) == this);
      m_backend = canonical_value(v);
   }
   /*
   big_number(unsigned digits10, typename enable_if_c<false == Backend::is_fixed>::type* dummy = 0)
      : base_type(digits10)
   {
      proto::value(*this) = this;
      BOOST_ASSERT(proto::value(*this) == this);
   }*/
   big_number(const big_number& e, unsigned digits10) : m_backend(e.m_backend, digits10)
   {
      proto::value(*this) = this;
      BOOST_ASSERT(proto::value(*this) == this);
   }
   template <class V>
   big_number(V v, unsigned digits10, typename enable_if<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> > >::type* dummy1 = 0)
      : base_type(digits10)
   {
      proto::value(*this) = this;
      BOOST_ASSERT(proto::value(*this) == this);
      m_backend = canonical_value(v);
   }

   template <class Exp>
   big_number& operator=(const detail::big_number_exp<Exp>& e)
   {
      do_assign(e, typename proto::tag_of<Exp>::type());
      return *this;
   }

   big_number& operator=(const big_number& e)
   {
      m_backend = e.m_backend;
      return *this;
   }

   template <class V>
   typename enable_if<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> >, big_number<Backend>& >::type 
      operator=(const V& v)
   {
      m_backend = canonical_value(v);
      return *this;
   }

   template <class Exp>
   big_number(const detail::big_number_exp<Exp>& e)
   {
      proto::value(*this) = this;
      BOOST_ASSERT(proto::value(*this) == this);
      do_assign(e, typename proto::tag_of<Exp>::type());
   }

   template <class Exp>
   big_number& operator+=(const detail::big_number_exp<Exp>& e)
   {
      // Create a copy if e contains this, but not if we're just doing a
      //    x *= x
      if(contains_self(e) && !is_self(e))
      {
         self_type temp(e);
         do_add(temp, typename proto::tag_of<self_type>::type());
      }
      else
      {
         do_add(e, typename proto::tag_of<Exp>::type());
      }
      return *this;
   }

   template <class V>
   typename enable_if<boost::is_arithmetic<V>, big_number<Backend>& >::type 
      operator+=(const V& v)
   {
      do_add_value(canonical_value(v), mpl::false_());
      return *this;
   }

   template <class Exp>
   big_number& operator-=(const detail::big_number_exp<Exp>& e)
   {
      // Create a copy if e contains this, but not if we're just doing a
      if(contains_self(e))
      {
         self_type temp(e);
         do_subtract(temp, typename proto::tag_of<self_type>::type());
      }
      else
      {
         do_subtract(e, typename proto::tag_of<Exp>::type());
      }
      return *this;
   }

   template <class V>
   typename enable_if<boost::is_arithmetic<V>, big_number<Backend>& >::type 
      operator-=(const V& v)
   {
      do_subtract_value(canonical_value(v), mpl::false_());
      return *this;
   }

   template <class Exp>
   big_number& operator*=(const detail::big_number_exp<Exp>& e)
   {
      // Create a temporary if the RHS references *this, but not
      // if we're just doing an   x += x;
      if(contains_self(e) && !is_self(e))
      {
         self_type temp(e);
         do_multiplies(temp, typename proto::tag_of<self_type>::type());
      }
      else
      {
         do_multiplies(e, typename proto::tag_of<Exp>::type());
      }
      return *this;
   }

   template <class V>
   typename enable_if<boost::is_arithmetic<V>, big_number<Backend>& >::type 
      operator*=(const V& v)
   {
      do_multiplies_value(canonical_value(v), mpl::false_());
      return *this;
   }

   template <class Exp>
   big_number& operator%=(const detail::big_number_exp<Exp>& e)
   {
      BOOST_STATIC_ASSERT_MSG(is_extended_integer<Backend>::value, "The modulus operation is only valid for integer types");
      // Create a temporary if the RHS references *this:
      if(contains_self(e))
      {
         self_type temp(e);
         do_modulus(temp, typename proto::tag_of<self_type>::type());
      }
      else
      {
         do_modulus(e, typename proto::tag_of<Exp>::type());
      }
      return *this;
   }

   template <class V>
   typename enable_if<boost::is_arithmetic<V>, big_number<Backend>& >::type 
      operator%=(const V& v)
   {
      BOOST_STATIC_ASSERT_MSG(is_extended_integer<Backend>::value, "The modulus operation is only valid for integer types");
      do_modulus_value(canonical_value(v), mpl::false_());
      return *this;
   }

   template <class Exp>
   big_number& operator/=(const detail::big_number_exp<Exp>& e)
   {
      // Create a temporary if the RHS references *this:
      if(contains_self(e))
      {
         self_type temp(e);
         do_divide(temp, typename proto::tag_of<self_type>::type());
      }
      else
      {
         do_divide(e, typename proto::tag_of<Exp>::type());
      }
      return *this;
   }

   template <class V>
   typename enable_if<boost::is_arithmetic<V>, big_number<Backend>& >::type 
      operator/=(const V& v)
   {
      do_divide_value(canonical_value(v), mpl::false_());
      return *this;
   }

   //
   // String conversion functions:
   //
   std::string str()const
   {
      return m_backend.str();
   }
   //
   // Default precision:
   //
   static unsigned default_precision()
   {
      return Backend::default_precision();
   }
   static void default_precision(unsigned digits10)
   {
      Backend::default_precision(digits10);
   }
   unsigned precision()const
   {
      return m_backend.precision();
   }
   void precision(unsigned digits10)
   {
      m_backend.precision(digits10);
   }
   //
   // Comparison:
   //
   int compare(const big_number<Backend>& o)const
   {
      return m_backend.compare(o.m_backend);
   }
   template <class V>
   typename enable_if<is_arithmetic<V>, int>::type compare(const V& o)const
   {
      return m_backend.compare(canonical_value(o));
   }
   Backend& backend()
   {
      return m_backend;
   }
   const Backend& backend()const
   {
      return m_backend;
   }
private:
   template <class Exp>
   void do_assign(const Exp& e, const proto::tag::unary_plus&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      do_assign(proto::left(e), typename proto::tag_of<left_type>::type());
   }

   template <class Exp>
   void do_assign(const Exp& e, const proto::tag::negate&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      do_assign(proto::left(e), typename proto::tag_of<left_type>::type());
      m_backend.negate();
   }

   template <class Exp>
   void do_assign(const Exp& e, const proto::tag::plus&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;

      static int const left_depth = boost::result_of<detail::CalcDepth(left_type)>::type::value;
      static int const right_depth = boost::result_of<detail::CalcDepth(right_type)>::type::value;

      bool bl = contains_self(proto::left(e));
      bool br = contains_self(proto::right(e));

      if(bl && is_self(proto::left(e)))
      {
         // Ignore the left node, it's *this, just add the right:
         do_add(proto::right(e), typename proto::tag_of<right_type>::type());
      }
      else if(br && is_self(proto::right(e)))
      {
         // Ignore the right node, it's *this, just add the left:
         do_add(proto::left(e), typename proto::tag_of<left_type>::type());
      }
      else if(bl || br)
      {
         self_type temp(e);
         temp.m_backend.swap(this->m_backend);
      }
      else if(left_depth >= right_depth)
      {
         do_assign(proto::left(e), typename proto::tag_of<left_type>::type());
         do_add(proto::right(e), typename proto::tag_of<right_type>::type());
      }
      else
      {
         do_assign(proto::right(e), typename proto::tag_of<right_type>::type());
         do_add(proto::left(e), typename proto::tag_of<left_type>::type());
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const proto::tag::minus&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;

      static int const left_depth = boost::result_of<detail::CalcDepth(left_type)>::type::value;
      static int const right_depth = boost::result_of<detail::CalcDepth(right_type)>::type::value;

      bool bl = contains_self(proto::left(e));
      bool br = contains_self(proto::right(e));

      if(bl && is_self(proto::left(e)))
      {
         // Ignore the left node, it's *this, just subtract the right:
         do_subtract(proto::right(e), typename proto::tag_of<right_type>::type());
      }
      else if(br && is_self(proto::right(e)))
      {
         // Ignore the right node, it's *this, just subtract the left and negate the result:
         do_subtract(proto::left(e), typename proto::tag_of<left_type>::type());
         m_backend.negate();
      }
      else if(bl || br)
      {
         self_type temp(e);
         temp.m_backend.swap(this->m_backend);
      }
      else if(left_depth >= right_depth)
      {
         do_assign(proto::left(e), typename proto::tag_of<left_type>::type());
         do_subtract(proto::right(e), typename proto::tag_of<right_type>::type());
      }
      else
      {
         do_assign(proto::right(e), typename proto::tag_of<right_type>::type());
         do_subtract(proto::left(e), typename proto::tag_of<left_type>::type());
         m_backend.negate();
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const proto::tag::multiplies&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;

      static int const left_depth = boost::result_of<detail::CalcDepth(left_type)>::type::value;
      static int const right_depth = boost::result_of<detail::CalcDepth(right_type)>::type::value;

      bool bl = contains_self(proto::left(e));
      bool br = contains_self(proto::right(e));

      if(bl && is_self(proto::left(e)))
      {
         // Ignore the left node, it's *this, just add the right:
         do_multiplies(proto::right(e), typename proto::tag_of<right_type>::type());
      }
      else if(br && is_self(proto::right(e)))
      {
         // Ignore the right node, it's *this, just add the left:
         do_multiplies(proto::left(e), typename proto::tag_of<left_type>::type());
      }
      else if(bl || br)
      {
         self_type temp(e);
         temp.m_backend.swap(this->m_backend);
      }
      else if(left_depth >= right_depth)
      {
         do_assign(proto::left(e), typename proto::tag_of<left_type>::type());
         do_multiplies(proto::right(e), typename proto::tag_of<right_type>::type());
      }
      else
      {
         do_assign(proto::right(e), typename proto::tag_of<right_type>::type());
         do_multiplies(proto::left(e), typename proto::tag_of<left_type>::type());
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const proto::tag::divides&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;

      static int const left_depth = boost::result_of<detail::CalcDepth(left_type)>::type::value;
      static int const right_depth = boost::result_of<detail::CalcDepth(right_type)>::type::value;

      bool bl = contains_self(proto::left(e));
      bool br = contains_self(proto::right(e));

      if(bl && is_self(proto::left(e)))
      {
         // Ignore the left node, it's *this, just add the right:
         do_divide(proto::right(e), typename proto::tag_of<right_type>::type());
      }
      else if(bl || br)
      {
         self_type temp(e);
         temp.m_backend.swap(this->m_backend);
      }
      else
      {
         do_assign(proto::left(e), typename proto::tag_of<left_type>::type());
         do_divide(proto::right(e), typename proto::tag_of<right_type>::type());
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const proto::tag::modulus&)
   {
      //
      // This operation is only valid for integer backends:
      //
      BOOST_STATIC_ASSERT_MSG(is_extended_integer<Backend>::value, "The modulus operation is only valid for integer types");

      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;

      static int const left_depth = boost::result_of<detail::CalcDepth(left_type)>::type::value;
      static int const right_depth = boost::result_of<detail::CalcDepth(right_type)>::type::value;

      bool bl = contains_self(proto::left(e));
      bool br = contains_self(proto::right(e));

      if(bl && is_self(proto::left(e)))
      {
         // Ignore the left node, it's *this, just add the right:
         do_modulus(proto::right(e), typename proto::tag_of<right_type>::type());
      }
      else if(bl || br)
      {
         self_type temp(e);
         temp.m_backend.swap(this->m_backend);
      }
      else
      {
         do_assign(proto::left(e), typename proto::tag_of<left_type>::type());
         do_modulus(proto::right(e), typename proto::tag_of<right_type>::type());
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const proto::tag::terminal&)
   {
      if(!is_self(e))
      {
         m_backend = canonical_value(proto::value(e));
      }
   }

   template <class Exp>
   void do_add(const Exp& e, const proto::tag::terminal&)
   {
      typedef typename proto::result_of::value<Exp>::type t1;
      typedef typename remove_reference<t1>::type t2;
      typedef typename remove_cv<t2>::type t3;
      typedef typename detail::canonical<t3, Backend>::type t4;
      typedef typename is_convertible<t4, const char*>::type tag;
      do_add_value(canonical_value(proto::value(e)), tag());
   }

   template <class V>
   void do_add_value(const V& v, const mpl::false_&)
   {
      m_backend += v;
   }
   template <class V>
   void do_add_value(const V& v, const mpl::true_&)
   {
      self_type temp(v);
      m_backend += temp.m_backend;
   }

   template <class Exp>
   void do_add(const Exp& e, const proto::tag::unary_plus&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      do_add(proto::left(e), typename proto::tag_of<left_type>::type());
   }

   template <class Exp>
   void do_add(const Exp& e, const proto::tag::negate&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      do_subtract(proto::left(e), typename proto::tag_of<left_type>::type());
   }

   template <class Exp>
   void do_add(const Exp& e, const proto::tag::plus&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;
      do_add(proto::left(e), typename proto::tag_of<left_type>::type());
      do_add(proto::right(e), typename proto::tag_of<right_type>::type());
   }

   template <class Exp>
   void do_add(const Exp& e, const proto::tag::minus&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;
      do_add(proto::left(e), typename proto::tag_of<left_type>::type());
      do_subtract(proto::right(e), typename proto::tag_of<right_type>::type());
   }

   template <class Exp, class unknown>
   void do_add(const Exp& e, const unknown&)
   {
      self_type temp(e);
      do_add(temp, proto::tag::terminal());
   }

   template <class Exp>
   void do_subtract(const Exp& e, const proto::tag::terminal&)
   {
      typedef typename proto::result_of::value<Exp>::type t1;
      typedef typename remove_reference<t1>::type t2;
      typedef typename remove_cv<t2>::type t3;
      typedef typename detail::canonical<t3, Backend>::type t4;
      typedef typename is_convertible<t4, const char*>::type tag;
      do_subtract_value(canonical_value(proto::value(e)), tag());
   }

   template <class V>
   void do_subtract_value(const V& v, const mpl::false_&)
   {
      m_backend -= v;
   }
   template <class V>
   void do_subtract_value(const V& v, const mpl::true_&)
   {
      self_type temp(v);
      m_backend -= temp.m_backend;
   }

   template <class Exp>
   void do_subtract(const Exp& e, const proto::tag::unary_plus&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      do_subtract(proto::left(e), typename proto::tag_of<left_type>::type());
   }

   template <class Exp>
   void do_subtract(const Exp& e, const proto::tag::negate&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      do_add(proto::left(e), typename proto::tag_of<left_type>::type());
   }

   template <class Exp>
   void do_subtract(const Exp& e, const proto::tag::plus&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;
      do_subtract(proto::left(e), typename proto::tag_of<left_type>::type());
      do_subtract(proto::right(e), typename proto::tag_of<right_type>::type());
   }

   template <class Exp>
   void do_subtract(const Exp& e, const proto::tag::minus&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;
      do_subtract(proto::left(e), typename proto::tag_of<left_type>::type());
      do_add(proto::right(e), typename proto::tag_of<right_type>::type());
   }

   template <class Exp, class unknown>
   void do_subtract(const Exp& e, const unknown&)
   {
      self_type temp(e);
      do_subtract(temp, proto::tag::terminal());
   }

   template <class Exp>
   void do_multiplies(const Exp& e, const proto::tag::terminal&)
   {
      typedef typename proto::result_of::value<Exp>::type t1;
      typedef typename remove_reference<t1>::type t2;
      typedef typename remove_cv<t2>::type t3;
      typedef typename detail::canonical<t3, Backend>::type t4;
      typedef typename is_convertible<t4, const char*>::type tag;
      do_multiplies_value(canonical_value(proto::value(e)), tag());
   }

   template <class Val>
   void do_multiplies_value(const Val& v, const mpl::false_&)
   {
      m_backend *= v;
   }

   template <class Val>
   void do_multiplies_value(const Val& e, const mpl::true_&)
   {
      self_type temp(e);
      m_backend *= temp.m_backend;
   }

   template <class Exp>
   void do_multiplies(const Exp& e, const proto::tag::unary_plus&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      do_multiplies(proto::left(e), typename proto::tag_of<left_type>::type());
   }

   template <class Exp>
   void do_multiplies(const Exp& e, const proto::tag::negate&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      do_multiplies(proto::left(e), typename proto::tag_of<left_type>::type());
      m_backend.negate();
   }

   template <class Exp>
   void do_multiplies(const Exp& e, const proto::tag::multiplies&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;
      do_multiplies(proto::left(e), typename proto::tag_of<left_type>::type());
      do_multiplies(proto::right(e), typename proto::tag_of<right_type>::type());
   }

   template <class Exp>
   void do_multiplies(const Exp& e, const proto::tag::divides&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;
      do_multiplies(proto::left(e), typename proto::tag_of<left_type>::type());
      do_divide(proto::right(e), typename proto::tag_of<right_type>::type());
   }

   template <class Exp, class unknown>
   void do_multiplies(const Exp& e, const unknown&)
   {
      self_type temp(e);
      m_backend *= temp.m_backend;
   }

   template <class Exp>
   void do_divide(const Exp& e, const proto::tag::terminal&)
   {
      typedef typename proto::result_of::value<Exp>::type t1;
      typedef typename remove_reference<t1>::type t2;
      typedef typename remove_cv<t2>::type t3;
      typedef typename detail::canonical<t3, Backend>::type t4;
      typedef typename is_convertible<t4, const char*>::type tag;
      do_divide_value(canonical_value(proto::value(e)), tag());
   }

   template <class Val>
   void do_divide_value(const Val& v, const mpl::false_&)
   {
      m_backend /= v;
   }

   template <class Val>
   void do_divide_value(const Val& e, const mpl::true_&)
   {
      self_type temp(e);
      m_backend /= temp.m_backend;
   }

   template <class Exp>
   void do_divide(const Exp& e, const proto::tag::unary_plus&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      do_divide(proto::left(e), typename proto::tag_of<left_type>::type());
   }

   template <class Exp>
   void do_divide(const Exp& e, const proto::tag::negate&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      do_divide(proto::left(e), typename proto::tag_of<left_type>::type());
      m_backend.negate();
   }

   template <class Exp>
   void do_divide(const Exp& e, const proto::tag::multiplies&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;
      do_divide(proto::left(e), typename proto::tag_of<left_type>::type());
      do_divide(proto::right(e), typename proto::tag_of<right_type>::type());
   }

   template <class Exp>
   void do_divide(const Exp& e, const proto::tag::divides&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::result_of::right<Exp>::type right_type;
      do_divide(proto::left(e), typename proto::tag_of<left_type>::type());
      do_multiplies(proto::right(e), typename proto::tag_of<right_type>::type());
   }

   template <class Exp, class unknown>
   void do_divide(const Exp& e, const unknown&)
   {
      self_type temp(e);
      m_backend /= temp.m_backend;
   }

   template <class Exp>
   void do_modulus(const Exp& e, const proto::tag::terminal&)
   {
      typedef typename proto::result_of::value<Exp>::type t1;
      typedef typename remove_reference<t1>::type t2;
      typedef typename remove_cv<t2>::type t3;
      typedef typename detail::canonical<t3, Backend>::type t4;
      typedef typename is_convertible<t4, const char*>::type tag;
      do_modulus_value(canonical_value(proto::value(e)), tag());
   }

   template <class Val>
   void do_modulus_value(const Val& v, const mpl::false_&)
   {
      m_backend %= v;
   }

   template <class Val>
   void do_modulus_value(const Val& e, const mpl::true_&)
   {
      self_type temp(e);
      m_backend %= temp.m_backend;
   }

   template <class Exp, class Unknown>
   void do_modulus(const Exp& e, const Unknown&)
   {
      self_type temp(e);
      do_modulus_value(canonical_value(proto::value(temp)), mpl::false_());
   }

   // Tests if the expression contains a reference to *this:
   template <class Exp>
   bool contains_self(const Exp& e)const
   {
      return contains_self(e, mpl::int_<proto::arity_of<Exp>::value>());
   }
   template <class Exp>
   bool contains_self(const Exp& e, mpl::int_<0> const&)const
   {
      return is_really_self(proto::value(e));
   }
   template <class Exp>
   bool contains_self(const Exp& e, mpl::int_<1> const&)const
   {
      typedef typename proto::result_of::child_c<Exp, 0>::type child_type;
      return contains_self(proto::child_c<0>(e), mpl::int_<proto::arity_of<child_type>::value>());
   }
   template <class Exp>
   bool contains_self(const Exp& e, mpl::int_<2> const&)const
   {
      typedef typename proto::result_of::child_c<Exp, 0>::type child0_type;
      typedef typename proto::result_of::child_c<Exp, 1>::type child1_type;
      return contains_self(proto::child_c<0>(e), mpl::int_<proto::arity_of<child0_type>::value>()) || contains_self(proto::child_c<1>(e), mpl::int_<proto::arity_of<child1_type>::value>());
   }

   // Test if the expression is a reference to *this:
   template <class Exp>
   bool is_self(const Exp& e)const
   {
      return is_self(e, mpl::int_<proto::arity_of<Exp>::value>());
   }
   template <class Exp>
   bool is_self(const Exp& e, mpl::int_<0> const&)const
   {
      return is_really_self(proto::value(e));
   }
   template <class Exp, int v>
   bool is_self(const Exp& e, mpl::int_<v> const&)const
   {
      return false;
   }

   template <class Val>
   bool is_really_self(const Val&)const { return false; }
   bool is_really_self(const self_type* v)const 
   { 
      return v == this; 
   }
   bool is_really_self(self_type* v)const
   { 
      return v == this; 
   }

   static const Backend& canonical_value(const self_type& v){  return v.m_backend;  }
   static const Backend& canonical_value(const self_type* v){  return v->m_backend;  }
   static const Backend& canonical_value(self_type* v){  return v->m_backend;  }
   template <class V>
   static typename detail::canonical<V, Backend>::type canonical_value(const V& v){  return v;  }
   static typename detail::canonical<std::string, Backend>::type canonical_value(const std::string& v){  return v.c_str();  }

   Backend m_backend;
};

namespace detail
{

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
struct expression_type
{
   typedef typename expression_type_imp<Exp, proto::arity_of<Exp>::value>::type type;
};

template <class Backend>
inline int big_number_compare(const big_number<Backend>& a, const big_number<Backend>& b)
{
   return a.compare(b);
}

template <class Backend, class Exp>
inline int big_number_compare(const big_number<Backend>& a, const big_number_exp<Exp>& b)
{
   return a.compare(big_number<Backend>(b));
}

template <class Exp, class Backend>
inline int big_number_compare(const big_number_exp<Exp>& a, const big_number<Backend>& b)
{
   return -b.compare(big_number<Backend>(a));
}

template <class Backend, class Val>
inline int big_number_compare(const big_number<Backend>& a, const Val b)
{
   return a.compare(b);
}

template <class Val, class Backend>
inline int big_number_compare(const Val a, const big_number<Backend>& b)
{
   return -b.compare(a);
}

template <class Exp1, class Exp2>
inline int big_number_compare(const big_number_exp<Exp1>& a, const big_number_exp<Exp2>& b)
{
   typedef typename expression_type<Exp1>::type real1;
   typedef typename expression_type<Exp2>::type real2;
   return real1(a).compare(real2(b));
}

template <class Exp, class Val>
inline typename enable_if<is_arithmetic<Val>, int>::type big_number_compare(const big_number_exp<Exp>& a, const Val b)
{
   typedef typename expression_type<Exp>::type real;
   real t(a);
   return t.compare(b);
}

template <class Val, class Exp>
inline typename enable_if<is_arithmetic<Val>, int>::type big_number_compare(const Val a, const big_number_exp<Exp>& b)
{
   typedef typename expression_type<Exp>::type real;
   return -real(b).compare(a);
}

template <class Exp1, class Exp2>
struct is_valid_comparison_imp
{
   typedef typename mpl::or_<
      is_big_number<Exp1>,
      is_big_number_exp<Exp1>
   >::type is1;
   typedef typename mpl::or_<
      is_big_number<Exp2>,
      is_big_number_exp<Exp2>
   >::type is2;
   typedef typename mpl::or_<
      mpl::and_<
         is1,
         mpl::or_<
            is2,
            is_arithmetic<Exp2>
         >
      >,
      mpl::and_<
         is2,
         mpl::or_<
            is1,
            is_arithmetic<Exp1>
         >
      >
   >::type type;
};

template <class Exp1, class Exp2>
struct is_valid_comparison : public boost::math::detail::is_valid_comparison_imp<Exp1, Exp2>::type {};

}


template <class Exp1, class Exp2>
inline typename boost::enable_if<detail::is_valid_comparison<Exp1, Exp2>, bool>::type 
   operator == (const Exp1& a, const Exp2& b)
{
   return 0 == detail::big_number_compare(a, b);
}

template <class Exp1, class Exp2>
inline typename boost::enable_if<detail::is_valid_comparison<Exp1, Exp2>, bool>::type 
   operator != (const Exp1& a, const Exp2& b)
{
   return 0 != detail::big_number_compare(a, b);
}

template <class Exp1, class Exp2>
inline typename boost::enable_if<detail::is_valid_comparison<Exp1, Exp2>, bool>::type 
   operator <= (const Exp1& a, const Exp2& b)
{
   return 0 >= detail::big_number_compare(a, b);
}

template <class Exp1, class Exp2>
inline typename boost::enable_if<detail::is_valid_comparison<Exp1, Exp2>, bool>::type 
   operator < (const Exp1& a, const Exp2& b)
{
   return 0 > detail::big_number_compare(a, b);
}

template <class Exp1, class Exp2>
inline typename boost::enable_if<detail::is_valid_comparison<Exp1, Exp2>, bool>::type 
   operator >= (const Exp1& a, const Exp2& b)
{
   return 0 <= detail::big_number_compare(a, b);
}

template <class Exp1, class Exp2>
inline typename boost::enable_if<detail::is_valid_comparison<Exp1, Exp2>, bool>::type 
   operator > (const Exp1& a, const Exp2& b)
{
   return 0 < detail::big_number_compare(a, b);
}

template <class Backend>
std::ostream& operator << (std::ostream& os, const big_number<Backend>& r)
{
   return os << r.str();
}

template <class Backend>
std::istream& operator >> (std::istream& is, big_number<Backend>& r)
{
   std::string s;
   is >> s;
   r = s;
   return is;
}

//
// Non-member functions accepting an expression-template as argument:
//
#undef sqrt
template <class Exp>
typename boost::math::detail::expression_type<Exp>::type sqrt(const detail::big_number_exp<Exp>& val)
{
   typedef typename detail::expression_type<Exp>::type result_type;
   return sqrt(result_type(val));
}
template <class Exp>
typename detail::expression_type<Exp>::type abs(const detail::big_number_exp<Exp>& val)
{
   typedef typename detail::expression_type<Exp>::type result_type;
   return abs(result_type(val));
}
template <class Exp>
typename detail::expression_type<Exp>::type fabs(const detail::big_number_exp<Exp>& val)
{
   typedef typename detail::expression_type<Exp>::type result_type;
   return fabs(result_type(val));
}
template <class Exp>
typename detail::expression_type<Exp>::type ceil(const detail::big_number_exp<Exp>& val)
{
   typedef typename detail::expression_type<Exp>::type result_type;
   return ceil(result_type(val));
}
template <class Exp>
typename detail::expression_type<Exp>::type floor(const detail::big_number_exp<Exp>& val)
{
   typedef typename detail::expression_type<Exp>::type result_type;
   return floor(result_type(val));
}
template <class Exp>
typename detail::expression_type<Exp>::type trunc(const detail::big_number_exp<Exp>& val)
{
   typedef typename detail::expression_type<Exp>::type result_type;
   return trunc(result_type(val));
}

}} // namespaces

#endif
