///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MATH_RATIONAL_ADAPTER_HPP
#define BOOST_MATH_RATIONAL_ADAPTER_HPP

#include <iostream>
#include <iomanip>
#include <boost/cstdint.hpp>
#include <boost/multiprecision/mp_number.hpp>
#include <boost/array.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>

namespace boost{
namespace multiprecision{

template <unsigned Bits, bool Signed>
struct packed_cpp_int
{
   typedef mpl::list<boost::int32_t, boost::intmax_t>      signed_types;
   typedef mpl::list<boost::uint32_t, boost::uintmax_t>    unsigned_types;
   typedef mpl::list<long double>                          float_types;

   typedef boost::uint32_t   limb_type;
   BOOST_STATIC_CONSTANT(unsigned, limb_bits = sizeof(limb_type) * CHAR_BIT);
   BOOST_STATIC_CONSTANT(unsigned, limb_count = Bits / limb_bits + (Bits % limb_bits ? 1 : 0));
   BOOST_STATIC_CONSTANT(limb_type, max_limb_value = ~static_cast<limb_type>(0u));
   BOOST_STATIC_CONSTANT(limb_type, upper_limb_mask = (Bits % limb_bits ? (1 << (Bits % limb_bits)) - 1 : max_limb_value));
   BOOST_STATIC_CONSTANT(limb_type, upper_limb_not_mask = ~upper_limb_mask);
   BOOST_STATIC_CONSTANT(limb_type, sign_bit_mask = 1 << ((Bits % limb_bits ? Bits % limb_bits : limb_bits) - 1));
   typedef boost::array<limb_type, limb_count> data_type;

