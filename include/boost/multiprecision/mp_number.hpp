///////////////////////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_EXTENDED_REAL_HPP
#define BOOST_MATH_EXTENDED_REAL_HPP

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
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <boost/throw_exception.hpp>
#include <boost/multiprecision/detail/generic_interconvert.hpp>
#include <istream>  // stream operators
#include <cstdio>   // EOF

namespace boost{ namespace multiprecision{

#ifdef BOOST_MSVC
// warning C4127: conditional expression is constant
#pragma warning(push)
#pragma warning(disable:4127)
#endif

template <class Backend, bool ExpressionTemplates>
class mp_number
{
   typedef mp_number<Backend, ExpressionTemplates> self_type;
public:
   typedef Backend backend_type;
   BOOST_CONSTEXPR mp_number() BOOST_NOEXCEPT_IF(noexcept(Backend())) {}
   mp_number(const mp_number& e) BOOST_NOEXCEPT_IF(noexcept(Backend(static_cast<const Backend&>(std::declval<Backend>())))) : m_backend(e.m_backend){}
   template <class V>
   mp_number(V v, typename enable_if<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> > >::type* = 0)
   {
      m_backend = canonical_value(v);
   }
   mp_number(const mp_number& e, unsigned digits10)
      : m_backend(e.m_backend, digits10){}
   /*
   //
   // This conflicts with component based initialization (for rational and complex types)
   // which is arguably more useful.  Disabled for now.
   //
   template <class V>
   mp_number(V v, unsigned digits10, typename enable_if<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> > >::type* dummy1 = 0)
   {
      m_backend.precision(digits10);
      m_backend = canonical_value(v);
   }
   */
   template<bool ET>
   mp_number(const mp_number<Backend, ET>& val) BOOST_NOEXCEPT_IF(noexcept(Backend(static_cast<const Backend&>(std::declval<Backend>())))) : m_backend(val.m_backend) {}

   template <class Other, bool ET>
   mp_number(const mp_number<Other, ET>& val, typename enable_if<boost::is_convertible<Other, Backend> >::type* = 0) BOOST_NOEXCEPT_IF(noexcept(std::declval<Backend>() = std::declval<Other>()))
   {
      m_backend = val.backend();
   }
   template <class Other, bool ET>
   mp_number(const mp_number<Other, ET>& val, typename disable_if<boost::is_convertible<Other, Backend> >::type* = 0)
   {
      //
      // Attempt a generic interconvertion:
      //
      detail::generic_interconvert(backend(), val.backend(), number_category<Backend>(), number_category<Other>());
   }
   template <class V>
   mp_number(V v1, V v2, typename enable_if<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> > >::type* = 0)
   {
      using default_ops::assign_components;
      assign_components(m_backend, canonical_value(v1), canonical_value(v2));
   }
   template <class Other, bool ET>
   mp_number(const mp_number<Other, ET>& v1, const mp_number<Other, ET>& v2, typename enable_if<boost::is_convertible<Other, Backend> >::type* = 0)
   {
      using default_ops::assign_components;
      assign_components(m_backend, v1.backend(), v2.backend());
   }

   template <class V>
   mp_number(V v, typename enable_if<mpl::and_<is_convertible<V, Backend>, mpl::not_<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> > > > >::type* = 0)
      BOOST_NOEXCEPT_IF(noexcept(Backend(static_cast<const V&>(std::declval<V>()))))
      : m_backend(v){}

   template <class tag, class Arg1, class Arg2, class Arg3, class Arg4>
   mp_number& operator=(const detail::mp_exp<tag, Arg1, Arg2, Arg3, Arg4>& e)
   {
      typedef typename is_same<mp_number, typename detail::mp_exp<tag, Arg1, Arg2, Arg3, Arg4>::result_type>::type tag_type;
      do_assign(e, tag_type());
      return *this;
   }

   mp_number& operator=(const mp_number& e) BOOST_NOEXCEPT_IF(noexcept(std::declval<Backend>() = static_cast<const Backend&>(std::declval<Backend>())))
   {
      m_backend = e.m_backend;
      return *this;
   }

   template <class V>
   typename enable_if<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> >, mp_number<Backend, ExpressionTemplates>& >::type 
      operator=(const V& v) BOOST_NOEXCEPT_IF(noexcept(std::declval<Backend>() = std::declval<typename boost::multiprecision::detail::canonical<V, Backend>::type>()))
   {
      m_backend = canonical_value(v);
      return *this;
   }

   template <class V>
   typename enable_if<mpl::and_<is_convertible<V, Backend>, mpl::not_<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> > > >, mp_number<Backend, ExpressionTemplates>& >::type 
      operator=(const V& v) BOOST_NOEXCEPT_IF(noexcept(std::declval<Backend>() = static_cast<const V&>(std::declval<V>())))
   {
      m_backend = v;
      return *this;
   }

   template <bool ET>
   mp_number& operator=(const mp_number<Backend, ET>& v) BOOST_NOEXCEPT_IF(noexcept(std::declval<Backend>() = static_cast<const Backend&>(std::declval<Backend>())))
   {
      m_backend = v.backend();
      return *this;
   }

   template <class Other>
   typename enable_if<is_convertible<Other, Backend>, mp_number<Backend, ExpressionTemplates>& >::type 
      operator=(const mp_number<Other>& v) BOOST_NOEXCEPT_IF(noexcept(std::declval<Backend>() = static_cast<const Other&>(std::declval<Other>())))
   {
      m_backend = v.backend();
      return *this;
   }

   template <class Other>
   typename disable_if<is_convertible<Other, Backend>, mp_number<Backend, ExpressionTemplates>& >::type 
      operator=(const mp_number<Other>& v)
   {
      //
      // Attempt a generic interconvertion:
      //
      detail::generic_interconvert(backend(), v.backend(), number_category<Backend>(), number_category<Other>());
      return *this;
   }

   template <class tag, class Arg1, class Arg2, class Arg3, class Arg4>
   mp_number(const detail::mp_exp<tag, Arg1, Arg2, Arg3, Arg4>& e)
   {
      *this = e;
   }

#ifndef BOOST_NO_RVALUE_REFERENCES
   mp_number(mp_number&& r) BOOST_NOEXCEPT : m_backend(static_cast<Backend&&>(r.m_backend)){}
   mp_number& operator=(mp_number&& r) BOOST_NOEXCEPT 
   {
      m_backend.swap(r.m_backend);
      return *this;
   }
#endif

   mp_number& operator+=(const self_type& val)
   {
      do_add(detail::mp_exp<detail::terminal, self_type>(val), detail::terminal());
      return *this;
   }

   template <class tag, class Arg1, class Arg2, class Arg3, class Arg4>
   mp_number& operator+=(const detail::mp_exp<tag, Arg1, Arg2, Arg3, Arg4>& e)
   {
      // Create a copy if e contains this, but not if we're just doing a
      //    x += x
      if(contains_self(e) && !is_self(e))
      {
         self_type temp(e);
         do_add(detail::mp_exp<detail::terminal, self_type>(temp), detail::terminal());
      }
      else
      {
         do_add(e, tag());
      }
      return *this;
   }

