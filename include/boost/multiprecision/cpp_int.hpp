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
#include <boost/array.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/multiprecision/fixed_int.hpp>
#include <boost/multiprecision/rational_adapter.hpp>

namespace boost{
namespace multiprecision{

   /*
typedef boost::uint32_t limb_type;
typedef boost::int32_t signed_limb_type;
typedef boost::uint64_t double_limb_type;
typedef boost::int64_t signed_double_limb_type;
static const limb_type max_block_10 = 1000000000;
static const limb_type digits_per_block_10 = 9;

inline limb_type block_multiplier(int count)
{
   static const limb_type values[digits_per_block_10] 
      = { 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
   BOOST_ASSERT(count < digits_per_block_10);
   return values[count];
}
*/
template <class T>
inline void minmax(const T& a, const T& b, T& aa, T& bb)
{
   if(a < b)
   {
      aa = a;
      bb = b;
   }
   else
   {
      aa = b;
      bb = a;
   }
}

template <unsigned InternalLimbs = 0, class Allocator = std::allocator<limb_type> >
struct cpp_int_backend : private Allocator::template rebind<limb_type>::other
{
   typedef typename Allocator::template rebind<limb_type>::other allocator_type;
   typedef typename allocator_type::pointer                      limb_pointer;
   typedef typename allocator_type::const_pointer                const_limb_pointer;

private:
   struct limb_data
   {
      unsigned capacity;
      limb_pointer data;
   };

public:
   typedef mpl::list<signed_limb_type, signed_double_limb_type>      signed_types;
   typedef mpl::list<limb_type, double_limb_type>                    unsigned_types;
   typedef mpl::list<long double>                                    float_types;

   BOOST_STATIC_CONSTANT(unsigned, limb_bits = sizeof(limb_type) * CHAR_BIT);
   BOOST_STATIC_CONSTANT(limb_type, max_limb_value = ~static_cast<limb_type>(0u));
   BOOST_STATIC_CONSTANT(limb_type, sign_bit_mask = 1u << (limb_bits - 1));
   BOOST_STATIC_CONSTANT(unsigned, internal_limb_count = InternalLimbs ? InternalLimbs : sizeof(limb_data) / sizeof(limb_type));

private:

   union data_type
   {
      limb_data ld;
      limb_type la[internal_limb_count];
   };

   data_type   m_data;
   unsigned    m_limbs;
   bool        m_sign, m_internal;

public:

   //
   // Helper functions for getting at our internal data, and manipulating storage:
   //
   allocator_type& allocator(){ return *this; }
   const allocator_type& allocator()const{ return *this; }
   unsigned size()const { return m_limbs; }
   limb_pointer limbs() { return m_internal ? m_data.la : m_data.ld.data; }
   const_limb_pointer limbs()const { return m_internal ? m_data.la : m_data.ld.data; }
   unsigned capacity()const { return m_internal ? internal_limb_count : m_data.ld.capacity; }
   bool sign()const { return m_sign; }
   void sign(bool b) 
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
         std::memcpy(pl, limbs(), size() * sizeof(limb_type));
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

