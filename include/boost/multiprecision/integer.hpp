///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_MP_INTEGER_HPP
#define BOOST_MP_INTEGER_HPP

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/detail/bitscan.hpp>

namespace boost {
namespace multiprecision {

template <class Integer, class I2>
inline BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value && boost::multiprecision::detail::is_integral<I2>::value, Integer&>::type
multiply(Integer& result, const I2& a, const I2& b)
{
   return result = static_cast<Integer>(a) * static_cast<Integer>(b);
}
template <class Integer, class I2>
inline BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value && boost::multiprecision::detail::is_integral<I2>::value, Integer&>::type
add(Integer& result, const I2& a, const I2& b)
{
   return result = static_cast<Integer>(a) + static_cast<Integer>(b);
}
template <class Integer, class I2>
inline BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value && boost::multiprecision::detail::is_integral<I2>::value, Integer&>::type
subtract(Integer& result, const I2& a, const I2& b)
{
   return result = static_cast<Integer>(a) - static_cast<Integer>(b);
}

template <class Integer>
inline BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value>::type divide_qr(const Integer& x, const Integer& y, Integer& q, Integer& r)
{
   q = x / y;
   r = x % y;
}

template <class I1, class I2>
inline BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<I1>::value && boost::multiprecision::detail::is_integral<I2>::value, I2>::type integer_modulus(const I1& x, I2 val)
{
   return static_cast<I2>(x % val);
}

namespace detail {
//
// Figure out the kind of integer that has twice as many bits as some builtin
// integer type I.  Use a native type if we can (including types which may not
// be recognised by boost::int_t because they're larger than boost::long_long_type),
// otherwise synthesize a cpp_int to do the job.
//
template <class I>
struct double_integer
{
   static constexpr const unsigned int_t_digits =
       2 * sizeof(I) <= sizeof(boost::long_long_type) ? std::numeric_limits<I>::digits * 2 : 1;

