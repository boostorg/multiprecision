///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MP_CPP_INT_HPP
#define BOOST_MP_CPP_INT_HPP

#include <iostream>
#include <iomanip>
#include <boost/cstdint.hpp>
#include <boost/multiprecision/mp_number.hpp>
#include <boost/multiprecision/integer_ops.hpp>
#include <boost/array.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/multiprecision/detail/cpp_int_core.hpp>
#include <boost/multiprecision/rational_adapter.hpp>
#include <boost/detail/endian.hpp>

namespace boost{
namespace multiprecision{
namespace backends{

#ifdef BOOST_MSVC
// warning C4127: conditional expression is constant
#pragma warning(push)
#pragma warning(disable:4127 4351)
#endif

template <unsigned MinBits = 0, bool Signed = true, class Allocator = std::allocator<limb_type>, bool trivial = is_void<Allocator>::value && (MinBits <= sizeof(double_limb_type) * CHAR_BIT) >
struct cpp_int_backend;

template <unsigned MinBits = 0, bool Signed = true, class Allocator = std::allocator<limb_type>, bool trivial = false>
struct cpp_int_base;

template <class T>
struct is_trivial_cpp_int;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial>
struct is_trivial_cpp_int<cpp_int_backend<MinBits, Signed, Allocator, trivial> >
{
   typedef mpl::bool_<trivial> type;
};

template <unsigned MinBits, bool Signed, class Allocator>
struct cpp_int_base<MinBits, Signed, Allocator> : private Allocator::template rebind<limb_type>::other
{
   typedef typename Allocator::template rebind<limb_type>::other allocator_type;
   typedef typename allocator_type::pointer                      limb_pointer;
   typedef typename allocator_type::const_pointer                const_limb_pointer;

   //
   // Interface invariants:
   //
   BOOST_STATIC_ASSERT(!is_void<Allocator>::value);

private:
   struct limb_data
   {
      unsigned capacity;
      limb_pointer data;
   };

public:
   BOOST_STATIC_CONSTANT(unsigned, limb_bits = sizeof(limb_type) * CHAR_BIT);
   BOOST_STATIC_CONSTANT(limb_type, max_limb_value = ~static_cast<limb_type>(0u));
   BOOST_STATIC_CONSTANT(limb_type, sign_bit_mask = 1u << (limb_bits - 1));
   BOOST_STATIC_CONSTANT(unsigned, internal_limb_count = 
      MinBits 
         ? MinBits / limb_bits + (MinBits % limb_bits ? 1 : 0) 
         : sizeof(limb_data) / sizeof(limb_type));
   BOOST_STATIC_CONSTANT(bool, variable = true);

   BOOST_STATIC_ASSERT_MSG(Signed, "There is curently no support for unsigned arbitrary precision integers.");

private:
   union data_type
   {
      limb_data ld;
      limb_type la[internal_limb_count];
      limb_type first;
      double_limb_type double_first;

      BOOST_CONSTEXPR data_type() : first(0) {}
      BOOST_CONSTEXPR data_type(limb_type i) : first(i) {}
      BOOST_CONSTEXPR data_type(signed_limb_type i) : first(i < 0 ? -i : i) {}
#ifdef BOOST_LITTLE_ENDIAN
      BOOST_CONSTEXPR data_type(double_limb_type i) : double_first(i) {}
      BOOST_CONSTEXPR data_type(signed_double_limb_type i) : double_first(i < 0 ? -i : i) {}
#endif
   };

   data_type   m_data;
   unsigned    m_limbs;
   bool        m_sign, m_internal;

public:
   //
   // Direct construction:
   //
   BOOST_CONSTEXPR cpp_int_base(limb_type i)BOOST_NOEXCEPT 
      : m_data(i), m_limbs(1), m_sign(false), m_internal(true) { }
   BOOST_CONSTEXPR cpp_int_base(signed_limb_type i)BOOST_NOEXCEPT 
      : m_data(i), m_limbs(1), m_sign(i < 0), m_internal(true) { }
#if defined(BOOST_LITTLE_ENDIAN)
   BOOST_CONSTEXPR cpp_int_base(double_limb_type i)BOOST_NOEXCEPT 
      : m_data(i), m_limbs(i > max_limb_value ? 2 : 1), m_sign(false), m_internal(true) { }
   BOOST_CONSTEXPR cpp_int_base(signed_double_limb_type i)BOOST_NOEXCEPT 
      : m_data(i), m_limbs(i < 0 ? (-i > max_limb_value ? 2 : 1) : (i > max_limb_value ? 2 : 1)), m_sign(i < 0), m_internal(true) { }
#endif
   //
   // Helper functions for getting at our internal data, and manipulating storage:
   //
   allocator_type& allocator() BOOST_NOEXCEPT { return *this; }
   const allocator_type& allocator()const BOOST_NOEXCEPT { return *this; }
   unsigned size()const  BOOST_NOEXCEPT { return m_limbs; }
   limb_pointer limbs()  BOOST_NOEXCEPT { return m_internal ? m_data.la : m_data.ld.data; }
   const_limb_pointer limbs()const  BOOST_NOEXCEPT { return m_internal ? m_data.la : m_data.ld.data; }
   unsigned capacity()const  BOOST_NOEXCEPT { return m_internal ? internal_limb_count : m_data.ld.capacity; }
   bool sign()const  BOOST_NOEXCEPT { return m_sign; }
   void sign(bool b)  BOOST_NOEXCEPT 
   { 
      m_sign = b; 
      // Check for zero value:
      if(m_sign && (m_limbs == 1))
      {
         if(limbs()[0] == 0)
            m_sign = false;
      }
   }
   void resize(unsigned new_size)
   {
      unsigned cap = capacity();
      if(new_size > cap)
      {
         cap = (std::max)(cap * 4, new_size);
         limb_pointer pl = allocator().allocate(cap);
         std::copy(limbs(), limbs() + size(), pl);
         if(!m_internal)
            allocator().deallocate(limbs(), capacity());
         else
            m_internal = false;
         m_limbs = new_size;
         m_data.ld.capacity = cap;
         m_data.ld.data = pl;
      }
      else
      {
         m_limbs = new_size;
      }
   }
   void normalize() BOOST_NOEXCEPT
   {
      limb_pointer p = limbs();
      while((m_limbs-1) && !p[m_limbs - 1])--m_limbs;
   }
   BOOST_CONSTEXPR cpp_int_base() BOOST_NOEXCEPT : m_data(), m_limbs(1), m_sign(false), m_internal(true) {}
   cpp_int_base(const cpp_int_base& o) : allocator_type(o), m_limbs(0), m_internal(true)
   {
      resize(o.size());
      std::copy(o.limbs(), o.limbs() + o.size(), limbs());
      m_sign = o.m_sign;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   cpp_int_base(cpp_int_base&& o) BOOST_NOEXCEPT : allocator_type(static_cast<Allocator&&>(o)), m_limbs(o.m_limbs), m_sign(o.m_sign), m_internal(o.m_internal) 
   {
      if(m_internal)
      {
         std::copy(o.limbs(), o.limbs() + o.size(), limbs());
      }
      else
      {
         m_data.ld = o.m_data.ld;
         o.m_limbs = 0;
         o.m_internal = true;
      }
   }
   cpp_int_base& operator = (cpp_int_base&& o) BOOST_NOEXCEPT
   {
      if(!m_internal)
         allocator().deallocate(m_data.ld.data, m_data.ld.capacity);
      *static_cast<Allocator*>(this) = static_cast<Allocator&&>(o);
      m_limbs = o.m_limbs;
      m_sign = o.m_sign;
      m_internal = o.m_internal;
      if(m_internal)
      {
         std::copy(o.limbs(), o.limbs() + o.size(), limbs());
      }
      else
      {
         m_data.ld = o.m_data.ld;
         o.m_limbs = 0;
         o.m_internal = true;
      }
      return *this;
   }
#endif
   ~cpp_int_base() BOOST_NOEXCEPT
   {
      if(!m_internal)
         allocator().deallocate(limbs(), capacity());
   }
   void assign(const cpp_int_base& o)
   {
      if(this != &o)
      {
         static_cast<allocator_type&>(*this) = static_cast<const allocator_type&>(o);
         m_limbs = 0;
         resize(o.size());
         std::copy(o.limbs(), o.limbs() + o.size(), limbs());
         m_sign = o.m_sign;
      }
   }
   void negate() BOOST_NOEXCEPT
   {
      m_sign = !m_sign;
      // Check for zero value:
      if(m_sign && (m_limbs == 1))
      {
         if(limbs()[0] == 0)
            m_sign = false;
      }
   }
   bool isneg()const BOOST_NOEXCEPT
   {
      return m_sign; 
   }
   void do_swap(cpp_int_base& o) BOOST_NOEXCEPT
   {
      std::swap(m_data, o.m_data);
      std::swap(m_sign, o.m_sign);
      std::swap(m_internal, o.m_internal);
      std::swap(m_limbs, o.m_limbs);
   }
};

template <unsigned MinBits>
struct cpp_int_base<MinBits, true, void, false>
{
   typedef limb_type*                      limb_pointer;
   typedef const limb_type*                const_limb_pointer;

   //
   // Interface invariants:
   //
   BOOST_STATIC_ASSERT_MSG(MinBits > sizeof(double_limb_type) * CHAR_BIT, "Template parameter MinBits is inconsistent with the parameter trivial - did you mistakingly try to override the trivial parameter?");

public:
   BOOST_STATIC_CONSTANT(unsigned, limb_bits = sizeof(limb_type) * CHAR_BIT);
   BOOST_STATIC_CONSTANT(limb_type, max_limb_value = ~static_cast<limb_type>(0u));
   BOOST_STATIC_CONSTANT(limb_type, sign_bit_mask = 1u << (limb_bits - 1));
   BOOST_STATIC_CONSTANT(unsigned, internal_limb_count = MinBits / limb_bits + (MinBits % limb_bits ? 1 : 0));
   BOOST_STATIC_CONSTANT(bool, variable = false);
   BOOST_STATIC_CONSTANT(limb_type, upper_limb_mask = MinBits % limb_bits ? (limb_type(1) << (MinBits % limb_bits)) -1 : (~limb_type(0)));
   BOOST_STATIC_ASSERT_MSG(internal_limb_count >= 2, "A fixed precision integer type must have at least 2 limbs");

private:
   union data_type{
      limb_type          m_data[internal_limb_count];
      limb_type          m_first_limb;
      double_limb_type   m_double_first_limb;

      BOOST_CONSTEXPR data_type(){}
      BOOST_CONSTEXPR data_type(limb_type i) : m_first_limb(i) {}
      BOOST_CONSTEXPR data_type(double_limb_type i) : m_double_first_limb(i) {}
   } m_wrapper;
   boost::uint16_t    m_limbs;
   bool               m_sign;

public:
   //
   // Direct construction:
   //
   BOOST_CONSTEXPR cpp_int_base(limb_type i)BOOST_NOEXCEPT 
      : m_wrapper(i), m_limbs(1), m_sign(false) {}
   BOOST_CONSTEXPR cpp_int_base(signed_limb_type i)BOOST_NOEXCEPT 
      : m_wrapper(limb_type(i < 0 ? -i : i)), m_limbs(1), m_sign(i < 0) {}
#if defined(BOOST_LITTLE_ENDIAN)
   BOOST_CONSTEXPR cpp_int_base(double_limb_type i)BOOST_NOEXCEPT 
      : m_wrapper(i), m_limbs(i > max_limb_value ? 2 : 1), m_sign(false) {}
   BOOST_CONSTEXPR cpp_int_base(signed_double_limb_type i)BOOST_NOEXCEPT 
      : m_wrapper(double_limb_type(i < 0 ? -i : i)), m_limbs(i < 0 ? (-i > max_limb_value ? 2 : 1) : (i > max_limb_value ? 2 : 1)), m_sign(i < 0) {}
#endif
   //
   // Helper functions for getting at our internal data, and manipulating storage:
   //
   unsigned size()const BOOST_NOEXCEPT { return m_limbs; }
   limb_pointer limbs() BOOST_NOEXCEPT { return m_wrapper.m_data; }
   const_limb_pointer limbs()const BOOST_NOEXCEPT { return m_wrapper.m_data; }
   bool sign()const BOOST_NOEXCEPT { return m_sign; }
   void sign(bool b) BOOST_NOEXCEPT
   { 
      m_sign = b; 
      // Check for zero value:
      if(m_sign && (m_limbs == 1))
      {
         if(limbs()[0] == 0)
            m_sign = false;
      }
   }
   void resize(unsigned new_size) BOOST_NOEXCEPT
   {
      m_limbs = static_cast<boost::uint16_t>((std::min)(new_size, internal_limb_count));
   }
   void normalize() BOOST_NOEXCEPT
   {
      limb_pointer p = limbs();
      p[internal_limb_count-1] &= upper_limb_mask;
      while((m_limbs-1) && !p[m_limbs - 1])--m_limbs;
      if((m_limbs == 1) && (!*p)) m_sign = false; // zero is always unsigned
   }