   cpp_int_backend() : m_limbs(1), m_internal(true), m_sign(false)
   {
      *limbs() = 0;
   }
   cpp_int_backend(const cpp_int_backend& o) : m_limbs(0), m_internal(true)
   {
      resize(o.size());
      std::memcpy(limbs(), o.limbs(), size() * sizeof(limb_type));
      m_sign = o.m_sign;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   cpp_int_backend(cpp_int_backend&& o) : m_limbs(o.m_limbs), m_sign(o.m_sign), m_internal(o.m_internal) 
   {
      if(m_internal)
      {
         std::memcpy(limbs(), o.limbs(), size() * sizeof(limb_type));
      }
      else
      {
         data.ld = o.data.ld;
         o.m_limbs = 0;
         o.m_internal = true;
      }
   }
#endif
   ~cpp_int_backend()
   {
      if(!m_internal)
         allocator().deallocate(limbs(), capacity());
   }
   cpp_int_backend& operator = (const cpp_int_backend& o)
   {
      if(this != &o)
      {
         m_limbs = 0;
         resize(o.size());
         std::memcpy(limbs(), o.limbs(), size() * sizeof(limb_type));
         m_sign = o.m_sign;
      }
      return *this;
   }
   cpp_int_backend& operator = (limb_type i)
   {
      m_limbs = 1;
      *limbs() = i;
      m_sign = false;
      return *this;
   }
   cpp_int_backend& operator = (signed_limb_type i)
   {
      m_limbs = 1;
      *limbs() = static_cast<limb_type>(std::abs(i));
      m_sign = i < 0;
      return *this;
   }
   cpp_int_backend& operator = (double_limb_type i)
   {
      BOOST_STATIC_ASSERT(sizeof(i) == 2 * sizeof(limb_type));
      BOOST_STATIC_ASSERT(internal_limb_count >= 2);
      limb_pointer p = limbs();
      *p = static_cast<limb_type>(i);
      p[1] = static_cast<limb_type>(i >> limb_bits);
      m_limbs = p[1] ? 2 : 1;
      m_sign = false;
      return *this;
   }
   cpp_int_backend& operator = (signed_double_limb_type i)
   {
      BOOST_STATIC_ASSERT(sizeof(i) == 2 * sizeof(limb_type));
      BOOST_STATIC_ASSERT(internal_limb_count >= 2);
      if(i < 0)
      {
         m_sign = true;
         i = -i;
      }
      else
         m_sign = false;
      limb_pointer p = limbs();
      *p = static_cast<limb_type>(i);
      p[1] = static_cast<limb_type>(i >> limb_bits);
      m_limbs = p[1] ? 2 : 1;
      return *this;
   }

   cpp_int_backend& operator = (long double a)
   {
      using default_ops::add;
      using default_ops::subtract;
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
         left_shift(*this, shift);
         if(term > 0)
            add(*this, static_cast<limb_type>(term));
         else
            subtract(*this, static_cast<limb_type>(-term));
         f -= term;
      }
      if(e > 0)
         left_shift(*this, e);
      else if(e < 0)
         right_shift(*this, -e);
      return *this;
   }
   cpp_int_backend& operator = (const char* s)
   {
      using default_ops::multiply;
      using default_ops::add;
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
            unsigned block_count = limb_bits / shift;
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
                     val = max_limb_value;
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
               left_shift(*this, block_shift);
               limbs()[0] |= block;
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
               multiply(*this, block_mult);
               add(*this, block);
            }
         }
      }
      if(isneg)
         negate();
      return *this;
   }
   void swap(cpp_int_backend& o)
   {
      std::swap(m_data, o.m_data);
      std::swap(m_sign, o.m_sign);
      std::swap(m_internal, o.m_internal);
      std::swap(m_limbs, o.m_limbs);
   }
   std::string str(std::streamsize digits, std::ios_base::fmtflags f)const
   {
      int base = 10;
      if((f & std::ios_base::oct) == std::ios_base::oct)
         base = 8;
      else if((f & std::ios_base::hex) == std::ios_base::hex)
         base = 16;
      std::string result;

      unsigned Bits = size() * limb_bits;

      if(base == 8 || base == 16)
      {
         limb_type shift = base == 8 ? 3 : 4;
         limb_type mask = static_cast<limb_type>((1u << shift) - 1);
         cpp_int_backend t(*this);
         result.assign(Bits / shift + (Bits % shift ? 1 : 0), '0');
         int pos = result.size() - 1;
         for(unsigned i = 0; i < Bits / shift; ++i)
         {
            char c = '0' + (t.limbs()[0] & mask);
            if(c > '9')
               c += 'A' - '9' - 1;
            result[pos--] = c;
            right_shift(t, shift);
         }
         if(Bits % shift)
         {
            mask = static_cast<limb_type>((1u << (Bits % shift)) - 1);
            char c = '0' + (t.limbs()[0] & mask);
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
         if(size() == 1)
         {
            result = boost::lexical_cast<std::string>(t.limbs()[0]);
         }
         else
         {
            cpp_int_backend block10;
            block10 = max_block_10;
            while(get_sign(t) != 0)
            {
               cpp_int_backend t2;
               divide_unsigned_helper(t2, t, block10, r);
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
   void negate()
   {
      m_sign = !m_sign;
      // Check for zero value:
      if(m_sign && (m_limbs == 1))
      {
         if(limbs()[0] == 0)
            m_sign = false;
      }
   }
   bool isneg()const
   {
      return m_sign; 
   }
   int compare(const cpp_int_backend& o)const
   {
      if(m_sign != o.m_sign)
         return m_sign ? -1 : 1;
      int result = 0;
      // Only do the compare if the same sign:
      result = compare_unsigned(o);

      if(m_sign)
         result = -result;
      return result;
   }
   int compare_unsigned(const cpp_int_backend& o)const
   {
      if(m_limbs != o.m_limbs)
      {
         return m_limbs > o.m_limbs ? 1 : -1;
      }
      const_limb_pointer pa = limbs();
      const_limb_pointer pb = o.limbs();
      for(int i = m_limbs - 1; i >= 0; --i)
      {
         if(pa[i] != pb[i])
            return pa[i] > pb[i] ? 1 : -1;
      }
      return 0;
   }
   template <class Arithmatic>
   typename enable_if<is_arithmetic<Arithmatic>, int>::type compare(Arithmatic i)const
   {
      // braindead version:
      cpp_int_backend t;
      t = i;
      return compare(t);
   }
};

template <unsigned InternalLimbs, class Allocator>
const unsigned cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
template <unsigned InternalLimbs, class Allocator>
const limb_type cpp_int_backend<InternalLimbs, Allocator>::max_limb_value;
template <unsigned InternalLimbs, class Allocator>
const limb_type cpp_int_backend<InternalLimbs, Allocator>::sign_bit_mask;
template <unsigned InternalLimbs, class Allocator>
const unsigned cpp_int_backend<InternalLimbs, Allocator>::internal_limb_count;


template <unsigned InternalLimbs, class Allocator>
inline void add(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& o)
{
   add(result, result, o);
}
template <unsigned InternalLimbs, class Allocator>
inline void add_unsigned(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, const cpp_int_backend<InternalLimbs, Allocator>& b)
{
   using std::swap;

   // Nothing fancy, just let uintmax_t take the strain:
   double_limb_type carry = 0;
   unsigned m, x;
   minmax(a.size(), b.size(), m, x);
   if(x == 1)
   {
      bool s = a.sign();
      result = static_cast<double_limb_type>(*a.limbs()) + static_cast<double_limb_type>(*b.limbs());
      result.sign(s);
      return;
   }
   result.resize(x);
   typename cpp_int_backend<InternalLimbs, Allocator>::const_limb_pointer pa = a.limbs();
   typename cpp_int_backend<InternalLimbs, Allocator>::const_limb_pointer pb = b.limbs();
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pr = result.limbs();
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pr_end = pr + m;

   if(a.size() < b.size())
      swap(pa, pb);
   
   // First where a and b overlap:
   while(pr != pr_end)
   {
      carry += static_cast<double_limb_type>(*pa) + static_cast<double_limb_type>(*pb);
      *pr = static_cast<limb_type>(carry);
      carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
      ++pr, ++pa, ++pb;
   }
   pr_end += x - m;
   // Now where only a has digits:
   while(pr != pr_end)
   {
      if(!carry)
      {
         if(pa != pr)
            std::memcpy(pr, pa, (pr_end - pr) * sizeof(limb_type));
         break;
      }
      carry += static_cast<double_limb_type>(*pa);
      *pr = static_cast<limb_type>(carry);
      carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
      ++pr, ++pa;
   }
   if(carry)
   {
      result.resize(x + 1);
      // We overflowed, need to add one more limb:
      result.limbs()[x] = static_cast<limb_type>(carry);
   }
   result.sign(a.sign());
}
template <unsigned InternalLimbs, class Allocator>
inline void add(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, const cpp_int_backend<InternalLimbs, Allocator>& b)
{
   if(a.sign() != b.sign())
   {
      subtract_unsigned(result, a, b);
      return;
   }
   add_unsigned(result, a, b);
}

template <unsigned InternalLimbs, class Allocator>
inline void add_unsigned(cpp_int_backend<InternalLimbs, Allocator>& result, const limb_type& o)
{
   // Addition using modular arithmatic.
   // Nothing fancy, just let uintmax_t take the strain:
   double_limb_type carry = o;
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pr = result.limbs();
   for(unsigned i = 0; carry && (i < result.size()); ++i)
   {
      carry += static_cast<double_limb_type>(pr[i]);
      pr[i] = static_cast<limb_type>(carry);
      carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
   }
}
template <unsigned InternalLimbs, class Allocator>
inline void add(cpp_int_backend<InternalLimbs, Allocator>& result, const limb_type& o)
{
   if(result.sign())
   {
      subtract_unsigned(result, o);
   }
   else
      add_unsigned(result, o);
}
template <unsigned InternalLimbs, class Allocator>
inline void add(cpp_int_backend<InternalLimbs, Allocator>& result, const signed_limb_type& o)
{
   if(o < 0)
      subtract(result, static_cast<limb_type>(-o));
   else if(o > 0)
      add(result, static_cast<limb_type>(o));
}

template <unsigned InternalLimbs, class Allocator>
inline void subtract_unsigned(cpp_int_backend<InternalLimbs, Allocator>& result, const limb_type& o)
{
   // Subtract one limb.
   // Nothing fancy, just let uintmax_t take the strain:
   BOOST_STATIC_CONSTANT(double_limb_type, borrow = static_cast<double_limb_type>(cpp_int_backend<InternalLimbs, Allocator>::max_limb_value) + 1);
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer p = result.limbs();
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
         *p = cpp_int_backend<InternalLimbs, Allocator>::max_limb_value;
         ++p;
      }
      --*p;
      if(!result.limbs()[result.size() - 1])
         result.resize(result.size() - 1);
   }
}
template <unsigned InternalLimbs, class Allocator>
inline void subtract(cpp_int_backend<InternalLimbs, Allocator>& result, const limb_type& o)
{
   if(result.sign())
      add_unsigned(result, o);
   else
      subtract_unsigned(result, o);
}
template <unsigned InternalLimbs, class Allocator>
inline void subtract(cpp_int_backend<InternalLimbs, Allocator>& result, const signed_limb_type& o)
{
   if(o)
   {
      if(o < 0)
         add(result, static_cast<limb_type>(-o));
      else
         subtract(result, static_cast<limb_type>(o));
   }
}
template <unsigned InternalLimbs, class Allocator>
inline void increment(cpp_int_backend<InternalLimbs, Allocator>& result)
{
   static const limb_type one = 1;
   if(!result.sign() && (result.limbs()[0] < cpp_int_backend<InternalLimbs, Allocator>::max_limb_value))
      ++result.limbs()[0];
   else if(result.sign() && result.limbs()[0])
      --result.limbs()[0];
   else
      add(result, one);
}
template <unsigned InternalLimbs, class Allocator>
inline void decrement(cpp_int_backend<InternalLimbs, Allocator>& result)
{
   static const limb_type one = 1;
   if(!result.sign() && result.limbs()[0])
      --result.limbs()[0];
   else if(result.sign() && (result.limbs()[0] < cpp_int_backend<InternalLimbs, Allocator>::max_limb_value))
      ++result.limbs()[0];
   else
      subtract(result, one);
}
template <unsigned InternalLimbs, class Allocator>
inline void subtract(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& o)
{
   subtract(result, result, o);
}
template <unsigned InternalLimbs, class Allocator>
inline void subtract_unsigned(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, const cpp_int_backend<InternalLimbs, Allocator>& b)
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
   }/*
   else if(m == 1)
   {
      if(b.size() == 1)
      {
         result = a;
         subtract_unsigned(result, *b.limbs());
      }
      else
      {
         result = b;
         subtract_unsigned(result, *a.limbs());
         result.negate();
      }
      return;
   }*/
   result.resize(x);
   typename cpp_int_backend<InternalLimbs, Allocator>::const_limb_pointer pa = a.limbs();
   typename cpp_int_backend<InternalLimbs, Allocator>::const_limb_pointer pb = b.limbs();
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pr = result.limbs();
   bool swapped = false;
   int c = a.compare_unsigned(b);
   if(c < 0)
   {
      swap(pa, pb);
      swapped = true;
   }
   else if(c == 0)
   {
      result = 0;
      return;
   }
   
   unsigned i = 0;
   BOOST_STATIC_CONSTANT(double_limb_type, borrow_value = static_cast<double_limb_type>(cpp_int_backend<InternalLimbs, Allocator>::max_limb_value) + 1);
   // First where a and b overlap:
   while(i < m)
   {
      borrow += pb[i];
      borrow = (borrow_value + static_cast<double_limb_type>(pa[i])) - borrow;
      pr[i] = static_cast<limb_type>(borrow);
      borrow = borrow & borrow_value ? 0 : 1;
      ++i;
   }
   // Now where only a has digits, only as long as we've borrowed:
   while(borrow && (i < x))
   {
      borrow = (borrow_value + static_cast<double_limb_type>(pa[i])) - borrow;
      pr[i] = static_cast<limb_type>(borrow);
      borrow = borrow & borrow_value ? 0 : 1;
      ++i;
   }
   // Any remaining digits are the same as those in pa:
   if((x != i) && (pa != pr))
      std::memcpy(pr + i, pa + i, (x - i) * sizeof(limb_type));
   BOOST_ASSERT(0 == borrow);

   //
   // We may have lost digits, if so update limb usage count:
   //
   i = result.size() - 1;
   while(!pr[i] && i)
   {
      --i;
   }
   result.resize(i + 1);
   result.sign(a.sign());
   if(swapped)
      result.negate();
}
template <unsigned InternalLimbs, class Allocator>
inline void subtract(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, const cpp_int_backend<InternalLimbs, Allocator>& b)
{
   if(a.sign() != b.sign())
   {
      add_unsigned(result, a, b);
      return;
   }
   subtract_unsigned(result, a, b);
}
template <unsigned InternalLimbs, class Allocator>
inline void multiply(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, const cpp_int_backend<InternalLimbs, Allocator>& b)
{
   // Very simple long multiplication, only usable for small numbers of limb_type's
   // but that's the typical use case for this type anyway:
   //
   // Special cases first:
   //
   unsigned as = a.size();
   unsigned bs = b.size();
   typename cpp_int_backend<InternalLimbs, Allocator>::const_limb_pointer pa = a.limbs();
   typename cpp_int_backend<InternalLimbs, Allocator>::const_limb_pointer pb = b.limbs();
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
         result = b;
         multiply(result, l);
      }
      result.sign(s);
      return;
   }
   if(bs == 1)
   {
      limb_type l = *pb;
      bool s = b.sign() != a.sign();
      result = a;
      multiply(result, l);
      result.sign(s);
      return;
   }

   if(&result == &a)
   {
      cpp_int_backend<InternalLimbs, Allocator> t(a);
      multiply(result, t, b);
      return;
   }
   if(&result == &b)
   {
      cpp_int_backend<InternalLimbs, Allocator> t(b);
      multiply(result, a, t);
      return;
   }

   result.resize(as + bs);
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pr = result.limbs();

   double_limb_type carry = 0;
   std::memset(pr, 0, (as + bs) * sizeof(limb_type));
   for(unsigned i = 0; i < as; ++i)
   {
      for(unsigned j = 0; j < bs; ++j)
      {
         carry += static_cast<double_limb_type>(pa[i]) * static_cast<double_limb_type>(pb[j]);
         carry += pr[i + j];
         pr[i + j] = static_cast<limb_type>(carry);
         carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
      }
      pr[i + bs] = static_cast<limb_type>(carry);
      carry = 0;
   }
   //
   // We may not have filled all the digits, if so update limb usage count:
   //
   unsigned i = result.size() - 1;
   while(!pr[i] && i)
   {
      --i;
   }
   result.resize(i + 1);
   //
   // Set the sign of the result:
   //
   result.sign(a.sign() != b.sign());
}
template <unsigned InternalLimbs, class Allocator>
inline void multiply(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a)
{
    multiply(result, result, a);
}
template <unsigned InternalLimbs, class Allocator>
inline void multiply(cpp_int_backend<InternalLimbs, Allocator>& result, cpp_int_backend<InternalLimbs, Allocator>& a, const limb_type& val)
{
   if(!val)
   {
      result = 0;
      return;
   }
   double_limb_type carry = 0;
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer p = result.limbs();
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pe = result.limbs() + result.size();
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pa = a.limbs();
   while(p != pe)
   {
      carry += static_cast<double_limb_type>(*pa * static_cast<double_limb_type>(val));
      *p = static_cast<limb_type>(carry);
      carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
      ++p, ++pa;
   }
   if(carry)
   {
      result.resize(result.size() + 1);
      result.limbs()[result.size() - 1] = static_cast<limb_type>(carry);
   }
}
template <unsigned InternalLimbs, class Allocator>
inline void multiply(cpp_int_backend<InternalLimbs, Allocator>& result, const limb_type& val)
{
   multiply(result, result, val);
}
template <unsigned InternalLimbs, class Allocator>
inline void multiply(cpp_int_backend<InternalLimbs, Allocator>& result, cpp_int_backend<InternalLimbs, Allocator>& a, const signed_limb_type& val)
{
   if(val > 0)
      multiply(result, a, static_cast<limb_type>(val));
   else
   {
      multiply(result, a, static_cast<limb_type>(-val));
      result.negate();
   }
}
template <unsigned InternalLimbs, class Allocator>
inline void multiply(cpp_int_backend<InternalLimbs, Allocator>& result, const signed_limb_type& val)
{
   multiply(result, result, val);
}
template <unsigned InternalLimbs, class Allocator>
void divide_unsigned_helper(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& x, const cpp_int_backend<InternalLimbs, Allocator>& y, cpp_int_backend<InternalLimbs, Allocator>& r)
{
   if((&result == &x) || (&r == &x))
   {
      cpp_int_backend<InternalLimbs, Allocator> t(x);
      divide_unsigned_helper(result, t, y, r);
      return;
   }
   if((&result == &y) || (&r == &y))
   {
      cpp_int_backend<InternalLimbs, Allocator> t(y);
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


   using default_ops::subtract;

   if(&result == &r)
   {
      cpp_int_backend<InternalLimbs, Allocator> rem;
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

   typename cpp_int_backend<InternalLimbs, Allocator>::const_limb_pointer px = x.limbs();
   typename cpp_int_backend<InternalLimbs, Allocator>::const_limb_pointer py = y.limbs();

   limb_type r_order = x.size() - 1;
   if((r_order == 0) && (*px == 0))
   {
      // x is zero, so is the result:
      r = y;
      result = x;
      return;
   }

   r = x;
   r.sign(false);
   result = static_cast<limb_type>(0u);
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

   cpp_int_backend<InternalLimbs, Allocator> t;
   bool r_neg = false;

   //
   // See if we can short-circuit long division, and use basic arithmetic instead:
   //
   if(r_order == 0)
   {
      result = px[0] / py[0];
      r = px[0] % py[0];
      return;
   }
   else if(r_order == 1)
   {
      double_limb_type a, b;
      a = (static_cast<double_limb_type>(px[1]) << cpp_int_backend<InternalLimbs, Allocator>::limb_bits) | px[0];
      b = y_order > 1 ? 
         (static_cast<double_limb_type>(py[1]) << cpp_int_backend<InternalLimbs, Allocator>::limb_bits) | py[0] 
         : py[0];
      result = a / b;
      r = a % b;
      return;
   }
   //
   // prepare result:
   //
   result.resize(1 + r_order - y_order);
   typename cpp_int_backend<InternalLimbs, Allocator>::const_limb_pointer prem = r.limbs();
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pr = result.limbs();
   for(unsigned i = 1; i < 1 + r_order - y_order; ++i)
      pr[i] = 0;
   bool first_pass = true;

   do
   {
      //
      // Update r_order, this can't run past the end as r must be non-zero at this point:
      //
      r_order = r.size() - 1;
      //
      // Calculate our best guess for how many times y divides into r:
      //
      limb_type guess;
      if((prem[r_order] <= py[y_order]) && (r_order > 0))
      {
         double_limb_type a, b, v;
         a = (static_cast<double_limb_type>(prem[r_order]) << cpp_int_backend<InternalLimbs, Allocator>::limb_bits) | prem[r_order - 1];
         b = py[y_order];
         v = a / b;
         if(v > cpp_int_backend<InternalLimbs, Allocator>::max_limb_value)
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
         a = (static_cast<double_limb_type>(prem[r_order]) << cpp_int_backend<InternalLimbs, Allocator>::limb_bits) | prem[r_order - 1];
         b = (y_order > 0) ? (static_cast<double_limb_type>(py[y_order]) << cpp_int_backend<InternalLimbs, Allocator>::limb_bits) | py[y_order - 1] : (static_cast<double_limb_type>(py[y_order])  << cpp_int_backend<InternalLimbs, Allocator>::limb_bits);
         v = a / b;
         guess = static_cast<limb_type>(v);
      }
      BOOST_ASSERT(guess); // If the guess ever gets to zero we go on forever....
      //
      // Update result:
      //
      limb_type shift = r_order - y_order;
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
            subtract(result, t);
         }
      }
      else if(cpp_int_backend<InternalLimbs, Allocator>::max_limb_value - pr[shift] > guess)
         pr[shift] += guess;
      else
      {
         t.resize(shift + 1);
         t.limbs()[shift] = guess;
         for(unsigned i = 0; i < shift; ++i)
            t.limbs()[i] = 0;
         add(result, t);
      }
      //
      // Calculate guess * y, we use a fused mutiply-shift O(N) for this
      // rather than a full O(N^2) multiply:
      //
      double_limb_type carry = 0;
      t.resize(y.size() + shift + 1);
      typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pt = t.limbs();
      for(unsigned i = 0; i < shift; ++i)
         pt[i] = 0;
      for(unsigned i = 0; i < y.size(); ++i)
      {
         carry += static_cast<double_limb_type>(py[i]) * static_cast<double_limb_type>(guess);
         pt[i + shift] = static_cast<limb_type>(carry);
         carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
      }
      if(carry)
      {
         pt[t.size() - 1] = static_cast<limb_type>(carry);
      }
      else
      {
         t.resize(t.size() - 1);
      }
      //
      // Update r:
      //
      subtract(r, t);
      if(r.isneg())
      {
         r.negate();
         r_neg = !r_neg;
      }
      //
      // First time through we need to strip any leading zero, otherwise
      // the termination condition goes belly-up:
      //
      if(first_pass)
      {
         first_pass = false;
         while(pr[result.size() - 1] == 0)
            result.resize(result.size() - 1);
      }
   }
   // Termination condition is really just a check that r > y, but with two common
   // short-circuit cases handled first:
   while((r_order > y_order) || (prem[r_order] > py[y_order]) || (r.compare_unsigned(y) > 0));

   //
   // We now just have to normalise the result:
   //
   if(r_neg)
   {
      // We have one too many in the result:
      decrement(result);
      r.negate();
      if(y.sign())
         subtract(r, y);
      else
         add(r, y);
   }

   BOOST_ASSERT(r.compare_unsigned(y) < 0); // remainder must be less than the divisor or our code has failed
}

