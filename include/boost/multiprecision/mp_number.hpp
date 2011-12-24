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
#include <boost/multiprecision/detail/default_ops.hpp>

namespace boost{ namespace multiprecision{

template <class Backend>
class mp_number
{
   typedef mp_number<Backend> self_type;
public:
   typedef Backend backend_type;
   mp_number(){}
   mp_number(const mp_number& e) : m_backend(e.m_backend){}
   template <class V>
   mp_number(V v, typename enable_if<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> > >::type* dummy1 = 0)
   {
      m_backend = canonical_value(v);
   }
   mp_number(const mp_number& e, unsigned digits10) : m_backend(e.m_backend, digits10){}
   template <class V>
   mp_number(V v, unsigned digits10, typename enable_if<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> > >::type* dummy1 = 0)
   {
      m_backend.precision(digits10);
      m_backend = canonical_value(v);
   }
   template <class Other>
   mp_number(const mp_number<Other>& val, typename enable_if<boost::is_convertible<Other, Backend> >::type* dummy1 = 0)
   {
      m_backend = val.backend();
   }

   template <class V>
   mp_number(V v, typename enable_if<mpl::and_<is_convertible<V, Backend>, mpl::not_<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> > > > >::type* dummy1 = 0)
      : m_backend(v){}

   template <class tag, class Arg1, class Arg2, class Arg3>
   mp_number& operator=(const detail::mp_exp<tag, Arg1, Arg2, Arg3>& e)
   {
      do_assign(e, tag());
      return *this;
   }

   mp_number& operator=(const mp_number& e)
   {
      m_backend = e.m_backend;
      return *this;
   }

   template <class V>
   typename enable_if<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> >, mp_number<Backend>& >::type 
      operator=(const V& v)
   {
      m_backend = canonical_value(v);
      return *this;
   }

   template <class V>
   typename enable_if<mpl::and_<is_convertible<V, Backend>, mpl::not_<mpl::or_<boost::is_arithmetic<V>, is_same<std::string, V>, is_convertible<V, const char*> > > >, mp_number<Backend>& >::type 
      operator=(const V& v)
   {
      m_backend = v;
      return *this;
   }

   template <class Other>
   typename enable_if<is_convertible<Other, Backend>, mp_number<Backend>& >::type 
      operator=(const mp_number<Other>& v)
   {
      m_backend = v.backend();
      return *this;
   }

   template <class tag, class Arg1, class Arg2, class Arg3>
   mp_number(const detail::mp_exp<tag, Arg1, Arg2, Arg3>& e)
   {
      do_assign(e, tag());
   }

#ifndef BOOST_NO_RVALUE_REFERENCES
   mp_number(mp_number&& r) : m_backend(r.m_backend){}
   mp_number& operator=(mp_number&& r)
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