   template <class V>
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend, ExpressionTemplates>& >::type 
      operator+=(const V& v)
   {
      using default_ops::eval_add;
      eval_add(m_backend, canonical_value(v));
      return *this;
   }

   mp_number& operator-=(const self_type& val)
   {
      do_subtract(detail::mp_exp<detail::terminal, self_type>(val), detail::terminal());
      return *this;
   }

   template <class Exp>
   mp_number& operator-=(const detail::mp_exp<Exp>& e)
   {
      // Create a copy if e contains this:
      if(contains_self(e))
      {
         self_type temp(e);
         do_subtract(temp, detail::terminal());
      }
      else
      {
         do_subtract(e, typename Exp::tag_type());
      }
      return *this;
   }

   template <class V>
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend, ExpressionTemplates>& >::type 
      operator-=(const V& v)
   {
      using default_ops::eval_subtract;
      eval_subtract(m_backend, canonical_value(v));
      return *this;
   }


   mp_number& operator *= (const self_type& e)
   {
      do_multiplies(detail::mp_exp<detail::terminal, self_type>(e), detail::terminal());
      return *this;
   }

   template <class Exp>
   mp_number& operator*=(const detail::mp_exp<Exp>& e)
   {
      // Create a temporary if the RHS references *this, but not
      // if we're just doing an   x *= x;
      if(contains_self(e) && !is_self(e))
      {
         self_type temp(e);
         do_multiplies(detail::mp_exp<detail::terminal, self_type>(temp), detail::terminal());
      }
      else
      {
         do_multiplies(e, typename Exp::tag_type());
      }
      return *this;
   }

   template <class V>
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend, ExpressionTemplates>& >::type 
      operator*=(const V& v)
   {
      using default_ops::eval_multiply;
      eval_multiply(m_backend, canonical_value(v));
      return *this;
   }

   mp_number& operator%=(const self_type& e)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The modulus operation is only valid for integer types");
      do_modulus(detail::mp_exp<detail::terminal, self_type>(e), detail::terminal());
      return *this;
   }
   template <class Exp>
   mp_number& operator%=(const detail::mp_exp<Exp>& e)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The modulus operation is only valid for integer types");
      // Create a temporary if the RHS references *this:
      if(contains_self(e))
      {
         self_type temp(e);
         do_modulus(temp, detail::terminal());
      }
      else
      {
         do_modulus(e, typename Exp::tag_type());
      }
      return *this;
   }
   template <class V>
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend, ExpressionTemplates>& >::type 
      operator%=(const V& v)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The modulus operation is only valid for integer types");
      using default_ops::eval_modulus;
      eval_modulus(m_backend, canonical_value(v));
      return *this;
   }

   //
   // These operators are *not* proto-ized.
   // The issue is that the increment/decrement must happen
   // even if the result of the operator *is never used*.
   // Possibly we could modify our expression wrapper to
   // execute the increment/decrement on destruction, but
   // correct implemetation will be tricky, so defered for now...
   //
   mp_number& operator++()
   {
      using default_ops::eval_increment;
      eval_increment(m_backend);
      return *this;
   }

   mp_number& operator--()
   {
      using default_ops::eval_decrement;
      eval_decrement(m_backend);
      return *this;
   }

   mp_number operator++(int)
   {
      using default_ops::eval_increment;
      self_type temp(*this);
      eval_increment(m_backend);
      return BOOST_MP_MOVE(temp);
   }

   mp_number operator--(int)
   {
      using default_ops::eval_decrement;
      self_type temp(*this);
      eval_decrement(m_backend);
      return BOOST_MP_MOVE(temp);
   }

   template <class V>
   typename enable_if<is_integral<V>, mp_number&>::type operator <<= (V val)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The left-shift operation is only valid for integer types");
      check_shift_range(val, mpl::bool_<(sizeof(V) > sizeof(std::size_t))>(), is_signed<V>());
      eval_left_shift(m_backend, canonical_value(val));
      return *this;
   }

   template <class V>
   typename enable_if<is_integral<V>, mp_number&>::type operator >>= (V val)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The right-shift operation is only valid for integer types");
      check_shift_range(val, mpl::bool_<(sizeof(V) > sizeof(std::size_t))>(), is_signed<V>());
      eval_right_shift(m_backend, canonical_value(val));
      return *this;
   }

   mp_number& operator /= (const self_type& e)
   {
      do_divide(detail::mp_exp<detail::terminal, self_type>(e), detail::terminal());
      return *this;
   }

   template <class Exp>
   mp_number& operator/=(const detail::mp_exp<Exp>& e)
   {
      // Create a temporary if the RHS references *this:
      if(contains_self(e))
      {
         self_type temp(e);
         do_divide(detail::mp_exp<detail::terminal, self_type>(temp), detail::terminal());
      }
      else
      {
         do_divide(e, typename Exp::tag_type());
      }
      return *this;
   }

   template <class V>
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend, ExpressionTemplates>& >::type 
      operator/=(const V& v)
   {
      using default_ops::eval_divide;
      eval_divide(m_backend, canonical_value(v));
      return *this;
   }

   mp_number& operator&=(const self_type& e)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise & operation is only valid for integer types");
      do_bitwise_and(detail::mp_exp<detail::terminal, self_type>(e), detail::terminal());
      return *this;
   }

   template <class Exp>
   mp_number& operator&=(const detail::mp_exp<Exp>& e)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise & operation is only valid for integer types");
      // Create a temporary if the RHS references *this, but not
      // if we're just doing an   x &= x;
      if(contains_self(e) && !is_self(e))
      {
         self_type temp(e);
         do_bitwise_and(temp, detail::terminal());
      }
      else
      {
         do_bitwise_and(e, typename Exp::tag_type());
      }
      return *this;
   }

   template <class V>
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend, ExpressionTemplates>& >::type 
      operator&=(const V& v)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise & operation is only valid for integer types");
      using default_ops::eval_bitwise_and;
      eval_bitwise_and(m_backend, canonical_value(v));
      return *this;
   }

   mp_number& operator|=(const self_type& e)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise | operation is only valid for integer types");
      do_bitwise_or(detail::mp_exp<detail::terminal, self_type>(e), detail::terminal());
      return *this;
   }

   template <class Exp>
   mp_number& operator|=(const detail::mp_exp<Exp>& e)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise | operation is only valid for integer types");
      // Create a temporary if the RHS references *this, but not
      // if we're just doing an   x |= x;
      if(contains_self(e) && !is_self(e))
      {
         self_type temp(e);
         do_bitwise_or(temp, detail::terminal());
      }
      else
      {
         do_bitwise_or(e, typename Exp::tag_type());
      }
      return *this;
   }

   template <class V>
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend, ExpressionTemplates>& >::type 
      operator|=(const V& v)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise | operation is only valid for integer types");
      using default_ops::eval_bitwise_or;
      eval_bitwise_or(m_backend, canonical_value(v));
      return *this;
   }

   mp_number& operator^=(const self_type& e)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise ^ operation is only valid for integer types");
      do_bitwise_xor(detail::mp_exp<detail::terminal, self_type>(e), detail::terminal());
      return *this;
   }

   template <class Exp>
   mp_number& operator^=(const detail::mp_exp<Exp>& e)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise ^ operation is only valid for integer types");
      if(contains_self(e))
      {
         self_type temp(e);
         do_bitwise_xor(temp, detail::terminal());
      }
      else
      {
         do_bitwise_xor(e, typename Exp::tag_type());
      }
      return *this;
   }

   template <class V>
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend, ExpressionTemplates>& >::type 
      operator^=(const V& v)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise ^ operation is only valid for integer types");
      using default_ops::eval_bitwise_xor;
      eval_bitwise_xor(m_backend, canonical_value(v));
      return *this;
   }
   //
   // Use in boolean context:
   //
   typedef bool (self_type::*unmentionable_type)()const;

   operator unmentionable_type()const
   {
      return is_zero() ? 0 : &self_type::is_zero;
   }

   //
   // swap:
   //
   void swap(self_type& other) BOOST_NOEXCEPT
   {
      m_backend.swap(other.backend());
   }
   //
   // Zero and sign:
   //
   bool is_zero()const
   {
      using default_ops::eval_is_zero;
      return eval_is_zero(m_backend);
   }
   int sign()const
   {
      using default_ops::eval_get_sign;
      return eval_get_sign(m_backend);
   }
   //
   // String conversion functions:
   //
   std::string str(std::streamsize digits = 0, std::ios_base::fmtflags f = std::ios_base::fmtflags(0))const
   {
      return m_backend.str(digits, f);
   }
   template <class T>
   T convert_to()const
   {
      using default_ops::eval_convert_to;
      T result;
      eval_convert_to(&result, m_backend);
      return result;
   }
   //
   // Default precision:
   //
   static unsigned default_precision() BOOST_NOEXCEPT
   {
      return Backend::default_precision();
   }
   static void default_precision(unsigned digits10)
   {
      Backend::default_precision(digits10);
   }
   unsigned precision()const BOOST_NOEXCEPT
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
   int compare(const mp_number<Backend, ExpressionTemplates>& o)const
      BOOST_NOEXCEPT_IF(noexcept(std::declval<Backend>().compare(std::declval<Backend>())))
   {
      return m_backend.compare(o.m_backend);
   }
   template <class V>
   typename enable_if<is_arithmetic<V>, int>::type compare(const V& o)const
   {
      using default_ops::eval_get_sign;
      if(o == 0)
         return eval_get_sign(m_backend);
      return m_backend.compare(canonical_value(o));
   }
   Backend& backend() BOOST_NOEXCEPT
   {
      return m_backend;
   }
   const Backend& backend()const BOOST_NOEXCEPT
   {
      return m_backend;
   }
