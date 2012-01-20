///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MP_PACKED_INT_HPP
#define BOOST_MP_PACKED_INT_HPP

#include <iostream>
#include <iomanip>
#include <boost/cstdint.hpp>
#include <boost/multiprecision/mp_number.hpp>
#include <boost/array.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>

namespace boost{
namespace multiprecision{

typedef boost::uint32_t limb_type;
typedef boost::int32_t signed_limb_type;
typedef boost::uint64_t double_limb_type;
typedef boost::int64_t signed_double_limb_type;

template <unsigned Bits, bool Signed>
struct fixed_int
{
   typedef mpl::list<signed_limb_type, signed_double_limb_type>      signed_types;
   typedef mpl::list<limb_type, double_limb_type>                    unsigned_types;
   typedef mpl::list<long double>                                    float_types;

   BOOST_STATIC_CONSTANT(unsigned, limb_bits = sizeof(limb_type) * CHAR_BIT);
   BOOST_STATIC_CONSTANT(unsigned, limb_count = Bits / limb_bits + (Bits % limb_bits ? 1 : 0));
   BOOST_STATIC_CONSTANT(limb_type, max_limb_value = ~static_cast<limb_type>(0u));
   BOOST_STATIC_CONSTANT(limb_type, upper_limb_mask = (Bits % limb_bits ? (1 << (Bits % limb_bits)) - 1 : max_limb_value));
   BOOST_STATIC_CONSTANT(limb_type, upper_limb_not_mask = ~upper_limb_mask);
   BOOST_STATIC_CONSTANT(limb_type, sign_bit_mask = 1u << ((Bits % limb_bits ? Bits % limb_bits : limb_bits) - 1));
   typedef boost::array<limb_type, limb_count> data_type;