template <unsigned InternalLimbs, class Allocator>
void divide_unsigned_helper(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& x, limb_type y, cpp_int_backend<InternalLimbs, Allocator>& r)
{
   if((&result == &x) || (&r == &x))
   {
      cpp_int_backend<InternalLimbs, Allocator> t(x);
      divide_unsigned_helper(result, t, y, r);
      return;
   }

   if(&result == &r)
   {
      cpp_int_backend<InternalLimbs, Allocator> rem;
      divide_unsigned_helper(result, x, y, rem);
      r = rem;
      return;
   }

   // As above, but simplified for integer divisor:

   using default_ops::subtract;

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
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pr = r.limbs();

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
      result = static_cast<limb_type>(0u);
      return;
   }

   //
   // See if we can short-circuit long division, and use basic arithmetic instead:
   //
   if(r_order == 0)
   {
      result = *pr / y;
      result.sign(x.sign());
      r.sign(x.sign());
      *pr %= y;
      return;
   }
   else if(r_order == 1)
   {
      double_limb_type a;
      a = (static_cast<double_limb_type>(pr[r_order]) << cpp_int_backend<InternalLimbs, Allocator>::limb_bits) | pr[0];
      result = a / y;
      result.sign(x.sign());
      r = a % y;
      r.sign(x.sign());
      return;
   }

   result.resize(r_order + 1);
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pres = result.limbs();
   pres[r_order] = 0;  // just in case we don't set the most significant limb below.

   do
   {
      //
      // Calculate our best guess for how many times y divides into r:
      //
      if((pr[r_order] < y) && r_order)
      {
         double_limb_type a, b;
         a = (static_cast<double_limb_type>(pr[r_order]) << cpp_int_backend<InternalLimbs, Allocator>::limb_bits) | pr[r_order - 1];
         b = a % y;
         r.resize(r.size() - 1);
         --r_order;
         pr[r_order] = static_cast<limb_type>(b);
         pres[r_order] = static_cast<limb_type>(a / y);
      }
      else
      {
         pres[r_order] = pr[r_order] / y;
         pr[r_order] %= y;
      }
   }
   // Termination condition is really just a check that r > y, but with two common
   // short-circuit cases handled first:
   while(r_order || (pr[r_order] > y));

   if(pres[result.size() - 1] == 0)
      result.resize(result.size() - 1);

   result.sign(x.sign());
   r.sign(x.sign());

   BOOST_ASSERT(r.compare(y) < 0); // remainder must be less than the divisor or our code has failed
}