   template <class tag, class Arg1, class Arg2, class Arg3>
   mp_number& operator+=(const detail::mp_exp<tag, Arg1, Arg2, Arg3>& e)
   {
      // Create a copy if e contains this, but not if we're just doing a
      //    x *= x
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
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend>& >::type 
      operator+=(const V& v)
   {
      using default_ops::add;
      add(m_backend, canonical_value(v));
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
      // Create a copy if e contains this, but not if we're just doing a
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
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend>& >::type 
      operator-=(const V& v)
   {
      using default_ops::subtract;
      subtract(m_backend, canonical_value(v));
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
      // if we're just doing an   x += x;
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
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend>& >::type 
      operator*=(const V& v)
   {
      using default_ops::multiply;
      multiply(m_backend, canonical_value(v));
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
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend>& >::type 
      operator%=(const V& v)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The modulus operation is only valid for integer types");
      using default_ops::modulus;
      modulus(m_backend, canonical_value(v));
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
      using default_ops::increment;
      increment(m_backend);
      return *this;
   }

   mp_number& operator--()
   {
      using default_ops::decrement;
      decrement(m_backend);
      return *this;
   }

   mp_number operator++(int)
   {
      using default_ops::increment;
      self_type temp(*this);
      increment(m_backend);
      return temp;
   }

   mp_number operator--(int)
   {
      using default_ops::decrement;
      self_type temp(*this);
      decrement(m_backend);
      return temp;
   }

   template <class V>
   typename enable_if<is_integral<V>, mp_number&>::type operator <<= (V val)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The left-shift operation is only valid for integer types");
      check_shift_range(val, mpl::bool_<(sizeof(V) > sizeof(std::size_t))>(), is_signed<V>());
      left_shift(m_backend, canonical_value(val));
      return *this;
   }

   template <class V>
   typename enable_if<is_integral<V>, mp_number&>::type operator >>= (V val)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The right-shift operation is only valid for integer types");
      check_shift_range(val, mpl::bool_<(sizeof(V) > sizeof(std::size_t))>(), is_signed<V>());
      right_shift(m_backend, canonical_value(val));
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
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend>& >::type 
      operator/=(const V& v)
   {
      using default_ops::divide;
      divide(m_backend, canonical_value(v));
      return *this;
   }

   mp_number& operator&=(const self_type& e)
   {
      do_bitwise_and(detail::mp_exp<detail::terminal, self_type>(e), detail::terminal());
      return *this;
   }

   template <class Exp>
   mp_number& operator&=(const detail::mp_exp<Exp>& e)
   {
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
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend>& >::type 
      operator&=(const V& v)
   {
      using default_ops::bitwise_and;
      bitwise_and(m_backend, canonical_value(v));
      return *this;
   }

   mp_number& operator|=(const self_type& e)
   {
      do_bitwise_or(detail::mp_exp<detail::terminal, self_type>(e), detail::terminal());
      return *this;
   }

   template <class Exp>
   mp_number& operator|=(const detail::mp_exp<Exp>& e)
   {
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
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend>& >::type 
      operator|=(const V& v)
   {
      using default_ops::bitwise_or;
      bitwise_or(m_backend, canonical_value(v));
      return *this;
   }

   mp_number& operator^=(const self_type& e)
   {
      do_bitwise_xor(detail::mp_exp<detail::terminal, self_type>(e), detail::terminal());
      return *this;
   }

   template <class Exp>
   mp_number& operator^=(const detail::mp_exp<Exp>& e)
   {
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
   typename enable_if<boost::is_arithmetic<V>, mp_number<Backend>& >::type 
      operator^=(const V& v)
   {
      using default_ops::bitwise_xor;
      bitwise_xor(m_backend, canonical_value(v));
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
   void swap(self_type& other)
   {
      m_backend.swap(other.backend());
   }
   //
   // Zero and sign:
   //
   bool is_zero()const
   {
      using default_ops::is_zero;
      return is_zero(m_backend);
   }
   int sign()const
   {
      using default_ops::get_sign;
      return get_sign(m_backend);
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
      using default_ops::convert_to;
      T result;
      convert_to(&result, m_backend);
      return result;
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
   int compare(const mp_number<Backend>& o)const
   {
      return m_backend.compare(o.m_backend);
   }
   template <class V>
   typename enable_if<is_arithmetic<V>, int>::type compare(const V& o)const
   {
      using default_ops::get_sign;
      if(o == 0)
         return get_sign(m_backend);
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
   void check_shift_range(V val, const mpl::false_&, const mpl::false_&){}

   template <class Exp>
   void do_assign(const Exp& e, const detail::add_immediates&)
   {
      using default_ops::add;
      add(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
   }
/*
   template <class Exp>
   void do_assign(const Exp& e, const detail::add_and_negate_immediates&)
   {
      using default_ops::add;
      add(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
      m_backend.negate();
   }
*/
   template <class Exp>
   void do_assign(const Exp& e, const detail::subtract_immediates&)
   {
      using default_ops::subtract;
      subtract(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_assign(const Exp& e, const detail::multiply_immediates&)
   {
      using default_ops::multiply;
      multiply(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::divide_immediates&)
   {
      using default_ops::divide;
      divide(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
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
      using default_ops::modulus;
      modulus(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
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
      using default_ops::bitwise_and;
      bitwise_and(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
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
      using default_ops::bitwise_or;
      bitwise_or(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
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
      using default_ops::bitwise_xor;
      bitwise_xor(m_backend, canonical_value(e.left().value()), canonical_value(e.right().value()));
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
      using default_ops::complement;
      self_type temp(e.left());
      complement(m_backend, temp.backend());
   }

   template <class Exp>
   void do_assign(const Exp& e, const detail::complement_immediates&)
   {
      using default_ops::complement;
      complement(m_backend, canonical_value(e.left().value()));
   }

   template <class Exp, class Val>
   void do_assign_right_shift(const Exp& e, const Val& val, const detail::terminal&)
   {
      using default_ops::right_shift;
      right_shift(m_backend, canonical_value(e.value()), val);
   }

   template <class Exp, class Val>
   void do_assign_left_shift(const Exp& e, const Val& val, const detail::terminal&)
   {
      using default_ops::left_shift;
      left_shift(m_backend, canonical_value(e.value()), val);
   }

   template <class Exp, class Val, class Tag>
   void do_assign_right_shift(const Exp& e, const Val& val, const Tag&)
   {
      using default_ops::right_shift;
      self_type temp(e);
      right_shift(m_backend, temp.backend(), val);
   }

   template <class Exp, class Val, class Tag>
   void do_assign_left_shift(const Exp& e, const Val& val, const Tag&)
   {
      using default_ops::left_shift;
      self_type temp(e);
      left_shift(m_backend, temp.backend(), val);
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
      do_assign_function_1(e.left().value(), e.right(), tag_type());
   }
   template <class F, class Exp>
   void do_assign_function_1(const F& f, const Exp& val, const detail::terminal&)
   {
      f(m_backend, function_arg_value(val.value()));
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
      do_assign_function_2(e.left().value(), e.middle(), e.right(), tag_type(), end_tag());
   }
   template <class F, class Exp1, class Exp2>
   void do_assign_function_2(const F& f, const Exp1& val1, const Exp2& val2, const detail::terminal&, const detail::terminal&)
   {
      f(m_backend, function_arg_value(val1.value()), function_arg_value(val2.value()));
   }
   template <class F, class Exp1, class Exp2, class Tag1>
   void do_assign_function_2(const F& f, const Exp1& val1, const Exp2& val2, const Tag1&, const detail::terminal&)
   {
      self_type temp1(val1);
      f(m_backend, temp1.backend(), function_arg_value(val2.value()));
   }
   template <class F, class Exp1, class Exp2, class Tag2>
   void do_assign_function_2(const F& f, const Exp1& val1, const Exp2& val2, const detail::terminal&, const Tag2&)
   {
      self_type temp2(val2);
      f(m_backend, function_arg_value(val1.value()), temp2.backend());
   }
   template <class F, class Exp1, class Exp2, class Tag1, class Tag2>
   void do_assign_function_2(const F& f, const Exp1& val1, const Exp2& val2, const Tag1&, const Tag2&)
   {
      self_type temp1(val1);
      self_type temp2(val2);
      f(m_backend, temp1.backend(), temp2.backend());
   }

   template <class Exp>
   void do_add(const Exp& e, const detail::terminal&)
   {
      using default_ops::add;
      add(m_backend, canonical_value(e.value()));
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
      using default_ops::add;
      add(m_backend, canonical_value(e.left().value()));
      add(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_add(const Exp& e, const detail::subtract_immediates&)
   {
      using default_ops::add;
      using default_ops::subtract;
      add(m_backend, canonical_value(e.left().value()));
      subtract(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_subtract(const Exp& e, const detail::terminal&)
   {
      using default_ops::subtract;
      subtract(m_backend, canonical_value(e.value()));
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
      using default_ops::subtract;
      subtract(m_backend, canonical_value(e.left().value()));
      subtract(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_subtract(const Exp& e, const detail::subtract_immediates&)
   {
      using default_ops::add;
      using default_ops::subtract;
      subtract(m_backend, canonical_value(e.left().value()));
      add(m_backend, canonical_value(e.right().value()));
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
      using default_ops::multiply;
      multiply(m_backend, canonical_value(e.value()));
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
      using default_ops::multiply;
      multiply(m_backend, canonical_value(e.left().value()));
      multiply(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_multiplies(const Exp& e, const detail::divide_immediates&)
   {
      using default_ops::multiply;
      using default_ops::divide;
      multiply(m_backend, canonical_value(e.left().value()));
      divide(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp, class unknown>
   void do_multiplies(const Exp& e, const unknown&)
   {
      using default_ops::multiply;
      self_type temp(e);
      multiply(m_backend, temp.m_backend);
   }

   template <class Exp>
   void do_divide(const Exp& e, const detail::terminal&)
   {
      using default_ops::divide;
      divide(m_backend, canonical_value(e.value()));
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
      using default_ops::divide;
      divide(m_backend, canonical_value(e.left().value()));
      divide(m_backend, canonical_value(e.right().value()));
   }
   template <class Exp>
   void do_divides(const Exp& e, const detail::divide_immediates&)
   {
      using default_ops::multiply;
      using default_ops::divide;
      divide(m_backend, canonical_value(e.left().value()));
      mutiply(m_backend, canonical_value(e.right().value()));
   }

   template <class Exp, class unknown>
   void do_divide(const Exp& e, const unknown&)
   {
      using default_ops::multiply;
      self_type temp(e);
      divide(m_backend, temp.m_backend);
   }

   template <class Exp>
   void do_modulus(const Exp& e, const detail::terminal&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The modulus operation is only valid for integer types");
      using default_ops::modulus;
      modulus(m_backend, canonical_value(e.value()));
   }

   template <class Exp, class Unknown>
   void do_modulus(const Exp& e, const Unknown&)
   {
      BOOST_STATIC_ASSERT_MSG(number_category<Backend>::value == number_kind_integer, "The modulus operation is only valid for integer types");
      using default_ops::modulus;
      self_type temp(e);
      modulus(m_backend, canonical_value(temp));
   }

   template <class Exp>
   void do_bitwise_and(const Exp& e, const detail::terminal&)
   {
      using default_ops::bitwise_and;
      bitwise_and(m_backend, canonical_value(e.value()));
   }
   template <class Exp>
   void do_bitwise_and(const Exp& e, const detail::bitwise_and&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_bitwise_and(e.left(), typename left_type::tag_type());
      do_bitwise_and(e.right(), typename right_type::tag_type());
   }
   template <class Exp, class unknown>
   void do_bitwise_and(const Exp& e, const unknown&)
   {
      using default_ops::bitwise_and;
      self_type temp(e);
      bitwise_and(m_backend, temp.m_backend);
   }

   template <class Exp>
   void do_bitwise_or(const Exp& e, const detail::terminal&)
   {
      using default_ops::bitwise_or;
      bitwise_or(m_backend, canonical_value(e.value()));
   }
   template <class Exp>
   void do_bitwise_or(const Exp& e, const detail::bitwise_or&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_bitwise_or(e.left(), typename left_type::tag_type());
      do_bitwise_or(e.right(), typename right_type::tag_type());
   }
   template <class Exp, class unknown>
   void do_bitwise_or(const Exp& e, const unknown&)
   {
      using default_ops::bitwise_or;
      self_type temp(e);
      bitwise_or(m_backend, temp.m_backend);
   }

   template <class Exp>
   void do_bitwise_xor(const Exp& e, const detail::terminal&)
   {
      using default_ops::bitwise_xor;
      bitwise_xor(m_backend, canonical_value(e.value()));
   }
   template <class Exp>
   void do_bitwise_xor(const Exp& e, const detail::bitwise_xor&)
   {
      typedef typename Exp::left_type left_type;
      typedef typename Exp::right_type right_type;
      do_bitwise_xor(e.left(), typename left_type::tag_type());
      do_bitwise_xor(e.right(), typename right_type::tag_type());
   }
   template <class Exp, class unknown>
   void do_bitwise_xor(const Exp& e, const unknown&)
   {
      using default_ops::bitwise_xor;
      self_type temp(e);
      bitwise_xor(m_backend, temp.m_backend);
   }

   // Tests if the expression contains a reference to *this:
   template <class Exp>
   bool contains_self(const Exp& e)const
   {
      return contains_self(e, typename Exp::arity());
   }
   template <class Exp>
   bool contains_self(const Exp& e, mpl::int_<0> const&)const
   {
      return is_floatly_self(e.value());
   }
   template <class Exp>
   bool contains_self(const Exp& e, mpl::int_<1> const&)const
   {
      typedef typename Exp::left_type child_type;
      return contains_self(e.left(), typename child_type::arity());
   }
   template <class Exp>
   bool contains_self(const Exp& e, mpl::int_<2> const&)const
   {
      typedef typename Exp::left_type child0_type;
      typedef typename Exp::right_type child1_type;
      return contains_self(e.left(), typename child0_type::arity()) 
         || contains_self(e.right(), typename child1_type::arity());
   }
   template <class Exp>
   bool contains_self(const Exp& e, mpl::int_<3> const&)const
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
   bool is_self(const Exp& e)const
   {
      return is_self(e, typename Exp::arity());
   }
   template <class Exp>
   bool is_self(const Exp& e, mpl::int_<0> const&)const
   {
      return is_floatly_self(e.value());
   }
   template <class Exp, int v>
   bool is_self(const Exp& e, mpl::int_<v> const&)const
   {
      return false;
   }

   template <class Val>
   bool is_floatly_self(const Val&)const { return false; }
   bool is_floatly_self(const self_type& v)const { return &v == this; }

   static const Backend& canonical_value(const self_type& v){  return v.m_backend;  }
   template <class V>
   static typename detail::canonical<V, Backend>::type canonical_value(const V& v){  return v;  }
   static typename detail::canonical<std::string, Backend>::type canonical_value(const std::string& v){  return v.c_str();  }

   static const Backend& function_arg_value(const self_type& v) {  return v.backend();  }
   template <class V>
   static const V& function_arg_value(const V& v) {  return v;  }
   Backend m_backend;
};

namespace detail
{

template <class Backend>
inline int mp_number_compare(const mp_number<Backend>& a, const mp_number<Backend>& b)
{
   return a.compare(b);
}

template <class Backend, class tag, class A1, class A2, class A3>
inline int mp_number_compare(const mp_number<Backend>& a, const mp_exp<tag, A1, A2, A3>& b)
{
   return a.compare(mp_number<Backend>(b));
}

template <class tag, class A1, class A2, class A3, class Backend>
inline int mp_number_compare(const mp_exp<tag, A1, A2, A3>& a, const mp_number<Backend>& b)
{
   return -b.compare(mp_number<Backend>(a));
}

template <class Backend, class Val>
inline int mp_number_compare(const mp_number<Backend>& a, const Val b)
{
   return a.compare(b);
}

template <class Val, class Backend>
inline int mp_number_compare(const Val a, const mp_number<Backend>& b)
{
   return -b.compare(a);
}

template <class tag, class A1, class A2, class A3, class tag2, class A1b, class A2b, class A3b>
inline int mp_number_compare(const mp_exp<tag, A1, A2, A3>& a, const mp_exp<tag2, A1b, A2b, A3b>& b)
{
   typedef typename mp_exp<tag, A1, A2, A3>::result_type real1;
   typedef typename mp_exp<tag2, A1b, A2b, A3b>::result_type real2;
   return real1(a).compare(real2(b));
}

template <class tag, class A1, class A2, class A3, class Val>
inline typename enable_if<is_arithmetic<Val>, int>::type mp_number_compare(const mp_exp<tag, A1, A2, A3>& a, const Val b)
{
   typedef typename mp_exp<tag, A1, A2, A3>::result_type real;
   real t(a);
   return t.compare(b);
}

template <class Val, class tag, class A1, class A2, class A3>
inline typename enable_if<is_arithmetic<Val>, int>::type mp_number_compare(const Val a, const mp_exp<tag, A1, A2, A3>& b)
{
   typedef typename mp_exp<tag, A1, A2, A3>::result_type real;
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

template <class Backend>
inline std::ostream& operator << (std::ostream& os, const mp_number<Backend>& r)
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

template <class tag, class A1, class A2, class A3>
inline std::ostream& operator << (std::ostream& os, const mp_exp<tag, A1, A2, A3>& r)
{
   typedef typename mp_exp<tag, A1, A2, A3>::result_type value_type;
   value_type temp(r);
   return os << temp;
}

} // namespace detail

template <class Backend>
inline std::istream& operator >> (std::istream& is, mp_number<Backend>& r)
{
   std::string s;
   is >> s;
   r = s;
   return is;
}

template <class Backend>
inline void swap(mp_number<Backend>& a, mp_number<Backend>& b)
{
   a.swap(b);
}

}  // namespace multipreciion

template <class T>
class rational;

template <class Backend>
inline std::istream& operator >> (std::istream& is, rational<multiprecision::mp_number<Backend> >& r)
{
   std::string s1;
   multiprecision::mp_number<Backend> v1, v2;
   char c;
   bool have_hex = false;

   while((EOF != (c = is.peek())) && (c == 'x' || c == 'X' || c == '-' || c == '+' || (c >= '0' && c <= '9') || (have_hex && (c >= 'a' && c <= 'f')) || (have_hex && (c >= 'A' && c <= 'F'))))
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
      while((EOF != (c = is.peek())) && (c == 'x' || c == 'X' || c == '-' || c == '+' || (c >= '0' && c <= '9') || (have_hex && (c >= 'a' && c <= 'f')) || (have_hex && (c >= 'A' && c <= 'F'))))
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

template <class T, class Arithmetic>
typename enable_if<boost::is_arithmetic<Arithmetic>, bool>::type operator == (const rational<multiprecision::mp_number<T> >& a, const Arithmetic& b)
{
   return a == multiprecision::mp_number<T>(b);
}

template <class T, class Arithmetic>
typename enable_if<boost::is_arithmetic<Arithmetic>, bool>::type operator == (const Arithmetic& b, const rational<multiprecision::mp_number<T> >& a)
{
   return a == multiprecision::mp_number<T>(b);
}

template <class T, class Arithmetic>
typename enable_if<boost::is_arithmetic<Arithmetic>, bool>::type operator != (const rational<multiprecision::mp_number<T> >& a, const Arithmetic& b)
{
   return a != multiprecision::mp_number<T>(b);
}

template <class T, class Arithmetic>
typename enable_if<boost::is_arithmetic<Arithmetic>, bool>::type operator != (const Arithmetic& b, const rational<multiprecision::mp_number<T> >& a)
{
   return a != multiprecision::mp_number<T>(b);
}

} // namespaces

#endif