   using type = typename std::conditional<
       2 * sizeof(I) <= sizeof(boost::long_long_type),
       typename std::conditional<
           boost::multiprecision::detail::is_signed<I>::value && boost::multiprecision::detail::is_integral<I>::value,
           typename boost::int_t<int_t_digits>::least,
           typename boost::uint_t<int_t_digits>::least>::type,
       typename std::conditional<
           2 * sizeof(I) <= sizeof(double_limb_type),
           typename std::conditional<
               boost::multiprecision::detail::is_signed<I>::value && boost::multiprecision::detail::is_integral<I>::value,
               signed_double_limb_type,
               double_limb_type>::type,
           number<cpp_int_backend<sizeof(I) * CHAR_BIT * 2, sizeof(I) * CHAR_BIT * 2, (boost::multiprecision::detail::is_signed<I>::value ? signed_magnitude : unsigned_magnitude), unchecked, void> > >::type>::type;
};

} // namespace detail

template <class I1, class I2, class I3>
BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<I1>::value && boost::multiprecision::detail::is_unsigned<I2>::value && is_integral<I3>::value, I1>::type
powm(const I1& a, I2 b, I3 c)
{
   using double_type = typename detail::double_integer<I1>::type;

   I1          x(1), y(a);
   double_type result(0);

   while (b > 0)
   {
      if (b & 1)
      {
         multiply(result, x, y);
         x = integer_modulus(result, c);
      }
      multiply(result, y, y);
      y = integer_modulus(result, c);
      b >>= 1;
   }
   return x % c;
}

template <class I1, class I2, class I3>
inline BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<I1>::value && boost::multiprecision::detail::is_signed<I2>::value && boost::multiprecision::detail::is_integral<I2>::value && boost::multiprecision::detail::is_integral<I3>::value, I1>::type
powm(const I1& a, I2 b, I3 c)
{
   if (b < 0)
   {
      BOOST_THROW_EXCEPTION(std::runtime_error("powm requires a positive exponent."));
   }
   return powm(a, static_cast<typename boost::multiprecision::detail::make_unsigned<I2>::type>(b), c);
}

template <class Integer>
BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value, unsigned>::type lsb(const Integer& val)
{
   if (val <= 0)
   {
      if (val == 0)
      {
         BOOST_THROW_EXCEPTION(std::domain_error("No bits were set in the operand."));
      }
      else
      {
         BOOST_THROW_EXCEPTION(std::domain_error("Testing individual bits in negative values is not supported - results are undefined."));
      }
   }
   return detail::find_lsb(val);
}

template <class Integer>
BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value, unsigned>::type msb(Integer val)
{
   if (val <= 0)
   {
      if (val == 0)
      {
         BOOST_THROW_EXCEPTION(std::domain_error("No bits were set in the operand."));
      }
      else
      {
         BOOST_THROW_EXCEPTION(std::domain_error("Testing individual bits in negative values is not supported - results are undefined."));
      }
   }
   return detail::find_msb(val);
}

template <class Integer>
BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value, bool>::type bit_test(const Integer& val, unsigned index)
{
   Integer mask = 1;
   if (index >= sizeof(Integer) * CHAR_BIT)
      return 0;
   if (index)
      mask <<= index;
   return val & mask ? true : false;
}

template <class Integer>
BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value, Integer&>::type bit_set(Integer& val, unsigned index)
{
   Integer mask = 1;
   if (index >= sizeof(Integer) * CHAR_BIT)
      return val;
   if (index)
      mask <<= index;
   val |= mask;
   return val;
}

template <class Integer>
BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value, Integer&>::type bit_unset(Integer& val, unsigned index)
{
   Integer mask = 1;
   if (index >= sizeof(Integer) * CHAR_BIT)
      return val;
   if (index)
      mask <<= index;
   val &= ~mask;
   return val;
}

template <class Integer>
BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value, Integer&>::type bit_flip(Integer& val, unsigned index)
{
   Integer mask = 1;
   if (index >= sizeof(Integer) * CHAR_BIT)
      return val;
   if (index)
      mask <<= index;
   val ^= mask;
   return val;
}

template <class Integer>
BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value, Integer>::type karatsuba_sqrt(const Integer& x, Integer& r, size_t bits)
{
   // we can calculate it faster with std::sqrt
   if (bits <= 64) {
      const uint64_t int32max = uint64_t((std::numeric_limits<uint32_t>::max)());
      uint64_t val = static_cast<uint64_t>(x);
      uint64_t s = static_cast<uint64_t>(std::sqrt(static_cast<long double>(val)));
      // converting to long double can loose some precision, and `sqrt` can give eps error, so we'll fix this
      // this is needed
      while (s > int32max || s * s > val) s--;
      // in my tests this never fired, but theoretically this might be needed
      while (s < int32max && (s + 1) * (s + 1) <= val) s++;
      r = Integer(val - s * s);
      return Integer(s);
   }
   // https://hal.inria.fr/file/index/docid/72854/filename/RR-3805.pdf
   size_t b = bits / 4;
   Integer s = karatsuba_sqrt(Integer(x >> (b * 2)), r, bits - b * 2);
   Integer q{};
   r <<= b;
   divide_qr(Integer(r + ((x & ((Integer(1) << (b * 2)) - 1)) >> b)), Integer(s << 1), q, r);
   r <<= b;
   r += (x & ((Integer(1) << b) - 1));
   s <<= b;
   s += q;
   q *= q;
   // we substract after, so it works for unsigned integers too
   if (r < q) {
      r += (s << 1) - 1;
      s--;
   }
   r -= q;
   return s;
}

template <class Integer>
BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value, Integer>::type sqrt(const Integer& x, Integer& r)
{
   if (x == Integer(0)) {
      r = Integer(0);
      return Integer(0);
   }
   return karatsuba_sqrt(x, r, msb(x) + 1);
}

template <class Integer>
BOOST_MP_CXX14_CONSTEXPR typename std::enable_if<boost::multiprecision::detail::is_integral<Integer>::value, Integer>::type sqrt(const Integer& x)
{
   Integer r(0);
   return sqrt(x, r);
}

}} // namespace boost::multiprecision

#endif