template <unsigned InternalLimbs, class Allocator>
inline void divide(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, const cpp_int_backend<InternalLimbs, Allocator>& b)
{
   cpp_int_backend<InternalLimbs, Allocator> r;
   divide_unsigned_helper(result, a, b, r);
   result.sign(a.sign() != b.sign());
}
template <unsigned InternalLimbs, class Allocator>
inline void divide(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, limb_type& b)
{
   cpp_int_backend<InternalLimbs, Allocator> r;
   divide_unsigned_helper(result, a, b, r);
}
template <unsigned InternalLimbs, class Allocator>
inline void divide(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, signed_limb_type& b)
{
   cpp_int_backend<InternalLimbs, Allocator> r;
   divide_unsigned_helper(result, a, std::abs(b), r);
   if(b < 0)
      result.negate();
}
template <unsigned InternalLimbs, class Allocator>
inline void divide(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& b)
{
   // There is no in place divide:
   cpp_int_backend<InternalLimbs, Allocator> a(result);
   divide(result, a, b);
}
template <unsigned InternalLimbs, class Allocator>
inline void divide(cpp_int_backend<InternalLimbs, Allocator>& result, limb_type b)
{
   // There is no in place divide:
   cpp_int_backend<InternalLimbs, Allocator> a(result);
   divide(result, a, b);
}
template <unsigned InternalLimbs, class Allocator>
inline void divide(cpp_int_backend<InternalLimbs, Allocator>& result, signed_limb_type b)
{
   // There is no in place divide:
   cpp_int_backend<InternalLimbs, Allocator> a(result);
   divide(result, a, b);
}
template <unsigned InternalLimbs, class Allocator>
inline void modulus(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, const cpp_int_backend<InternalLimbs, Allocator>& b)
{
   cpp_int_backend<InternalLimbs, Allocator> r;
   divide_unsigned_helper(r, a, b, result);
   result.sign(a.sign());
}
template <unsigned InternalLimbs, class Allocator>
inline void modulus(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, limb_type b)
{
   cpp_int_backend<InternalLimbs, Allocator> r;
   divide_unsigned_helper(r, a, b, result);
}
template <unsigned InternalLimbs, class Allocator>
inline void modulus(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, signed_limb_type b)
{
   cpp_int_backend<InternalLimbs, Allocator> r;
   divide_unsigned_helper(r, a, static_cast<limb_type>(std::abs(b)), result);
}
template <unsigned InternalLimbs, class Allocator>
inline void modulus(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& b)
{
   // There is no in place divide:
   cpp_int_backend<InternalLimbs, Allocator> a(result);
   modulus(result, a, b);
}
template <unsigned InternalLimbs, class Allocator>
inline void modulus(cpp_int_backend<InternalLimbs, Allocator>& result, limb_type b)
{
   // There is no in place divide:
   cpp_int_backend<InternalLimbs, Allocator> a(result);
   modulus(result, a, b);
}
template <unsigned InternalLimbs, class Allocator>
inline void modulus(cpp_int_backend<InternalLimbs, Allocator>& result, signed_limb_type b)
{
   // There is no in place divide:
   cpp_int_backend<InternalLimbs, Allocator> a(result);
   modulus(result, a, b);
}
template <unsigned InternalLimbs, class Allocator, class Op>
void bitwise_op(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& o, Op op)
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
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pr = result.limbs();
   typename cpp_int_backend<InternalLimbs, Allocator>::const_limb_pointer po = o.limbs();
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
            carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
         }
         for(unsigned i = os; i < x; ++i)
         {
            carry += static_cast<double_limb_type>(~limb_type(0));
            pr[i] = op(pr[i], static_cast<limb_type>(carry));
            carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
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
            carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
         }
         for(unsigned i = os; i < x; ++i)
         {
            carry += static_cast<double_limb_type>(~pr[i]);
            pr[i] = op(static_cast<limb_type>(carry), limb_type(0));
            carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
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
            r_carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
            o_carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
         }
         for(unsigned i = os; i < x; ++i)
         {
            r_carry += static_cast<double_limb_type>(~pr[i]);
            o_carry += static_cast<double_limb_type>(~limb_type(0));
            pr[i] = op(static_cast<limb_type>(r_carry), static_cast<limb_type>(o_carry));
            r_carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
            o_carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
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
         carry >>= cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
      }
   }
   else
      result.sign(false);
   //
   // Strip leading zeros:
   //
   while(result.limbs()[result.size()-1] == 0)
      result.resize(result.size()-1);
}