   packed_cpp_int(){}
   packed_cpp_int(const packed_cpp_int& o)
   {
      m_value = o.m_value;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   packed_cpp_int(packed_cpp_int&& o) : m_value(o.m_value) {}
#endif
   packed_cpp_int& operator = (const packed_cpp_int& o)
   {
      m_value = o.m_value;
      return *this;
   }
   packed_cpp_int& operator = (boost::uint32_t i)
   {
      m_value[limb_count - 1] = i;
      for(int j = limb_count - 2; j >= 0; --j)
         m_value[j] = 0;
      m_value[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
      return *this;
   }
   packed_cpp_int& operator = (boost::int32_t i)
   {
      m_value[limb_count - 1] = i;
      // sign extend:
      for(int j = limb_count - 2; j >= 0; --j)
         m_value[j] = i < 0 ? max_limb_value : 0;
      m_value[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
      return *this;
   }
   packed_cpp_int& operator = (boost::uintmax_t i)
   {
      BOOST_STATIC_ASSERT(sizeof(i) % sizeof(limb_type) == 0);
      boost::uintmax_t mask = max_limb_value;
      unsigned shift = 0;
      for(int j = limb_count - 1; j >= static_cast<int>(limb_count) - static_cast<int>(sizeof(boost::intmax_t) / sizeof(limb_type)); --j)
      {
         m_value[j] = static_cast<limb_type>((i & mask) >> shift);
         mask <<= limb_bits;
         shift += limb_bits;
      }
      for(int j = static_cast<int>(limb_count) - static_cast<int>(sizeof(boost::intmax_t) / sizeof(limb_type)) - 1; j >= 0; --j)
         m_value[j] = 0;
      m_value[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
      return *this;
   }
   packed_cpp_int& operator = (boost::intmax_t i)
   {
      BOOST_STATIC_ASSERT(sizeof(i) % sizeof(limb_type) == 0);
      boost::uintmax_t mask = max_limb_value;
      unsigned shift = 0;
      for(int j = limb_count - 1; j >= static_cast<int>(limb_count) - static_cast<int>(sizeof(boost::intmax_t) / sizeof(limb_type)); --j)
      {
         m_value[j] = static_cast<limb_type>((i & mask) >> shift);
         mask <<= limb_bits;
         shift += limb_bits;
      }
      for(int j = static_cast<int>(limb_count) - static_cast<int>(sizeof(boost::intmax_t) / sizeof(limb_type)) - 1; j >= 0; --j)
         m_value[j] = i < 0 ? max_limb_value : 0;
      m_value[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
      return *this;
   }
   packed_cpp_int& operator = (long double a)
   {
      BOOST_STATIC_ASSERT(Bits >= std::numeric_limits<long double>::digits);
      using std::frexp;
      using std::ldexp;
      using std::floor;

      if (a == 0) {
         return *this = static_cast<boost::uint32_t>(0u);
      }

      if (a == 1) {
         return *this = static_cast<boost::uint32_t>(1u);
      }

      BOOST_ASSERT(!(boost::math::isinf)(a));
      BOOST_ASSERT(!(boost::math::isnan)(a));

      int e;
      long double f, term;
      *this = static_cast<boost::uint32_t>(0u);

      f = frexp(a, &e);

      static const boost::uint32_t shift = std::numeric_limits<boost::uint32_t>::digits;

      while(f)
      {
         // extract int sized bits from f:
         f = ldexp(f, shift);
         term = floor(f);
         e -= shift;
         left_shift(*this, shift);
         if(term > 0)
            add(*this, static_cast<boost::uint32_t>(term));
         else
            subtract(*this, static_cast<boost::uint32_t>(-term));
         f -= term;
      }
      if(e > 0)
         left_shift(*this, e);
      else if(e < 0)
         right_shift(*this, -e);
      data()[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
      return *this;
   }
   packed_cpp_int& operator = (const char* s)
   {
      std::size_t n = s ? std::strlen(s) : 0;
      *this = static_cast<boost::uint32_t>(0u);
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
            boost::uint32_t val = max_limb_value;
            while(*s)
            {
               left_shift(*this, shift);
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
                  m_value[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
                  return *this; // TODO raise an exception here?
               }
               m_value[limb_count - 1] |= val;
               ++s;
            }
         }
         else
         {
            // Base 10:
            while(*s)
            {
               // TODO: this implementation is brain dead, Fix Me!
               multiply(*this, static_cast<boost::uint32_t>(10));
               boost::uint32_t val;
               if(*s >= '0' && *s <= '9')
                  val = *s - '0';
               else
                  return *this; // TODO raise an exception here?
               add(*this, val);
               ++s;
            }
         }
      }
      m_value[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
      if(isneg)
         negate();
      return *this;
   }
   void swap(packed_cpp_int& o)
   {
      std::swap(m_value, o.m_value);
   }
   std::string str(std::streamsize digits, std::ios_base::fmtflags f)const
   {
      int base = 10;
      if((f & std::ios_base::oct) == std::ios_base::oct)
         base = 8;
      else if((f & std::ios_base::hex) == std::ios_base::hex)
         base = 16;
      std::string result;

      if(base == 8 || base == 16)
      {
         boost::uint32_t shift = base == 8 ? 3 : 4;
         boost::uint32_t mask = static_cast<boost::uint32_t>((1u << shift) - 1);
         packed_cpp_int t(*this);
         for(unsigned i = 0; i < Bits / shift; ++i)
         {
            char c = '0' + (t.data()[limb_count-1] & mask);
            if(c > '9')
               c += 'A' - '9' - 1;
            result.insert(0, 1, c);
            right_shift(t, shift);
         }
         if(Bits % shift)
         {
            mask = static_cast<boost::uint32_t>((1u << (Bits % shift)) - 1);
            char c = '0' + (t.data()[limb_count-1] & mask);
            if(c > '9')
               c += 'A' - '9';
            result.insert(0, 1, c);
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
         packed_cpp_int t(*this);
         packed_cpp_int ten, r;
         ten = boost::uint32_t(1000000000);
         bool neg = false;
         if(Signed && (t.data()[0] & sign_bit_mask))
         {
            t.negate();
            neg = true;
         }
         if(limb_count == 1)
         {
            result = boost::lexical_cast<std::string>(t.data()[0]);
         }
         else
         {
            while(get_sign(t) != 0)
            {
               packed_cpp_int t2;
               divide_unsigned_helper(t2, t, ten, r);
               t = t2;
               boost::uint32_t v = r.data()[limb_count - 1];
               for(unsigned i = 0; i < 9; ++i)
               {
                  char c = '0' + v % 10;
                  v /= 10;
                  result.insert(0, 1, c);
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
      using default_ops::increment;
      // complement and add 1:
      complement(*this, *this);
      increment(*this);
   }
   int compare(const packed_cpp_int& o)const
   {
      int result = 0;
      if(Signed && ((m_value[0] & sign_bit_mask) != (o.data()[0] & sign_bit_mask)))
      {
         return m_value[0] & sign_bit_mask ? -1 : 1;
      }
      for(data_type::size_type i = 0; i < limb_count; ++i)
      {
         if(m_value[i] != o.data()[i])
         {
            result = (m_value[i] < o.data()[i]) ? -1 : 1;
            break;
         }
      }
      return Signed && (m_value[0] & sign_bit_mask) ? -result : result ;
   }
   template <class Arithmatic>
   typename enable_if<is_arithmetic<Arithmatic>, int>::type compare(Arithmatic i)const
   {
      // braindead version:
      packed_cpp_int<Bits, Signed> t;
      t = i;
      return compare(t);
   }
   data_type& data() { return m_value; }
   const data_type& data()const { return m_value; }
private:
   data_type m_value;
};

template <unsigned Bits, bool Signed>
inline void add(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& o)
{
   // Addition using modular arithmatic.
   // Nothing fancy, just let uintmax_t take the strain:
   boost::uintmax_t carry = 0;
   for(int i = packed_cpp_int<Bits, Signed>::limb_count - 1; i >= 0; --i)
   {
      boost::uintmax_t v = static_cast<boost::uintmax_t>(result.data()[i]) + static_cast<boost::uintmax_t>(o.data()[i]) + carry;
      result.data()[i] = static_cast<packed_cpp_int<Bits, Signed>::limb_type>(v);
      carry = v > packed_cpp_int<Bits, Signed>::max_limb_value ? 1 : 0;
   }
   result.data()[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void add(packed_cpp_int<Bits, Signed>& result, const boost::uint32_t& o)
{
   // Addition using modular arithmatic.
   // Nothing fancy, just let uintmax_t take the strain:
   boost::uintmax_t carry = o;
   for(int i = packed_cpp_int<Bits, Signed>::limb_count - 1; carry && i >= 0; --i)
   {
      boost::uintmax_t v = static_cast<boost::uintmax_t>(result.data()[i]) + carry;
      result.data()[i] = static_cast<packed_cpp_int<Bits, Signed>::limb_type>(v);
      carry = v > packed_cpp_int<Bits, Signed>::max_limb_value ? 1 : 0;
   }
   result.data()[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void add(packed_cpp_int<Bits, Signed>& result, const boost::int32_t& o)
{
   if(o < 0)
      subtract(result, static_cast<boost::uint32_t>(-o));
   else if(o > 0)
      add(result, static_cast<boost::uint32_t>(o));
}
template <unsigned Bits, bool Signed>
inline void subtract(packed_cpp_int<Bits, Signed>& result, const boost::uint32_t& o)
{
   if(o)
   {
      packed_cpp_int<Bits, Signed> t;
      t.data()[packed_cpp_int<Bits, Signed>::limb_count - 1] = ~o + 1;
      for(int i = packed_cpp_int<Bits, Signed>::limb_count - 2; i >= 0; --i)
      {
         t.data()[i] = packed_cpp_int<Bits, Signed>::max_limb_value;
      }
      add(result, t);
   }
}
template <unsigned Bits, bool Signed>
inline void subtract(packed_cpp_int<Bits, Signed>& result, const boost::int32_t& o)
{
   if(o)
   {
      if(o < 0)
         add(result, static_cast<boost::uint32_t>(-o));
      else
         subtract(result, static_cast<boost::uint32_t>(o));
   }
}
template <unsigned Bits, bool Signed>
inline void increment(packed_cpp_int<Bits, Signed>& result)
{
   static const boost::uint32_t one = 1;
   add(result, one);
}
template <unsigned Bits, bool Signed>
inline void decrement(packed_cpp_int<Bits, Signed>& result)
{
   static const boost::uint32_t one = 1;
   subtract(result, one);
}
template <unsigned Bits, bool Signed>
inline void subtract(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& o)
{
   // Negate and add:
   packed_cpp_int<Bits, Signed> t(o);
   t.negate();
   add(result, t);
}
template <unsigned Bits, bool Signed>
inline void multiply(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& a, const packed_cpp_int<Bits, Signed>& b)
{
   // Very simple long multiplication, only usable for small numbers of limb_type's
   // but that's the typical use case for this type anyway:
   if(&result == &a)
   {
      packed_cpp_int<Bits, Signed> t(a);
      multiply(result, t, b);
      return;
   }
   if(&result == &b)
   {
      packed_cpp_int<Bits, Signed> t(b);
      multiply(result, a, t);
      return;
   }
   boost::uintmax_t carry = 0;
   for(unsigned i = 0; i < packed_cpp_int<Bits, Signed>::limb_count; ++i)
      result.data()[i] = 0;
   for(int i = packed_cpp_int<Bits, Signed>::limb_count - 1; i >= 0; --i)
   {
      for(int j = packed_cpp_int<Bits, Signed>::limb_count - 1; j >= static_cast<int>(packed_cpp_int<Bits, Signed>::limb_count) - i - 1; --j)
      {
         boost::uintmax_t v = static_cast<boost::uintmax_t>(a.data()[i]) * static_cast<boost::uintmax_t>(b.data()[j]) + carry;
         v += result.data()[i + j + 1 - packed_cpp_int<Bits, Signed>::limb_count];
         result.data()[i + j + 1 - packed_cpp_int<Bits, Signed>::limb_count] = static_cast<packed_cpp_int<Bits, Signed>::limb_type>(v);
         carry = v >> sizeof(packed_cpp_int<Bits, Signed>::limb_type) * CHAR_BIT;
      }
      carry = 0;
   }
   result.data()[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void multiply(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& a)
{
   // There is no in-place multiply:
   packed_cpp_int<Bits, Signed> b(result);
   multiply(result, b, a);
}
template <unsigned Bits, bool Signed>
inline void multiply(packed_cpp_int<Bits, Signed>& result, const boost::uint32_t& a)
{
   boost::uintmax_t carry = 0;
   for(int i = packed_cpp_int<Bits, Signed>::limb_count - 1; i >= 0; --i)
   {
      boost::uintmax_t v = static_cast<boost::uintmax_t>(result.data()[i]) * static_cast<boost::uintmax_t>(a) + carry;
      result.data()[i] = static_cast<packed_cpp_int<Bits, Signed>::limb_type>(v);
      carry = v >> sizeof(packed_cpp_int<Bits, Signed>::limb_type) * CHAR_BIT;
   }
   result.data()[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void multiply(packed_cpp_int<Bits, Signed>& result, const boost::int32_t& a)
{
   if(a > 0)
      multiply(result, static_cast<boost::uint32_t>(a));
   else
   {
      multiply(result, static_cast<boost::uint32_t>(-a));
      result.negate();
   }
}

template <unsigned Bits, bool Signed>
boost::uint32_t bitcount(const packed_cpp_int<Bits, Signed>& a)
{
   // returns the location of the MSB in a:
   boost::uint32_t i = 0;
   for(; (i < packed_cpp_int<Bits, Signed>::limb_count) && (a.data()[i] == 0); ++i){}
   boost::uint32_t count = (packed_cpp_int<Bits, Signed>::limb_count - i) * packed_cpp_int<Bits, Signed>::limb_bits;
   if(!count)
      return count; // no bits are set, value is zero
   boost::uint32_t mask = static_cast<boost::uint32_t>(1u) << (packed_cpp_int<Bits, Signed>::limb_bits - 1);
   while((a.data()[i] & mask) == 0)
   {
      --count;
      mask >>= 1;
   }
   return count;
}

template <unsigned Bits, bool Signed>
void divide_unsigned_helper(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& x, const packed_cpp_int<Bits, Signed>& y, packed_cpp_int<Bits, Signed>& r)
{
   if((&result == &x) || (&r == &x))
   {
      packed_cpp_int<Bits, Signed> t(x);
      divide_unsigned_helper(result, t, y, r);
      return;
   }
   if((&result == &y) || (&r == &y))
   {
      packed_cpp_int<Bits, Signed> t(y);
      divide_unsigned_helper(result, x, t, r);
      return;
   }


   using default_ops::subtract;


   if (is_zero(y))
   {
      volatile int i = 0;
      i /= i;
      return;
   }

   if (is_zero(x))
   {
      r = y;
      result = x;
      return;
   }

   if(&result == &r)
   {
      packed_cpp_int<Bits, Signed> rem;
      divide_unsigned_helper(result, x, y, rem);
      r = rem;
      return;
   }

   r = x;
   result = static_cast<boost::uint32_t>(0u);
   if(x.compare(y) < 0)
   {
      return; // We already have the answer: zero.
   }

   boost::uint32_t n = bitcount(x) - bitcount(y);

   if(n == 0)
   {
      // result is exactly 1:
      result = static_cast<boost::uint32_t>(1u);
      subtract(r, y);
      return;
   }

   // Together mask_index and mask give us the bit we may be about to set in the result:
   boost::uint32_t mask_index = packed_cpp_int<Bits, Signed>::limb_count - 1 - n / packed_cpp_int<Bits, Signed>::limb_bits;
   boost::uint32_t mask = static_cast<boost::uint32_t>(1u) << n % packed_cpp_int<Bits, Signed>::limb_bits;
   packed_cpp_int<Bits, Signed> t(y);
   left_shift(t, n);
   while(mask_index < packed_cpp_int<Bits, Signed>::limb_count)
   {
      int comp = r.compare(t);
      if(comp >= 0)
      {
         result.data()[mask_index] |= mask;
         subtract(r, t);
         if(comp == 0)
            break; // no remainder left - we have an exact result!
      }
      right_shift(t, 1u);
      if(0 == (mask >>= 1))
      {
         ++mask_index;
         mask = static_cast<boost::uint32_t>(1u) << (packed_cpp_int<Bits, Signed>::limb_bits - 1);
      }
   }
   BOOST_ASSERT(r.compare(y) < 0); // remainder must be less than the divisor or our code has failed
}

template <unsigned Bits, bool Signed>
inline void divide(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& a, const packed_cpp_int<Bits, Signed>& b)
{
   packed_cpp_int<Bits, Signed> r;
   if(Signed && (a.data()[0] & packed_cpp_int<Bits, Signed>::sign_bit_mask))
   {
      if(Signed && (b.data()[0] & packed_cpp_int<Bits, Signed>::sign_bit_mask))
      {
         packed_cpp_int<Bits, Signed> t1(a), t2(b);
         t1.negate();
         t2.negate();
         divide_unsigned_helper(result, t1, t2, r);
      }
      else
      {
         packed_cpp_int<Bits, Signed> t(a);
         t.negate();
         divide_unsigned_helper(result, t, b, r);
         result.negate();
      }
   }
   else if(Signed && (b.data()[0] & packed_cpp_int<Bits, Signed>::sign_bit_mask))
   {
      packed_cpp_int<Bits, Signed> t(b);
      t.negate();
      divide_unsigned_helper(result, a, t, r);
      result.negate();
   }
   else
   {
      divide_unsigned_helper(result, a, b, r);
   }
}
template <unsigned Bits, bool Signed>
inline void divide(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& b)
{
   // There is no in place divide:
   packed_cpp_int<Bits, Signed> a(result);
   divide(result, a, b);
}
template <unsigned Bits, bool Signed>
inline void modulus(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& a, const packed_cpp_int<Bits, Signed>& b)
{
   packed_cpp_int<Bits, Signed> r;
   if(Signed && (a.data()[0] & packed_cpp_int<Bits, Signed>::sign_bit_mask))
   {
      if(Signed && (b.data()[0] & packed_cpp_int<Bits, Signed>::sign_bit_mask))
      {
         packed_cpp_int<Bits, Signed> t1(a), t2(b);
         t1.negate();
         t2.negate();
         divide_unsigned_helper(r, t1, t2, result);
         result.negate();
      }
      else
      {
         packed_cpp_int<Bits, Signed> t(a);
         t.negate();
         divide_unsigned_helper(r, t, b, result);
         result.negate();
      }
   }
   else if(Signed && (b.data()[0] & packed_cpp_int<Bits, Signed>::sign_bit_mask))
   {
      packed_cpp_int<Bits, Signed> t(b);
      t.negate();
      divide_unsigned_helper(r, a, t, result);
   }
   else
   {
      divide_unsigned_helper(r, a, b, result);
   }
}
template <unsigned Bits, bool Signed>
inline void modulus(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& b)
{
   // There is no in place divide:
   packed_cpp_int<Bits, Signed> a(result);
   modulus(result, a, b);
}

template <unsigned Bits, bool Signed>
inline void bitwise_and(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& o)
{
   for(packed_cpp_int<Bits, Signed>::data_type::size_type i = 0; i < packed_cpp_int<Bits, Signed>::limb_count; ++i)
      result.data()[i] &= o.data()[i];
}
template <unsigned Bits, bool Signed>
inline void bitwise_or(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& o)
{
   for(packed_cpp_int<Bits, Signed>::data_type::size_type i = 0; i < packed_cpp_int<Bits, Signed>::limb_count; ++i)
      result.data()[i] |= o.data()[i];
}
template <unsigned Bits, bool Signed>
inline void bitwise_xor(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& o)
{
   for(packed_cpp_int<Bits, Signed>::data_type::size_type i = 0; i < packed_cpp_int<Bits, Signed>::limb_count; ++i)
      result.data()[i] ^= o.data()[i];
   result.data()[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void complement(packed_cpp_int<Bits, Signed>& result, const packed_cpp_int<Bits, Signed>& o)
{
   for(packed_cpp_int<Bits, Signed>::data_type::size_type i = 0; i < packed_cpp_int<Bits, Signed>::limb_count; ++i)
      result.data()[i] = ~o.data()[i];
   result.data()[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void left_shift(packed_cpp_int<Bits, Signed>& result, boost::uintmax_t s)
{
   if(s >= Bits)
   {
      result = static_cast<boost::uint32_t>(0);
      return;
   }
   boost::uint32_t offset = static_cast<boost::uint32_t>(s / packed_cpp_int<Bits, Signed>::limb_bits);
   boost::uint32_t shift  = static_cast<boost::uint32_t>(s % packed_cpp_int<Bits, Signed>::limb_bits);
   unsigned i = 0;
   if(shift)
   {
      // This code only works when shift is non-zero, otherwise we invoke undefined behaviour!
      for(; i + offset + 1 < packed_cpp_int<Bits, Signed>::limb_count; ++i)
      {
         result.data()[i] = result.data()[i+offset] << shift;
         result.data()[i] |= result.data()[i+offset+1] >> (packed_cpp_int<Bits, Signed>::limb_bits - shift);
      }
      result.data()[i] = result.data()[i+offset] << shift;
      ++i;
      for(; i < packed_cpp_int<Bits, Signed>::limb_count; ++i)
         result.data()[i] = 0;
   }
   else
   {
      for(; i + offset < packed_cpp_int<Bits, Signed>::limb_count; ++i)
         result.data()[i] = result.data()[i+offset];
      for(; i < packed_cpp_int<Bits, Signed>::limb_count; ++i)
         result.data()[i] = 0;
   }
   result.data()[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void right_shift(packed_cpp_int<Bits, Signed>& result, boost::uintmax_t s)
{
   boost::uint32_t fill = (Signed && (result.data()[0] & packed_cpp_int<Bits, Signed>::sign_bit_mask)) ? packed_cpp_int<Bits, Signed>::max_limb_value : 0u;
   if(s >= Bits)
   {
      for(unsigned i = 0; i < packed_cpp_int<Bits, Signed>::limb_count; ++i)
         result.data()[i] = fill;
      return;
   }
   boost::uint32_t offset = static_cast<boost::uint32_t>(s / packed_cpp_int<Bits, Signed>::limb_bits);
   boost::uint32_t shift  = static_cast<boost::uint32_t>(s % packed_cpp_int<Bits, Signed>::limb_bits);
   int i = packed_cpp_int<Bits, Signed>::limb_count - 1;
   if(shift)
   {
      // This code only works for non-zero shift, otherwise we invoke undefined behaviour!
      if(fill && (Bits % packed_cpp_int<Bits, Signed>::limb_bits))
      {
         // We need to sign extend the leftmost bits, otherwise we may shift zeros into the result:
         result.data()[0] |= fill << (Bits % packed_cpp_int<Bits, Signed>::limb_bits);
      }
      for(; i - offset > 0; --i)
      {
         result.data()[i] = result.data()[i-offset] >> shift;
         result.data()[i] |= result.data()[i-offset-1] << (packed_cpp_int<Bits, Signed>::limb_bits - shift);
      }
      result.data()[i] = result.data()[i+offset] >> shift;
      result.data()[i] |= fill << (packed_cpp_int<Bits, Signed>::limb_bits - shift);
      --i;
      for(; i >= 0; --i)
         result.data()[i] = fill;
   }
   else
   {
      for(; i - offset > 0; --i)
         result.data()[i] = result.data()[i - offset];
      for(; i >= 0; --i)
         result.data()[i] = fill;
   }
   result.data()[0] &= packed_cpp_int<Bits, Signed>::upper_limb_mask;
}

template <class R, unsigned Bits, bool Signed>
inline typename enable_if<is_integral<R>, void>::type convert_to(R* result, const packed_cpp_int<Bits, Signed>& backend)
{
   unsigned shift = (packed_cpp_int<Bits, Signed>::limb_count - 1) * packed_cpp_int<Bits, Signed>::limb_bits;
   *result = 0;
   for(unsigned i = 0; i < packed_cpp_int<Bits, Signed>::limb_count; ++i)
   {
      *result += static_cast<R>(backend.data()[i]) << shift;
      shift -= packed_cpp_int<Bits, Signed>::limb_bits;
   }
}

template <class R, unsigned Bits, bool Signed>
inline typename enable_if<is_floating_point<R>, void>::type convert_to(R* result, const packed_cpp_int<Bits, Signed>& backend)
{
   unsigned shift = (packed_cpp_int<Bits, Signed>::limb_count - 1) * packed_cpp_int<Bits, Signed>::limb_bits;
   *result = 0;
   for(unsigned i = 0; i < packed_cpp_int<Bits, Signed>::limb_count; ++i)
   {
      *result += static_cast<R>(std::ldexp(static_cast<double>(backend.data()[i]), shift));
      shift -= packed_cpp_int<Bits, Signed>::limb_bits;
   }
}

template <unsigned Bits, bool Signed>
inline bool is_zero(const packed_cpp_int<Bits, Signed>& val)
{
   for(typename packed_cpp_int<Bits, Signed>::data_type::size_type i = 0; i < packed_cpp_int<Bits, Signed>::limb_count; ++i)
   {
      if(val.data()[i])
         return false;
   }
   return true;
}
template <unsigned Bits>
inline int get_sign(const packed_cpp_int<Bits, false>& val)
{
   return is_zero(val) ? 0 : 1;
}
template <unsigned Bits>
inline int get_sign(const packed_cpp_int<Bits, true>& val)
{
   return is_zero(val) ? 0 : val.data()[0] & packed_cpp_int<Bits, true>::sign_bit_mask ? -1 : 1;
}

template <unsigned Bits, bool Signed>
struct number_category<packed_cpp_int<Bits, Signed> > : public mpl::int_<number_kind_integer>{};

typedef mp_number<packed_cpp_int<64, false> > mp_uint64_t;
typedef mp_number<packed_cpp_int<128, false> > mp_uint128_t;
typedef mp_number<packed_cpp_int<256, false> > mp_uint256_t;
typedef mp_number<packed_cpp_int<512, false> > mp_uint512_t;

typedef mp_number<packed_cpp_int<64, true> > mp_int64_t;
typedef mp_number<packed_cpp_int<128, true> > mp_int128_t;
typedef mp_number<packed_cpp_int<256, true> > mp_int256_t;
typedef mp_number<packed_cpp_int<512, true> > mp_int512_t;

}} // namespaces


namespace std{

template <unsigned Bits, bool Signed>
class numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::packed_cpp_int<Bits, Signed> > >
{
   typedef boost::multiprecision::mp_number<boost::multiprecision::packed_cpp_int<Bits, Signed> > number_type;
public:
   BOOST_STATIC_CONSTEXPR bool is_specialized = true;
   //
   // Largest and smallest numbers are bounded only by available memory, set
   // to zero:
   //
   BOOST_STATIC_CONSTEXPR number_type (min)() BOOST_MP_NOEXCEPT
   { 
      return number_type();
   }
   BOOST_STATIC_CONSTEXPR number_type (max)() BOOST_MP_NOEXCEPT 
   { 
      return number_type();
   }
   BOOST_STATIC_CONSTEXPR number_type lowest() BOOST_MP_NOEXCEPT { return (min)(); }
   BOOST_STATIC_CONSTEXPR int digits = Bits;
   BOOST_STATIC_CONSTEXPR int digits10 = (digits * 301L) / 1000;
   BOOST_STATIC_CONSTEXPR int max_digits10 = digits10 + 2;
   BOOST_STATIC_CONSTEXPR bool is_signed = Signed;
   BOOST_STATIC_CONSTEXPR bool is_integer = true;
   BOOST_STATIC_CONSTEXPR bool is_exact = true;
   BOOST_STATIC_CONSTEXPR int radix = 2;
   BOOST_STATIC_CONSTEXPR number_type epsilon() BOOST_MP_NOEXCEPT { return number_type(); }
   BOOST_STATIC_CONSTEXPR number_type round_error() BOOST_MP_NOEXCEPT { return number_type(); }
   BOOST_STATIC_CONSTEXPR int min_exponent = 0;
   BOOST_STATIC_CONSTEXPR int min_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR bool has_infinity = false;
   BOOST_STATIC_CONSTEXPR bool has_quiet_NaN = false;
   BOOST_STATIC_CONSTEXPR bool has_signaling_NaN = false;
   BOOST_STATIC_CONSTEXPR float_denorm_style has_denorm = denorm_absent;
   BOOST_STATIC_CONSTEXPR bool has_denorm_loss = false;
   BOOST_STATIC_CONSTEXPR number_type infinity() BOOST_MP_NOEXCEPT { return number_type(); }
   BOOST_STATIC_CONSTEXPR number_type quiet_NaN() BOOST_MP_NOEXCEPT { return number_type(); }
   BOOST_STATIC_CONSTEXPR number_type signaling_NaN() BOOST_MP_NOEXCEPT { return number_type(); }
   BOOST_STATIC_CONSTEXPR number_type denorm_min() BOOST_MP_NOEXCEPT { return number_type(); }
   BOOST_STATIC_CONSTEXPR bool is_iec559 = false;
   BOOST_STATIC_CONSTEXPR bool is_bounded = true;
   BOOST_STATIC_CONSTEXPR bool is_modulo = true;
   BOOST_STATIC_CONSTEXPR bool traps = false;
   BOOST_STATIC_CONSTEXPR bool tinyness_before = false;
   BOOST_STATIC_CONSTEXPR float_round_style round_style = round_toward_zero;
};

}

#endif