private:
   template <class tag, class Arg1, class Arg2, class Arg3, class Arg4>
   void do_assign(const detail::mp_exp<tag, Arg1, Arg2, Arg3, Arg4>& e, const mpl::true_&)
   {
      do_assign(e, tag());
   }
   template <class tag, class Arg1, class Arg2, class Arg3, class Arg4>
   void do_assign(const detail::mp_exp<tag, Arg1, Arg2, Arg3, Arg4>& e, const mpl::false_&)
   {
      // The result of the expression isn't the same type as this -
      // create a temporary result and assign it to *this:
      typedef typename detail::mp_exp<tag, Arg1, Arg2, Arg3, Arg4>::result_type temp_type;
      temp_type t(e);
      *this = t;
   }


   template <class V>
   void check_shift_range(V val, const mpl::true_&, const mpl::true_&)
   {
      if(val > (std::numeric_limits<std::size_t>::max)())
         BOOST_THROW_EXCEPTION(std::out_of_range("Can not shift by a value greater than std::numeric_limits<std::size_t>::max()."));
      if(val < 0)
         BOOST_THROW_EXCEPTION(std::out_of_range("Can not shift by a negative value."));
   }
   template <class V>
   void check_shift_range(V val, const mpl::false_&, const mpl::true_&)
   {
      if(val < 0)
         BOOST_THROW_EXCEPTION(std::out_of_range("Can not shift by a negative value."));
   }
   template <class V>
   void check_shift_range(V val, const mpl::true_&, const mpl::false_&)
   {
      if(val > (std::numeric_limits<std::size_t>::max)())
         BOOST_THROW_EXCEPTION(std::out_of_range("Can not shift by a value greater than std::numeric_limits<std::size_t>::max()."));
   }
   template <class V>
   void check_shift_range(V, const mpl::false_&, const mpl::false_&) BOOST_NOEXCEPT{}

   template <class Exp>
   void do_assign(const Exp& e, const detail::add_immediates&)
   {
      using default_ops::eval_add;
      eval_add(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::subtract_immediates&)
   {
      using default_ops::eval_subtract;
      eval_subtract(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::multiply_immediates&)
   {
      using default_ops::eval_multiply;
      eval_multiply(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::divide_immediates&)
   {
      using default_ops::eval_divide;
      eval_divide(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::negate&)
   {
      typedef typename Exp::left_type left_type;
      do_assign(e.left(), typename left_type::tag_type());
      m_backend.negate();
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::plus&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;

      static int const left_depth = left_type::depth;
      static int const right_depth = right_type::depth;

      bool bl = contains_self(e.left());
      bool br = contains_self(e.right());

      if(bl && is_self(e.left()))
      {
         // Ignore the left node, it's *this, just add the right:
         do_add(e.right(), typename right_type::tag_type());
      }
      else if(br && is_self(e.right()))
      {
         // Ignore the right node, it's *this, just add the left:
         do_add(e.left(), typename left_type::tag_type());
      }
      else if(bl || br)
      {
         self_type temp(e);
         temp.m_backend.swap(this->m_backend);
      }
      else if(left_depth >= right_depth)
      {
         do_assign(e.left(), typename left_type::tag_type());
         do_add(e.right(), typename right_type::tag_type());
      }
      else
      {
         do_assign(e.right(), typename right_type::tag_type());
         do_add(e.left(), typename left_type::tag_type());
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::minus&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;

      static int const left_depth = left_type::depth;
      static int const right_depth = right_type::depth;

      bool bl = contains_self(e.left());
      bool br = contains_self(e.right());

      if(bl && is_self(e.left()))
      {
         // Ignore the left node, it's *this, just subtract the right:
         do_subtract(e.right(), typename right_type::tag_type());
      }
      else if(br && is_self(e.right()))
      {
         // Ignore the right node, it's *this, just subtract the left and negate the result:
         do_subtract(e.left(), typename left_type::tag_type());
         m_backend.negate();
      }
      else if(bl || br)
      {
         self_type temp(e);
         temp.m_backend.swap(this->m_backend);
      }
      else if(left_depth >= right_depth)
      {
         do_assign(e.left(), typename left_type::tag_type());
         do_subtract(e.right(), typename right_type::tag_type());
      }
      else
      {
         do_assign(e.right(), typename right_type::tag_type());
         do_subtract(e.left(), typename left_type::tag_type());
         m_backend.negate();
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::multiplies&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;

      static int const left_depth = left_type::depth;
      static int const right_depth = right_type::depth;

      bool bl = contains_self(e.left());
      bool br = contains_self(e.right());

      if(bl && is_self(e.left()))
      {
         // Ignore the left node, it's *this, just add the right:
         do_multiplies(e.right(), typename right_type::tag_type());
      }
      else if(br && is_self(e.right()))
      {
         // Ignore the right node, it's *this, just add the left:
         do_multiplies(e.left(), typename left_type::tag_type());
      }
      else if(bl || br)
      {
         self_type temp(e);
         temp.m_backend.swap(this->m_backend);
      }
      else if(left_depth >= right_depth)
      {
         do_assign(e.left(), typename left_type::tag_type());
         do_multiplies(e.right(), typename right_type::tag_type());
      }
      else
      {
         do_assign(e.right(), typename right_type::tag_type());
         do_multiplies(e.left(), typename left_type::tag_type());
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::divides&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;

      bool bl = contains_self(e.left());
      bool br = contains_self(e.right());

      if(bl && is_self(e.left()))
      {
         // Ignore the left node, it's *this, just add the right:
         do_divide(e.right(), typename right_type::tag_type());
      }
      else if(bl || br)
      {
         self_type temp(e);
         temp.m_backend.swap(this->m_backend);
      }
      else
      {
         do_assign(e.left(), typename left_type::tag_type());
         do_divide(e.right(), typename right_type::tag_type());
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::modulus&)
   {
      //
      // This operation is only valid for integer backends:
      //
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The modulus operation is only valid for integer types");

      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;

      bool bl = contains_self(e.left());
      bool br = contains_self(e.right());

      if(bl && is_self(e.left()))
      {
         // Ignore the left node, it's *this, just add the right:
         do_modulus(e.right(), typename right_type::tag_type());
      }
      else if(bl || br)
      {
         self_type temp(e);
         temp.m_backend.swap(this->m_backend);
      }
      else
      {
         do_assign(e.left(), typename left_type::tag_type());
         do_modulus(e.right(), typename right_type::tag_type());
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::modulus_immediates&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The modulus operation is only valid for integer types");
      using default_ops::eval_modulus;
      eval_modulus(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::bitwise_and&)
   {
      //
      // This operation is only valid for integer backends:
      //
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "Bitwise operations are only valid for integer types");

      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;

      static int const left_depth = left_type::depth;
      static int const right_depth = right_type::depth;

      bool bl = contains_self(e.left());
      bool br = contains_self(e.right());

      if(bl && is_self(e.left()))
      {
         // Ignore the left node, it's *this, just add the right:
         do_bitwise_and(e.right(), typename right_type::tag_type());
      }
      else if(br && is_self(e.right()))
      {
         do_bitwise_and(e.left(), typename left_type::tag_type());
      }
      else if(left_depth >= right_depth)
      {
         do_assign(e.left(), typename left_type::tag_type());
         do_bitwise_and(e.right(), typename right_type::tag_type());
      }
      else
      {
         do_assign(e.right(), typename right_type::tag_type());
         do_bitwise_and(e.left(), typename left_type::tag_type());
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::bitwise_and_immediates&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "Bitwise operations are only valid for integer types");
      using default_ops::eval_bitwise_and;
      eval_bitwise_and(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::bitwise_or&)
   {
      //
      // This operation is only valid for integer backends:
      //
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "Bitwise operations are only valid for integer types");

      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;

      static int const left_depth = left_type::depth;
      static int const right_depth = right_type::depth;

      bool bl = contains_self(e.left());
      bool br = contains_self(e.right());

      if(bl && is_self(e.left()))
      {
         // Ignore the left node, it's *this, just add the right:
         do_bitwise_or(e.right(), typename right_type::tag_type());
      }
      else if(br && is_self(e.right()))
      {
         do_bitwise_or(e.left(), typename left_type::tag_type());
      }
      else if(left_depth >= right_depth)
      {
         do_assign(e.left(), typename left_type::tag_type());
         do_bitwise_or(e.right(), typename right_type::tag_type());
      }
      else
      {
         do_assign(e.right(), typename right_type::tag_type());
         do_bitwise_or(e.left(), typename left_type::tag_type());
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::bitwise_or_immediates&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "Bitwise operations are only valid for integer types");
      using default_ops::eval_bitwise_or;
      eval_bitwise_or(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::bitwise_xor&)
   {
      //
      // This operation is only valid for integer backends:
      //
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "Bitwise operations are only valid for integer types");

      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;

      static int const left_depth = left_type::depth;
      static int const right_depth = right_type::depth;

      bool bl = contains_self(e.left());
      bool br = contains_self(e.right());

      if(bl && is_self(e.left()))
      {
         // Ignore the left node, it's *this, just add the right:
         do_bitwise_xor(e.right(), typename right_type::tag_type());
      }
      else if(br && is_self(e.right()))
      {
         do_bitwise_xor(e.left(), typename left_type::tag_type());
      }
      else if(left_depth >= right_depth)
      {
         do_assign(e.left(), typename left_type::tag_type());
         do_bitwise_xor(e.right(), typename right_type::tag_type());
      }
      else
      {
         do_assign(e.right(), typename right_type::tag_type());
         do_bitwise_xor(e.left(), typename left_type::tag_type());
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::bitwise_xor_immediates&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "Bitwise operations are only valid for integer types");
      using default_ops::eval_bitwise_xor;
      eval_bitwise_xor(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::terminal&)
   {
      if(!is_self(e))
      {
         m_backend = canonical_value(e.value());
      }
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::function&)
   {
      typedef typename Exp::arity tag_type;
      do_assign_function(e, tag_type());
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::shift_left&)
   {
      // We can only shift by an integer value, not an arbitrary expression:
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      typedef typename right_type::arity right_arity;
      BOOST_STATIC_ASSERT_MSG(right_arity::value == 0, "The left shift operator requires an integer value for the shift operand.");
      typedef typename right_type::result_type right_value_type;
      BOOST_STATIC_ASSERT_MSG(is_integral<right_value_type>::value, "The left shift operator requires an integer value for the shift operand.");
      typedef typename left_type::tag_type tag_type;
      do_assign_left_shift(e.left(), canonical_value(e.right().value()), tag_type());
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::shift_right&)
   {
      // We can only shift by an integer value, not an arbitrary expression:
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      typedef typename right_type::arity right_arity;
      BOOST_STATIC_ASSERT_MSG(right_arity::value == 0, "The left shift operator requires an integer value for the shift operand.");
      typedef typename right_type::result_type right_value_type;
      BOOST_STATIC_ASSERT_MSG(is_integral<right_value_type>::value, "The left shift operator requires an integer value for the shift operand.");
      typedef typename left_type::tag_type tag_type;
      do_assign_right_shift(e.left(), canonical_value(e.right().value()), tag_type());
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::bitwise_complement&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise ~ operation is only valid for integer types");
      using default_ops::eval_complement;
      self_type temp(e.left());
      eval_complement(m_backend, temp.backend());
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::complement_immediates&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise ~ operation is only valid for integer types");
      using default_ops::eval_complement;
      eval_complement(m_backend, canonical_value(e.left().value()));
   }

   template <class Exp, class Val>
   void do_assign_right_shift(const Exp& e, const Val& val, const detail::terminal&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The right shift operation is only valid for integer types");
      using default_ops::eval_right_shift;
      eval_right_shift(m_backend, canonical_value(e.value()), val);
   }

   template <class Exp, class Val>
   void do_assign_left_shift(const Exp& e, const Val& val, const detail::terminal&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The left shift operation is only valid for integer types");
      using default_ops::eval_left_shift;
      eval_left_shift(m_backend, canonical_value(e.value()), val);
   }

   template <class Exp, class Val, class Tag>
   void do_assign_right_shift(const Exp& e, const Val& val, const Tag&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The right shift operation is only valid for integer types");
      using default_ops::eval_right_shift;
      self_type temp(e);
      eval_right_shift(m_backend, temp.backend(), val);
   }

   template <class Exp, class Val, class Tag>
   void do_assign_left_shift(const Exp& e, const Val& val, const Tag&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The left shift operation is only valid for integer types");
      using default_ops::eval_left_shift;
      self_type temp(e);
      eval_left_shift(m_backend, temp.backend(), val);
   }

   template <class Exp>
   void do_assign_function(const Exp& e, const mpl::int_<1>&)
   {
      e.left().value()(&m_backend);
   }
   template <class Exp>
   void do_assign_function(const Exp& e, const mpl::int_<2>&)
   {
      typedef typename Exp::right_type right_type;
      typedef typename right_type::tag_type tag_type;
      do_assign_function_1(e.left().value(), e.right_ref(), tag_type());
   }
   template <class F, class Exp>
   void do_assign_function_1(const F& f, const Exp& val, const detail::terminal&)
   {
      f(m_backend, function_arg_value(val));
   }
   template <class F, class Exp, class Tag>
   void do_assign_function_1(const F& f, const Exp& val, const Tag&)
   {
      mp_number t(val);
      f(m_backend, t.backend());
   }
   template <class Exp>
   void do_assign_function(const Exp& e, const mpl::int_<3>&)
   {
      typedef typename Exp::middle_type middle_type;
      typedef typename middle_type::tag_type tag_type;
      typedef typename Exp::right_type end_type;
      typedef typename end_type::tag_type end_tag;
      do_assign_function_2(e.left().value(), e.middle_ref(), e.right_ref(), tag_type(), end_tag());
   }
   template <class F, class Exp1, class Exp2>
   void do_assign_function_2(const F& f, const Exp1& val1, const Exp2& val2, const detail::terminal&, const detail::terminal&)
   {
      f(m_backend, function_arg_value(val1), function_arg_value(val2));
   }
   template <class F, class Exp1, class Exp2, class Tag1>
   void do_assign_function_2(const F& f, const Exp1& val1, const Exp2& val2, const Tag1&, const detail::terminal&)
   {
      self_type temp1(val1);
      f(m_backend, BOOST_MP_MOVE(temp1.backend()), function_arg_value(val2));
   }
   template <class F, class Exp1, class Exp2, class Tag2>
   void do_assign_function_2(const F& f, const Exp1& val1, const Exp2& val2, const detail::terminal&, const Tag2&)
   {
      self_type temp2(val2);
      f(m_backend, function_arg_value(val1), BOOST_MP_MOVE(temp2.backend()));
   }
   template <class F, class Exp1, class Exp2, class Tag1, class Tag2>
   void do_assign_function_2(const F& f, const Exp1& val1, const Exp2& val2, const Tag1&, const Tag2&)
   {
      self_type temp1(val1);
      self_type temp2(val2);
      f(m_backend, BOOST_MP_MOVE(temp1.backend()), BOOST_MP_MOVE(temp2.backend()));
   }

   template <class Exp>
   void do_assign_function(const Exp& e, const mpl::int_<4>&)
   {
      typedef typename Exp::left_middle_type left_type;
      typedef typename left_type::tag_type left_tag_type;
      typedef typename Exp::right_middle_type middle_type;
      typedef typename middle_type::tag_type middle_tag_type;
      typedef typename Exp::right_type right_type;
      typedef typename right_type::tag_type right_tag_type;
      do_assign_function_3a(e.left().value(), e.left_middle_ref(), e.right_middle_ref(), e.right_ref(), left_tag_type(), middle_tag_type(), right_tag_type());
   }
   template <class F, class Exp1, class Exp2, class Exp3, class Tag2, class Tag3>
   void do_assign_function_3a(const F& f, const Exp1& val1, const Exp2& val2, const Exp3& val3, const detail::terminal&, const Tag2& t2, const Tag3& t3)
   {
      do_assign_function_3b(f, val1, val2, val3, t2, t3);
   }
   template <class F, class Exp1, class Exp2, class Exp3, class Tag1, class Tag2, class Tag3>
   void do_assign_function_3a(const F& f, const Exp1& val1, const Exp2& val2, const Exp3& val3, const Tag1&, const Tag2& t2, const Tag3& t3)
   {
      mp_number t(val1);
      do_assign_function_3b(f, BOOST_MP_MOVE(t), val2, val3, t2, t3);
   }
   template <class F, class Exp1, class Exp2, class Exp3, class Tag3>
   void do_assign_function_3b(const F& f, const Exp1& val1, const Exp2& val2, const Exp3& val3, const detail::terminal&, const Tag3& t3)
   {
      do_assign_function_3c(f, val1, val2, val3, t3);
   }
   template <class F, class Exp1, class Exp2, class Exp3, class Tag2, class Tag3>
   void do_assign_function_3b(const F& f, const Exp1& val1, const Exp2& val2, const Exp3& val3, const Tag2& /*t2*/, const Tag3& t3)
   {
      mp_number t(val2);
      do_assign_function_3c(f, val1, BOOST_MP_MOVE(t), val3, t3);
   }
   template <class F, class Exp1, class Exp2, class Exp3>
   void do_assign_function_3c(const F& f, const Exp1& val1, const Exp2& val2, const Exp3& val3, const detail::terminal&)
   {
      f(m_backend, function_arg_value(val1), function_arg_value(val2), function_arg_value(val3));
   }
   template <class F, class Exp1, class Exp2, class Exp3, class Tag3>
   void do_assign_function_3c(const F& f, const Exp1& val1, const Exp2& val2, const Exp3& val3, const Tag3& /*t3*/)
   {
      mp_number t(val3);
      do_assign_function_3c(f, val1, val2, BOOST_MP_MOVE(t), detail::terminal());
   }

   template <class Exp>
   void do_add(const Exp& e, const detail::terminal&)
   {
      using default_ops::eval_add;
      eval_add(m_backend, canonical_value(e.value()));
   }

   template <class Exp>
   void do_add(const Exp& e, const detail::negate&)
   {
      typedef typename Exp::left_type left_type;
      do_subtract(e.left(), typename left_type::tag_type());
   }

   template <class Exp>
   void do_add(const Exp& e, const detail::plus&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_add(e.left(), typename left_type::tag_type());
      do_add(e.right(), typename right_type::tag_type());
   }

   template <class Exp>
   void do_add(const Exp& e, const detail::minus&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_add(e.left(), typename left_type::tag_type());
      do_subtract(e.right(), typename right_type::tag_type());
   }

   template <class Exp, class unknown>
   void do_add(const Exp& e, const unknown&)
   {
      self_type temp(e);
      do_add(detail::mp_exp<detail::terminal, self_type>(temp), detail::terminal());
   }

   template <class Exp>
   void do_add(const Exp& e, const detail::add_immediates&)
   {
      using default_ops::eval_add;
      eval_add(m_backend, canonical_value(e.left().value()));
      eval_add(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_add(const Exp& e, const detail::subtract_immediates&)
   {
      using default_ops::eval_add;
      using default_ops::eval_subtract;
      eval_add(m_backend, canonical_value(e.left().value()));
      eval_subtract(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_subtract(const Exp& e, const detail::terminal&)
   {
      using default_ops::eval_subtract;
      eval_subtract(m_backend, canonical_value(e.value()));
   }

   template <class Exp>
   void do_subtract(const Exp& e, const detail::negate&)
   {
      typedef typename Exp::left_type left_type;
      do_add(e.left(), typename left_type::tag_type());
   }

   template <class Exp>
   void do_subtract(const Exp& e, const detail::plus&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_subtract(e.left(), typename left_type::tag_type());
      do_subtract(e.right(), typename right_type::tag_type());
   }

   template <class Exp>
   void do_subtract(const Exp& e, const detail::minus&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_subtract(e.left(), typename left_type::tag_type());
      do_add(e.right(), typename right_type::tag_type());
   }
   template <class Exp>
   void do_subtract(const Exp& e, const detail::add_immediates&)
   {
      using default_ops::eval_subtract;
      eval_subtract(m_backend, canonical_value(e.left().value()));
      eval_subtract(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_subtract(const Exp& e, const detail::subtract_immediates&)
   {
      using default_ops::eval_add;
      using default_ops::eval_subtract;
      eval_subtract(m_backend, canonical_value(e.left().value()));
      eval_add(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp, class unknown>
   void do_subtract(const Exp& e, const unknown&)
   {
      self_type temp(e);
      do_subtract(detail::mp_exp<detail::terminal, self_type>(temp), detail::terminal());
   }

   template <class Exp>
   void do_multiplies(const Exp& e, const detail::terminal&)
   {
      using default_ops::eval_multiply;
      eval_multiply(m_backend, canonical_value(e.value()));
   }

   template <class Exp>
   void do_multiplies(const Exp& e, const detail::negate&)
   {
      typedef typename Exp::left_type left_type;
      do_multiplies(e.left(), typename left_type::tag_type());
      m_backend.negate();
   }

   template <class Exp>
   void do_multiplies(const Exp& e, const detail::multiplies&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_multiplies(e.left(), typename left_type::tag_type());
      do_multiplies(e.right(), typename right_type::tag_type());
   }

   template <class Exp>
   void do_multiplies(const Exp& e, const detail::divides&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_multiplies(e.left(), typename left_type::tag_type());
      do_divide(e.right(), typename right_type::tag_type());
   }

   template <class Exp>
   void do_multiplies(const Exp& e, const detail::multiply_immediates&)
   {
      using default_ops::eval_multiply;
      eval_multiply(m_backend, canonical_value(e.left().value()));
      eval_multiply(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_multiplies(const Exp& e, const detail::divide_immediates&)
   {
      using default_ops::eval_multiply;
      using default_ops::eval_divide;
      eval_multiply(m_backend, canonical_value(e.left().value()));
      eval_divide(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp, class unknown>
   void do_multiplies(const Exp& e, const unknown&)
   {
      using default_ops::eval_multiply;
      self_type temp(e);
      eval_multiply(m_backend, temp.m_backend);
   }

   template <class Exp>
   void do_divide(const Exp& e, const detail::terminal&)
   {
      using default_ops::eval_divide;
      eval_divide(m_backend, canonical_value(e.value()));
   }

   template <class Exp>
   void do_divide(const Exp& e, const detail::negate&)
   {
      typedef typename Exp::left_type left_type;
      do_divide(e.left(), typename left_type::tag_type());
      m_backend.negate();
   }

   template <class Exp>
   void do_divide(const Exp& e, const detail::multiplies&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_divide(e.left(), typename left_type::tag_type());
      do_divide(e.right(), typename right_type::tag_type());
   }

   template <class Exp>
   void do_divide(const Exp& e, const detail::divides&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_divide(e.left(), typename left_type::tag_type());
      do_multiplies(e.right(), typename right_type::tag_type());
   }

   template <class Exp>
   void do_divides(const Exp& e, const detail::multiply_immediates&)
   {
      using default_ops::eval_divide;
      eval_divide(m_backend, canonical_value(e.left().value()));
      eval_divide(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_divides(const Exp& e, const detail::divide_immediates&)
   {
      using default_ops::eval_multiply;
      using default_ops::eval_divide;
      eval_divide(m_backend, canonical_value(e.left().value()));
      mutiply(m_backend, canonical_value(e.right().value()));
   }

   template <class Exp, class unknown>
   void do_divide(const Exp& e, const unknown&)
   {
      using default_ops::eval_multiply;
      self_type temp(e);
      eval_divide(m_backend, temp.m_backend);
   }

   template <class Exp>
   void do_modulus(const Exp& e, const detail::terminal&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The modulus operation is only valid for integer types");
      using default_ops::eval_modulus;
      eval_modulus(m_backend, canonical_value(e.value()));
   }

   template <class Exp, class Unknown>
   void do_modulus(const Exp& e, const Unknown&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The modulus operation is only valid for integer types");
      using default_ops::eval_modulus;
      self_type temp(e);
      eval_modulus(m_backend, canonical_value(temp));
   }

   template <class Exp>
   void do_bitwise_and(const Exp& e, const detail::terminal&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise & operation is only valid for integer types");
      using default_ops::eval_bitwise_and;
      eval_bitwise_and(m_backend, canonical_value(e.value()));
   }
   template <class Exp>
   void do_bitwise_and(const Exp& e, const detail::bitwise_and&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise & operation is only valid for integer types");
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_bitwise_and(e.left(), typename left_type::tag_type());
      do_bitwise_and(e.right(), typename right_type::tag_type());
   }
   template <class Exp, class unknown>
   void do_bitwise_and(const Exp& e, const unknown&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise & operation is only valid for integer types");
      using default_ops::eval_bitwise_and;
      self_type temp(e);
      eval_bitwise_and(m_backend, temp.m_backend);
   }

   template <class Exp>
   void do_bitwise_or(const Exp& e, const detail::terminal&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise | operation is only valid for integer types");
      using default_ops::eval_bitwise_or;
      eval_bitwise_or(m_backend, canonical_value(e.value()));
   }
   template <class Exp>
   void do_bitwise_or(const Exp& e, const detail::bitwise_or&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise | operation is only valid for integer types");
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_bitwise_or(e.left(), typename left_type::tag_type());
      do_bitwise_or(e.right(), typename right_type::tag_type());
   }
   template <class Exp, class unknown>
   void do_bitwise_or(const Exp& e, const unknown&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise | operation is only valid for integer types");
      using default_ops::eval_bitwise_or;
      self_type temp(e);
      eval_bitwise_or(m_backend, temp.m_backend);
   }

   template <class Exp>
   void do_bitwise_xor(const Exp& e, const detail::terminal&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise ^ operation is only valid for integer types");
      using default_ops::eval_bitwise_xor;
      eval_bitwise_xor(m_backend, canonical_value(e.value()));
   }
   template <class Exp>
   void do_bitwise_xor(const Exp& e, const detail::bitwise_xor&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise ^ operation is only valid for integer types");
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_bitwise_xor(e.left(), typename left_type::tag_type());
      do_bitwise_xor(e.right(), typename right_type::tag_type());
   }
   template <class Exp, class unknown>
   void do_bitwise_xor(const Exp& e, const unknown&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The bitwise ^ operation is only valid for integer types");
      using default_ops::eval_bitwise_xor;
      self_type temp(e);
      eval_bitwise_xor(m_backend, temp.m_backend);
   }

   // Tests if the expression contains a reference to *this:
   template <class Exp>
   BOOST_CONSTEXPR bool contains_self(const Exp& e)const BOOST_NOEXCEPT
   {
      return contains_self(e, typename Exp::arity());
   }
   template <class Exp>
   BOOST_CONSTEXPR bool contains_self(const Exp& e, mpl::int_<0> const&)const BOOST_NOEXCEPT
   {
      return is_realy_self(e.value());
   }
   template <class Exp>
   BOOST_CONSTEXPR bool contains_self(const Exp& e, mpl::int_<1> const&)const BOOST_NOEXCEPT
   {
      typedef typename Exp::left_type child_type;
      return contains_self(e.left(), typename child_type::arity());
   }
   template <class Exp>
   BOOST_CONSTEXPR bool contains_self(const Exp& e, mpl::int_<2> const&)const BOOST_NOEXCEPT
   {
      typedef typename Exp::left_type child0_type;
      typedef typename Exp::right_type child1_type;
      return contains_self(e.left(), typename child0_type::arity()) 
         || contains_self(e.right(), typename child1_type::arity());
   }
   template <class Exp>
   BOOST_CONSTEXPR bool contains_self(const Exp& e, mpl::int_<3> const&)const BOOST_NOEXCEPT
   {
      typedef typename Exp::left_type child0_type;
      typedef typename Exp::middle_type child1_type;
      typedef typename Exp::right_type child2_type;
      return contains_self(e.left(), typename child0_type::arity()) 
         || contains_self(e.middle(), typename child1_type::arity())
         || contains_self(e.right(), typename child2_type::arity());
   }

   // Test if the expression is a reference to *this:
   template <class Exp>
   BOOST_CONSTEXPR bool is_self(const Exp& e)const BOOST_NOEXCEPT
   {
      return is_self(e, typename Exp::arity());
   }
   template <class Exp>
   BOOST_CONSTEXPR bool is_self(const Exp& e, mpl::int_<0> const&)const BOOST_NOEXCEPT
   {
      return is_realy_self(e.value());
   }
   template <class Exp, int v>
   BOOST_CONSTEXPR bool is_self(const Exp&, mpl::int_<v> const&)const BOOST_NOEXCEPT
   {
      return false;
   }

   template <class Val>
   BOOST_CONSTEXPR bool is_realy_self(const Val&)const BOOST_NOEXCEPT{ return false; } 
   BOOST_CONSTEXPR bool is_realy_self(const self_type& v)const BOOST_NOEXCEPT{ return &v == this; } 

   static BOOST_CONSTEXPR const Backend& canonical_value(const self_type& v) BOOST_NOEXCEPT {  return v.m_backend;  }
   template <class V>
   static BOOST_CONSTEXPR typename detail::canonical<V, Backend>::type canonical_value(const V& v) BOOST_NOEXCEPT {  return static_cast<typename detail::canonical<V, Backend>::type>(v);  }
   static typename detail::canonical<std::string, Backend>::type canonical_value(const std::string& v) BOOST_NOEXCEPT {  return v.c_str();  }

   static BOOST_CONSTEXPR const Backend& function_arg_value(const self_type& v) BOOST_NOEXCEPT {  return v.backend();  }
   template <class V>
   static BOOST_CONSTEXPR const V& function_arg_value(const V& v) BOOST_NOEXCEPT {  return v;  }
   template <class A1, class A2, class A3, class A4>
   static const A1& function_arg_value(const detail::mp_exp<detail::terminal, A1, A2, A3, A4>& exp) BOOST_NOEXCEPT { return exp.value(); }
   template <class A2, class A3, class A4>
   static BOOST_CONSTEXPR const Backend& function_arg_value(const detail::mp_exp<detail::terminal, mp_number<Backend>, A2, A3, A4>& exp) BOOST_NOEXCEPT { return exp.value().backend(); }
   Backend m_backend;
};

namespace detail
{

template <class Backend, bool ExpressionTemplates>
inline int mp_number_compare(const mp_number<Backend, ExpressionTemplates>& a, const mp_number<Backend, ExpressionTemplates>& b)
{
   return a.compare(b);
}

template <class Backend, bool ExpressionTemplates, class tag, class A1, class A2, class A3, class A4>
inline int mp_number_compare(const mp_number<Backend, ExpressionTemplates>& a, const mp_exp<tag, A1, A2, A3, A4>& b)
{
   return a.compare(mp_number<Backend, ExpressionTemplates>(b));
}

template <class tag, class A1, class A2, class A3, class A4, class Backend, bool ExpressionTemplates>
inline int mp_number_compare(const mp_exp<tag, A1, A2, A3, A4>& a, const mp_number<Backend, ExpressionTemplates>& b)
{
   return -b.compare(mp_number<Backend, ExpressionTemplates>(a));
}

template <class Backend, bool ExpressionTemplates, class Val>
inline int mp_number_compare(const mp_number<Backend, ExpressionTemplates>& a, const Val b)
{
   return a.compare(b);
}

template <class Val, class Backend, bool ExpressionTemplates>
inline int mp_number_compare(const Val a, const mp_number<Backend, ExpressionTemplates>& b)
{
   return -b.compare(a);
}

template <class tag, class A1, class A2, class A3, class A4, class tag2, class A1b, class A2b, class A3b>
inline int mp_number_compare(const mp_exp<tag, A1, A2, A3, A4>& a, const mp_exp<tag2, A1b, A2b, A3b>& b)
{
   typedef typename mp_exp<tag, A1, A2, A3, A4>::result_type real1;
   typedef typename mp_exp<tag2, A1b, A2b, A3b>::result_type real2;
   return real1(a).compare(real2(b));
}

template <class tag, class A1, class A2, class A3, class A4, class Val>
inline typename enable_if<is_arithmetic<Val>, int>::type mp_number_compare(const mp_exp<tag, A1, A2, A3, A4>& a, const Val b)
{
   typedef typename mp_exp<tag, A1, A2, A3, A4>::result_type real;
   real t(a);
   return t.compare(b);
}

template <class Val, class tag, class A1, class A2, class A3, class A4>
inline typename enable_if<is_arithmetic<Val>, int>::type mp_number_compare(const Val a, const mp_exp<tag, A1, A2, A3, A4>& b)
{
   typedef typename mp_exp<tag, A1, A2, A3, A4>::result_type real;
   return -real(b).compare(a);
}

template <class Exp1, class Exp2>
struct is_valid_comparison_imp
{
   typedef typename mpl::or_<
      is_mp_number<Exp1>,
      is_mp_number_exp<Exp1>
   >::type is1;
   typedef typename mpl::or_<
      is_mp_number<Exp2>,
      is_mp_number_exp<Exp2>
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
struct is_valid_comparison : public boost::multiprecision::detail::is_valid_comparison_imp<Exp1, Exp2>::type {};

}


template <class Exp1, class Exp2>
inline typename boost::enable_if<detail::is_valid_comparison<Exp1, Exp2>, bool>::type 
   operator == (const Exp1& a, const Exp2& b)
{
   return 0 == detail::mp_number_compare(a, b);
}

template <class Exp1, class Exp2>
inline typename boost::enable_if<detail::is_valid_comparison<Exp1, Exp2>, bool>::type 
   operator != (const Exp1& a, const Exp2& b)
{
   return 0 != detail::mp_number_compare(a, b);
}

template <class Exp1, class Exp2>
inline typename boost::enable_if<detail::is_valid_comparison<Exp1, Exp2>, bool>::type 
   operator <= (const Exp1& a, const Exp2& b)
{
   return 0 >= detail::mp_number_compare(a, b);
}

template <class Exp1, class Exp2>
inline typename boost::enable_if<detail::is_valid_comparison<Exp1, Exp2>, bool>::type 
   operator < (const Exp1& a, const Exp2& b)
{
   return 0 > detail::mp_number_compare(a, b);
}

template <class Exp1, class Exp2>
inline typename boost::enable_if<detail::is_valid_comparison<Exp1, Exp2>, bool>::type 
   operator >= (const Exp1& a, const Exp2& b)
{
   return 0 <= detail::mp_number_compare(a, b);
}

template <class Exp1, class Exp2>
inline typename boost::enable_if<detail::is_valid_comparison<Exp1, Exp2>, bool>::type 
   operator > (const Exp1& a, const Exp2& b)
{
   return 0 < detail::mp_number_compare(a, b);
}

template <class Backend, bool ExpressionTemplates>
inline std::ostream& operator << (std::ostream& os, const mp_number<Backend, ExpressionTemplates>& r)
{
   std::streamsize d = os.precision();
   std::string s = r.str(d, os.flags());
   std::streamsize ss = os.width();
   if(ss > static_cast<std::streamsize>(s.size()))
   {
      char fill = os.fill();
      if((os.flags() & std::ios_base::left) == std::ios_base::left)
         s.append(static_cast<std::string::size_type>(ss - s.size()), fill);
      else
         s.insert(0, static_cast<std::string::size_type>(ss - s.size()), fill);
   }
   return os << s;
}

namespace detail{

template <class tag, class A1, class A2, class A3, class A4>
inline std::ostream& operator << (std::ostream& os, const mp_exp<tag, A1, A2, A3, A4>& r)
{
   typedef typename mp_exp<tag, A1, A2, A3, A4>::result_type value_type;
   value_type temp(r);
   return os << temp;
}

} // namespace detail

template <class Backend, bool ExpressionTemplates>
inline std::istream& operator >> (std::istream& is, mp_number<Backend, ExpressionTemplates>& r)
{
   std::string s;
   is >> s;
   r = s;
   return is;
}

template <class Backend, bool ExpressionTemplates>
inline void swap(mp_number<Backend, ExpressionTemplates>& a, mp_number<Backend, ExpressionTemplates>& b)
{
   a.swap(b);
}

}  // namespace multipreciion

template <class T>
class rational;

template <class Backend, bool ExpressionTemplates>
inline std::istream& operator >> (std::istream& is, rational<multiprecision::mp_number<Backend, ExpressionTemplates> >& r)
{
   std::string s1;
   multiprecision::mp_number<Backend, ExpressionTemplates> v1, v2;
   char c;
   bool have_hex = false;

   while((EOF != (c = static_cast<char>(is.peek()))) && (c == 'x' || c == 'X' || c == '-' || c == '+' || (c >= '0' && c <= '9') || (have_hex && (c >= 'a' && c <= 'f')) || (have_hex && (c >= 'A' && c <= 'F'))))
   {
      if(c == 'x' || c == 'X')
         have_hex = true;
      s1.append(1, c);
      is.get();
   }
   v1 = s1;
   s1.erase();
   if(c == '/')
   {
      is.get();
      while((EOF != (c = static_cast<char>(is.peek()))) && (c == 'x' || c == 'X' || c == '-' || c == '+' || (c >= '0' && c <= '9') || (have_hex && (c >= 'a' && c <= 'f')) || (have_hex && (c >= 'A' && c <= 'F'))))
      {
         if(c == 'x' || c == 'X')
            have_hex = true;
         s1.append(1, c);
         is.get();
      }
      v2 = s1;
   }
   else
      v2 = 1;
   r.assign(v1, v2);
   return is;
}

template <class T, bool ExpressionTemplates, class Arithmetic>
typename enable_if<boost::is_arithmetic<Arithmetic>, bool>::type operator == (const rational<multiprecision::mp_number<T, ExpressionTemplates> >& a, const Arithmetic& b)
{
   return a == multiprecision::mp_number<T, ExpressionTemplates>(b);
}

template <class T, bool ExpressionTemplates, class Arithmetic>
typename enable_if<boost::is_arithmetic<Arithmetic>, bool>::type operator == (const Arithmetic& b, const rational<multiprecision::mp_number<T, ExpressionTemplates> >& a)
{
   return a == multiprecision::mp_number<T, ExpressionTemplates>(b);
}

template <class T, bool ExpressionTemplates, class Arithmetic>
typename enable_if<boost::is_arithmetic<Arithmetic>, bool>::type operator != (const rational<multiprecision::mp_number<T, ExpressionTemplates> >& a, const Arithmetic& b)
{
   return a != multiprecision::mp_number<T, ExpressionTemplates>(b);
}

template <class T, bool ExpressionTemplates, class Arithmetic>
typename enable_if<boost::is_arithmetic<Arithmetic>, bool>::type operator != (const Arithmetic& b, const rational<multiprecision::mp_number<T, ExpressionTemplates> >& a)
{
   return a != multiprecision::mp_number<T, ExpressionTemplates>(b);
}

template <class T, bool ExpressionTemplates>
inline multiprecision::mp_number<T, ExpressionTemplates> numerator(const rational<multiprecision::mp_number<T, ExpressionTemplates> >& a)
{
   return a.numerator();
}

template <class T, bool ExpressionTemplates>
inline multiprecision::mp_number<T, ExpressionTemplates> denominator(const rational<multiprecision::mp_number<T, ExpressionTemplates> >& a)
{
   return a.denominator();
}

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

} // namespaces

#endif