struct bit_and{ limb_type operator()(limb_type a, limb_type b)const{ return a & b; } };
struct bit_or{ limb_type operator()(limb_type a, limb_type b)const{ return a | b; } };
struct bit_xor{ limb_type operator()(limb_type a, limb_type b)const{ return a ^ b; } };

template <unsigned InternalLimbs, class Allocator>
inline void bitwise_and(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& o)
{
   bitwise_op(result, o, bit_and());
}
#if 0
template <unsigned InternalLimbs, class Allocator>
inline void bitwise_and(cpp_int_backend<InternalLimbs, Allocator>& result, limb_type o)
{
   result.data()[cpp_int_backend<InternalLimbs, Allocator>::limb_count - 1] &= o;
   for(typename cpp_int_backend<InternalLimbs, Allocator>::data_type::size_type i = 0; i < cpp_int_backend<InternalLimbs, Allocator>::limb_count - 1; ++i)
      result.data()[i] = 0;
}
template <unsigned InternalLimbs, class Allocator>
inline void bitwise_and(cpp_int_backend<InternalLimbs, Allocator>& result, signed_limb_type o)
{
   result.data()[cpp_int_backend<InternalLimbs, Allocator>::limb_count - 1] &= o;
   limb_type mask = o < 0 ? cpp_int_backend<InternalLimbs, Allocator>::max_limb_value : 0;
   for(typename cpp_int_backend<InternalLimbs, Allocator>::data_type::size_type i = 0; i < cpp_int_backend<InternalLimbs, Allocator>::limb_count - 1; ++i)
      result.data()[i] &= mask;
}
template <unsigned InternalLimbs, class Allocator>
inline void bitwise_or(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& o)
{
   for(typename cpp_int_backend<InternalLimbs, Allocator>::data_type::size_type i = 0; i < cpp_int_backend<InternalLimbs, Allocator>::limb_count; ++i)
      result.data()[i] |= o.data()[i];
}
template <unsigned InternalLimbs, class Allocator>
inline void bitwise_or(cpp_int_backend<InternalLimbs, Allocator>& result, limb_type o)
{
   result.data()[cpp_int_backend<InternalLimbs, Allocator>::limb_count - 1] |= o;
}
#endif
template <unsigned InternalLimbs, class Allocator>
inline void bitwise_or(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& o)
{
   bitwise_op(result, o, bit_or());
}
template <unsigned InternalLimbs, class Allocator>
inline void bitwise_xor(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& o)
{
   bitwise_op(result, o, bit_xor());
}
#if 0
template <unsigned InternalLimbs, class Allocator>
inline void bitwise_xor(cpp_int_backend<InternalLimbs, Allocator>& result, limb_type o)
{
   result.data()[cpp_int_backend<InternalLimbs, Allocator>::limb_count - 1] ^= o;
}
template <unsigned InternalLimbs, class Allocator>
inline void bitwise_xor(cpp_int_backend<InternalLimbs, Allocator>& result, signed_limb_type o)
{
   result.data()[cpp_int_backend<InternalLimbs, Allocator>::limb_count - 1] ^= o;
   limb_type mask = o < 0 ? cpp_int_backend<InternalLimbs, Allocator>::max_limb_value : 0;
   for(typename cpp_int_backend<InternalLimbs, Allocator>::data_type::size_type i = 0; i < cpp_int_backend<InternalLimbs, Allocator>::limb_count - 1; ++i)
      result.data()[i] ^= mask;
}
#endif
template <unsigned InternalLimbs, class Allocator>
inline void complement(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& o)
{
   // Increment and negate:
   result = o;
   increment(result);
   result.negate();
}
template <unsigned InternalLimbs, class Allocator>
inline void left_shift(cpp_int_backend<InternalLimbs, Allocator>& result, double_limb_type s)
{
   if(!s)
      return;

   limb_type offset = static_cast<limb_type>(s / cpp_int_backend<InternalLimbs, Allocator>::limb_bits);
   limb_type shift  = static_cast<limb_type>(s % cpp_int_backend<InternalLimbs, Allocator>::limb_bits);

   unsigned ors = result.size();
   if((ors == 1) && (!*result.limbs()))
      return; // shifting zero yields zero.
   unsigned rs = ors;
   if(shift && (result.limbs()[rs - 1] >> (cpp_int_backend<InternalLimbs, Allocator>::limb_bits - shift)))
      ++rs; // Most significant limb will overflow when shifted
   rs += offset;
   result.resize(rs);
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pr = result.limbs();

   unsigned i = 0;
   if(shift)
   {
      // This code only works when shift is non-zero, otherwise we invoke undefined behaviour!
      i = 0;
      if(rs > ors)
      {
         pr[rs - 1 - i] = pr[ors - 1 - i] >> (cpp_int_backend<InternalLimbs, Allocator>::limb_bits - shift);
         --rs;
      }
      else
      {
         pr[rs - 1 - i] = pr[ors - 1 - i] << shift;
         if(ors > 1)
            pr[rs - 1 - i] |= pr[ors - 2 - i] >> (cpp_int_backend<InternalLimbs, Allocator>::limb_bits - shift);
         ++i;
      }
      for(; ors > 1 + i; ++i)
      {
         pr[rs - 1 - i] = pr[ors - 1 - i] << shift;
         pr[rs - 1 - i] |= pr[ors - 2 - i] >> (cpp_int_backend<InternalLimbs, Allocator>::limb_bits - shift);
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
template <unsigned InternalLimbs, class Allocator>
inline void right_shift(cpp_int_backend<InternalLimbs, Allocator>& result, double_limb_type s)
{
   if(!s)
      return;

   limb_type offset = static_cast<limb_type>(s / cpp_int_backend<InternalLimbs, Allocator>::limb_bits);
   limb_type shift  = static_cast<limb_type>(s % cpp_int_backend<InternalLimbs, Allocator>::limb_bits);
   unsigned ors = result.size();
   unsigned rs = ors;
   if(offset >= rs)
   {
      result = limb_type(0);
      return;
   }
   rs -= offset;
   typename cpp_int_backend<InternalLimbs, Allocator>::limb_pointer pr = result.limbs();
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
         pr[i] |= pr[i + offset + 1] << (cpp_int_backend<InternalLimbs, Allocator>::limb_bits - shift);
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
inline Integer negate_integer(Integer i, const mpl::true_&)
{
   return -i;
}
template <class Integer>
inline Integer negate_integer(Integer i, const mpl::false_&)
{
   return ~--i;
}

template <class R, unsigned InternalLimbs, class Allocator>
inline typename enable_if<is_integral<R>, void>::type convert_to(R* result, const cpp_int_backend<InternalLimbs, Allocator>& backend)
{
   *result = static_cast<R>(backend.limbs()[0]);
   unsigned shift = cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
   for(unsigned i = 1; i < backend.size(); ++i)
   {
      *result += static_cast<R>(backend.limbs()[i]) << shift;
      shift += cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
      if(shift > std::numeric_limits<R>::digits)
         break;
   }
   if(backend.sign())
   {
      *result = negate_integer(*result, mpl::bool_<std::numeric_limits<R>::is_signed>());
   }
}

template <class R, unsigned InternalLimbs, class Allocator>
inline typename enable_if<is_floating_point<R>, void>::type convert_to(R* result, const cpp_int_backend<InternalLimbs, Allocator>& backend)
{
   typename cpp_int_backend<InternalLimbs, Allocator>::const_limb_pointer p = backend.limbs();
   unsigned shift = cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
   *result = static_cast<R>(*p);
   for(unsigned i = 1; i < backend.size(); ++i)
   {
      *result += static_cast<R>(std::ldexp(static_cast<long double>(p[i]), shift));
      shift += cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
   }
   if(backend.sign())
      *result = -*result;
}

template <unsigned InternalLimbs, class Allocator>
inline bool is_zero(const cpp_int_backend<InternalLimbs, Allocator>& val)
{
   return (val.size() == 1) && (val.limbs()[0] == 0);
}
template <unsigned InternalLimbs, class Allocator>
inline int get_sign(const cpp_int_backend<InternalLimbs, Allocator>& val)
{
   return is_zero(val) ? 0 : val.sign() ? -1 : 1;
}

namespace detail{
//
// Get the location of the least-significant-bit:
//
template <unsigned InternalLimbs, class Allocator>
inline unsigned get_lsb(const cpp_int_backend<InternalLimbs, Allocator>& a)
{
   BOOST_ASSERT(get_sign(a) != 0);
   
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

   return result + index * cpp_int_backend<InternalLimbs, Allocator>::limb_bits;
}

}

template <unsigned InternalLimbs, class Allocator>
inline void eval_gcd(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, const cpp_int_backend<InternalLimbs, Allocator>& b)
{
   int shift;

   cpp_int_backend<InternalLimbs, Allocator> u(a), v(b);

   int s = get_sign(u);

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
   s = get_sign(v);
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

   unsigned us = detail::get_lsb(u);
   unsigned vs = detail::get_lsb(v);
   shift = (std::min)(us, vs);
   right_shift(u, us);
   right_shift(v, vs);

   do 
   {
      /* Now u and v are both odd, so diff(u, v) is even.
      Let u = min(u, v), v = diff(u, v)/2. */
      if(u.compare(v) > 0)
         u.swap(v);
      subtract(v, u);
      // Termination condition tries not to do a full compare if possible:
      if(!v.limbs()[0] && is_zero(v))
         break;
      vs = detail::get_lsb(v);
      right_shift(v, vs);
      BOOST_ASSERT((v.limbs()[0] & 1));
      BOOST_ASSERT((u.limbs()[0] & 1));
   } 
   while(true);

   result = u;
   left_shift(result, shift);
}

template <unsigned InternalLimbs, class Allocator>
inline void eval_lcm(cpp_int_backend<InternalLimbs, Allocator>& result, const cpp_int_backend<InternalLimbs, Allocator>& a, const cpp_int_backend<InternalLimbs, Allocator>& b)
{
   cpp_int_backend<InternalLimbs, Allocator> t;
   eval_gcd(t, a, b);

   if(is_zero(t))
   {
      result = 0;
   }
   else
   {
      divide(result, a, t);
      multiply(result, b);
   }
   if(get_sign(result) < 0)
      result.negate();
}

template <unsigned InternalLimbs, class Allocator>
struct number_category<cpp_int_backend<InternalLimbs, Allocator> > : public mpl::int_<number_kind_integer>{};

typedef mp_number<cpp_int_backend<> >          cpp_int;
typedef rational_adapter<cpp_int_backend<> >   cpp_rational_backend;
typedef mp_number<cpp_rational_backend>        cpp_rational;

}} // namespaces


namespace std{

template <unsigned InternalLimbs, class Allocator>
class numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<InternalLimbs, Allocator> > >
{
   typedef boost::multiprecision::mp_number<boost::multiprecision::cpp_int_backend<InternalLimbs, Allocator> > number_type;

public:
   BOOST_STATIC_CONSTEXPR bool is_specialized = true;
   //
   // Largest and smallest numbers are bounded only by available memory, set
   // to zero:
   //
   static number_type (min)() BOOST_MP_NOEXCEPT
   {
      return number_type(0);
   }
   static number_type (max)() BOOST_MP_NOEXCEPT 
   { 
      return number_type(0);
   }
   static number_type lowest() BOOST_MP_NOEXCEPT { return (min)(); }
   BOOST_STATIC_CONSTEXPR int digits = 0;
   BOOST_STATIC_CONSTEXPR int digits10 = 0;
   BOOST_STATIC_CONSTEXPR int max_digits10 = 0;
   BOOST_STATIC_CONSTEXPR bool is_signed = true;
   BOOST_STATIC_CONSTEXPR bool is_integer = true;
   BOOST_STATIC_CONSTEXPR bool is_exact = true;
   BOOST_STATIC_CONSTEXPR int radix = 2;
   static number_type epsilon() BOOST_MP_NOEXCEPT { return 0; }
   static number_type round_error() BOOST_MP_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR int min_exponent = 0;
   BOOST_STATIC_CONSTEXPR int min_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR bool has_infinity = false;
   BOOST_STATIC_CONSTEXPR bool has_quiet_NaN = false;
   BOOST_STATIC_CONSTEXPR bool has_signaling_NaN = false;
   BOOST_STATIC_CONSTEXPR float_denorm_style has_denorm = denorm_absent;
   BOOST_STATIC_CONSTEXPR bool has_denorm_loss = false;
   static number_type infinity() BOOST_MP_NOEXCEPT { return 0; }
   static number_type quiet_NaN() BOOST_MP_NOEXCEPT { return 0; }
   static number_type signaling_NaN() BOOST_MP_NOEXCEPT { return 0; }
   static number_type denorm_min() BOOST_MP_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR bool is_iec559 = false;
   BOOST_STATIC_CONSTEXPR bool is_bounded = false;
   BOOST_STATIC_CONSTEXPR bool is_modulo = false;
   BOOST_STATIC_CONSTEXPR bool traps = false;
   BOOST_STATIC_CONSTEXPR bool tinyness_before = false;
   BOOST_STATIC_CONSTEXPR float_round_style round_style = round_toward_zero;
};

}

#endif