   BOOST_CONSTEXPR cpp_int_base() : m_wrapper(limb_type(0u)), m_limbs(1), m_sign(false) {}
   cpp_int_base(const cpp_int_base& o) BOOST_NOEXCEPT : m_limbs(o.m_limbs), m_sign(o.m_sign)
   {
      std::copy(o.limbs(), o.limbs() + o.size(), limbs());
   }
   //~cpp_int_base() BOOST_NOEXCEPT {}
   void assign(const cpp_int_base& o) BOOST_NOEXCEPT
   {
      if(this != &o)
      {
         resize(o.size());
         std::copy(o.limbs(), o.limbs() + o.size(), limbs());
         m_sign = o.m_sign;
      }
   }
   void negate() BOOST_NOEXCEPT
   {
      m_sign = !m_sign;
      // Check for zero value:
      if(m_sign && (m_limbs == 1))
      {
         if(limbs()[0] == 0)
            m_sign = false;
      }
   }
   bool isneg()const BOOST_NOEXCEPT
   {
      return m_sign; 
   }
   void do_swap(cpp_int_base& o) BOOST_NOEXCEPT
   {
      for(unsigned i = 0; i < (std::max)(size(), o.size()); ++i)
         std::swap(m_wrapper.m_data[i], o.m_wrapper.m_data[i]);
      std::swap(m_sign, o.m_sign);
      std::swap(m_limbs, o.m_limbs);
   }
};

template <unsigned MinBits>
struct cpp_int_base<MinBits, false, void, false>
{
   typedef limb_type*                      limb_pointer;
   typedef const limb_type*                const_limb_pointer;

   //
   // Interface invariants:
   //
   BOOST_STATIC_ASSERT_MSG(MinBits > sizeof(double_limb_type) * CHAR_BIT, "Template parameter MinBits is inconsistent with the parameter trivial - did you mistakingly try to override the trivial parameter?");

public:
   BOOST_STATIC_CONSTANT(unsigned, limb_bits = sizeof(limb_type) * CHAR_BIT);
   BOOST_STATIC_CONSTANT(limb_type, max_limb_value = ~static_cast<limb_type>(0u));
   BOOST_STATIC_CONSTANT(limb_type, sign_bit_mask = 1u << (limb_bits - 1));
   BOOST_STATIC_CONSTANT(unsigned, internal_limb_count = MinBits / limb_bits + (MinBits % limb_bits ? 1 : 0));
   BOOST_STATIC_CONSTANT(bool, variable = false);
   BOOST_STATIC_CONSTANT(limb_type, upper_limb_mask = MinBits % limb_bits ? (limb_type(1) << (MinBits % limb_bits)) -1 : (~limb_type(0)));
   BOOST_STATIC_ASSERT_MSG(internal_limb_count >= 2, "A fixed precision integer type must have at least 2 limbs");

private:
   union data_type{
      limb_type          m_data[internal_limb_count];
      limb_type          m_first_limb;
      double_limb_type   m_double_first_limb;

      BOOST_CONSTEXPR data_type() {}
      BOOST_CONSTEXPR data_type(limb_type i) : m_first_limb(i) {}
      BOOST_CONSTEXPR data_type(double_limb_type i) : m_double_first_limb(i) {}
   } m_wrapper;
   limb_type          m_limbs;

public:
   //
   // Direct construction:
   //
   BOOST_CONSTEXPR cpp_int_base(limb_type i)BOOST_NOEXCEPT 
      : m_wrapper(i), m_limbs(1) {}
   cpp_int_base(signed_limb_type i)BOOST_NOEXCEPT 
      : m_wrapper(limb_type(i < 0 ? -i : i)), m_limbs(1) { if(i < 0) negate(); }
#ifdef BOOST_LITTLE_ENDIAN
   BOOST_CONSTEXPR cpp_int_base(double_limb_type i)BOOST_NOEXCEPT 
      : m_wrapper(i), m_limbs(i > max_limb_value ? 2 : 1) {}
   cpp_int_base(signed_double_limb_type i)BOOST_NOEXCEPT 
      : m_wrapper(double_limb_type(i < 0 ? -i : i)), m_limbs(i < 0 ? (-i > max_limb_value ? 2 : 1) : (i > max_limb_value ? 2 : 1)) { if(i < 0) negate(); }
#endif
   //
   // Helper functions for getting at our internal data, and manipulating storage:
   //
   unsigned size()const BOOST_NOEXCEPT { return m_limbs; }
   limb_pointer limbs() BOOST_NOEXCEPT { return m_wrapper.m_data; }
   const_limb_pointer limbs()const BOOST_NOEXCEPT { return m_wrapper.m_data; }
   BOOST_CONSTEXPR bool sign()const BOOST_NOEXCEPT { return false; }
   void sign(bool b) BOOST_NOEXCEPT {  if(b) negate(); }
   void resize(unsigned new_size) BOOST_NOEXCEPT
   {
      m_limbs = (std::min)(new_size, internal_limb_count);
   }
   void normalize() BOOST_NOEXCEPT
   {
      limb_pointer p = limbs();
      p[internal_limb_count-1] &= upper_limb_mask;
      while((m_limbs-1) && !p[m_limbs - 1])--m_limbs;
   }

   BOOST_CONSTEXPR cpp_int_base() BOOST_NOEXCEPT : m_wrapper(limb_type(0u)), m_limbs(1) {}
   cpp_int_base(const cpp_int_base& o) BOOST_NOEXCEPT : m_limbs(o.m_limbs)
   {
      std::copy(o.limbs(), o.limbs() + o.size(), limbs());
   }
   //~cpp_int_base() BOOST_NOEXCEPT {}
   void assign(const cpp_int_base& o) BOOST_NOEXCEPT
   {
      if(this != &o)
      {
         resize(o.size());
         std::copy(o.limbs(), o.limbs() + o.size(), limbs());
      }
   }
   void negate() BOOST_NOEXCEPT
   {
      // Not so much a negate as a complement - this gets called when subtraction
      // would result in a "negative" number:
      unsigned i;
      for(i = m_limbs; i < internal_limb_count; ++i)
         m_wrapper.m_data[i] = 0;
      m_limbs = internal_limb_count;
      for(i = 0; i < internal_limb_count; ++i)
         m_wrapper.m_data[i] = ~m_wrapper.m_data[i];
      normalize();
      eval_increment(static_cast<cpp_int_backend<MinBits, false, void>& >(*this));
   }
   BOOST_CONSTEXPR bool isneg()const BOOST_NOEXCEPT
   {
      return false; 
   }
   void do_swap(cpp_int_base& o) BOOST_NOEXCEPT
   {
      for(unsigned i = 0; i < (std::max)(size(), o.size()); ++i)
         std::swap(m_wrapper.m_data[i], o.m_wrapper.m_data[i]);
      std::swap(m_limbs, o.m_limbs);
   }
};

template <unsigned N, bool s>
struct trivial_limb_type_imp
{
   typedef double_limb_type type;
};

template <unsigned N>
struct trivial_limb_type_imp<N, true>
{
   typedef typename boost::uint_t<N>::least type;
};

template <unsigned N>
struct trivial_limb_type : public trivial_limb_type_imp<N, N <= sizeof(long long) * CHAR_BIT> {};

#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4244 4293) // loss of data in initialization, shift count too large
#endif

template <unsigned MinBits>
struct cpp_int_base<MinBits, true, void, true>
{
   typedef typename trivial_limb_type<MinBits>::type  local_limb_type;
   typedef local_limb_type*                           limb_pointer;
   typedef const local_limb_type*                     const_limb_pointer;
protected:
   BOOST_STATIC_CONSTANT(unsigned, limb_bits = sizeof(local_limb_type) * CHAR_BIT);
   BOOST_STATIC_CONSTANT(local_limb_type, limb_mask = MinBits < limb_bits ? (local_limb_type(1) << MinBits) -1 : (~local_limb_type(0)));
private:
   local_limb_type    m_data;
   bool               m_sign;

   //
   // Interface invariants:
   //
   BOOST_STATIC_ASSERT_MSG(MinBits <= sizeof(double_limb_type) * CHAR_BIT, "Template parameter MinBits is inconsistent with the parameter trivial - did you mistakingly try to override the trivial parameter?");

public:
   //
   // Direct construction:
   //
   template <class SI>
   BOOST_CONSTEXPR cpp_int_base(SI i, typename enable_if<is_signed<SI> >::type const* = 0) BOOST_NOEXCEPT
      : m_data(i < 0 ? -i : i), m_sign(i < 0) {}
   template <class UI>
   BOOST_CONSTEXPR cpp_int_base(UI i, typename enable_if<is_unsigned<UI> >::type const* = 0) BOOST_NOEXCEPT
      : m_data(i), m_sign(false) {}
   template <class F>
   cpp_int_base(F i, typename enable_if<is_floating_point<F> >::type const* = 0) BOOST_NOEXCEPT
      : m_data(std::fabs(i)), m_sign(i < 0) {}
   //
   // Helper functions for getting at our internal data, and manipulating storage:
   //
   BOOST_CONSTEXPR unsigned size()const BOOST_NOEXCEPT { return 1; }
   limb_pointer limbs() BOOST_NOEXCEPT { return &m_data; }
   const_limb_pointer limbs()const BOOST_NOEXCEPT { return &m_data; }
   bool sign()const BOOST_NOEXCEPT { return m_sign; }
   void sign(bool b) BOOST_NOEXCEPT
   { 
      m_sign = b; 
      // Check for zero value:
      if(m_sign && !m_data)
      {
         m_sign = false;
      }
   }
   void resize(unsigned new_size) BOOST_NOEXCEPT {}
   void normalize() BOOST_NOEXCEPT
   {
      if(!m_data) 
         m_sign = false; // zero is always unsigned
      m_data &= limb_mask;
   }

   BOOST_CONSTEXPR cpp_int_base() : m_data(0), m_sign(false) {}
   BOOST_CONSTEXPR cpp_int_base(const cpp_int_base& o) BOOST_NOEXCEPT : m_data(o.m_data), m_sign(o.m_sign) {}
   //~cpp_int_base() BOOST_NOEXCEPT {}
   void assign(const cpp_int_base& o) BOOST_NOEXCEPT
   {
      m_data = o.m_data;
      m_sign = o.m_sign;
   }
   void negate() BOOST_NOEXCEPT
   {
      m_sign = !m_sign;
      // Check for zero value:
      if(m_data == 0)
      {
         m_sign = false;
      }
   }
   bool isneg()const BOOST_NOEXCEPT
   {
      return m_sign; 
   }
   void do_swap(cpp_int_base& o) BOOST_NOEXCEPT
   {
      std::swap(m_sign, o.m_sign);
      std::swap(m_data, o.m_data);
   }
};

template <unsigned MinBits>
struct cpp_int_base<MinBits, false, void, true>
{
   typedef typename trivial_limb_type<MinBits>::type  local_limb_type;
   typedef local_limb_type*                           limb_pointer;
   typedef const local_limb_type*                     const_limb_pointer;
private:
   BOOST_STATIC_CONSTANT(unsigned, limb_bits = sizeof(local_limb_type) * CHAR_BIT);
   BOOST_STATIC_CONSTANT(local_limb_type, limb_mask = MinBits < limb_bits ? (local_limb_type(1) << MinBits) -1 : (~local_limb_type(0)));

   local_limb_type    m_data;

   //
   // Interface invariants:
   //
   BOOST_STATIC_ASSERT_MSG(MinBits <= sizeof(double_limb_type) * CHAR_BIT, "Template parameter MinBits is inconsistent with the parameter trivial - did you mistakingly try to override the trivial parameter?");

public:
   //
   // Direct construction:
   //
   template <class SI>
   BOOST_CONSTEXPR cpp_int_base(SI i, typename enable_if<is_signed<SI> >::type const* = 0) BOOST_NOEXCEPT
      : m_data(i < 0 ? 1 + ~static_cast<local_limb_type>(-i) : static_cast<local_limb_type>(i)) {}
   template <class UI>
   BOOST_CONSTEXPR cpp_int_base(UI i, typename enable_if<is_unsigned<UI> >::type const* = 0) BOOST_NOEXCEPT
      : m_data(i) {}
   template <class F>
   cpp_int_base(F i, typename enable_if<is_floating_point<F> >::type const* = 0) BOOST_NOEXCEPT
      : m_data(std::fabs(i)) 
   {
      if(i < 0)
         negate();
   }
   //
   // Helper functions for getting at our internal data, and manipulating storage:
   //
   BOOST_CONSTEXPR unsigned size()const BOOST_NOEXCEPT { return 1; }
   limb_pointer limbs() BOOST_NOEXCEPT { return &m_data; }
   const_limb_pointer limbs()const BOOST_NOEXCEPT { return &m_data; }
   BOOST_CONSTEXPR bool sign()const BOOST_NOEXCEPT { return false; }
   void sign(bool b) BOOST_NOEXCEPT
   {
      if(b)
         negate();
   }
   void resize(unsigned new_size) BOOST_NOEXCEPT {}
   void normalize() BOOST_NOEXCEPT 
   {
      m_data &= limb_mask;
   }

