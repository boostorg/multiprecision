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
#include <boost/mpl/find_if.hpp>
#include <boost/assert.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/math/big_number/default_ops.hpp>

namespace boost{ namespace math{

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
      do_assign(e, typename detail::assign_and_eval<Exp>::type());
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
      do_assign(e, typename detail::assign_and_eval<Exp>::type());
   }

#ifndef BOOST_NO_RVALUE_REFERENCES
   big_number(big_number&& r) : m_backend(r.m_backend)
   {
      proto::value(*this) = this;
      BOOST_ASSERT(proto::value(*this) == this);
   }
   big_number& operator=(big_number&& r)
   {
      m_backend.swap(r.m_backend);
      return *this;
   }
#endif

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
      using big_num_default_ops::add;
      add(m_backend, canonical_value(v));
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
      using big_num_default_ops::subtract;
      subtract(m_backend, canonical_value(v));
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
      using big_num_default_ops::multiply;
      multiply(m_backend, canonical_value(v));
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
      using big_num_default_ops::modulus;
      modulus(m_backend, canonical_value(v));
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
      using big_num_default_ops::divide;
      divide(m_backend, canonical_value(v));
      return *this;
   }

   //
   // String conversion functions:
   //
   std::string str(unsigned digits = 0, bool scientific = true)const
   {
      return m_backend.str(digits, scientific);
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
   void do_assign(const Exp& e, const detail::add_immediates&)
   {
      using big_num_default_ops::add;
      typedef typename proto::tag_of<typename proto::result_of::left<Exp>::type>::type left_tag;
      typedef typename proto::tag_of<typename proto::result_of::right<Exp>::type>::type right_tag;
      add(m_backend, canonical_value(underlying_value(proto::left(e), left_tag())), canonical_value(underlying_value(proto::right(e), right_tag())));
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::add_and_negate_immediates&)
   {
      using big_num_default_ops::add;
      typedef typename proto::tag_of<typename proto::result_of::left<Exp>::type>::type left_tag;
      typedef typename proto::tag_of<typename proto::result_of::right<Exp>::type>::type right_tag;
      add(m_backend, canonical_value(underlying_value(proto::left(e), left_tag())), canonical_value(underlying_value(proto::right(e), right_tag())));
      m_backend.negate();
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::subtract_immediates&)
   {
      using big_num_default_ops::subtract;
      typedef typename proto::tag_of<typename proto::result_of::left<Exp>::type>::type left_tag;
      typedef typename proto::tag_of<typename proto::result_of::right<Exp>::type>::type right_tag;
      subtract(m_backend, canonical_value(underlying_value(proto::left(e), left_tag())), canonical_value(underlying_value(proto::right(e), right_tag())));
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::subtract_and_negate_immediates&)
   {
      using big_num_default_ops::subtract;
      typedef typename proto::tag_of<typename proto::result_of::left<Exp>::type>::type left_tag;
      typedef typename proto::tag_of<typename proto::result_of::right<Exp>::type>::type right_tag;
      subtract(m_backend, canonical_value(underlying_value(proto::left(e), left_tag())), canonical_value(underlying_value(proto::right(e), right_tag())));
      m_backend.negate();
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::multiply_immediates&)
   {
      using big_num_default_ops::multiply;
      typedef typename proto::tag_of<typename proto::result_of::left<Exp>::type>::type left_tag;
      typedef typename proto::tag_of<typename proto::result_of::right<Exp>::type>::type right_tag;
      multiply(m_backend, canonical_value(underlying_value(proto::left(e), left_tag())), canonical_value(underlying_value(proto::right(e), right_tag())));
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::multiply_and_negate_immediates&)
   {
      using big_num_default_ops::multiply;
      typedef typename proto::tag_of<typename proto::result_of::left<Exp>::type>::type left_tag;
      typedef typename proto::tag_of<typename proto::result_of::right<Exp>::type>::type right_tag;
      multiply(m_backend, canonical_value(underlying_value(proto::left(e), left_tag())), canonical_value(underlying_value(proto::right(e), right_tag())));
      m_backend.negate();
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::divide_immediates&)
   {
      using big_num_default_ops::divide;
      typedef typename proto::tag_of<typename proto::result_of::left<Exp>::type>::type left_tag;
      typedef typename proto::tag_of<typename proto::result_of::right<Exp>::type>::type right_tag;
      divide(m_backend, canonical_value(underlying_value(proto::left(e), left_tag())), canonical_value(underlying_value(proto::right(e), right_tag())));
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::divide_and_negate_immediates&)
   {
      using big_num_default_ops::divide;
      typedef typename proto::tag_of<typename proto::result_of::left<Exp>::type>::type left_tag;
      typedef typename proto::tag_of<typename proto::result_of::right<Exp>::type>::type right_tag;
      divide(m_backend, canonical_value(underlying_value(proto::left(e), left_tag())), canonical_value(underlying_value(proto::right(e), right_tag())));
      m_backend.negate();
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::modulus_immediates&)
   {
      using big_num_default_ops::modulus;
      typedef typename proto::tag_of<typename proto::result_of::left<Exp>::type>::type left_tag;
      typedef typename proto::tag_of<typename proto::result_of::right<Exp>::type>::type right_tag;
      modulus(m_backend, canonical_value(underlying_value(proto::left(e), left_tag())), canonical_value(underlying_value(proto::right(e), right_tag())));
   }

   template <class Exp>
   void do_assign(const Exp& e, const proto::tag::unary_plus&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      do_assign(proto::left(e), typename detail::assign_and_eval<left_type>::type());
   }

   template <class Exp>
   void do_assign(const Exp& e, const proto::tag::negate&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      do_assign(proto::left(e), typename detail::assign_and_eval<left_type>::type());
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
         do_assign(proto::left(e), typename detail::assign_and_eval<left_type>::type());
         do_add(proto::right(e), typename proto::tag_of<right_type>::type());
      }
      else
      {
         do_assign(proto::right(e), typename detail::assign_and_eval<right_type>::type());
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
         do_assign(proto::left(e), typename detail::assign_and_eval<left_type>::type());
         do_subtract(proto::right(e), typename proto::tag_of<right_type>::type());
      }
      else
      {
         do_assign(proto::right(e), typename detail::assign_and_eval<right_type>::type());
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
         do_assign(proto::left(e), typename detail::assign_and_eval<left_type>::type());
         do_multiplies(proto::right(e), typename proto::tag_of<right_type>::type());
      }
      else
      {
         do_assign(proto::right(e), typename detail::assign_and_eval<right_type>::type());
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
         do_assign(proto::left(e), typename detail::assign_and_eval<left_type>::type());
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
         do_assign(proto::left(e), typename detail::assign_and_eval<left_type>::type());
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
   void do_assign(const Exp& e, const proto::tag::function&)
   {
      typedef typename proto::arity_of<Exp>::type tag_type;
      do_assign_function(e, tag_type());
   }
   template <class Exp>
   void do_assign_function(const Exp& e, const mpl::long_<1>&)
   {
      proto::value(proto::left(e))(&m_backend);
   }
   template <class Exp>
   void do_assign_function(const Exp& e, const mpl::long_<2>&)
   {
      typedef typename proto::result_of::right<Exp>::type right_type;
      typedef typename proto::tag_of<right_type>::type tag_type;
      do_assign_function_1(proto::value(proto::left(e)), proto::right(e), tag_type());
   }
   template <class F, class Exp>
   void do_assign_function_1(const F& f, const Exp& val, const proto::tag::terminal&)
   {
      f(&m_backend, canonical_value(proto::value(val)));
   }
   template <class F, class Exp, class Tag>
   void do_assign_function_1(const F& f, const Exp& val, const Tag&)
   {
      big_number t(val);
      f(&m_backend, t.backend());
   }
   template <class Exp>
   void do_assign_function(const Exp& e, const mpl::long_<3>&)
   {
      typedef typename proto::result_of::right<Exp>::type right_type;
      typedef typename proto::tag_of<right_type>::type tag_type;
      typedef typename proto::result_of::child_c<Exp, 2>::type end_type;
      typedef typename proto::tag_of<end_type>::type end_tag;
      do_assign_function_2(proto::value(proto::left(e)), proto::right(e), proto::child_c<2>(e), tag_type(), end_tag());
   }
   template <class F, class Exp1, class Exp2>
   void do_assign_function_2(const F& f, const Exp1& val1, const Exp2& val2, const proto::tag::terminal&, const proto::tag::terminal&)
   {
      f(&m_backend, canonical_value(proto::value(val1)), canonical_value(proto::value(val2)));
   }

   template <class Exp>
   void do_add(const Exp& e, const proto::tag::terminal&)
   {
      using big_num_default_ops::add;
      add(m_backend, canonical_value(proto::value(e)));
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
      using big_num_default_ops::subtract;
      subtract(m_backend, canonical_value(proto::value(e)));
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
      using big_num_default_ops::multiply;
      multiply(m_backend, canonical_value(proto::value(e)));
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
      using big_num_default_ops::multiply;
      self_type temp(e);
      multiply(m_backend, temp.m_backend);
   }

   template <class Exp>
   void do_divide(const Exp& e, const proto::tag::terminal&)
   {
      using big_num_default_ops::divide;
      divide(m_backend, canonical_value(proto::value(e)));
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
      using big_num_default_ops::multiply;
      self_type temp(e);
      divide(m_backend, temp.m_backend);
   }

   template <class Exp>
   void do_modulus(const Exp& e, const proto::tag::terminal&)
   {
      using big_num_default_ops::modulus;
      modulus(m_backend, canonical_value(proto::value(e)));
   }

   template <class Exp, class Unknown>
   void do_modulus(const Exp& e, const Unknown&)
   {
      using big_num_default_ops::modulus;
      self_type temp(e);
      modulus(m_backend, canonical_value(proto::value(temp)));
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
   template <class Exp>
   static typename detail::underlying_result<Exp>::type underlying_value(const big_number_exp<Exp>& e, const proto::tag::terminal&)
   {
      return proto::value(e);
   }
   template <class Exp, class tag>
   static typename detail::underlying_result<Exp>::type 
      underlying_value(const big_number_exp<Exp>& e, const tag&)
   {
      typedef typename proto::result_of::left<Exp>::type left_type;
      typedef typename proto::tag_of<left_type>::type tag_type;
      return underlying_value(proto::left(e), tag_type());
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
inline std::ostream& operator << (std::ostream& os, const big_number<Backend>& r)
{
   return os << r.str(static_cast<unsigned>(os.precision(), os.flags() & os.scientific));
}
template <class Exp>
inline std::ostream& operator << (std::ostream& os, const detail::big_number_exp<Exp>& r)
{
   typedef typename detail::expression_type<Exp>::type value_type;
   value_type temp(r);
   return os << temp;
}

template <class Backend>
inline std::istream& operator >> (std::istream& is, big_number<Backend>& r)
{
   std::string s;
   is >> s;
   r = s;
   return is;
}

}} // namespaces

#endif
