///////////////////////////////////////////////////////////////
//  Copyright 2012-2020 John Maddock.
//  Copyright 2020 Madhur Chauhan.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//   https://www.boost.org/LICENSE_1_0.txt)
//
// Comparison operators for cpp_int_backend:
//
#ifndef BOOST_MP_CPP_INT_MISC_HPP
#define BOOST_MP_CPP_INT_MISC_HPP

#include <boost/multiprecision/detail/constexpr.hpp>
#include <boost/multiprecision/detail/bitscan.hpp> // lsb etc
#include <boost/integer/common_factor_rt.hpp>      // gcd/lcm
#include <boost/functional/hash_fwd.hpp>
#include <numeric> // std::gcd

#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable : 4702)
#pragma warning(disable : 4127) // conditional expression is constant
#pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned
#endif

namespace boost { namespace multiprecision { namespace backends {

template <class T, bool has_limits = std::numeric_limits<T>::is_specialized>
struct numeric_limits_workaround : public std::numeric_limits<T>
{
};
template <class R>
struct numeric_limits_workaround<R, false>
{
   BOOST_STATIC_CONSTEXPR unsigned digits = ~static_cast<R>(0) < 0 ? sizeof(R) * CHAR_BIT - 1 : sizeof(R) * CHAR_BIT;
   static BOOST_CONSTEXPR R (min)(){ return (static_cast<R>(-1) < 0) ? static_cast<R>(1) << digits : 0; }
   static BOOST_CONSTEXPR R (max)() { return (static_cast<R>(-1) < 0) ? ~(static_cast<R>(1) << digits) : ~static_cast<R>(0); }
};

template <class R, class CppInt>
BOOST_MP_CXX14_CONSTEXPR void check_in_range(const CppInt& val, const mpl::int_<checked>&)
{
   typedef typename boost::multiprecision::detail::canonical<R, CppInt>::type cast_type;

   if (val.sign())
   {
      if (boost::is_signed<R>::value == false)
         BOOST_THROW_EXCEPTION(std::range_error("Attempt to assign a negative value to an unsigned type."));
      if (val.compare(static_cast<cast_type>((numeric_limits_workaround<R>::min)())) < 0)
         BOOST_THROW_EXCEPTION(std::overflow_error("Could not convert to the target type - -value is out of range."));
   }
   else
   {
      if (val.compare(static_cast<cast_type>((numeric_limits_workaround<R>::max)())) > 0)
         BOOST_THROW_EXCEPTION(std::overflow_error("Could not convert to the target type - -value is out of range."));
   }
}
template <class R, class CppInt>
inline BOOST_MP_CXX14_CONSTEXPR void check_in_range(const CppInt& /*val*/, const mpl::int_<unchecked>&) BOOST_NOEXCEPT {}

inline BOOST_MP_CXX14_CONSTEXPR void check_is_negative(const mpl::true_&) BOOST_NOEXCEPT {}
inline void                          check_is_negative(const mpl::false_&)
{
   BOOST_THROW_EXCEPTION(std::range_error("Attempt to assign a negative value to an unsigned type."));
}

template <class Integer>
inline BOOST_MP_CXX14_CONSTEXPR Integer negate_integer(Integer i, const mpl::true_&) BOOST_NOEXCEPT
{
   return -i;
}
template <class Integer>
inline BOOST_MP_CXX14_CONSTEXPR Integer negate_integer(Integer i, const mpl::false_&) BOOST_NOEXCEPT
{
   return ~(i - 1);
}

template <class R, unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_integral<R>::value && !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, void>::type
eval_convert_to(R* result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& backend)
{
   typedef mpl::int_<Checked1> checked_type;
   check_in_range<R>(backend, checked_type());

   BOOST_IF_CONSTEXPR(numeric_limits_workaround<R>::digits < cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits)
   {
      if ((backend.sign() && boost::is_signed<R>::value) && (1 + static_cast<boost::multiprecision::limb_type>((std::numeric_limits<R>::max)()) <= backend.limbs()[0]))
      {
         *result = (numeric_limits_workaround<R>::min)();
         return;
      }
      else if (boost::is_signed<R>::value && !backend.sign() && static_cast<boost::multiprecision::limb_type>((std::numeric_limits<R>::max)()) <= backend.limbs()[0])
      {
         *result = (numeric_limits_workaround<R>::max)();
         return;
      }
      else
         *result = static_cast<R>(backend.limbs()[0]);
   }
   else
      *result = static_cast<R>(backend.limbs()[0]);
   unsigned shift = cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   unsigned i     = 1;
   BOOST_IF_CONSTEXPR(numeric_limits_workaround<R>::digits > cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits)
   {
      while ((i < backend.size()) && (shift < static_cast<unsigned>(numeric_limits_workaround<R>::digits - cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits)))
      {
         *result += static_cast<R>(backend.limbs()[i]) << shift;
         shift += cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
         ++i;
      }
      //
      // We have one more limb to extract, but may not need all the bits, so treat this as a special case:
      //
      if (i < backend.size())
      {
         const limb_type mask = numeric_limits_workaround<R>::digits - shift == cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits ? ~static_cast<limb_type>(0) : (static_cast<limb_type>(1u) << (numeric_limits_workaround<R>::digits - shift)) - 1;
         *result += (static_cast<R>(backend.limbs()[i]) & mask) << shift;
         if ((static_cast<R>(backend.limbs()[i]) & static_cast<limb_type>(~mask)) || (i + 1 < backend.size()))
         {
            // Overflow:
            if (backend.sign())
            {
               check_is_negative(boost::is_signed<R>());
               *result = (numeric_limits_workaround<R>::min)();
            }
            else if (boost::is_signed<R>::value)
               *result = (numeric_limits_workaround<R>::max)();
            return;
         }
      }
   }
   else if (backend.size() > 1)
   {
      // Overflow:
      if (backend.sign())
      {
         check_is_negative(boost::is_signed<R>());
         *result = (numeric_limits_workaround<R>::min)();
      }
      else if (boost::is_signed<R>::value)
         *result = (numeric_limits_workaround<R>::max)();
      return;
   }
   if (backend.sign())
   {
      check_is_negative(mpl::bool_<boost::is_signed<R>::value>());
      *result = negate_integer(*result, mpl::bool_<boost::is_signed<R>::value>());
   }
}

template <class R, unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_floating_point<R>::value && !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, void>::type
eval_convert_to(R* result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& backend) BOOST_MP_NOEXCEPT_IF(is_arithmetic<R>::value)
{
   typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::const_limb_pointer p     = backend.limbs();
   unsigned                                                                                          shift = cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   *result                                                                                                 = static_cast<R>(*p);
   for (unsigned i = 1; i < backend.size(); ++i)
   {
      *result += static_cast<R>(std::ldexp(static_cast<long double>(p[i]), shift));
      shift += cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   }
   if (backend.sign())
      *result = -*result;
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, bool>::type
eval_is_zero(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val) BOOST_NOEXCEPT
{
   return (val.size() == 1) && (val.limbs()[0] == 0);
}
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, int>::type
eval_get_sign(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val) BOOST_NOEXCEPT
{
   return eval_is_zero(val) ? 0 : val.sign() ? -1 : 1;
}
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_abs(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   result = val;
   result.sign(false);
}

//
// Get the location of the least-significant-bit:
//
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, unsigned>::type
eval_lsb(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a)
{
   using default_ops::eval_get_sign;
   if (eval_get_sign(a) == 0)
   {
      BOOST_THROW_EXCEPTION(std::range_error("No bits were set in the operand."));
   }
   if (a.sign())
   {
      BOOST_THROW_EXCEPTION(std::range_error("Testing individual bits in negative values is not supported - results are undefined."));
   }

   //
   // Find the index of the least significant limb that is non-zero:
   //
   unsigned index = 0;
   while (!a.limbs()[index] && (index < a.size()))
      ++index;
   //
   // Find the index of the least significant bit within that limb:
   //
   unsigned result = boost::multiprecision::detail::find_lsb(a.limbs()[index]);

   return result + index * cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
}

//
// Get the location of the most-significant-bit:
//
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, unsigned>::type
eval_msb_imp(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a)
{
   //
   // Find the index of the most significant bit that is non-zero:
   //
   return (a.size() - 1) * cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits + boost::multiprecision::detail::find_msb(a.limbs()[a.size() - 1]);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, unsigned>::type
eval_msb(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a)
{
   using default_ops::eval_get_sign;
   if (eval_get_sign(a) == 0)
   {
      BOOST_THROW_EXCEPTION(std::range_error("No bits were set in the operand."));
   }
   if (a.sign())
   {
      BOOST_THROW_EXCEPTION(std::range_error("Testing individual bits in negative values is not supported - results are undefined."));
   }
   return eval_msb_imp(a);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, bool>::type
eval_bit_test(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val, unsigned index) BOOST_NOEXCEPT
{
   unsigned  offset = index / cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   unsigned  shift  = index % cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   limb_type mask   = shift ? limb_type(1u) << shift : limb_type(1u);
   if (offset >= val.size())
      return false;
   return val.limbs()[offset] & mask ? true : false;
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_bit_set(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val, unsigned index)
{
   unsigned  offset = index / cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   unsigned  shift  = index % cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   limb_type mask   = shift ? limb_type(1u) << shift : limb_type(1u);
   if (offset >= val.size())
   {
      unsigned os = val.size();
      val.resize(offset + 1, offset + 1);
      if (offset >= val.size())
         return; // fixed precision overflow
      for (unsigned i = os; i <= offset; ++i)
         val.limbs()[i] = 0;
   }
   val.limbs()[offset] |= mask;
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_bit_unset(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val, unsigned index) BOOST_NOEXCEPT
{
   unsigned  offset = index / cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   unsigned  shift  = index % cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   limb_type mask   = shift ? limb_type(1u) << shift : limb_type(1u);
   if (offset >= val.size())
      return;
   val.limbs()[offset] &= ~mask;
   val.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_bit_flip(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val, unsigned index)
{
   unsigned  offset = index / cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   unsigned  shift  = index % cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   limb_type mask   = shift ? limb_type(1u) << shift : limb_type(1u);
   if (offset >= val.size())
   {
      unsigned os = val.size();
      val.resize(offset + 1, offset + 1);
      if (offset >= val.size())
         return; // fixed precision overflow
      for (unsigned i = os; i <= offset; ++i)
         val.limbs()[i] = 0;
   }
   val.limbs()[offset] ^= mask;
   val.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_qr(
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& x,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& y,
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       q,
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       r) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   divide_unsigned_helper(&q, x, y, r);
   q.sign(x.sign() != y.sign());
   r.sign(x.sign());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_qr(
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& x,
    limb_type                                                                   y,
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       q,
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       r) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   divide_unsigned_helper(&q, x, y, r);
   q.sign(x.sign());
   r.sign(x.sign());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, class U>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_integral<U>::value>::type eval_qr(
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& x,
    U                                                                           y,
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       q,
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       r) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   using default_ops::eval_qr;
   cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> t;
   t = y;
   eval_qr(x, t, q, r);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, class Integer>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_unsigned<Integer>::value && !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, Integer>::type
eval_integer_modulus(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a, Integer mod)
{
   if ((sizeof(Integer) <= sizeof(limb_type)) || (mod <= (std::numeric_limits<limb_type>::max)()))
   {
      const int              n = a.size();
      const double_limb_type two_n_mod = static_cast<limb_type>(1u) + (~static_cast<limb_type>(0u) - mod) % mod;
      limb_type              res = a.limbs()[n - 1] % mod;

      for (int i = n - 2; i >= 0; --i)
         res = static_cast<limb_type>((res * two_n_mod + a.limbs()[i]) % mod);
      return res;
   }
   else
   {
      return default_ops::eval_integer_modulus(a, mod);
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, class Integer>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_signed<Integer>::value && !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, Integer>::type
eval_integer_modulus(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& x, Integer val)
{
   return eval_integer_modulus(x, boost::multiprecision::detail::unsigned_abs(val));
}

BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR limb_type eval_gcd(limb_type u, limb_type v)
{
   // boundary cases
   if (!u || !v)
      return u | v;
#if __cpp_lib_gcd_lcm >= 201606L
   return std::gcd(u, v);
#else
   unsigned shift = boost::multiprecision::detail::find_lsb(u | v);
   u >>= boost::multiprecision::detail::find_lsb(u);
   do
   {
      v >>= boost::multiprecision::detail::find_lsb(v);
      if (u > v)
         std_constexpr::swap(u, v);
      v -= u;
   } while (v);
   return u << shift;
#endif
}

inline BOOST_MP_CXX14_CONSTEXPR double_limb_type eval_gcd(double_limb_type u, double_limb_type v)
{
#if (__cpp_lib_gcd_lcm >= 201606L) && (!defined(BOOST_HAS_INT128) || !defined(__STRICT_ANSI__))
   return std::gcd(u, v);
#else
   unsigned shift = boost::multiprecision::detail::find_lsb(u | v);
   u >>= boost::multiprecision::detail::find_lsb(u);
   do
   {
      v >>= boost::multiprecision::detail::find_lsb(v);
      if (u > v)
         std_constexpr::swap(u, v);
      v -= u;
   } while (v);
   return u << shift;
#endif
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_gcd(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a,
    limb_type                                                                   b)
{
   int s = eval_get_sign(a);
   if (!b || !s)
   {
      result = a;
      *result.limbs() |= b;
   }
   else
   {
      eval_modulus(result, a, b);
      limb_type& res = *result.limbs();
      res = eval_gcd(res, b);
   }
   result.sign(false);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_gcd(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a,
    double_limb_type                                                            b)
{
   int s = eval_get_sign(a);
   if (!b || !s)
   {
      if (!s)
         result = b;
      else
         result = a;
      return;
   }
   double_limb_type res = 0;
   if(a.sign() == 0)
      res = eval_integer_modulus(a, b);
   else
   {
      cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> t(a);
      t.negate();
      res = eval_integer_modulus(t, b);
   }
   res            = eval_gcd(res, b);
   result = res;
   result.sign(false);
}
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_gcd(
   cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result,
   const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a,
   signed_double_limb_type                                                     v)
{
   eval_gcd(result, a, static_cast<double_limb_type>(v < 0 ? -v : v));
}
//
// These 2 overloads take care of gcd against an (unsigned) short etc:
//
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, class Integer>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_unsigned<Integer>::value && (sizeof(Integer) <= sizeof(limb_type)) && !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_gcd(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a,
    const Integer&                                                              v)
{
   eval_gcd(result, a, static_cast<limb_type>(v));
}
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, class Integer>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_signed<Integer>::value && (sizeof(Integer) <= sizeof(limb_type)) && !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_gcd(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a,
    const Integer&                                                              v)
{
   eval_gcd(result, a, static_cast<limb_type>(v < 0 ? -v : v));
}

#ifndef TEST_ORIGINAL_GCD
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, class Storage>
BOOST_MP_CXX14_CONSTEXPR void eval_gcd_lehmer(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& U, cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& V, unsigned lu, Storage& storage)
{
   unsigned  h = lu % bits_per_limb;
   limb_type u = U.limbs()[U.size() - 1];
   limb_type v = V.size() < U.size() ? 0 : V.limbs()[V.size() - 1];
   if (h)
   {
      u <<= bits_per_limb - h;
      u |= U.limbs()[U.size() - 2] >> h;
      v <<= bits_per_limb - h;
      v |= V.limbs()[U.size() - 2] >> h;
   }
   //
   // x[i+0] is positive for even i.
   // y[i+0] is positive for odd i.
   //
   // However we track only absolute values here:
   //
   limb_type x[3] = { 1, 0 };
   limb_type y[3] = { 0, 1 };
   unsigned         i = 0;
   bool             done = false;

   while (true)
   {
      limb_type q = u / v;
      x[2] = x[0] + q * x[1];
      y[2] = y[0] + q * y[1];
      limb_type tu = u;
      u = v;
      v = tu - q * v;
      ++i;
      if ((i & 1u) == 0)
      {
         BOOST_ASSERT(u > v);
         done = (v < x[2]) || ((u - v) < (y[2] - y[1]));
      }
      else
      {
         BOOST_ASSERT(u > v);
         done = (v < y[2]) || ((u - v) < (x[2] - x[1]));
      }
      if (done)
      {
         if (i == 1)
         {
            // No change to U and V we've stalled!
            cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> t;
            eval_modulus(t, U, V);
            U.swap(V);
            V.swap(t);
            return;
         }
         unsigned ts = U.size() + 1;
         cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> t1(storage, ts), t2(storage, ts), TU(storage, ts);
         eval_multiply(t1, U, x[0]);
         eval_multiply(t2, V, y[0]);
         if ((i & 1u) == 0)
         {
            if (x[0] == 0)
               TU.swap(t2);
            else
            {
               BOOST_ASSERT(t2.compare(t1) >= 0);
               eval_subtract(TU, t2, t1);
               BOOST_ASSERT(TU.sign() == false);
            }
         }
         else
         {
            BOOST_ASSERT(t1.compare(t2) >= 0);
            eval_subtract(TU, t1, t2);
            BOOST_ASSERT(TU.sign() == false);
         }
         eval_multiply(t1, U, x[1]);
         eval_multiply(t2, V, y[1]);
         if (i & 1u)
         {
            if (x[1] == 0)
               V = t2;
            else
            {
               BOOST_ASSERT(t2.compare(t1) >= 0);
               eval_subtract(V, t2, t1);
               BOOST_ASSERT(V.sign() == false);
            }
         }
         else
         {
            BOOST_ASSERT(t1.compare(t2) >= 0);
            eval_subtract(V, t1, t2);
            BOOST_ASSERT(V.sign() == false);
         }
         eval_modulus(U, TU, V);
         U.swap(V);
         BOOST_ASSERT(lu > eval_msb(U));
         storage.deallocate(ts * 3);
         return;
      }
      x[0] = x[1];
      x[1] = x[2];
      y[0] = y[1];
      y[1] = y[2];
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_gcd(
   cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result,
   const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a,
   const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& b)
{
   using default_ops::eval_get_sign;
   using default_ops::eval_is_zero;
   using default_ops::eval_lsb;

   if (a.size() == 1)
   {
      eval_gcd(result, b, *a.limbs());
      return;
   }
   if (b.size() == 1)
   {
      eval_gcd(result, a, *b.limbs());
      return;
   }
   unsigned temp_size = std::max(a.size(), b.size()) + 1;
   typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::scoped_shared_storage storage(a, temp_size * 6);

   cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> U(storage, temp_size);
   cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> V(storage, temp_size);
   cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> t(storage, temp_size);
   U = a;
   V = b;

   int s = eval_get_sign(U);

   /* GCD(0,x) := x */
   if (s < 0)
   {
      U.negate();
   }
   else if (s == 0)
   {
      result = V;
      return;
   }
   s = eval_get_sign(V);
   if (s < 0)
   {
      V.negate();
   }
   else if (s == 0)
   {
      result = U;
      return;
   }
   //
   // Remove common factors of 2:
   //
   unsigned us = eval_lsb(U);
   unsigned vs = eval_lsb(V);
   int      shift = (std::min)(us, vs);
   if (us)
      eval_right_shift(U, us);
   if (vs)
      eval_right_shift(V, vs);

   if (U.compare(V) < 0)
      U.swap(V);

   while (!eval_is_zero(V))
   {
      if (U.size() <= 2)
      {
         //
         // Special case: if V has no more than 2 limbs
         // then we can reduce U and V to a pair of integers and perform
         // direct integer gcd:
         //
         if (U.size() == 1)
            U = eval_gcd(*V.limbs(), *U.limbs());
         else
         {
            double_limb_type i = U.limbs()[0] | (static_cast<double_limb_type>(U.limbs()[1]) << sizeof(limb_type) * CHAR_BIT);
            double_limb_type j = (V.size() == 1) ? *V.limbs() : V.limbs()[0] | (static_cast<double_limb_type>(V.limbs()[1]) << sizeof(limb_type) * CHAR_BIT);
            U = eval_gcd(i, j);
         }
         break;
      }
      unsigned lu = eval_msb(U) + 1;
      unsigned lv = eval_msb(V) + 1;
      if (lu - lv <= bits_per_limb / 2)
      {
         eval_gcd_lehmer(U, V, lu, storage);
      }
      else
      {
         eval_modulus(t, U, V);
         U.swap(V);
         V.swap(t);
      }
   }
   result = U;
   if (shift)
      eval_left_shift(result, shift);
}
#else
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_gcd(
    cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>&       result,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a,
    const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& b)
{
   using default_ops::eval_get_sign;
   using default_ops::eval_is_zero;
   using default_ops::eval_lsb;

   if (a.size() == 1)
   {
      eval_gcd(result, b, *a.limbs());
      return;
   }
   if (b.size() == 1)
   {
      eval_gcd(result, a, *b.limbs());
      return;
   }

   cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> u(a), v(b);

   int s = eval_get_sign(u);

   /* GCD(0,x) := x */
   if (s < 0)
   {
      u.negate();
   }
   else if (s == 0)
   {
      result = v;
      return;
   }
   s = eval_get_sign(v);
   if (s < 0)
   {
      v.negate();
   }
   else if (s == 0)
   {
      result = u;
      return;
   }

   /* Let shift := lg K, where K is the greatest power of 2
   dividing both u and v. */

   unsigned us    = eval_lsb(u);
   unsigned vs    = eval_lsb(v);
   int      shift = (std::min)(us, vs);
   eval_right_shift(u, us);
   eval_right_shift(v, vs);

   do
   {
      /* Now u and v are both odd, so diff(u, v) is even.
      Let u = min(u, v), v = diff(u, v)/2. */
      s = u.compare(v);
      if (s > 0)
         u.swap(v);
      if (s == 0)
         break;

      while(((u.size() + 2 < v.size()) && (v.size() * 100 / u.size() > 105)) || ((u.size() <= 2) && (v.size() > 4)))
      {
         //
         // Speical case: if u and v differ considerably in size, then a Euclid step
         // is more efficient as we reduce v by several limbs in one go.
         // Unfortunately it requires an expensive long division:
         //
         eval_modulus(v, v, u);
         u.swap(v);
      }
      if (v.size() <= 2)
      {
         //
         // Special case: if v has no more than 2 limbs
         // then we can reduce u and v to a pair of integers and perform
         // direct integer gcd:
         //
         if (v.size() == 1)
            u = eval_gcd(*v.limbs(), *u.limbs());
         else
         {
            double_limb_type i = v.limbs()[0] | (static_cast<double_limb_type>(v.limbs()[1]) << sizeof(limb_type) * CHAR_BIT);
            double_limb_type j = (u.size() == 1) ? *u.limbs() : u.limbs()[0] | (static_cast<double_limb_type>(u.limbs()[1]) << sizeof(limb_type) * CHAR_BIT);
            u                  = eval_gcd(i, j);
         }
         break;
      }
      //
      // Regular binary gcd case:
      //
      eval_subtract(v, u);
      vs = eval_lsb(v);
      eval_right_shift(v, vs);
   } while (true);

   result = u;
   eval_left_shift(result, shift);
}
#endif
//
// Now again for trivial backends:
//
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
eval_gcd(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& b) BOOST_NOEXCEPT
{
   *result.limbs() = boost::integer::gcd(*a.limbs(), *b.limbs());
}
// This one is only enabled for unchecked cpp_int's, for checked int's we need the checking in the default version:
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && (Checked1 == unchecked)>::type
eval_lcm(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& b) BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   *result.limbs() = boost::integer::lcm(*a.limbs(), *b.limbs());
   result.normalize(); // result may overflow the specified number of bits
}

inline void conversion_overflow(const mpl::int_<checked>&)
{
   BOOST_THROW_EXCEPTION(std::overflow_error("Overflow in conversion to narrower type"));
}
inline BOOST_MP_CXX14_CONSTEXPR void conversion_overflow(const mpl::int_<unchecked>&) {}

template <class R, unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<
    is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && is_signed_number<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && boost::is_convertible<typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::local_limb_type, R>::value>::type
eval_convert_to(R* result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val)
{
   typedef typename common_type<R, typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::local_limb_type>::type common_type;
   if (std::numeric_limits<R>::is_specialized && (static_cast<common_type>(*val.limbs()) > static_cast<common_type>((std::numeric_limits<R>::max)())))
   {
      if (val.isneg())
      {
         check_is_negative(mpl::bool_ < boost::is_signed<R>::value || (number_category<R>::value == number_kind_floating_point) > ());
         if (static_cast<common_type>(*val.limbs()) > -static_cast<common_type>((std::numeric_limits<R>::min)()))
            conversion_overflow(typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::checked_type());
         *result = (std::numeric_limits<R>::min)();
      }
      else
      {
         conversion_overflow(typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::checked_type());
         *result = boost::is_signed<R>::value ? (std::numeric_limits<R>::max)() : static_cast<R>(*val.limbs());
      }
   }
   else
   {
      *result = static_cast<R>(*val.limbs());
      if (val.isneg())
      {
         check_is_negative(mpl::bool_ < boost::is_signed<R>::value || (number_category<R>::value == number_kind_floating_point) > ());
         *result = negate_integer(*result, mpl::bool_ < is_signed_number<R>::value || (number_category<R>::value == number_kind_floating_point) > ());
      }
   }
}

template <class R, unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<
    is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && is_unsigned_number<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && boost::is_convertible<typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::local_limb_type, R>::value>::type
eval_convert_to(R* result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val)
{
   typedef typename common_type<R, typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::local_limb_type>::type common_type;
   if (std::numeric_limits<R>::is_specialized && (static_cast<common_type>(*val.limbs()) > static_cast<common_type>((std::numeric_limits<R>::max)())))
   {
      conversion_overflow(typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::checked_type());
      *result = boost::is_signed<R>::value ? (std::numeric_limits<R>::max)() : static_cast<R>(*val.limbs());
   }
   else
      *result = static_cast<R>(*val.limbs());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, unsigned>::type
eval_lsb(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a)
{
   using default_ops::eval_get_sign;
   if (eval_get_sign(a) == 0)
   {
      BOOST_THROW_EXCEPTION(std::range_error("No bits were set in the operand."));
   }
   if (a.sign())
   {
      BOOST_THROW_EXCEPTION(std::range_error("Testing individual bits in negative values is not supported - results are undefined."));
   }
   //
   // Find the index of the least significant bit within that limb:
   //
   return boost::multiprecision::detail::find_lsb(*a.limbs());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, unsigned>::type
eval_msb_imp(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a)
{
   //
   // Find the index of the least significant bit within that limb:
   //
   return boost::multiprecision::detail::find_msb(*a.limbs());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR typename enable_if_c<is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, unsigned>::type
eval_msb(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a)
{
   using default_ops::eval_get_sign;
   if (eval_get_sign(a) == 0)
   {
      BOOST_THROW_EXCEPTION(std::range_error("No bits were set in the operand."));
   }
   if (a.sign())
   {
      BOOST_THROW_EXCEPTION(std::range_error("Testing individual bits in negative values is not supported - results are undefined."));
   }
   return eval_msb_imp(a);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline BOOST_MP_CXX14_CONSTEXPR std::size_t hash_value(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val) BOOST_NOEXCEPT
{
   std::size_t result = 0;
   for (unsigned i = 0; i < val.size(); ++i)
   {
      boost::hash_combine(result, val.limbs()[i]);
   }
   boost::hash_combine(result, val.sign());
   return result;
}

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

}}} // namespace boost::multiprecision::backends

#endif