   BOOST_CONSTEXPR cpp_int_base() : m_data(0) {}
   BOOST_CONSTEXPR cpp_int_base(const cpp_int_base& o) BOOST_NOEXCEPT : m_data(o.m_data) {}
   //~cpp_int_base() BOOST_NOEXCEPT {}
   void assign(const cpp_int_base& o) BOOST_NOEXCEPT
   {
      m_data = o.m_data;
   }
   void negate() BOOST_NOEXCEPT
   {
      m_data = ~m_data;
      ++m_data;
   }
   BOOST_CONSTEXPR bool isneg()const BOOST_NOEXCEPT
   {
      return false; 
   }
   void do_swap(cpp_int_base& o) BOOST_NOEXCEPT
   {
      std::swap(m_data, o.m_data);
   }
};

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

template <unsigned MinBits, bool Signed, class Allocator>
const unsigned cpp_int_base<MinBits, Signed, Allocator, false>::limb_bits;
template <unsigned MinBits, bool Signed, class Allocator>
const limb_type cpp_int_base<MinBits, Signed, Allocator, false>::max_limb_value;
template <unsigned MinBits, bool Signed, class Allocator>
const limb_type cpp_int_base<MinBits, Signed, Allocator, false>::sign_bit_mask;
template <unsigned MinBits, bool Signed, class Allocator>
const unsigned cpp_int_base<MinBits, Signed, Allocator, false>::internal_limb_count;
template <unsigned MinBits, bool Signed, class Allocator>
const bool cpp_int_base<MinBits, Signed, Allocator, false>::variable;

template <unsigned MinBits>
const unsigned cpp_int_base<MinBits, true, void, false>::limb_bits;
template <unsigned MinBits>
const limb_type cpp_int_base<MinBits, true, void, false>::max_limb_value;
template <unsigned MinBits>
const limb_type cpp_int_base<MinBits, true, void, false>::sign_bit_mask;
template <unsigned MinBits>
const unsigned cpp_int_base<MinBits, true, void, false>::internal_limb_count;
template <unsigned MinBits>
const bool cpp_int_base<MinBits, true, void, false>::variable;

template <unsigned MinBits>
const unsigned cpp_int_base<MinBits, false, void, false>::limb_bits;
template <unsigned MinBits>
const limb_type cpp_int_base<MinBits, false, void, false>::max_limb_value;
template <unsigned MinBits>
const limb_type cpp_int_base<MinBits, false, void, false>::sign_bit_mask;
template <unsigned MinBits>
const unsigned cpp_int_base<MinBits, false, void, false>::internal_limb_count;
template <unsigned MinBits>
const bool cpp_int_base<MinBits, false, void, false>::variable;

template <unsigned MinBits, bool Signed, class Allocator>
struct cpp_int_backend<MinBits, Signed, Allocator, false> : public cpp_int_base<MinBits, Signed, Allocator, false>
{
   typedef cpp_int_backend<MinBits, Signed, Allocator>               self_type;
   typedef cpp_int_base<MinBits, Signed, Allocator>                  base_type;
   typedef typename is_trivial_cpp_int<self_type>::type              trivial_tag;
public:
   typedef mpl::list<signed_limb_type, signed_double_limb_type>      signed_types;
   typedef mpl::list<limb_type, double_limb_type>                    unsigned_types;
   typedef mpl::list<long double>                                    float_types;