   fixed_int(){}
   fixed_int(const fixed_int& o)
   {
      m_value = o.m_value;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   fixed_int(fixed_int&& o) : m_value(o.m_value) {}
#endif
   fixed_int& operator = (const fixed_int& o)
   {
      m_value = o.m_value;
      return *this;
   }
   fixed_int& operator = (limb_type i)
   {
      m_value[limb_count - 1] = i;
      for(int j = static_cast<int>(limb_count) - 2; j >= 0; --j)
         m_value[j] = 0;
      m_value[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
      return *this;
   }
   fixed_int& operator = (signed_limb_type i)
   {
      m_value[limb_count - 1] = i;
      // sign extend:
      for(int j = static_cast<int>(limb_count) - 2; j >= 0; --j)
         m_value[j] = i < 0 ? max_limb_value : 0;
      m_value[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
      return *this;
   }
   fixed_int& operator = (double_limb_type i)
   {
      BOOST_STATIC_ASSERT(sizeof(i) % sizeof(limb_type) == 0);
      double_limb_type mask = max_limb_value;
      unsigned shift = 0;
      for(int j = limb_count - 1; j >= static_cast<int>(limb_count) - static_cast<int>(sizeof(signed_double_limb_type) / sizeof(limb_type)); --j)
      {
         m_value[j] = static_cast<limb_type>((i & mask) >> shift);
         mask <<= limb_bits;
         shift += limb_bits;
      }
      for(int j = static_cast<int>(limb_count) - static_cast<int>(sizeof(signed_double_limb_type) / sizeof(limb_type)) - 1; j >= 0; --j)
         m_value[j] = 0;
      m_value[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
      return *this;
   }
   fixed_int& operator = (signed_double_limb_type i)
   {
      BOOST_STATIC_ASSERT(sizeof(i) % sizeof(limb_type) == 0);
      double_limb_type mask = max_limb_value;
      unsigned shift = 0;
      for(int j = limb_count - 1; j >= static_cast<int>(limb_count) - static_cast<int>(sizeof(signed_double_limb_type) / sizeof(limb_type)); --j)
      {
         m_value[j] = static_cast<limb_type>((i & mask) >> shift);
         mask <<= limb_bits;
         shift += limb_bits;
      }
      for(int j = static_cast<int>(limb_count) - static_cast<int>(sizeof(signed_double_limb_type) / sizeof(limb_type)) - 1; j >= 0; --j)
         m_value[j] = i < 0 ? max_limb_value : 0;
      m_value[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
      return *this;
   }
   fixed_int& operator = (long double a)
   {
      BOOST_STATIC_ASSERT(Bits >= (unsigned)std::numeric_limits<long double>::digits);
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
      data()[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
      return *this;
   }
   fixed_int& operator = (const char* s)
   {
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
            limb_type val = max_limb_value;
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
                  m_value[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
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
               multiply(*this, static_cast<limb_type>(10));
               limb_type val;
               if(*s >= '0' && *s <= '9')
                  val = *s - '0';
               else
                  return *this; // TODO raise an exception here?
               add(*this, val);
               ++s;
            }
         }
      }
      m_value[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
      if(isneg)
         negate();
      return *this;
   }
   void swap(fixed_int& o)
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
         limb_type shift = base == 8 ? 3 : 4;
         limb_type mask = static_cast<limb_type>((1u << shift) - 1);
         fixed_int t(*this);
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
            mask = static_cast<limb_type>((1u << (Bits % shift)) - 1);
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
         fixed_int t(*this);
         fixed_int ten, r;
         ten = limb_type(1000000000);
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
               fixed_int t2;
               divide_unsigned_helper(t2, t, ten, r);
               t = t2;
               limb_type v = r.data()[limb_count - 1];
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
      double_limb_type carry = 1;
      for(int i = fixed_int<Bits, Signed>::limb_count - 1; i >= 0; --i)
      {
         carry += static_cast<double_limb_type>(~m_value[i]);
         m_value[i] = static_cast<limb_type>(carry);
         carry >>= limb_bits;
      }
      m_value[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
   }
   int compare(const fixed_int& o)const
   {
      int result = 0;
      if(Signed && ((m_value[0] & sign_bit_mask) != (o.data()[0] & sign_bit_mask)))
      {
         return m_value[0] & sign_bit_mask ? -1 : 1;
      }
      for(typename data_type::size_type i = 0; i < limb_count; ++i)
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
      fixed_int<Bits, Signed> t;
      t = i;
      return compare(t);
   }
   data_type& data() { return m_value; }
   const data_type& data()const { return m_value; }
private:
   data_type m_value;
};

template <unsigned Bits, bool Signed>
inline void add(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& o)
{
   add(result, result, o);
}
template <unsigned Bits, bool Signed>
inline void add(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& a, const fixed_int<Bits, Signed>& b)
{
   // Addition using modular arithmatic.
   // Nothing fancy, just let uintmax_t take the strain:
   double_limb_type carry = 0;
   for(int i = fixed_int<Bits, Signed>::limb_count - 1; i >= 0; --i)
   {
      carry += static_cast<double_limb_type>(a.data()[i]) + static_cast<double_limb_type>(b.data()[i]);
      result.data()[i] = static_cast<limb_type>(carry);
      carry >>= fixed_int<Bits, Signed>::limb_bits;
   }
   result.data()[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void add(fixed_int<Bits, Signed>& result, const limb_type& o)
{
   // Addition using modular arithmatic.
   // Nothing fancy, just let uintmax_t take the strain:
   double_limb_type carry = o;
   for(int i = fixed_int<Bits, Signed>::limb_count - 1; carry && i >= 0; --i)
   {
      carry += static_cast<double_limb_type>(result.data()[i]);
      result.data()[i] = static_cast<limb_type>(carry);
      carry >>= fixed_int<Bits, Signed>::limb_bits;
   }
   result.data()[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void add(fixed_int<Bits, Signed>& result, const signed_limb_type& o)
{
   if(o < 0)
      subtract(result, static_cast<limb_type>(-o));
   else if(o > 0)
      add(result, static_cast<limb_type>(o));
}
template <unsigned Bits, bool Signed>
inline void subtract(fixed_int<Bits, Signed>& result, const limb_type& o)
{
   // Subtract using modular arithmatic.
   // This is the same code as for addition, with the twist that we negate o "on the fly":
   double_limb_type carry = static_cast<double_limb_type>(result.data()[fixed_int<Bits, Signed>::limb_count - 1]) 
      + 1uLL + static_cast<double_limb_type>(~o);
   result.data()[fixed_int<Bits, Signed>::limb_count - 1] = static_cast<limb_type>(carry);
   carry >>= fixed_int<Bits, Signed>::limb_bits;
   for(int i = static_cast<int>(fixed_int<Bits, Signed>::limb_count) - 2; i >= 0; --i)
   {
      carry += static_cast<double_limb_type>(result.data()[i]) + 0xFFFFFFFF;
      result.data()[i] = static_cast<limb_type>(carry);
      carry >>= fixed_int<Bits, Signed>::limb_bits;
   }
   result.data()[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void subtract(fixed_int<Bits, Signed>& result, const signed_limb_type& o)
{
   if(o)
   {
      if(o < 0)
         add(result, static_cast<limb_type>(-o));
      else
         subtract(result, static_cast<limb_type>(o));
   }
}
template <unsigned Bits, bool Signed>
inline void increment(fixed_int<Bits, Signed>& result)
{
   static const limb_type one = 1;
   if(result.data().elems[fixed_int<Bits, Signed>::limb_count - 1] < fixed_int<Bits, Signed>::max_limb_value)
      ++result.data().elems[fixed_int<Bits, Signed>::limb_count - 1];
   else
      add(result, one);
}
template <unsigned Bits, bool Signed>
inline void decrement(fixed_int<Bits, Signed>& result)
{
   static const limb_type one = 1;
   if(result.data().elems[fixed_int<Bits, Signed>::limb_count - 1])
      --result.data().elems[fixed_int<Bits, Signed>::limb_count - 1];
   else
      subtract(result, one);
}
template <unsigned Bits, bool Signed>
inline void subtract(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& o)
{
   subtract(result, result, o);
}
template <unsigned Bits, bool Signed>
inline void subtract(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& a, const fixed_int<Bits, Signed>& b)
{
   // Subtract using modular arithmatic.
   // This is the same code as for addition, with the twist that we negate b "on the fly":
   double_limb_type carry = 1;
   for(int i = fixed_int<Bits, Signed>::limb_count - 1; i >= 0; --i)
   {
      carry += static_cast<double_limb_type>(a.data()[i]) + static_cast<double_limb_type>(~b.data()[i]);
      result.data()[i] = static_cast<limb_type>(carry);
      carry >>= fixed_int<Bits, Signed>::limb_bits;
   }
   result.data()[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void multiply(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& a, const fixed_int<Bits, Signed>& b)
{
   // Very simple long multiplication, only usable for small numbers of limb_type's
   // but that's the typical use case for this type anyway:
   if(&result == &a)
   {
      fixed_int<Bits, Signed> t(a);
      multiply(result, t, b);
      return;
   }
   if(&result == &b)
   {
      fixed_int<Bits, Signed> t(b);
      multiply(result, a, t);
      return;
   }
   double_limb_type carry = 0;
   for(unsigned i = 0; i < fixed_int<Bits, Signed>::limb_count; ++i)
      result.data()[i] = 0;
   for(int i = fixed_int<Bits, Signed>::limb_count - 1; i >= 0; --i)
   {
      for(int j = fixed_int<Bits, Signed>::limb_count - 1; j >= static_cast<int>(fixed_int<Bits, Signed>::limb_count) - i - 1; --j)
      {
         carry += static_cast<double_limb_type>(a.data()[i]) * static_cast<double_limb_type>(b.data()[j]);
         carry += result.data()[i + j + 1 - fixed_int<Bits, Signed>::limb_count];
         result.data()[i + j + 1 - fixed_int<Bits, Signed>::limb_count] = static_cast<limb_type>(carry);
         carry >>= fixed_int<Bits, Signed>::limb_bits;
      }
      carry = 0;
   }
   result.data()[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void multiply(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& a)
{
   // There is no in-place multiply:
   fixed_int<Bits, Signed> b(result);
   multiply(result, b, a);
}
template <unsigned Bits, bool Signed>
inline void multiply(fixed_int<Bits, Signed>& result, const limb_type& a)
{
   double_limb_type carry = 0;
   for(int i = fixed_int<Bits, Signed>::limb_count - 1; i >= 0; --i)
   {
      carry += static_cast<double_limb_type>(result.data()[i]) * static_cast<double_limb_type>(a);
      result.data()[i] = static_cast<limb_type>(carry);
      carry >>= fixed_int<Bits, Signed>::limb_bits;
   }
   result.data()[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void multiply(fixed_int<Bits, Signed>& result, const signed_limb_type& a)
{
   if(a > 0)
      multiply(result, static_cast<limb_type>(a));
   else
   {
      multiply(result, static_cast<limb_type>(-a));
      result.negate();
   }
}

/*
template <unsigned Bits, bool Signed>
limb_type bitcount(const fixed_int<Bits, Signed>& a)
{
   // returns the location of the MSB in a:
   limb_type i = 0;
   for(; (i < fixed_int<Bits, Signed>::limb_count) && (a.data()[i] == 0); ++i){}
   limb_type count = (fixed_int<Bits, Signed>::limb_count - i) * fixed_int<Bits, Signed>::limb_bits;
   if(!count)
      return count; // no bits are set, value is zero
   limb_type mask = static_cast<limb_type>(1u) << (fixed_int<Bits, Signed>::limb_bits - 1);
   while((a.data()[i] & mask) == 0)
   {
      --count;
      mask >>= 1;
   }
   return count;
}
template <unsigned Bits, bool Signed>
void divide_unsigned_helper(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& x, const fixed_int<Bits, Signed>& y, fixed_int<Bits, Signed>& r)
{
   if((&result == &x) || (&r == &x))
   {
      fixed_int<Bits, Signed> t(x);
      divide_unsigned_helper(result, t, y, r);
      return;
   }
   if((&result == &y) || (&r == &y))
   {
      fixed_int<Bits, Signed> t(y);
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
      fixed_int<Bits, Signed> rem;
      divide_unsigned_helper(result, x, y, rem);
      r = rem;
      return;
   }

   r = x;
   result = static_cast<limb_type>(0u);
   if(x.compare(y) < 0)
   {
      return; // We already have the answer: zero.
   }

   limb_type n = bitcount(x) - bitcount(y);

   if(n == 0)
   {
      // result is exactly 1:
      result = static_cast<limb_type>(1u);
      subtract(r, y);
      return;
   }

   // Together mask_index and mask give us the bit we may be about to set in the result:
   limb_type mask_index = fixed_int<Bits, Signed>::limb_count - 1 - n / fixed_int<Bits, Signed>::limb_bits;
   limb_type mask = static_cast<limb_type>(1u) << n % fixed_int<Bits, Signed>::limb_bits;
   fixed_int<Bits, Signed> t(y);
   left_shift(t, n);
   while(mask_index < fixed_int<Bits, Signed>::limb_count)
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
         mask = static_cast<limb_type>(1u) << (fixed_int<Bits, Signed>::limb_bits - 1);
      }
   }
   BOOST_ASSERT(r.compare(y) < 0); // remainder must be less than the divisor or our code has failed
}
*/
template <unsigned Bits, bool Signed>
void divide_unsigned_helper(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& x, const fixed_int<Bits, Signed>& y, fixed_int<Bits, Signed>& r)
{
   if((&result == &x) || (&r == &x))
   {
      fixed_int<Bits, Signed> t(x);
      divide_unsigned_helper(result, t, y, r);
      return;
   }
   if((&result == &y) || (&r == &y))
   {
      fixed_int<Bits, Signed> t(y);
      divide_unsigned_helper(result, x, t, r);
      return;
   }

   /*
    Very simple, fairly braindead long division.
    Start by setting the remainder equal to x, and the
    result equal to 0.  Then in each loop we calculate our
    "best guess" for how many times y divides into r,
    add our guess to the result, and subtract guess*y
    from the remainder r.  One wrinckle is that the remainder
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
      fixed_int<Bits, Signed> rem;
      divide_unsigned_helper(result, x, y, rem);
      r = rem;
      return;
   }

   r = x;
   result = static_cast<limb_type>(0u);
   if(x.compare(y) < 0)
   {
      return; // We already have the answer: zero.
   }

   //
   // Find the most significant words of numerator and denominator.
   // Note that this code can't run past the end of the array because
   // we know already that neither are all zero:
   //
   limb_type r_order = 0;
   while(r.data()[r_order] == 0)
      ++r_order;
   limb_type y_order = 0;
   while(y.data()[y_order] == 0)
      ++y_order;

   fixed_int<Bits, Signed> t;
   bool r_neg = false;

   //
   // See if we can short-circuit long division, and use basic arithmetic instead:
   //
   if(r_order == fixed_int<Bits, Signed>::limb_count - 1)
   {
      result = r.data()[fixed_int<Bits, Signed>::limb_count - 1] / y.data()[fixed_int<Bits, Signed>::limb_count - 1];
      r = x.data()[fixed_int<Bits, Signed>::limb_count - 1] % y.data()[fixed_int<Bits, Signed>::limb_count - 1];
      return;
   }
   else if(r_order == static_cast<int>(fixed_int<Bits, Signed>::limb_count) - 2)
   {
      double_limb_type a, b;
      a = (static_cast<double_limb_type>(r.data()[r_order]) << fixed_int<Bits, Signed>::limb_bits) | r.data()[r_order + 1];
      b = y_order < fixed_int<Bits, Signed>::limb_count - 1 ? 
         (static_cast<double_limb_type>(y.data()[y_order]) << fixed_int<Bits, Signed>::limb_bits) | y.data()[y_order + 1] 
         : y.data()[y_order];
      result = a / b;
      r = a % b;
      return;
   }

   //fixed_int<Bits, Signed> last_r;
   //bool last_neg;
   do
   {
      //
      // Update r_order, this can't run past the end as r must be non-zero at this point:
      //
      while(r.data()[r_order] == 0)
         ++r_order;
      //
      // Calculate our best guess for how many times y divides into r:
      //
      limb_type guess;
      //last_r = r;
      //last_neg = r_neg;
      if((r.data()[r_order] <= y.data()[y_order]) && (r_order < fixed_int<Bits, Signed>::limb_count - 1))
      {
         double_limb_type a, b, v;
         a = (static_cast<double_limb_type>(r.data()[r_order]) << fixed_int<Bits, Signed>::limb_bits) | r.data()[r_order + 1];
         b = y.data()[y_order];
         v = a / b;
         if(v > fixed_int<Bits, Signed>::max_limb_value)
            guess = 1;
         else
         {
            guess = static_cast<limb_type>(v);
            ++r_order;
         }
      }
      else
      {
         double_limb_type a, b, v;
         a = (r_order < fixed_int<Bits, Signed>::limb_count - 1) ? (static_cast<double_limb_type>(r.data()[r_order]) << fixed_int<Bits, Signed>::limb_bits) | r.data()[r_order + 1] : r.data()[r_order];
         b = (y_order < fixed_int<Bits, Signed>::limb_count - 1) ? (static_cast<double_limb_type>(y.data()[y_order]) << fixed_int<Bits, Signed>::limb_bits) | y.data()[y_order + 1] : (static_cast<double_limb_type>(y.data()[y_order])  << fixed_int<Bits, Signed>::limb_bits);
         v = a / b;
         guess = static_cast<limb_type>(v);
         //guess = r.data()[r_order] / y.data()[y_order];
      }
      //
      // Update result:
      //
      limb_type shift = y_order - r_order;
      t = limb_type(0);
      t.data()[fixed_int<Bits, Signed>::limb_count - 1 - shift] = guess;
      if(r_neg)
         subtract(result, t);
      else
         add(result, t);
      //
      // Calculate guess * y, we use a fused mutiply-shift O(N) for this
      // rather than a full O(N^2) multiply:
      //
      double_limb_type carry = 0;
      for(unsigned i = fixed_int<Bits, Signed>::limb_count - 1; i > fixed_int<Bits, Signed>::limb_count - shift - 1; --i)
         t.data()[i] = 0;
      for(int i = fixed_int<Bits, Signed>::limb_count - 1; i >= static_cast<int>(shift); --i)
      {
         carry += static_cast<double_limb_type>(y.data()[i]) * static_cast<double_limb_type>(guess);
         t.data()[i - shift] = static_cast<limb_type>(carry);
         carry >>= fixed_int<Bits, Signed>::limb_bits;
      }
      t.data()[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
       //
      // Update r:
      //
      subtract(r, t);
      if(r.data()[0] & fixed_int<Bits, Signed>::sign_bit_mask)
      {
         r.negate();
         r_neg = !r_neg;
      }
   }
   while(r.compare(y) > 0);

   //
   // We now just have to normalise the result:
   //
   if(r_neg)
   {
      // We have one too many in the result:
      decrement(result);
      r.negate();
      add(r, y);
   }

   BOOST_ASSERT(r.compare(y) < 0); // remainder must be less than the divisor or our code has failed
}

template <unsigned Bits, bool Signed>
inline void divide(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& a, const fixed_int<Bits, Signed>& b)
{
   fixed_int<Bits, Signed> r;
   if(Signed && (a.data()[0] & fixed_int<Bits, Signed>::sign_bit_mask))
   {
      if(Signed && (b.data()[0] & fixed_int<Bits, Signed>::sign_bit_mask))
      {
         fixed_int<Bits, Signed> t1(a), t2(b);
         t1.negate();
         t2.negate();
         divide_unsigned_helper(result, t1, t2, r);
      }
      else
      {
         fixed_int<Bits, Signed> t(a);
         t.negate();
         divide_unsigned_helper(result, t, b, r);
         result.negate();
      }
   }
   else if(Signed && (b.data()[0] & fixed_int<Bits, Signed>::sign_bit_mask))
   {
      fixed_int<Bits, Signed> t(b);
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
inline void divide(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& b)
{
   // There is no in place divide:
   fixed_int<Bits, Signed> a(result);
   divide(result, a, b);
}
template <unsigned Bits, bool Signed>
inline void modulus(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& a, const fixed_int<Bits, Signed>& b)
{
   fixed_int<Bits, Signed> r;
   if(Signed && (a.data()[0] & fixed_int<Bits, Signed>::sign_bit_mask))
   {
      if(Signed && (b.data()[0] & fixed_int<Bits, Signed>::sign_bit_mask))
      {
         fixed_int<Bits, Signed> t1(a), t2(b);
         t1.negate();
         t2.negate();
         divide_unsigned_helper(r, t1, t2, result);
         result.negate();
      }
      else
      {
         fixed_int<Bits, Signed> t(a);
         t.negate();
         divide_unsigned_helper(r, t, b, result);
         result.negate();
      }
   }
   else if(Signed && (b.data()[0] & fixed_int<Bits, Signed>::sign_bit_mask))
   {
      fixed_int<Bits, Signed> t(b);
      t.negate();
      divide_unsigned_helper(r, a, t, result);
   }
   else
   {
      divide_unsigned_helper(r, a, b, result);
   }
}
template <unsigned Bits, bool Signed>
inline void modulus(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& b)
{
   // There is no in place divide:
   fixed_int<Bits, Signed> a(result);
   modulus(result, a, b);
}

template <unsigned Bits, bool Signed>
inline void bitwise_and(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& o)
{
   for(typename fixed_int<Bits, Signed>::data_type::size_type i = 0; i < fixed_int<Bits, Signed>::limb_count; ++i)
      result.data()[i] &= o.data()[i];
}
template <unsigned Bits, bool Signed>
inline void bitwise_or(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& o)
{
   for(typename fixed_int<Bits, Signed>::data_type::size_type i = 0; i < fixed_int<Bits, Signed>::limb_count; ++i)
      result.data()[i] |= o.data()[i];
}
template <unsigned Bits, bool Signed>
inline void bitwise_xor(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& o)
{
   for(typename fixed_int<Bits, Signed>::data_type::size_type i = 0; i < fixed_int<Bits, Signed>::limb_count; ++i)
      result.data()[i] ^= o.data()[i];
   result.data()[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void complement(fixed_int<Bits, Signed>& result, const fixed_int<Bits, Signed>& o)
{
   for(typename fixed_int<Bits, Signed>::data_type::size_type i = 0; i < fixed_int<Bits, Signed>::limb_count; ++i)
      result.data()[i] = ~o.data()[i];
   result.data()[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void left_shift(fixed_int<Bits, Signed>& result, double_limb_type s)
{
   if(s >= Bits)
   {
      result = static_cast<limb_type>(0);
      return;
   }
   limb_type offset = static_cast<limb_type>(s / fixed_int<Bits, Signed>::limb_bits);
   limb_type shift  = static_cast<limb_type>(s % fixed_int<Bits, Signed>::limb_bits);
   unsigned i = 0;
   if(shift)
   {
      // This code only works when shift is non-zero, otherwise we invoke undefined behaviour!
      for(; i + offset + 1 < fixed_int<Bits, Signed>::limb_count; ++i)
      {
         result.data()[i] = result.data()[i+offset] << shift;
         result.data()[i] |= result.data()[i+offset+1] >> (fixed_int<Bits, Signed>::limb_bits - shift);
      }
      result.data()[i] = result.data()[i+offset] << shift;
      ++i;
      for(; i < fixed_int<Bits, Signed>::limb_count; ++i)
         result.data()[i] = 0;
   }
   else
   {
      for(; i + offset < fixed_int<Bits, Signed>::limb_count; ++i)
         result.data()[i] = result.data()[i+offset];
      for(; i < fixed_int<Bits, Signed>::limb_count; ++i)
         result.data()[i] = 0;
   }
   result.data()[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
}
template <unsigned Bits, bool Signed>
inline void right_shift(fixed_int<Bits, Signed>& result, double_limb_type s)
{
   limb_type fill = (Signed && (result.data()[0] & fixed_int<Bits, Signed>::sign_bit_mask)) ? fixed_int<Bits, Signed>::max_limb_value : 0u;
   if(s >= Bits)
   {
      for(unsigned i = 0; i < fixed_int<Bits, Signed>::limb_count; ++i)
         result.data()[i] = fill;
      return;
   }
   limb_type offset = static_cast<limb_type>(s / fixed_int<Bits, Signed>::limb_bits);
   limb_type shift  = static_cast<limb_type>(s % fixed_int<Bits, Signed>::limb_bits);
   int i = fixed_int<Bits, Signed>::limb_count - 1;
   if(shift)
   {
      // This code only works for non-zero shift, otherwise we invoke undefined behaviour!
      if(fill && (Bits % fixed_int<Bits, Signed>::limb_bits))
      {
         // We need to sign extend the leftmost bits, otherwise we may shift zeros into the result:
         result.data()[0] |= fill << (Bits % fixed_int<Bits, Signed>::limb_bits);
      }
      for(; i - offset > 0; --i)
      {
         result.data()[i] = result.data()[i-offset] >> shift;
         result.data()[i] |= result.data()[i-offset-1] << (fixed_int<Bits, Signed>::limb_bits - shift);
      }
      result.data()[i] = result.data()[i+offset] >> shift;
      result.data()[i] |= fill << (fixed_int<Bits, Signed>::limb_bits - shift);
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
   result.data()[0] &= fixed_int<Bits, Signed>::upper_limb_mask;
}

template <class R, unsigned Bits, bool Signed>
inline typename enable_if<is_integral<R>, void>::type convert_to(R* result, const fixed_int<Bits, Signed>& backend)
{
   unsigned shift = (fixed_int<Bits, Signed>::limb_count - 1) * fixed_int<Bits, Signed>::limb_bits;
   *result = 0;
   for(unsigned i = 0; i < fixed_int<Bits, Signed>::limb_count; ++i)
   {
      *result += static_cast<R>(backend.data()[i]) << shift;
      shift -= fixed_int<Bits, Signed>::limb_bits;
   }
}

template <class R, unsigned Bits, bool Signed>
inline typename enable_if<is_floating_point<R>, void>::type convert_to(R* result, const fixed_int<Bits, Signed>& backend)
{
   unsigned shift = (fixed_int<Bits, Signed>::limb_count - 1) * fixed_int<Bits, Signed>::limb_bits;
   *result = 0;
   for(unsigned i = 0; i < fixed_int<Bits, Signed>::limb_count; ++i)
   {
      *result += static_cast<R>(std::ldexp(static_cast<double>(backend.data()[i]), shift));
      shift -= fixed_int<Bits, Signed>::limb_bits;
   }
}

template <unsigned Bits, bool Signed>
inline bool is_zero(const fixed_int<Bits, Signed>& val)
{
   for(typename fixed_int<Bits, Signed>::data_type::size_type i = 0; i < fixed_int<Bits, Signed>::limb_count; ++i)
   {
      if(val.data()[i])
         return false;
   }
   return true;
}
template <unsigned Bits>
inline int get_sign(const fixed_int<Bits, false>& val)
{
   return is_zero(val) ? 0 : 1;
}
template <unsigned Bits>
inline int get_sign(const fixed_int<Bits, true>& val)
{
   return is_zero(val) ? 0 : val.data()[0] & fixed_int<Bits, true>::sign_bit_mask ? -1 : 1;
}

template <unsigned Bits, bool Signed>
struct number_category<fixed_int<Bits, Signed> > : public mpl::int_<number_kind_integer>{};

typedef mp_number<fixed_int<128, false> > mp_uint128_t;
typedef mp_number<fixed_int<256, false> > mp_uint256_t;
typedef mp_number<fixed_int<512, false> > mp_uint512_t;

typedef mp_number<fixed_int<128, true> > mp_int128_t;
typedef mp_number<fixed_int<256, true> > mp_int256_t;
typedef mp_number<fixed_int<512, true> > mp_int512_t;

}} // namespaces


namespace std{

template <unsigned Bits, bool Signed>
class numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::fixed_int<Bits, Signed> > >
{
   typedef boost::multiprecision::mp_number<boost::multiprecision::fixed_int<Bits, Signed> > number_type;

   struct initializer
   {
      initializer()
      {
         (std::numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::fixed_int<Bits, Signed> > >::min)();
         (std::numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::fixed_int<Bits, Signed> > >::max)();
      }
      void do_nothing()const{}
   };
   static const initializer init;
public:
   BOOST_STATIC_CONSTEXPR bool is_specialized = true;
   //
   // Largest and smallest numbers are bounded only by available memory, set
   // to zero:
   //
   BOOST_STATIC_CONSTEXPR number_type (min)() BOOST_MP_NOEXCEPT
   {
      static bool init = false;
      static number_type val;
      if(!init)
      {
         init = true;
         val = Signed ? number_type(1) << Bits - 1 : 0;
      }
      return val;
   }
   BOOST_STATIC_CONSTEXPR number_type (max)() BOOST_MP_NOEXCEPT 
   { 
      static bool init = false;
      static number_type val;
      if(!init)
      {
         init = true;
         val = Signed ? number_type(~(number_type(1) << Bits - 1)) : number_type(~number_type(0));
      }
      return val;
   }
   BOOST_STATIC_CONSTEXPR number_type lowest() BOOST_MP_NOEXCEPT { return (min)(); }
   BOOST_STATIC_CONSTEXPR int digits = Bits;
   BOOST_STATIC_CONSTEXPR int digits10 = (digits * 301L) / 1000;
   BOOST_STATIC_CONSTEXPR int max_digits10 = digits10 + 2;
   BOOST_STATIC_CONSTEXPR bool is_signed = Signed;
   BOOST_STATIC_CONSTEXPR bool is_integer = true;
   BOOST_STATIC_CONSTEXPR bool is_exact = true;
   BOOST_STATIC_CONSTEXPR int radix = 2;
   BOOST_STATIC_CONSTEXPR number_type epsilon() BOOST_MP_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR number_type round_error() BOOST_MP_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR int min_exponent = 0;
   BOOST_STATIC_CONSTEXPR int min_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR bool has_infinity = false;
   BOOST_STATIC_CONSTEXPR bool has_quiet_NaN = false;
   BOOST_STATIC_CONSTEXPR bool has_signaling_NaN = false;
   BOOST_STATIC_CONSTEXPR float_denorm_style has_denorm = denorm_absent;
   BOOST_STATIC_CONSTEXPR bool has_denorm_loss = false;
   BOOST_STATIC_CONSTEXPR number_type infinity() BOOST_MP_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR number_type quiet_NaN() BOOST_MP_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR number_type signaling_NaN() BOOST_MP_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR number_type denorm_min() BOOST_MP_NOEXCEPT { return 0; }
   BOOST_STATIC_CONSTEXPR bool is_iec559 = false;
   BOOST_STATIC_CONSTEXPR bool is_bounded = true;
   BOOST_STATIC_CONSTEXPR bool is_modulo = true;
   BOOST_STATIC_CONSTEXPR bool traps = false;
   BOOST_STATIC_CONSTEXPR bool tinyness_before = false;
   BOOST_STATIC_CONSTEXPR float_round_style round_style = round_toward_zero;
};

template <unsigned Bits, bool Signed>
typename numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::fixed_int<Bits, Signed> > >::initializer const
   numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::fixed_int<Bits, Signed> > >::init;
}

#endif