   BOOST_CONSTEXPR cpp_int_backend() BOOST_NOEXCEPT{}
   BOOST_CONSTEXPR cpp_int_backend(const cpp_int_backend& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value) : base_type(o) {}
#ifndef BOOST_NO_RVALUE_REFERENCES
   BOOST_CONSTEXPR cpp_int_backend(cpp_int_backend&& o) BOOST_NOEXCEPT : base_type(static_cast<base_type&&>(o)) {}
#endif
   template <class LT>
   BOOST_CONSTEXPR cpp_int_backend(LT i, typename enable_if<is_same<LT, limb_type> >::type const* = 0)BOOST_NOEXCEPT 
      : base_type(i) {}
   template <class SLT>
   BOOST_CONSTEXPR cpp_int_backend(SLT i, typename enable_if<is_same<SLT, signed_limb_type> >::type const* = 0)BOOST_NOEXCEPT 
      : base_type(i) {}
#if defined(BOOST_LITTLE_ENDIAN)
   template <class LT>
   BOOST_CONSTEXPR cpp_int_backend(LT i, typename enable_if<is_same<LT, double_limb_type> >::type const* = 0)BOOST_NOEXCEPT 
      : base_type(i) {}
   template <class SLT>
   BOOST_CONSTEXPR cpp_int_backend(SLT i, typename enable_if<is_same<SLT, signed_double_limb_type> >::type const* = 0)BOOST_NOEXCEPT 
      : base_type(i) {}
#endif
   cpp_int_backend& operator = (const cpp_int_backend& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
   {
      this->assign(o);
      return *this;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   cpp_int_backend& operator = (cpp_int_backend&& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
   {
      *static_cast<base_type*>(this) = static_cast<base_type&&>(o);
      return *this;
   }
#endif
   cpp_int_backend& operator = (limb_type i) BOOST_NOEXCEPT
   {
      this->resize(1);
      *this->limbs() = i;
      this->sign(false);
      return *this;
   }
   cpp_int_backend& operator = (signed_limb_type i) BOOST_NOEXCEPT
   {
      this->resize(1);
      *this->limbs() = static_cast<limb_type>(std::abs(i));
      this->sign(i < 0);
      return *this;
   }
   cpp_int_backend& operator = (double_limb_type i) BOOST_NOEXCEPT_IF(MinBits <= sizeof(double_limb_type) * CHAR_BIT)
   {
      BOOST_STATIC_ASSERT(sizeof(i) == 2 * sizeof(limb_type));
      BOOST_STATIC_ASSERT(base_type::internal_limb_count >= 2);
      typename base_type::limb_pointer p = this->limbs();
      *p = static_cast<limb_type>(i);
      p[1] = static_cast<limb_type>(i >> base_type::limb_bits);
      this->resize(p[1] ? 2 : 1);
      this->sign(false);
      return *this;
   }
   cpp_int_backend& operator = (signed_double_limb_type i) BOOST_NOEXCEPT_IF(MinBits <= sizeof(signed_double_limb_type) * CHAR_BIT)
   {
      BOOST_STATIC_ASSERT(sizeof(i) == 2 * sizeof(limb_type));
      BOOST_STATIC_ASSERT(base_type::internal_limb_count >= 2);
      bool s = false;
      if(i < 0)
      {
         s = true;
         i = -i;
      }
      else
         this->sign(false);
      typename base_type::limb_pointer p = this->limbs();
      *p = static_cast<limb_type>(i);
      p[1] = static_cast<limb_type>(i >> base_type::limb_bits);
      this->resize(p[1] ? 2 : 1);
      this->sign(s);
      return *this;
   }

   cpp_int_backend& operator = (long double a) BOOST_NOEXCEPT_IF(MinBits <= sizeof(long double) * CHAR_BIT)
   {
      using default_ops::eval_add;
      using default_ops::eval_subtract;
      using std::frexp;
      using std::ldexp;
      using std::floor;

      if (a == 0) {
         return *this = static_cast<limb_type>(0u);
      }

      if (a == 1) {
         return *this = static_cast<limb_type>(1u);
      }

      BOOST_ASSERT(!(boost::math::isinf)(a));
      BOOST_ASSERT(!(boost::math::isnan)(a));

      int e;
      long double f, term;
      *this = static_cast<limb_type>(0u);

      f = frexp(a, &e);

      static const limb_type shift = std::numeric_limits<limb_type>::digits;

      while(f)
      {
         // extract int sized bits from f:
         f = ldexp(f, shift);
         term = floor(f);
         e -= shift;
         eval_left_shift(*this, shift);
         if(term > 0)
            eval_add(*this, static_cast<limb_type>(term));
         else
            eval_subtract(*this, static_cast<limb_type>(-term));
         f -= term;
      }
      if(e > 0)
         eval_left_shift(*this, e);
      else if(e < 0)
         eval_right_shift(*this, -e);
      return *this;
   }
   cpp_int_backend& operator = (const char* s)
   {
      using default_ops::eval_multiply;
      using default_ops::eval_add;
      std::size_t n = s ? std::strlen(s) : 0;
      *this = static_cast<limb_type>(0u);
      unsigned radix = 10;
      bool isneg = false;
      if(n && (*s == '-'))
      {
         --n;
         ++s;
         isneg = true;
      }
      if(n && (*s == '0'))
      {
         if((n > 1) && ((s[1] == 'x') || (s[1] == 'X')))
         {
            radix = 16;
            s +=2;
            n -= 2;
         }
         else
         {
            radix = 8;
            n -= 1;
         }
      }
      if(n)
      {
         if(radix == 8 || radix == 16)
         {
            unsigned shift = radix == 8 ? 3 : 4;
            unsigned block_count = base_type::limb_bits / shift;
            unsigned block_shift = shift * block_count;
            limb_type val, block;
            while(*s)
            {
               block = 0;
               for(unsigned i = 0; (i < block_count); ++i)
               {
                  if(*s >= '0' && *s <= '9')
                     val = *s - '0';
                  else if(*s >= 'a' && *s <= 'f')
                     val = 10 + *s - 'a';
                  else if(*s >= 'A' && *s <= 'F')
                     val = 10 + *s - 'A';
                  else
                     val = base_type::max_limb_value;
                  if(val > radix)
                  {
                     BOOST_THROW_EXCEPTION(std::runtime_error("Unexpected content found while parsing character string."));
                  }
                  block <<= shift;
                  block |= val;
                  if(!*++s)
                  {
                     // final shift is different:
                     block_shift = (i + 1) * shift;
                     break;
                  }
               }
               eval_left_shift(*this, block_shift);
               this->limbs()[0] |= block;
            }
         }
         else
         {
            // Base 10, we extract blocks of size 10^9 at a time, that way
            // the number of multiplications is kept to a minimum:
            limb_type block_mult = max_block_10;
            while(*s)
            {
               limb_type block = 0;
               for(unsigned i = 0; i < digits_per_block_10; ++i)
               {
                  limb_type val;
                  if(*s >= '0' && *s <= '9')
                     val = *s - '0';
                  else
                     BOOST_THROW_EXCEPTION(std::runtime_error("Unexpected character encountered in input."));
                  block *= 10;
                  block += val;
                  if(!*++s)
                  {
                     block_mult = block_multiplier(i);
                     break;
                  }
               }
               eval_multiply(*this, block_mult);
               eval_add(*this, block);
            }
         }
      }
      if(isneg)
         this->negate();
      return *this;
   }
   void swap(cpp_int_backend& o) BOOST_NOEXCEPT
   {
      this->do_swap(o);
   }
   std::string str(std::streamsize /*digits*/, std::ios_base::fmtflags f)const
   {
      int base = 10;
      if((f & std::ios_base::oct) == std::ios_base::oct)
         base = 8;
      else if((f & std::ios_base::hex) == std::ios_base::hex)
         base = 16;
      std::string result;

      unsigned Bits = this->size() * base_type::limb_bits;

      if(base == 8 || base == 16)
      {
         if(this->sign())
            BOOST_THROW_EXCEPTION(std::runtime_error("Base 8 or 16 printing of negative numbers is not supported."));
         limb_type shift = base == 8 ? 3 : 4;
         limb_type mask = static_cast<limb_type>((1u << shift) - 1);
         cpp_int_backend t(*this);
         result.assign(Bits / shift + (Bits % shift ? 1 : 0), '0');
         int pos = result.size() - 1;
         for(unsigned i = 0; i < Bits / shift; ++i)
         {
            char c = '0' + static_cast<char>(t.limbs()[0] & mask);
            if(c > '9')
               c += 'A' - '9' - 1;
            result[pos--] = c;
            eval_right_shift(t, shift);
         }
         if(Bits % shift)
         {
            mask = static_cast<limb_type>((1u << (Bits % shift)) - 1);
            char c = '0' + static_cast<char>(t.limbs()[0] & mask);
            if(c > '9')
               c += 'A' - '9';
            result[pos] = c;
         }
         //
         // Get rid of leading zeros:
         //
         std::string::size_type n = result.find_first_not_of('0');
         if(!result.empty() && (n == std::string::npos))
            n = result.size() - 1;
         result.erase(0, n);
         if(f & std::ios_base::showbase)
         {
            const char* pp = base == 8 ? "0" : "0x";
            result.insert(0, pp);
         }
      }
      else
      {
         result.assign(Bits / 3 + 1, '0');
         int pos = result.size() - 1;
         cpp_int_backend t(*this);
         cpp_int_backend r;
         bool neg = false;
         if(t.sign())
         {
            t.negate();
            neg = true;
         }
         if(this->size() == 1)
         {
            result = boost::lexical_cast<std::string>(t.limbs()[0]);
         }
         else
         {
            cpp_int_backend block10;
            block10 = max_block_10;
            while(eval_get_sign(t) != 0)
            {
               cpp_int_backend t2;
               divide_unsigned_helper(&t2, t, block10, r);
               t = t2;
               limb_type v = r.limbs()[0];
               for(unsigned i = 0; i < digits_per_block_10; ++i)
               {
                  char c = '0' + v % 10;
                  v /= 10;
                  result[pos] = c;
                  if(pos-- == 0)
                     break;
               }
            }
         }
         unsigned n = result.find_first_not_of('0');
         result.erase(0, n);
         if(result.empty())
            result = "0";
         if(neg)
            result.insert(0, 1, '-');
         else if(f & std::ios_base::showpos)
            result.insert(0, 1, '+');
      }
      return result;
   }
   int compare(const cpp_int_backend& o)const BOOST_NOEXCEPT
   {
      if(this->sign() != o.sign())
         return this->sign() ? -1 : 1;
      int result = 0;
      // Only do the compare if the same sign:
      result = compare_unsigned(o);

      if(this->sign())
         result = -result;
      return result;
   }
   int compare_unsigned(const cpp_int_backend& o)const BOOST_NOEXCEPT
   {
      if(this->size() != o.size())
      {
         return this->size() > o.size() ? 1 : -1;
      }
      typename base_type::const_limb_pointer pa = this->limbs();
      typename base_type::const_limb_pointer pb = o.limbs();
      for(int i = this->size() - 1; i >= 0; --i)
      {
         if(pa[i] != pb[i])
            return pa[i] > pb[i] ? 1 : -1;
      }
      return 0;
   }
   template <class Arithmatic>
   typename enable_if<is_arithmetic<Arithmatic>, int>::type compare(Arithmatic i)const BOOST_NOEXCEPT
   {
      // braindead version:
      cpp_int_backend t;
      t = i;
      return compare(t);
   }
};

template <unsigned MinBits, bool Signed>
struct cpp_int_backend<MinBits, Signed, void, true> : public cpp_int_base<MinBits, Signed, void, true>
{
public:
   typedef cpp_int_backend<MinBits, Signed, void, true>              self_type;
   typedef cpp_int_base<MinBits, Signed, void, true>                 base_type;
   typedef typename is_trivial_cpp_int<self_type>::type              trivial_tag;
   typedef mpl::list<
      signed char, short, int, long, 
      long long, signed_double_limb_type>                            signed_types;
   typedef mpl::list<unsigned char, unsigned short, unsigned,
      unsigned long long, double_limb_type>                          unsigned_types;
   typedef mpl::list<float, double, long double>                     float_types;

   BOOST_STATIC_CONSTANT(unsigned, limb_bits = sizeof(typename base_type::local_limb_type) * CHAR_BIT);

   BOOST_CONSTEXPR cpp_int_backend() BOOST_NOEXCEPT{}
   BOOST_CONSTEXPR cpp_int_backend(const cpp_int_backend& o) BOOST_NOEXCEPT : base_type(o) {}
   cpp_int_backend& operator = (const cpp_int_backend& o) BOOST_NOEXCEPT
   {
      this->assign(o);
      return *this;
   }

   template <class A>
   BOOST_CONSTEXPR cpp_int_backend(A i, typename enable_if<is_arithmetic<A> >::type const* = 0) BOOST_NOEXCEPT
      : base_type(i) {}

   template <class SI>
   typename enable_if<is_signed<SI>, cpp_int_backend&>::type operator = (SI i) BOOST_NOEXCEPT
   {
      *this->limbs() = static_cast<typename base_type::local_limb_type>(std::abs(i));
      this->sign(i < 0);
      return *this;
   }
   template <class UI>
   typename enable_if<is_unsigned<UI>, cpp_int_backend&>::type operator = (UI i) BOOST_NOEXCEPT
   {
      *this->limbs() = static_cast<typename base_type::local_limb_type>(i);
      return *this;
   }
   template <class F>
   typename enable_if<is_floating_point<F>, cpp_int_backend&>::type operator = (F i) BOOST_NOEXCEPT
   {
      *this->limbs() = static_cast<typename base_type::local_limb_type>(std::abs(i));
      this->sign(i < 0);
      return *this;
   }
   cpp_int_backend& operator = (const char* s)
   {
      try{
         *this->limbs() = boost::lexical_cast<limb_type>(s);
      }
      catch(const std::exception&)
      {
         BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Could not interpret the string \"") + s + std::string("\" as a valid integer value.")));
      }
      return *this;
   }
   void swap(cpp_int_backend& o) BOOST_NOEXCEPT
   {
      this->do_swap(o);
   }
private:
   std::string str(std::ios_base::fmtflags f, const mpl::false_&)const
   {
      std::stringstream ss;
      ss.flags(f);
      ss << *this->limbs();
      std::string result;
      if(this->sign())
         result += '-';
      result += ss.str();
      return result;
   }
   std::string str(std::ios_base::fmtflags f, const mpl::true_&)const
   {
      // Even though we have only one limb, we can't do IO on it :-(
      int base = 10;
      if((f & std::ios_base::oct) == std::ios_base::oct)
         base = 8;
      else if((f & std::ios_base::hex) == std::ios_base::hex)
         base = 16;
      std::string result;

      unsigned Bits = base_type::limb_bits;

      if(base == 8 || base == 16)
      {
         if(this->sign())
            BOOST_THROW_EXCEPTION(std::runtime_error("Base 8 or 16 printing of negative numbers is not supported."));
         limb_type shift = base == 8 ? 3 : 4;
         limb_type mask = static_cast<limb_type>((1u << shift) - 1);
         typename base_type::local_limb_type v = *this->limbs();
         result.assign(Bits / shift + (Bits % shift ? 1 : 0), '0');
         int pos = result.size() - 1;
         for(unsigned i = 0; i < Bits / shift; ++i)
         {
            char c = '0' + static_cast<char>(v & mask);
            if(c > '9')
               c += 'A' - '9' - 1;
            result[pos--] = c;
            v >>= shift;
         }
         if(Bits % shift)
         {
            mask = static_cast<limb_type>((1u << (Bits % shift)) - 1);
            char c = '0' + static_cast<char>(v & mask);
            if(c > '9')
               c += 'A' - '9';
            result[pos] = c;
         }
         //
         // Get rid of leading zeros:
         //
         std::string::size_type n = result.find_first_not_of('0');
         if(!result.empty() && (n == std::string::npos))
            n = result.size() - 1;
         result.erase(0, n);
         if(f & std::ios_base::showbase)
         {
            const char* pp = base == 8 ? "0" : "0x";
            result.insert(0, pp);
         }
      }
      else
      {
         result.assign(Bits / 3 + 1, '0');
         int pos = result.size() - 1;
         typename base_type::local_limb_type v(*this->limbs());
         bool neg = false;
         if(this->sign())
         {
            neg = true;
         }
         while(v)
         {
            result[pos] = (v % 10) + '0';
            --pos;
            v /= 10;
         }
         unsigned n = result.find_first_not_of('0');
         result.erase(0, n);
         if(result.empty())
            result = "0";
         if(neg)
            result.insert(0, 1, '-');
         else if(f & std::ios_base::showpos)
            result.insert(0, 1, '+');
      }
      return result;
   }
public:
   std::string str(std::streamsize /*digits*/, std::ios_base::fmtflags f)const
   {
#ifdef BOOST_MP_NO_DOUBLE_LIMB_TYPE_IO
      return str(f, mpl::bool_<is_same<typename base_type::local_limb_type, double_limb_type>::value>());
#else
      return str(f, mpl::bool_<false>());
#endif
   }
   int compare(const cpp_int_backend& o)const BOOST_NOEXCEPT
   {
      if(this->sign() != o.sign())
         return this->sign() ? -1 : 1;
      int result = 0;
      // Only do the compare if the same sign:
      result = *this->limbs() == *o.limbs() ? 0 : *this->limbs() < *o.limbs() ? -1 : 1;

      if(this->sign())
         result = -result;
      return result;
   }
   template <class Arithmatic>
   typename enable_if<is_arithmetic<Arithmatic>, int>::type compare(Arithmatic i)const BOOST_NOEXCEPT
   {
      // braindead version:
      cpp_int_backend t;
      t = i;
      return compare(t);
   }
};


template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_add(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   eval_add(result, result, o);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void add_unsigned(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const cpp_int_backend<MinBits, Signed, Allocator, false>& b) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   using std::swap;

   // Nothing fancy, just let uintmax_t take the strain:
   double_limb_type carry = 0;
   unsigned m, x;
   unsigned as = a.size();
   unsigned bs = b.size();
   minmax(as, bs, m, x);
   if(x == 1)
   {
      bool s = a.sign();
      result = static_cast<double_limb_type>(*a.limbs()) + static_cast<double_limb_type>(*b.limbs());
      result.sign(s);
      return;
   }
   result.resize(x);
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer pa = a.limbs();
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer pb = b.limbs();
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pr = result.limbs();
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pr_end = pr + m;

   if(as < bs)
      swap(pa, pb);
   
   // First where a and b overlap:
   while(pr != pr_end)
   {
      carry += static_cast<double_limb_type>(*pa) + static_cast<double_limb_type>(*pb);
      *pr = static_cast<limb_type>(carry);
      carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
      ++pr, ++pa, ++pb;
   }
   pr_end += x - m;
   // Now where only a has digits:
   while(pr != pr_end)
   {
      if(!carry)
      {
         if(pa != pr)
            std::copy(pa, pa + (pr_end - pr), pr);
         break;
      }
      carry += static_cast<double_limb_type>(*pa);
      *pr = static_cast<limb_type>(carry);
      carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
      ++pr, ++pa;
   }
   if(carry)
   {
      result.resize(x + 1);
      // We overflowed, need to add one more limb:
      if(cpp_int_backend<MinBits, Signed, Allocator, false>::variable || (result.size() > x))
         result.limbs()[x] = static_cast<limb_type>(carry);
   }
   result.normalize();
   result.sign(a.sign());
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_add(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const cpp_int_backend<MinBits, Signed, Allocator, false>& b) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(a.sign() != b.sign())
   {
      subtract_unsigned(result, a, b);
      return;
   }
   add_unsigned(result, a, b);
}

template <unsigned MinBits, bool Signed, class Allocator>
inline void add_unsigned(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const limb_type& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   // Addition using modular arithmatic.
   // Nothing fancy, just let uintmax_t take the strain:
   if(&result != &a)
      result.resize(a.size());
   double_limb_type carry = o;
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pr = result.limbs();
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer pa = a.limbs();
   unsigned i = 0;
   for(; carry && (i < result.size()); ++i)
   {
      carry += static_cast<double_limb_type>(pa[i]);
      pr[i] = static_cast<limb_type>(carry);
      carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   }
   if(&a != &result)
   {
      for(; i < result.size(); ++i)
         pr[i] = pa[i];
   }
   if(carry)
   {
      unsigned x = result.size();
      result.resize(x + 1);
      // We overflowed, need to add one more limb:
      if(cpp_int_backend<MinBits, Signed, Allocator, false>::variable || (result.size() > x))
         result.limbs()[x] = static_cast<limb_type>(carry);
   }
   result.normalize();
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_add(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const limb_type& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(result.sign())
   {
      subtract_unsigned(result, o);
   }
   else
      add_unsigned(result, result, o);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_add(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const limb_type& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(a.sign())
   {
      result = a;
      subtract_unsigned(result, o);
   }
   else
      add_unsigned(result, a, o);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_add(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const signed_limb_type& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(o < 0)
      eval_subtract(result, static_cast<limb_type>(-o));
   else if(o > 0)
      eval_add(result, static_cast<limb_type>(o));
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_add(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const signed_limb_type& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(o < 0)
      eval_subtract(result, a, static_cast<limb_type>(-o));
   else if(o > 0)
      eval_add(result, a, static_cast<limb_type>(o));
   else if(&result != &a)
      result = a;
}

template <unsigned MinBits, bool Signed, class Allocator>
inline void subtract_unsigned(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const limb_type& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   // Subtract one limb.
   // Nothing fancy, just let uintmax_t take the strain:
   BOOST_STATIC_CONSTANT(double_limb_type, borrow = static_cast<double_limb_type>(cpp_int_backend<MinBits, Signed, Allocator, false>::max_limb_value) + 1);
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer p = result.limbs();
   if(*p > o)
   {
      *p -= o;
   }
   else if(result.size() == 1)
   {
      *p = o - *p;
      result.negate();
   }
   else
   {
      *p = static_cast<limb_type>((borrow + *p) - o);
      ++p;
      while(!*p)
      {
         *p = cpp_int_backend<MinBits, Signed, Allocator, false>::max_limb_value;
         ++p;
      }
      --*p;
      result.normalize();
   }
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_subtract(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const limb_type& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(result.sign())
      add_unsigned(result, result, o);
   else
      subtract_unsigned(result, o);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_subtract(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const limb_type& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(a.sign())
      add_unsigned(result, a, o);
   else
   {
      result = a;
      subtract_unsigned(result, o);
   }
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_subtract(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const signed_limb_type& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(o)
   {
      if(o < 0)
         eval_add(result, static_cast<limb_type>(-o));
      else
         eval_subtract(result, static_cast<limb_type>(o));
   }
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_subtract(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const signed_limb_type& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(o)
   {
      if(o < 0)
         eval_add(result, a, static_cast<limb_type>(-o));
      else
         eval_subtract(result, a, static_cast<limb_type>(o));
   }
   else if(&result != &a)
      result = a;
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_increment(cpp_int_backend<MinBits, Signed, Allocator, false>& result) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   static const limb_type one = 1;
   if(!result.sign() && (result.limbs()[0] < cpp_int_backend<MinBits, Signed, Allocator, false>::max_limb_value))
      ++result.limbs()[0];
   else if(result.sign() && result.limbs()[0])
      --result.limbs()[0];
   else
      eval_add(result, one);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_decrement(cpp_int_backend<MinBits, Signed, Allocator, false>& result) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   static const limb_type one = 1;
   if(!result.sign() && result.limbs()[0])
      --result.limbs()[0];
   else if(result.sign() && (result.limbs()[0] < cpp_int_backend<MinBits, Signed, Allocator, false>::max_limb_value))
      ++result.limbs()[0];
   else
      eval_subtract(result, one);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_subtract(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   eval_subtract(result, result, o);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void subtract_unsigned(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const cpp_int_backend<MinBits, Signed, Allocator, false>& b) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   using std::swap;

   // Nothing fancy, just let uintmax_t take the strain:
   double_limb_type borrow = 0;
   unsigned m, x;
   minmax(a.size(), b.size(), m, x);
   //
   // special cases for small limb counts:
   //
   if(x == 1)
   {
      bool s = a.sign();
      limb_type al = *a.limbs();
      limb_type bl = *b.limbs();
      if(bl > al)
      {
         std::swap(al, bl);
         s = !s;
      }
      result = al - bl;
      result.sign(s);
      return;
   }
   // This isn't used till later, but comparison has to occur before we resize the result,
   // as that may also resize a or b if this is an inplace operation:
   int c = a.compare_unsigned(b);
   // Set up the result vector:
   result.resize(x);
   // Now that a, b, and result are stable, get pointers to their limbs:
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer pa = a.limbs();
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer pb = b.limbs();
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pr = result.limbs();
   bool swapped = false;
   if(c < 0)
   {
      swap(pa, pb);
      swapped = true;
   }
   else if(c == 0)
   {
      result = static_cast<limb_type>(0);
      return;
   }
   
   unsigned i = 0;
   // First where a and b overlap:
   while(i < m)
   {
      borrow = static_cast<double_limb_type>(pa[i]) - static_cast<double_limb_type>(pb[i]) - borrow;
      pr[i] = static_cast<limb_type>(borrow);
      borrow = (borrow >> cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits) & 1u;
      ++i;
   }
   // Now where only a has digits, only as long as we've borrowed:
   while(borrow && (i < x))
   {
      borrow = static_cast<double_limb_type>(pa[i]) - borrow;
      pr[i] = static_cast<limb_type>(borrow);
      borrow = (borrow >> cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits) & 1u;
      ++i;
   }
   // Any remaining digits are the same as those in pa:
   if((x != i) && (pa != pr))
      std::copy(pa + i, pa + x, pr + i);
   BOOST_ASSERT(0 == borrow);

   //
   // We may have lost digits, if so update limb usage count:
   //
   result.normalize();
   result.sign(a.sign());
   if(swapped)
      result.negate();
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_subtract(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const cpp_int_backend<MinBits, Signed, Allocator, false>& b) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(a.sign() != b.sign())
   {
      add_unsigned(result, a, b);
      return;
   }
   subtract_unsigned(result, a, b);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_multiply(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const cpp_int_backend<MinBits, Signed, Allocator, false>& b) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   // Very simple long multiplication, only usable for small numbers of limb_type's
   // but that's the typical use case for this type anyway:
   //
   // Special cases first:
   //
   unsigned as = a.size();
   unsigned bs = b.size();
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer pa = a.limbs();
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer pb = b.limbs();
   if(as == 1)
   {
      bool s = b.sign() != a.sign();
      if(bs == 1)
      {
         result = static_cast<double_limb_type>(*pa) * static_cast<double_limb_type>(*pb);
      }
      else
      {
         limb_type l = *pa;
         eval_multiply(result, b, l);
      }
      result.sign(s);
      return;
   }
   if(bs == 1)
   {
      bool s = b.sign() != a.sign();
      limb_type l = *pb;
      eval_multiply(result, a, l);
      result.sign(s);
      return;
   }

   if(&result == &a)
   {
      cpp_int_backend<MinBits, Signed, Allocator, false> t(a);
      eval_multiply(result, t, b);
      return;
   }
   if(&result == &b)
   {
      cpp_int_backend<MinBits, Signed, Allocator, false> t(b);
      eval_multiply(result, a, t);
      return;
   }

   result.resize(as + bs);
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pr = result.limbs();

   static const double_limb_type limb_max = ~static_cast<limb_type>(0u);
   static const double_limb_type double_limb_max = ~static_cast<double_limb_type>(0u);
   BOOST_STATIC_ASSERT(double_limb_max - 2 * limb_max >= limb_max * limb_max);

   double_limb_type carry = 0;
   std::memset(pr, 0, result.size() * sizeof(limb_type));
   for(unsigned i = 0; i < as; ++i)
   {
      unsigned inner_limit = cpp_int_backend<MinBits, Signed, Allocator, false>::variable ? bs : (std::min)(result.size() - i, bs);
      for(unsigned j = 0; j < inner_limit; ++j)
      {
         BOOST_ASSERT(i+j < result.size());
         BOOST_ASSERT(!std::numeric_limits<double_limb_type>::is_specialized || ((std::numeric_limits<double_limb_type>::max)() - carry > static_cast<double_limb_type>(cpp_int_backend<MinBits, Signed, Allocator, false>::max_limb_value) * static_cast<double_limb_type>(cpp_int_backend<MinBits, Signed, Allocator, false>::max_limb_value)));
         carry += static_cast<double_limb_type>(pa[i]) * static_cast<double_limb_type>(pb[j]);
         BOOST_ASSERT(!std::numeric_limits<double_limb_type>::is_specialized || ((std::numeric_limits<double_limb_type>::max)() - carry >= pr[i+j]));
         carry += pr[i + j];
         pr[i + j] = static_cast<limb_type>(carry);
         carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
         BOOST_ASSERT(carry <= (cpp_int_backend<MinBits, Signed, Allocator, false>::max_limb_value));
      }
      if(cpp_int_backend<MinBits, Signed, Allocator, false>::variable || (i + bs < result.size()))
         pr[i + bs] = static_cast<limb_type>(carry);
      carry = 0;
   }
   result.normalize();
   //
   // Set the sign of the result:
   //
   result.sign(a.sign() != b.sign());
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_multiply(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
    eval_multiply(result, result, a);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_multiply(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const limb_type& val) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(!val)
   {
      result = static_cast<limb_type>(0);
      return;
   }
   if(&a != &result)
      result.resize(a.size());
   double_limb_type carry = 0;
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer p = result.limbs();
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pe = result.limbs() + result.size();
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer pa = a.limbs();
   while(p != pe)
   {
      carry += static_cast<double_limb_type>(*pa) * static_cast<double_limb_type>(val);
      *p = static_cast<limb_type>(carry);
      carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
      ++p, ++pa;
   }
   if(carry)
   {
      unsigned i = result.size();
      result.resize(i + 1);
      if(cpp_int_backend<MinBits, Signed, Allocator, false>::variable || (result.size() > i))
         result.limbs()[i] = static_cast<limb_type>(carry);
   }
   result.sign(a.sign());
   if(!cpp_int_backend<MinBits, Signed, Allocator, false>::variable)
      result.normalize();
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_multiply(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const limb_type& val) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   eval_multiply(result, result, val);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_multiply(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const double_limb_type& val) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(val <= (std::numeric_limits<limb_type>::max)())
   {
      eval_multiply(result, a, static_cast<limb_type>(val));
   }
   else
   {
      cpp_int_backend<MinBits, Signed, Allocator, false> t(val);
      eval_multiply(result, a, t);
   }
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_multiply(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const double_limb_type& val) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   eval_multiply(result, result, val);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_multiply(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const signed_limb_type& val) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(val > 0)
      eval_multiply(result, a, static_cast<limb_type>(val));
   else
   {
      eval_multiply(result, a, static_cast<limb_type>(-val));
      result.negate();
   }
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_multiply(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const signed_limb_type& val) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   eval_multiply(result, result, val);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_multiply(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const signed_double_limb_type& val) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(val > 0)
   {
      if(val <= (std::numeric_limits<limb_type>::max)())
      {
         eval_multiply(result, a, static_cast<limb_type>(val));
         return;
      }
   }
   else if(val >= -static_cast<signed_double_limb_type>((std::numeric_limits<limb_type>::max)()))
   {
      eval_multiply(result, a, static_cast<limb_type>(-val));
      result.negate();
      return;
   }
   cpp_int_backend<MinBits, Signed, Allocator, false> t(val);
   eval_multiply(result, a, t);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_multiply(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const signed_double_limb_type& val) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   eval_multiply(result, result, val);
}

template <unsigned MinBits, bool Signed, class Allocator>
void divide_unsigned_helper(cpp_int_backend<MinBits, Signed, Allocator, false>* result, const cpp_int_backend<MinBits, Signed, Allocator, false>& x, const cpp_int_backend<MinBits, Signed, Allocator, false>& y, cpp_int_backend<MinBits, Signed, Allocator, false>& r)
{
   if((result == &x) || (&r == &x))
   {
      cpp_int_backend<MinBits, Signed, Allocator, false> t(x);
      divide_unsigned_helper(result, t, y, r);
      return;
   }
   if((result == &y) || (&r == &y))
   {
      cpp_int_backend<MinBits, Signed, Allocator, false> t(y);
      divide_unsigned_helper(result, x, t, r);
      return;
   }

   /*
    Very simple, fairly braindead long division.
    Start by setting the remainder equal to x, and the
    result equal to 0.  Then in each loop we calculate our
    "best guess" for how many times y divides into r,
    add our guess to the result, and subtract guess*y
    from the remainder r.  One wrinkle is that the remainder
    may go negative, in which case we subtract the current guess
    from the result rather than adding.  The value of the guess
    is determined by dividing the most-significant-limb of the
    current remainder by the most-significant-limb of y.

    Note that there are more efficient algorithms than this
    available, in particular see Knuth Vol 2.  However for small
    numbers of limbs this generally outperforms the alternatives
    and avoids the normalisation step which would require extra storage.
    */


   using default_ops::eval_subtract;

   if(result == &r)
   {
      cpp_int_backend<MinBits, Signed, Allocator, false> rem;
      divide_unsigned_helper(result, x, y, rem);
      r = rem;
      return;
   }

   //
   // Find the most significant words of numerator and denominator.
   //
   limb_type y_order = y.size() - 1;

   if(y_order == 0)
   {
      //
      // Only a single non-zero limb in the denominator, in this case
      // we can use a specialized divide-by-single-limb routine which is
      // much faster.  This also handles division by zero:
      //
      divide_unsigned_helper(result, x, y.limbs()[y_order], r);
      return;
   }

   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer px = x.limbs();
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer py = y.limbs();

   limb_type r_order = x.size() - 1;
   if((r_order == 0) && (*px == 0))
   {
      // x is zero, so is the result:
      r = y;
      if(result)
         *result = x;
      return;
   }

   r = x;
   r.sign(false);
   if(result)
      *result = static_cast<limb_type>(0u);
   //
   // Check if the remainder is already less than the divisor, if so
   // we already have the result.  Note we try and avoid a full compare
   // if we can:
   //
   if(r_order <= y_order)
   {
      if((r_order < y_order) || (r.compare(y) < 0))
      {
         return;
      }
   }

   cpp_int_backend<MinBits, Signed, Allocator, false> t;
   bool r_neg = false;

   //
   // See if we can short-circuit long division, and use basic arithmetic instead:
   //
   if(r_order == 0)
   {
      if(result)
         *result = px[0] / py[0];
      r = px[0] % py[0];
      return;
   }
   else if(r_order == 1)
   {
      double_limb_type a, b;
      a = (static_cast<double_limb_type>(px[1]) << cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits) | px[0];
      b = y_order ? 
         (static_cast<double_limb_type>(py[1]) << cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits) | py[0] 
         : py[0];
      if(result)
         *result = a / b;
      r = a % b;
      return;
   }
   //
   // prepare result:
   //
   if(result)
      result->resize(1 + r_order - y_order);
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer prem = r.limbs();
   // This is initialised just to keep the compiler from emitting useless warnings later on:
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pr = typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer();
   if(result)
   {
      pr = result->limbs();
      for(unsigned i = 1; i < 1 + r_order - y_order; ++i)
         pr[i] = 0;
   }
   bool first_pass = true;

   do
   {
      //
      // Calculate our best guess for how many times y divides into r:
      //
      limb_type guess;
      if((prem[r_order] <= py[y_order]) && (r_order > 0))
      {
         double_limb_type a, b, v;
         a = (static_cast<double_limb_type>(prem[r_order]) << cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits) | prem[r_order - 1];
         b = py[y_order];
         v = a / b;
         if(v > cpp_int_backend<MinBits, Signed, Allocator, false>::max_limb_value)
            guess = 1;
         else
         {
            guess = static_cast<limb_type>(v);
            --r_order;
         }
      }
      else if(r_order == 0)
      {
         guess = prem[0] / py[y_order];
      }
      else
      {
         double_limb_type a, b, v;
         a = (static_cast<double_limb_type>(prem[r_order]) << cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits) | prem[r_order - 1];
         b = (y_order > 0) ? (static_cast<double_limb_type>(py[y_order]) << cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits) | py[y_order - 1] : (static_cast<double_limb_type>(py[y_order])  << cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits);
         v = a / b;
         guess = static_cast<limb_type>(v);
      }
      BOOST_ASSERT(guess); // If the guess ever gets to zero we go on forever....
      //
      // Update result:
      //
      limb_type shift = r_order - y_order;
      if(result)
      {
         if(r_neg)
         {
            if(pr[shift] > guess)
               pr[shift] -= guess;
            else
            {
               t.resize(shift + 1);
               t.limbs()[shift] = guess;
               for(unsigned i = 0; i < shift; ++i)
                  t.limbs()[i] = 0;
               eval_subtract(*result, t);
            }
         }
         else if(cpp_int_backend<MinBits, Signed, Allocator, false>::max_limb_value - pr[shift] > guess)
            pr[shift] += guess;
         else
         {
            t.resize(shift + 1);
            t.limbs()[shift] = guess;
            for(unsigned i = 0; i < shift; ++i)
               t.limbs()[i] = 0;
            eval_add(*result, t);
         }
      }
      //
      // Calculate guess * y, we use a fused mutiply-shift O(N) for this
      // rather than a full O(N^2) multiply:
      //
      double_limb_type carry = 0;
      t.resize(y.size() + shift + 1);
      bool truncated_t = !cpp_int_backend<MinBits, Signed, Allocator, false>::variable && (t.size() != y.size() + shift + 1);
      typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pt = t.limbs();
      for(unsigned i = 0; i < shift; ++i)
         pt[i] = 0;
      for(unsigned i = 0; i < y.size(); ++i)
      {
         carry += static_cast<double_limb_type>(py[i]) * static_cast<double_limb_type>(guess);
         pt[i + shift] = static_cast<limb_type>(carry);
         carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
      }
      if(carry && !truncated_t)
      {
         pt[t.size() - 1] = static_cast<limb_type>(carry);
      }
      else if(!truncated_t)
      {
         t.resize(t.size() - 1);
      }
      //
      // Update r:
      //
      eval_subtract(r, t);
      if(r.isneg())
      {
         r.negate();
         r_neg = !r_neg;
      }
      //
      // First time through we need to strip any leading zero, otherwise
      // the termination condition goes belly-up:
      //
      if(result && first_pass)
      {
         first_pass = false;
         while(pr[result->size() - 1] == 0)
            result->resize(result->size() - 1);
      }
      //
      // Update r_order:
      //
      r_order = r.size() - 1;
      if(r_order < y_order)
         break;
   }
   // Termination condition is really just a check that r > y, but with a common
   // short-circuit case handled first:
   while((r_order > y_order) || (r.compare_unsigned(y) >= 0));

   //
   // We now just have to normalise the result:
   //
   if(r_neg && eval_get_sign(r))
   {
      // We have one too many in the result:
      if(result)
         eval_decrement(*result);
      r.negate();
      if(y.sign())
         eval_subtract(r, y);
      else
         eval_add(r, y);
   }

   BOOST_ASSERT(r.compare_unsigned(y) < 0); // remainder must be less than the divisor or our code has failed
}

template <unsigned MinBits, bool Signed, class Allocator>
void divide_unsigned_helper(cpp_int_backend<MinBits, Signed, Allocator, false>* result, const cpp_int_backend<MinBits, Signed, Allocator, false>& x, limb_type y, cpp_int_backend<MinBits, Signed, Allocator, false>& r)
{
   if((result == &x) || (&r == &x))
   {
      cpp_int_backend<MinBits, Signed, Allocator, false> t(x);
      divide_unsigned_helper(result, t, y, r);
      return;
   }

   if(result == &r)
   {
      cpp_int_backend<MinBits, Signed, Allocator, false> rem;
      divide_unsigned_helper(result, x, y, rem);
      r = rem;
      return;
   }

   // As above, but simplified for integer divisor:

   using default_ops::eval_subtract;

   if(y == 0)
   {
      BOOST_THROW_EXCEPTION(std::runtime_error("Integer Division by zero."));
   }
   //
   // Find the most significant word of numerator.
   //
   limb_type r_order = x.size() - 1;

   //
   // Set remainder and result to their initial values:
   //
   r = x;
   r.sign(false);
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pr = r.limbs();

   if((r_order == 0) && (*pr == 0))
   {
      // All the limbs in x are zero, so is the result:
      return;
   }
   //
   // check for x < y, try to do this without actually having to 
   // do a full comparison:
   //
   if((r_order == 0) && (*pr < y))
   {
      if(result)
         *result = static_cast<limb_type>(0u);
      return;
   }

   //
   // See if we can short-circuit long division, and use basic arithmetic instead:
   //
   if(r_order == 0)
   {
      if(result)
      {
         *result = *pr / y;
         result->sign(x.sign());
      }
      r.sign(x.sign());
      *pr %= y;
      return;
   }
   else if(r_order == 1)
   {
      double_limb_type a;
      a = (static_cast<double_limb_type>(pr[r_order]) << cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits) | pr[0];
      if(result)
      {
         *result = a / y;
         result->sign(x.sign());
      }
      r = a % y;
      r.sign(x.sign());
      return;
   }

   // This is initialised just to keep the compiler from emitting useless warnings later on:
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pres = typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer();
   if(result)
   {
      result->resize(r_order + 1);
      pres = result->limbs();
      if(result->size() > r_order)
         pres[r_order] = 0;  // just in case we don't set the most significant limb below.
   }

   do
   {
      //
      // Calculate our best guess for how many times y divides into r:
      //
      if((pr[r_order] < y) && r_order)
      {
         double_limb_type a, b;
         a = (static_cast<double_limb_type>(pr[r_order]) << cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits) | pr[r_order - 1];
         b = a % y;
         r.resize(r.size() - 1);
         --r_order;
         pr[r_order] = static_cast<limb_type>(b);
         if(result)
            pres[r_order] = static_cast<limb_type>(a / y);
         if(r_order && pr[r_order] == 0)
         {
            --r_order;  // No remainder, division was exact.
            r.resize(r.size() - 1);
            if(result)
               pres[r_order] = static_cast<limb_type>(0u);
         }
      }
      else
      {
         if(result)
            pres[r_order] = pr[r_order] / y;
         pr[r_order] %= y;
         if(r_order && pr[r_order] == 0)
         {
            --r_order;  // No remainder, division was exact.
            r.resize(r.size() - 1);
            if(result)
               pres[r_order] = static_cast<limb_type>(0u);
         }
      }
   }
   // Termination condition is really just a check that r > y, but with two common
   // short-circuit cases handled first:
   while(r_order || (pr[r_order] > y));

   if(result)
   {
      result->normalize();
      result->sign(x.sign());
   }
   r.normalize();
   r.sign(x.sign());

   BOOST_ASSERT(r.compare(y) < 0); // remainder must be less than the divisor or our code has failed
}

template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_divide(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const cpp_int_backend<MinBits, Signed, Allocator, false>& b)
{
   cpp_int_backend<MinBits, Signed, Allocator, false> r;
   divide_unsigned_helper(&result, a, b, r);
   result.sign(a.sign() != b.sign());
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_divide(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, limb_type& b)
{
   cpp_int_backend<MinBits, Signed, Allocator, false> r;
   divide_unsigned_helper(&result, a, b, r);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_divide(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, signed_limb_type& b)
{
   cpp_int_backend<MinBits, Signed, Allocator, false> r;
   divide_unsigned_helper(&result, a, std::abs(b), r);
   if(b < 0)
      result.negate();
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_divide(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& b)
{
   // There is no in place divide:
   cpp_int_backend<MinBits, Signed, Allocator, false> a(result);
   eval_divide(result, a, b);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_divide(cpp_int_backend<MinBits, Signed, Allocator, false>& result, limb_type b)
{
   // There is no in place divide:
   cpp_int_backend<MinBits, Signed, Allocator, false> a(result);
   eval_divide(result, a, b);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_divide(cpp_int_backend<MinBits, Signed, Allocator, false>& result, signed_limb_type b)
{
   // There is no in place divide:
   cpp_int_backend<MinBits, Signed, Allocator, false> a(result);
   eval_divide(result, a, b);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_modulus(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const cpp_int_backend<MinBits, Signed, Allocator, false>& b)
{
   divide_unsigned_helper(static_cast<cpp_int_backend<MinBits, Signed, Allocator, false>* >(0), a, b, result);
   result.sign(a.sign());
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_modulus(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, limb_type b)
{
   divide_unsigned_helper(static_cast<cpp_int_backend<MinBits, Signed, Allocator, false>* >(0), a, b, result);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_modulus(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, signed_limb_type b)
{
   divide_unsigned_helper(static_cast<cpp_int_backend<MinBits, Signed, Allocator, false>* >(0), a, static_cast<limb_type>(std::abs(b)), result);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_modulus(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& b)
{
   // There is no in place divide:
   cpp_int_backend<MinBits, Signed, Allocator, false> a(result);
   eval_modulus(result, a, b);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_modulus(cpp_int_backend<MinBits, Signed, Allocator, false>& result, limb_type b)
{
   // There is no in place divide:
   cpp_int_backend<MinBits, Signed, Allocator, false> a(result);
   eval_modulus(result, a, b);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_modulus(cpp_int_backend<MinBits, Signed, Allocator, false>& result, signed_limb_type b)
{
   // There is no in place divide:
   cpp_int_backend<MinBits, Signed, Allocator, false> a(result);
   eval_modulus(result, a, b);
}
template <unsigned MinBits, bool Signed, class Allocator, class Op>
void bitwise_op(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& o, Op op) BOOST_NOEXCEPT
{
   //
   // There are 4 cases:
   // * Both positive.
   // * result negative, o positive.
   // * o negative, result positive.
   // * Both negative.
   //
   // When one arg is negative we convert to 2's complement form "on the fly",
   // and then convert back to signed-magnitude form at the end.
   //
   // First figure out how big the result needs to be and set up some data:
   //
   unsigned rs = result.size();
   unsigned os = o.size();
   unsigned m, x;
   minmax(rs, os, m, x);
   result.resize(x);
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pr = result.limbs();
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer po = o.limbs();
   for(unsigned i = rs; i < x; ++i)
      pr[i] = 0;

   limb_type next_limb = 0;

   if(!result.sign())
   {
      if(!o.sign())
      {
         for(unsigned i = 0; i < os; ++i)
            pr[i] = op(pr[i], po[i]);
         for(unsigned i = os; i < x; ++i)
            pr[i] = op(pr[i], limb_type(0));
      }
      else
      {
         // "o" is negative:
         double_limb_type carry = 1;
         for(unsigned i = 0; i < os; ++i)
         {
            carry += static_cast<double_limb_type>(~po[i]);
            pr[i] = op(pr[i], static_cast<limb_type>(carry));
            carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
         }
         for(unsigned i = os; i < x; ++i)
         {
            carry += static_cast<double_limb_type>(~limb_type(0));
            pr[i] = op(pr[i], static_cast<limb_type>(carry));
            carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
         }
         // Set the overflow into the "extra" limb:
         carry += static_cast<double_limb_type>(~limb_type(0));
         next_limb = op(limb_type(0), static_cast<limb_type>(carry));
      }
   }
   else
   {
      if(!o.sign())
      {
         // "result" is negative:
         double_limb_type carry = 1;
         for(unsigned i = 0; i < os; ++i)
         {
            carry += static_cast<double_limb_type>(~pr[i]);
            pr[i] = op(static_cast<limb_type>(carry), po[i]);
            carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
         }
         for(unsigned i = os; i < x; ++i)
         {
            carry += static_cast<double_limb_type>(~pr[i]);
            pr[i] = op(static_cast<limb_type>(carry), limb_type(0));
            carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
         }
         // Set the overflow into the "extra" limb:
         carry += static_cast<double_limb_type>(~limb_type(0));
         next_limb = op(static_cast<limb_type>(carry), limb_type(0));
      }
      else
      {
         // both are negative:
         double_limb_type r_carry = 1;
         double_limb_type o_carry = 1;
         for(unsigned i = 0; i < os; ++i)
         {
            r_carry += static_cast<double_limb_type>(~pr[i]);
            o_carry += static_cast<double_limb_type>(~po[i]);
            pr[i] = op(static_cast<limb_type>(r_carry), static_cast<limb_type>(o_carry));
            r_carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
            o_carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
         }
         for(unsigned i = os; i < x; ++i)
         {
            r_carry += static_cast<double_limb_type>(~pr[i]);
            o_carry += static_cast<double_limb_type>(~limb_type(0));
            pr[i] = op(static_cast<limb_type>(r_carry), static_cast<limb_type>(o_carry));
            r_carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
            o_carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
         }
         // Set the overflow into the "extra" limb:
         r_carry += static_cast<double_limb_type>(~limb_type(0));
         o_carry += static_cast<double_limb_type>(~limb_type(0));
         next_limb = op(static_cast<limb_type>(r_carry), static_cast<limb_type>(o_carry));
      }
   }
   //
   // See if the result is negative or not:
   //
   if(static_cast<signed_limb_type>(next_limb) < 0)
   {
      result.sign(true);
      double_limb_type carry = 1;
      for(unsigned i = 0; i < x; ++i)
      {
         carry += static_cast<double_limb_type>(~pr[i]);
         pr[i] = static_cast<limb_type>(carry);
         carry >>= cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
      }
   }
   else
      result.sign(false);

   result.normalize();
}

struct bit_and{ limb_type operator()(limb_type a, limb_type b)const BOOST_NOEXCEPT { return a & b; } };
struct bit_or { limb_type operator()(limb_type a, limb_type b)const BOOST_NOEXCEPT { return a | b; } };
struct bit_xor{ limb_type operator()(limb_type a, limb_type b)const BOOST_NOEXCEPT { return a ^ b; } };

template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_bitwise_and(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& o)BOOST_NOEXCEPT_IF(is_void<Allocator>::value)
{
   bitwise_op(result, o, bit_and());
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_bitwise_or(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& o) BOOST_NOEXCEPT_IF(is_void<Allocator>::value)
{
   bitwise_op(result, o, bit_or());
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_bitwise_xor(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& o) BOOST_NOEXCEPT_IF(is_void<Allocator>::value)
{
   bitwise_op(result, o, bit_xor());
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_complement(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& o) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   // Increment and negate:
   result = o;
   eval_increment(result);
   result.negate();
}
template <unsigned MinBits, bool Signed>
inline void eval_left_shift(cpp_int_backend<MinBits, Signed, void, false>& result, double_limb_type s) BOOST_NOEXCEPT
{
   if(!s)
      return;

   limb_type offset = static_cast<limb_type>(s / cpp_int_backend<MinBits, Signed, void>::limb_bits);
   limb_type shift  = static_cast<limb_type>(s % cpp_int_backend<MinBits, Signed, void>::limb_bits);

   if(offset > cpp_int_backend<MinBits, Signed, void>::internal_limb_count)
   {
      result = static_cast<limb_type>(0);
      return;
   }

   unsigned ors = result.size();
   if((ors == 1) && (!*result.limbs()))
      return; // shifting zero yields zero.
   unsigned rs = ors;
   if(shift && (result.limbs()[ors - 1] >> (cpp_int_backend<MinBits, Signed, void>::limb_bits - shift)))
      ++rs; // Most significant limb will overflow when shifted
   rs += offset;
   result.resize(rs);
   bool truncated = result.size() != rs;
   if(truncated)
      rs = result.size();
   typename cpp_int_backend<MinBits, Signed, void>::limb_pointer pr = result.limbs();

   unsigned i = 0;
   if(shift)
   {
      // This code only works when shift is non-zero, otherwise we invoke undefined behaviour!
      i = 0;
      if(!truncated)
      {
         if(rs > ors + offset)
         {
            pr[rs - 1 - i] = pr[ors - 1 - i] >> (cpp_int_backend<MinBits, Signed, void>::limb_bits - shift);
            --rs;
         }
         else
         {
            pr[rs - 1 - i] = pr[ors - 1 - i] << shift;
            if(ors > 1)
               pr[rs - 1 - i] |= pr[ors - 2 - i] >> (cpp_int_backend<MinBits, Signed, void>::limb_bits - shift);
            ++i;
         }
      }
      for(; ors > 1 + i; ++i)
      {
         pr[rs - 1 - i] = pr[ors - 1 - i] << shift;
         pr[rs - 1 - i] |= pr[ors - 2 - i] >> (cpp_int_backend<MinBits, Signed, void>::limb_bits - shift);
      }
      if(ors >= 1 + i)
      {
         pr[rs - 1 - i] = pr[ors - 1 - i] << shift;
         ++i;
      }
      for(; i < rs; ++i)
         pr[rs - 1 - i] = 0;
   }
   else
   {
      for(; i < ors; ++i)
         pr[rs - 1 - i] = pr[ors - 1 - i];
      for(; i < rs; ++i)
         pr[rs - 1 - i] = 0;
   }
   //
   // We may have shifted off the end and have leading zeros:
   //
   if(truncated)
   {
      result.normalize();
   }
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_left_shift(cpp_int_backend<MinBits, Signed, Allocator, false>& result, double_limb_type s) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(!s)
      return;

   limb_type offset = static_cast<limb_type>(s / cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits);
   limb_type shift  = static_cast<limb_type>(s % cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits);

   unsigned ors = result.size();
   if((ors == 1) && (!*result.limbs()))
      return; // shifting zero yields zero.
   unsigned rs = ors;
   if(shift && (result.limbs()[rs - 1] >> (cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits - shift)))
      ++rs; // Most significant limb will overflow when shifted
   rs += offset;
   result.resize(rs);
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pr = result.limbs();

   unsigned i = 0;
   if(shift)
   {
      // This code only works when shift is non-zero, otherwise we invoke undefined behaviour!
      i = 0;
      if(rs > ors + offset)
      {
         pr[rs - 1 - i] = pr[ors - 1 - i] >> (cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits - shift);
         --rs;
      }
      else
      {
         pr[rs - 1 - i] = pr[ors - 1 - i] << shift;
         if(ors > 1)
            pr[rs - 1 - i] |= pr[ors - 2 - i] >> (cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits - shift);
         ++i;
      }
      for(; ors > 1 + i; ++i)
      {
         pr[rs - 1 - i] = pr[ors - 1 - i] << shift;
         pr[rs - 1 - i] |= pr[ors - 2 - i] >> (cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits - shift);
      }
      if(ors >= 1 + i)
      {
         pr[rs - 1 - i] = pr[ors - 1 - i] << shift;
         ++i;
      }
      for(; i < rs; ++i)
         pr[rs - 1 - i] = 0;
   }
   else
   {
      for(; i < ors; ++i)
         pr[rs - 1 - i] = pr[ors - 1 - i];
      for(; i < rs; ++i)
         pr[rs - 1 - i] = 0;
   }
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_right_shift(cpp_int_backend<MinBits, Signed, Allocator, false>& result, double_limb_type s) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if(!s)
      return;

   limb_type offset = static_cast<limb_type>(s / cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits);
   limb_type shift  = static_cast<limb_type>(s % cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits);
   unsigned ors = result.size();
   unsigned rs = ors;
   if(offset >= rs)
   {
      result = limb_type(0);
      return;
   }
   rs -= offset;
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::limb_pointer pr = result.limbs();
   if((pr[ors - 1] >> shift) == 0)
      --rs;
   if(rs == 0)
   {
      result = limb_type(0);
      return;
   }
   unsigned i = 0;
   if(shift)
   {
      // This code only works for non-zero shift, otherwise we invoke undefined behaviour!
      for(; i + offset + 1 < ors; ++i)
      {
         pr[i] = pr[i + offset] >> shift;
         pr[i] |= pr[i + offset + 1] << (cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits - shift);
      }
      pr[i] = pr[i + offset] >> shift;
   }
   else
   {
      for(; i < rs; ++i)
         pr[i] = pr[i + offset];
   }
   result.resize(rs);
}

template <class Integer>
inline Integer negate_integer(Integer i, const mpl::true_&) BOOST_NOEXCEPT
{
   return -i;
}
template <class Integer>
inline Integer negate_integer(Integer i, const mpl::false_&) BOOST_NOEXCEPT
{
   return ~--i;
}

template <class R, unsigned MinBits, bool Signed, class Allocator>
inline typename enable_if<is_integral<R>, void>::type eval_convert_to(R* result, const cpp_int_backend<MinBits, Signed, Allocator, false>& backend) BOOST_NOEXCEPT
{
   *result = static_cast<R>(backend.limbs()[0]);
   unsigned shift = cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   for(unsigned i = 1; (i < backend.size()) && (shift < static_cast<unsigned>(std::numeric_limits<R>::digits)); ++i)
   {
      *result += static_cast<R>(backend.limbs()[i]) << shift;
      shift += cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   }
   if(backend.sign())
   {
      *result = negate_integer(*result, mpl::bool_<std::numeric_limits<R>::is_signed>());
   }
}

template <class R, unsigned MinBits, bool Signed, class Allocator>
inline typename enable_if<is_floating_point<R>, void>::type eval_convert_to(R* result, const cpp_int_backend<MinBits, Signed, Allocator, false>& backend) BOOST_NOEXCEPT
{
   typename cpp_int_backend<MinBits, Signed, Allocator, false>::const_limb_pointer p = backend.limbs();
   unsigned shift = cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   *result = static_cast<R>(*p);
   for(unsigned i = 1; i < backend.size(); ++i)
   {
      *result += static_cast<R>(std::ldexp(static_cast<long double>(p[i]), shift));
      shift += cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   }
   if(backend.sign())
      *result = -*result;
}

template <unsigned MinBits, bool Signed, class Allocator>
inline bool eval_is_zero(const cpp_int_backend<MinBits, Signed, Allocator, false>& val) BOOST_NOEXCEPT
{
   return (val.size() == 1) && (val.limbs()[0] == 0);
}
template <unsigned MinBits, bool Signed, class Allocator>
inline int eval_get_sign(const cpp_int_backend<MinBits, Signed, Allocator, false>& val) BOOST_NOEXCEPT
{
   return eval_is_zero(val) ? 0 : val.sign() ? -1 : 1;
}
template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_abs(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& val) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   result = val;
   result.sign(false);
}

//
// Get the location of the least-significant-bit:
//
template <unsigned MinBits, bool Signed, class Allocator>
inline unsigned eval_lsb(const cpp_int_backend<MinBits, Signed, Allocator, false>& a) BOOST_NOEXCEPT
{
   BOOST_ASSERT(eval_get_sign(a) != 0);
   
   unsigned result = 0;
   //
   // Find the index of the least significant limb that is non-zero:
   //
   unsigned index = 0;
   while(!a.limbs()[index] && (index < a.size()))
      ++index;
   //
   // Find the index of the least significant bit within that limb:
   //
   limb_type l = a.limbs()[index];
   while(!(l & 1u))
   {
      l >>= 1;
      ++result;
   }

   return result + index * cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
}

template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_gcd(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const cpp_int_backend<MinBits, Signed, Allocator, false>& b) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   int shift;

   cpp_int_backend<MinBits, Signed, Allocator, false> u(a), v(b);

   int s = eval_get_sign(u);

   /* GCD(0,x) := x */
   if(s < 0)
   {
      u.negate();
   }
   else if(s == 0)
   {
      result = v;
      return;
   }
   s = eval_get_sign(v);
   if(s < 0)
   {
      v.negate();
   }
   else if(s == 0)
   {
      result = u;
      return;
   }

   /* Let shift := lg K, where K is the greatest power of 2
   dividing both u and v. */

   unsigned us = eval_lsb(u);
   unsigned vs = eval_lsb(v);
   shift = (std::min)(us, vs);
   eval_right_shift(u, us);
   eval_right_shift(v, vs);

   do 
   {
      /* Now u and v are both odd, so diff(u, v) is even.
      Let u = min(u, v), v = diff(u, v)/2. */
      if(u.compare(v) > 0)
         u.swap(v);
      eval_subtract(v, u);
      // Termination condition tries not to do a full compare if possible:
      if(!v.limbs()[0] && eval_is_zero(v))
         break;
      vs = eval_lsb(v);
      eval_right_shift(v, vs);
      BOOST_ASSERT((v.limbs()[0] & 1));
      BOOST_ASSERT((u.limbs()[0] & 1));
   } 
   while(true);

   result = u;
   eval_left_shift(result, shift);
}

template <unsigned MinBits, bool Signed, class Allocator>
inline bool eval_bit_test(const cpp_int_backend<MinBits, Signed, Allocator, false>& val, unsigned index) BOOST_NOEXCEPT
{
   unsigned offset = index / cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   unsigned shift = index % cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   limb_type mask = shift ? limb_type(1u) << shift : limb_type(1u);
   if(offset >= val.size())
      return false;
   return val.limbs()[offset] & mask ? true : false;
}

template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_bit_set(cpp_int_backend<MinBits, Signed, Allocator, false>& val, unsigned index) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   unsigned offset = index / cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   unsigned shift = index % cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   limb_type mask = shift ? limb_type(1u) << shift : limb_type(1u);
   if(offset >= val.size())
   {
      unsigned os = val.size();
      val.resize(offset + 1);
      if(offset >= val.size())
         return;  // fixed precision overflow
      for(unsigned i = os; i <= offset; ++i)
         val.limbs()[i] = 0;
   }
   val.limbs()[offset] |= mask;
}

template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_bit_unset(cpp_int_backend<MinBits, Signed, Allocator, false>& val, unsigned index) BOOST_NOEXCEPT
{
   unsigned offset = index / cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   unsigned shift = index % cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   limb_type mask = shift ? limb_type(1u) << shift : limb_type(1u);
   if(offset >= val.size())
      return;
   val.limbs()[offset] &= ~mask;
   val.normalize();
}

template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_bit_flip(cpp_int_backend<MinBits, Signed, Allocator, false>& val, unsigned index) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   unsigned offset = index / cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   unsigned shift = index % cpp_int_backend<MinBits, Signed, Allocator, false>::limb_bits;
   limb_type mask = shift ? limb_type(1u) << shift : limb_type(1u);
   if(offset >= val.size())
   {
      unsigned os = val.size();
      val.resize(offset + 1);
      if(offset >= val.size())
         return;  // fixed precision overflow
      for(unsigned i = os; i <= offset; ++i)
         val.limbs()[i] = 0;
   }
   val.limbs()[offset] ^= mask;
   val.normalize();
}

template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_lcm(cpp_int_backend<MinBits, Signed, Allocator, false>& result, const cpp_int_backend<MinBits, Signed, Allocator, false>& a, const cpp_int_backend<MinBits, Signed, Allocator, false>& b) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   cpp_int_backend<MinBits, Signed, Allocator, false> t;
   eval_gcd(t, a, b);

   if(eval_is_zero(t))
   {
      result = static_cast<limb_type>(0);
   }
   else
   {
      eval_divide(result, a, t);
      eval_multiply(result, b);
   }
   if(eval_get_sign(result) < 0)
      result.negate();
}

template <unsigned MinBits, bool Signed, class Allocator>
inline void eval_qr(const cpp_int_backend<MinBits, Signed, Allocator, false>& x, const cpp_int_backend<MinBits, Signed, Allocator, false>& y, 
   cpp_int_backend<MinBits, Signed, Allocator, false>& q, cpp_int_backend<MinBits, Signed, Allocator, false>& r) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   divide_unsigned_helper(&q, x, y, r);
   q.sign(x.sign() != y.sign());
   r.sign(x.sign());
}

template <unsigned MinBits, bool Signed, class Allocator, class Integer>
inline typename enable_if<is_unsigned<Integer>, Integer>::type eval_integer_modulus(const cpp_int_backend<MinBits, Signed, Allocator, false>& x, Integer val) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   if((sizeof(Integer) <= sizeof(limb_type)) || (val <= (std::numeric_limits<limb_type>::max)()))
   {
      cpp_int_backend<MinBits, Signed, Allocator, false> d;
      divide_unsigned_helper(static_cast<cpp_int_backend<MinBits, Signed, Allocator, false>*>(0), x, val, d);
      return d.limbs()[0];
   }
   else
   {
      return default_ops::eval_integer_modulus(x, val);
   }
}
template <unsigned MinBits, bool Signed, class Allocator, class Integer>
inline typename enable_if<is_signed<Integer>, Integer>::type eval_integer_modulus(const cpp_int_backend<MinBits, Signed, Allocator, false>& x, Integer val) BOOST_NOEXCEPT_IF(boost::is_void<Allocator>::value)
{
   typedef typename make_unsigned<Integer>::type unsigned_type;
   return eval_integer_modulus(x, static_cast<unsigned_type>(std::abs(val)));
}

} // namespace backends;

using boost::multiprecision::backends::cpp_int_backend;

template <unsigned MinBits, bool Signed, class Allocator, bool trivial>
struct number_category<cpp_int_backend<MinBits, Signed, Allocator, trivial> > : public mpl::int_<number_kind_integer>{};

typedef mp_number<cpp_int_backend<> >          cpp_int;
typedef rational_adapter<cpp_int_backend<> >   cpp_rational_backend;
typedef mp_number<cpp_rational_backend>        cpp_rational;

// Fixed precision unsigned types:
typedef mp_number<cpp_int_backend<128, false, void>, false>   mp_uint128_t;
typedef mp_number<cpp_int_backend<256, false, void>, false>   mp_uint256_t;
typedef mp_number<cpp_int_backend<512, false, void>, false>   mp_uint512_t;
typedef mp_number<cpp_int_backend<1024, false, void>, false>  mp_uint1024_t;

// Fixed precision signed types:
typedef mp_number<cpp_int_backend<128, true, void>, false>    mp_int128_t;
typedef mp_number<cpp_int_backend<256, true, void>, false>    mp_int256_t;
typedef mp_number<cpp_int_backend<512, true, void>, false>    mp_int512_t;
typedef mp_number<cpp_int_backend<1024, true, void>, false>   mp_int1024_t;

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

}} // namespaces

#include <boost/multiprecision/detail/cpp_int_trivial_ops.hpp>


namespace std{

template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
class numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >
{
   typedef boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> number_type;

public:
   BOOST_STATIC_CONSTEXPR bool is_specialized = true;
   //
   // Largest and smallest numbers are bounded only by available memory, set
   // to zero:
   //
   static number_type (min)() BOOST_NOEXCEPT
   {
      return number_type(0);
   }
   static number_type (max)() BOOST_NOEXCEPT 
   { 
      return number_type(0);
   }
   static number_type lowest() BOOST_NOEXCEPT { return (min)(); }
   BOOST_STATIC_CONSTEXPR int digits = 0;
   BOOST_STATIC_CONSTEXPR int digits10 = 0;
   BOOST_STATIC_CONSTEXPR int max_digits10 = 0;
   BOOST_STATIC_CONSTEXPR bool is_signed = true;
   BOOST_STATIC_CONSTEXPR bool is_integer = true;
   BOOST_STATIC_CONSTEXPR bool is_exact = true;
   BOOST_STATIC_CONSTEXPR int radix = 2;
   static number_type epsilon() BOOST_NOEXCEPT { return 0; }
   static number_type round_error() BOOST_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR int min_exponent = 0;
   BOOST_STATIC_CONSTEXPR int min_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR bool has_infinity = false;
   BOOST_STATIC_CONSTEXPR bool has_quiet_NaN = false;
   BOOST_STATIC_CONSTEXPR bool has_signaling_NaN = false;
   BOOST_STATIC_CONSTEXPR float_denorm_style has_denorm = denorm_absent;
   BOOST_STATIC_CONSTEXPR bool has_denorm_loss = false;
   static number_type infinity() BOOST_NOEXCEPT { return 0; }
   static number_type quiet_NaN() BOOST_NOEXCEPT { return 0; }
   static number_type signaling_NaN() BOOST_NOEXCEPT { return 0; }
   static number_type denorm_min() BOOST_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR bool is_iec559 = false;
   BOOST_STATIC_CONSTEXPR bool is_bounded = false;
   BOOST_STATIC_CONSTEXPR bool is_modulo = false;
   BOOST_STATIC_CONSTEXPR bool traps = false;
   BOOST_STATIC_CONSTEXPR bool tinyness_before = false;
   BOOST_STATIC_CONSTEXPR float_round_style round_style = round_toward_zero;
};

#ifndef BOOST_NO_INCLASS_MEMBER_INITIALIZATION

template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::digits;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::digits10;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::max_digits10;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::is_signed;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::is_integer;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::is_exact;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::radix;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::min_exponent;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::min_exponent10;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::max_exponent;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::max_exponent10;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::has_infinity;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::has_quiet_NaN;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::has_signaling_NaN;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST float_denorm_style numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::has_denorm;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::has_denorm_loss;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::is_iec559;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::is_bounded;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::is_modulo;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::traps;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::tinyness_before;
template <unsigned MinBits, bool Signed, class Allocator, bool trivial, bool ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST float_round_style numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, Signed, Allocator, trivial>, ExpressionTemplates> >::round_style;

#endif

template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
class numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates> >
{
   typedef boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates> number_type;

   struct inititializer
   {
      inititializer()
      {
         (std::numeric_limits<number_type>::max)();
         (std::numeric_limits<number_type>::min)();
      }
      void do_nothing()const{}
   };

   static const inititializer init;

public:
   BOOST_STATIC_CONSTEXPR bool is_specialized = true;
   //
   // Largest and smallest numbers are bounded only by available memory, set
   // to zero:
   //
   static number_type (min)() BOOST_NOEXCEPT
   {
      return -(max)();
   }
   static number_type (max)() BOOST_NOEXCEPT 
   {
      typedef typename number_type::backend_type backend_type;
      init.do_nothing();
      static bool init = false;
      static number_type val;
      if(!init)
      {
         boost::multiprecision::limb_type l = ~static_cast<boost::multiprecision::limb_type>(0);
         unsigned c = MinBits / backend_type::limb_bits + (MinBits % backend_type::limb_bits ? 1 : 0);
         for(unsigned i = 0; i < c; ++i)
         {
            val <<= backend_type::limb_bits;
            val |= l;
         }
         init = true;
      }
      return val;
   }
   static number_type lowest() BOOST_NOEXCEPT { return (min)(); }
   BOOST_STATIC_CONSTEXPR int digits = MinBits;
   BOOST_STATIC_CONSTEXPR int digits10 = static_cast<int>(MinBits * 301L / 1000L);
   BOOST_STATIC_CONSTEXPR int max_digits10 = digits10 + 2;
   BOOST_STATIC_CONSTEXPR bool is_signed = true;
   BOOST_STATIC_CONSTEXPR bool is_integer = true;
   BOOST_STATIC_CONSTEXPR bool is_exact = true;
   BOOST_STATIC_CONSTEXPR int radix = 2;
   static number_type epsilon() BOOST_NOEXCEPT { return 0; }
   static number_type round_error() BOOST_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR int min_exponent = 0;
   BOOST_STATIC_CONSTEXPR int min_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR bool has_infinity = false;
   BOOST_STATIC_CONSTEXPR bool has_quiet_NaN = false;
   BOOST_STATIC_CONSTEXPR bool has_signaling_NaN = false;
   BOOST_STATIC_CONSTEXPR float_denorm_style has_denorm = denorm_absent;
   BOOST_STATIC_CONSTEXPR bool has_denorm_loss = false;
   static number_type infinity() BOOST_NOEXCEPT { return 0; }
   static number_type quiet_NaN() BOOST_NOEXCEPT { return 0; }
   static number_type signaling_NaN() BOOST_NOEXCEPT { return 0; }
   static number_type denorm_min() BOOST_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR bool is_iec559 = false;
   BOOST_STATIC_CONSTEXPR bool is_bounded = true;
   BOOST_STATIC_CONSTEXPR bool is_modulo = true;
   BOOST_STATIC_CONSTEXPR bool traps = false;
   BOOST_STATIC_CONSTEXPR bool tinyness_before = false;
   BOOST_STATIC_CONSTEXPR float_round_style round_style = round_toward_zero;
};

template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
const typename numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates> >::inititializer numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates> >::init;

#ifndef BOOST_NO_INCLASS_MEMBER_INITIALIZATION

template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::digits;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::digits10;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::max_digits10;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::is_signed;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::is_integer;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::is_exact;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::radix;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::min_exponent;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::min_exponent10;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::max_exponent;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::max_exponent10;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::has_infinity;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::has_quiet_NaN;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::has_signaling_NaN;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST float_denorm_style numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::has_denorm;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::has_denorm_loss;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::is_iec559;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::is_bounded;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::is_modulo;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::traps;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::tinyness_before;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST float_round_style numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, true, void, trivial>, ExpressionTemplates>  >::round_style;

#endif

template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
class numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >
{
   typedef boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> number_type;

   struct inititializer
   {
      inititializer()
      {
         (std::numeric_limits<number_type>::max)();
      }
      void do_nothing()const{}
   };

   static const inititializer init;

public:
   BOOST_STATIC_CONSTEXPR bool is_specialized = true;
   //
   // Largest and smallest numbers are bounded only by available memory, set
   // to zero:
   //
   static number_type (min)() BOOST_NOEXCEPT
   {
      return number_type(0);
   }
   static number_type (max)() BOOST_NOEXCEPT 
   {
      typedef typename number_type::backend_type backend_type;
      init.do_nothing();
      static bool init = false;
      static number_type val(0);
      if(!init)
      {
         boost::multiprecision::limb_type l = ~static_cast<boost::multiprecision::limb_type>(0);
         unsigned c = MinBits / backend_type::limb_bits + (MinBits % backend_type::limb_bits ? 1 : 0);
         for(unsigned i = 0; i < c; ++i)
         {
            val <<= backend_type::limb_bits;
            val |= l;
         }
         init = true;
      }
      return val;
   }
   static number_type lowest() BOOST_NOEXCEPT { return (min)(); }
   BOOST_STATIC_CONSTEXPR int digits = MinBits;
   BOOST_STATIC_CONSTEXPR int digits10 = static_cast<int>(MinBits * 301L / 1000L);
   BOOST_STATIC_CONSTEXPR int max_digits10 = digits10 + 2;
   BOOST_STATIC_CONSTEXPR bool is_signed = false;
   BOOST_STATIC_CONSTEXPR bool is_integer = true;
   BOOST_STATIC_CONSTEXPR bool is_exact = true;
   BOOST_STATIC_CONSTEXPR int radix = 2;
   static number_type epsilon() BOOST_NOEXCEPT { return 0; }
   static number_type round_error() BOOST_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR int min_exponent = 0;
   BOOST_STATIC_CONSTEXPR int min_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR bool has_infinity = false;
   BOOST_STATIC_CONSTEXPR bool has_quiet_NaN = false;
   BOOST_STATIC_CONSTEXPR bool has_signaling_NaN = false;
   BOOST_STATIC_CONSTEXPR float_denorm_style has_denorm = denorm_absent;
   BOOST_STATIC_CONSTEXPR bool has_denorm_loss = false;
   static number_type infinity() BOOST_NOEXCEPT { return 0; }
   static number_type quiet_NaN() BOOST_NOEXCEPT { return 0; }
   static number_type signaling_NaN() BOOST_NOEXCEPT { return 0; }
   static number_type denorm_min() BOOST_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR bool is_iec559 = false;
   BOOST_STATIC_CONSTEXPR bool is_bounded = true;
   BOOST_STATIC_CONSTEXPR bool is_modulo = true;
   BOOST_STATIC_CONSTEXPR bool traps = false;
   BOOST_STATIC_CONSTEXPR bool tinyness_before = false;
   BOOST_STATIC_CONSTEXPR float_round_style round_style = round_toward_zero;
};

template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
const typename numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::inititializer numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::init;

#ifndef BOOST_NO_INCLASS_MEMBER_INITIALIZATION

template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::digits;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::digits10;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::max_digits10;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::is_signed;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::is_integer;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::is_exact;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::radix;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::min_exponent;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::min_exponent10;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::max_exponent;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::max_exponent10;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::has_infinity;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::has_quiet_NaN;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::has_signaling_NaN;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST float_denorm_style numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::has_denorm;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::has_denorm_loss;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::is_iec559;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::is_bounded;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::is_modulo;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::traps;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST bool numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::tinyness_before;
template <unsigned MinBits, bool ExpressionTemplates, bool trivial>
BOOST_CONSTEXPR_OR_CONST float_round_style numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<MinBits, false, void, trivial>, ExpressionTemplates> >::round_style;

#endif

}

#endif
