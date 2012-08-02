///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MP_CPP_INT_TRIVIAL_OPS_HPP
#define BOOST_MP_CPP_INT_TRIVIAL_OPS_HPP

#include <boost/math/common_factor_rt.hpp>

namespace boost{ namespace multiprecision{ namespace backends{

template <unsigned MinBits>
inline bool eval_eq(const cpp_int_backend<MinBits, true, void, true>& a, const cpp_int_backend<MinBits, true, void, true>& b) BOOST_NOEXCEPT
{
   return (a.sign() == b.sign()) && (*a.limbs() == *b.limbs());
}
template <unsigned MinBits>
inline bool eval_eq(const cpp_int_backend<MinBits, false, void, true>& a, const cpp_int_backend<MinBits, false, void, true>& b) BOOST_NOEXCEPT
{
   return *a.limbs() == *b.limbs();
}
template <unsigned MinBits, class U>
inline typename enable_if<is_unsigned<U>, bool>::type eval_eq(const cpp_int_backend<MinBits, true, void, true>& a, U b) BOOST_NOEXCEPT
{
   return !a.sign() && (*a.limbs() == b);
}
template <unsigned MinBits, class S>
inline typename enable_if<is_signed<S>, bool>::type eval_eq(const cpp_int_backend<MinBits, true, void, true>& a, S b) BOOST_NOEXCEPT
{
   return (a.sign() == (b < 0)) && (*a.limbs() == std::abs(b));
}
template <unsigned MinBits, class U>
inline typename enable_if<is_unsigned<U>, bool>::type eval_eq(const cpp_int_backend<MinBits, false, void, true>& a, U b) BOOST_NOEXCEPT
{
   return *a.limbs() == b;
}
template <unsigned MinBits, class S>
inline typename enable_if<is_signed<S>, bool>::type eval_eq(const cpp_int_backend<MinBits, false, void, true>& a, S b) BOOST_NOEXCEPT
{
   return *a.limbs() == b;
}

template <unsigned MinBits>
inline bool eval_lt(const cpp_int_backend<MinBits, true, void, true>& a, const cpp_int_backend<MinBits, true, void, true>& b) BOOST_NOEXCEPT
{
   if(a.sign() != b.sign())
      return a.sign();
   return a.sign() ? *a.limbs() > *b.limbs() : *a.limbs() < *b.limbs();
}
template <unsigned MinBits>
inline bool eval_lt(const cpp_int_backend<MinBits, false, void, true>& a, const cpp_int_backend<MinBits, false, void, true>& b) BOOST_NOEXCEPT
{
   return *a.limbs() < *b.limbs();
}
template <unsigned MinBits, class U>
inline typename enable_if<is_unsigned<U>, bool>::type eval_lt(const cpp_int_backend<MinBits, true, void, true>& a, U b) BOOST_NOEXCEPT
{
   if(a.sign())
      return true;
   return *a.limbs() < b;
}
template <unsigned MinBits, class S>
inline typename enable_if<is_signed<S>, bool>::type eval_lt(const cpp_int_backend<MinBits, true, void, true>& a, S b) BOOST_NOEXCEPT
{
   if(a.sign() != (b < 0))
      return a.sign();
   return a.sign() ? (*a.limbs() > std::abs(b)) : (*a.limbs() < std::abs(b));
}
template <unsigned MinBits, class U>
inline typename enable_if<is_unsigned<U>, bool>::type eval_lt(const cpp_int_backend<MinBits, false, void, true>& a, U b) BOOST_NOEXCEPT
{
   return *a.limbs() < b;
}
template <unsigned MinBits, class S>
inline typename enable_if<is_signed<S>, bool>::type eval_lt(const cpp_int_backend<MinBits, false, void, true>& a, S b) BOOST_NOEXCEPT
{
   return *a.limbs() < b;
}

template <unsigned MinBits>
inline bool eval_gt(const cpp_int_backend<MinBits, true, void, true>& a, const cpp_int_backend<MinBits, true, void, true>& b) BOOST_NOEXCEPT
{
   if(a.sign() != b.sign())
      return !a.sign();
   return a.sign() ? *a.limbs() < *b.limbs() : *a.limbs() > *b.limbs();
}
template <unsigned MinBits>
inline bool eval_gt(const cpp_int_backend<MinBits, false, void, true>& a, const cpp_int_backend<MinBits, false, void, true>& b) BOOST_NOEXCEPT
{
   return *a.limbs() > *b.limbs();
}
template <unsigned MinBits, class U>
inline typename enable_if<is_unsigned<U>, bool>::type eval_gt(const cpp_int_backend<MinBits, true, void, true>& a, U b) BOOST_NOEXCEPT
{
   if(a.sign())
      return false;
   return *a.limbs() > b;
}
template <unsigned MinBits, class S>
inline typename enable_if<is_signed<S>, bool>::type eval_gt(const cpp_int_backend<MinBits, true, void, true>& a, S b) BOOST_NOEXCEPT
{
   if(a.sign() != (b < 0))
      return !a.sign();
   return a.sign() ? (*a.limbs() < std::abs(b)) : (*a.limbs() > std::abs(b));
}
template <unsigned MinBits, class U>
inline typename enable_if<is_unsigned<U>, bool>::type eval_gt(const cpp_int_backend<MinBits, false, void, true>& a, U b) BOOST_NOEXCEPT
{
   return *a.limbs() > b;
}
template <unsigned MinBits, class S>
inline typename enable_if<is_signed<S>, bool>::type eval_gt(const cpp_int_backend<MinBits, false, void, true>& a, S b) BOOST_NOEXCEPT
{
   return *a.limbs() > b;
}

template <unsigned MinBits>
inline void eval_add(cpp_int_backend<MinBits, true, void, true>& result, const cpp_int_backend<MinBits, true, void, true>& o) BOOST_NOEXCEPT
{
   if(result.sign() != o.sign())
   {
      if(*o.limbs() > *result.limbs())
      {
         *result.limbs() = *o.limbs() - *result.limbs();
         result.negate();
      }
      else
         *result.limbs() -= *o.limbs();
   }
   else
      *result.limbs() += *o.limbs();
}

template <unsigned MinBits>
inline void eval_add(cpp_int_backend<MinBits, false, void, true>& result, const cpp_int_backend<MinBits, false, void, true>& o) BOOST_NOEXCEPT
{
   *result.limbs() += *o.limbs();
}

template <unsigned MinBits>
inline void eval_subtract(cpp_int_backend<MinBits, true, void, true>& result, const cpp_int_backend<MinBits, true, void, true>& o) BOOST_NOEXCEPT
{
   if(result.sign() != o.sign())
   {
      *result.limbs() += *o.limbs();
   }
   else if(*result.limbs() < *o.limbs())
   {
      *result.limbs() = *o.limbs() - *result.limbs();
      result.negate();
   }
   else
      *result.limbs() -= *o.limbs();
}

template <unsigned MinBits>
inline void eval_subtract(cpp_int_backend<MinBits, false, void, true>& result, const cpp_int_backend<MinBits, false, void, true>& o) BOOST_NOEXCEPT
{
   *result.limbs() -= *o.limbs();
}

template <unsigned MinBits>
inline void eval_multiply(cpp_int_backend<MinBits, true, void, true>& result, const cpp_int_backend<MinBits, true, void, true>& o) BOOST_NOEXCEPT
{
   *result.limbs() *= *o.limbs();
   result.sign(result.sign() != o.sign());
}

template <unsigned MinBits>
inline void eval_multiply(cpp_int_backend<MinBits, false, void, true>& result, const cpp_int_backend<MinBits, false, void, true>& o) BOOST_NOEXCEPT
{
   *result.limbs() *= *o.limbs();
}

template <unsigned MinBits>
inline void eval_divide(cpp_int_backend<MinBits, true, void, true>& result, const cpp_int_backend<MinBits, true, void, true>& o)
{
   if(!*o.limbs())
      BOOST_THROW_EXCEPTION(std::runtime_error("Division by zero."));
   *result.limbs() /= *o.limbs();
   result.sign(result.sign() != o.sign());
}

template <unsigned MinBits>
inline void eval_divide(cpp_int_backend<MinBits, false, void, true>& result, const cpp_int_backend<MinBits, false, void, true>& o)
{
   if(!*o.limbs())
      BOOST_THROW_EXCEPTION(std::runtime_error("Division by zero."));
   *result.limbs() /= *o.limbs();
}

template <unsigned MinBits, bool Signed>
inline void eval_modulus(cpp_int_backend<MinBits, Signed, void, true>& result, const cpp_int_backend<MinBits, Signed, void, true>& o)
{
   if(!*o.limbs())
      BOOST_THROW_EXCEPTION(std::runtime_error("Division by zero."));
   *result.limbs() %= *o.limbs();
   result.sign(result.sign());
}

template <unsigned MinBits, bool Signed, class T>
inline void eval_left_shift(cpp_int_backend<MinBits, Signed, void, true>& result, T s) BOOST_NOEXCEPT
{
   *result.limbs() <<= s;
}

template <unsigned MinBits, bool Signed, class T>
inline void eval_right_shift(cpp_int_backend<MinBits, Signed, void, true>& result, T s) BOOST_NOEXCEPT
{
   *result.limbs() >>= s;
}

template <unsigned MinBits>
inline void eval_bitwise_and(cpp_int_backend<MinBits, true, void, true>& result, const cpp_int_backend<MinBits, true, void, true>& o) BOOST_NOEXCEPT
{
   typename cpp_int_backend<MinBits, true, void, true>::local_limb_type a, b;
   a = *result.limbs();
   if(result.isneg())
   {
      a = ~a;
      ++a;
   }
   b = *o.limbs();
   if(o.isneg())
   {
      b = ~b;
      ++b;
   }
   a &= b;
   bool isneg = false;
   static const typename cpp_int_backend<MinBits, true, void, true>::local_limb_type mask = static_cast<typename cpp_int_backend<MinBits, true, void, true>::local_limb_type>(1) << (std::numeric_limits<typename cpp_int_backend<MinBits, true, void, true>::local_limb_type>::digits - 1);
   if(a & mask)
   {
      a = ~a;
      ++a;
      isneg = true;
   }
   *result.limbs() = a;
   result.sign(isneg);
}

template <unsigned MinBits>
inline void eval_bitwise_and(cpp_int_backend<MinBits, false, void, true>& result, const cpp_int_backend<MinBits, false, void, true>& o) BOOST_NOEXCEPT
{
   *result.limbs() &= *o.limbs();
}

template <unsigned MinBits>
inline void eval_bitwise_or(cpp_int_backend<MinBits, true, void, true>& result, const cpp_int_backend<MinBits, true, void, true>& o) BOOST_NOEXCEPT
{
   typename cpp_int_backend<MinBits, true, void, true>::local_limb_type a, b;
   a = *result.limbs();
   if(result.isneg())
   {
      a = ~a;
      ++a;
   }
   b = *o.limbs();
   if(o.isneg())
   {
      b = ~b;
      ++b;
   }
   a |= b;
   bool isneg = false;
   static const typename cpp_int_backend<MinBits, true, void, true>::local_limb_type mask = static_cast<typename cpp_int_backend<MinBits, true, void, true>::local_limb_type>(1) << (sizeof(typename cpp_int_backend<MinBits, true, void, true>::local_limb_type) * CHAR_BIT - 1);
   if(a & mask)
   {
      a = ~a;
      ++a;
      isneg = true;
   }
   *result.limbs() = a;
   result.sign(isneg);
}

template <unsigned MinBits>
inline void eval_bitwise_or(cpp_int_backend<MinBits, false, void, true>& result, const cpp_int_backend<MinBits, false, void, true>& o) BOOST_NOEXCEPT
{
   *result.limbs() |= *o.limbs();
}

template <unsigned MinBits>
inline void eval_bitwise_xor(cpp_int_backend<MinBits, true, void, true>& result, const cpp_int_backend<MinBits, true, void, true>& o) BOOST_NOEXCEPT
{
   typename cpp_int_backend<MinBits, true, void, true>::local_limb_type a, b;
   a = *result.limbs();
   if(result.isneg())
   {
      a = ~a;
      ++a;
   }
   b = *o.limbs();
   if(o.isneg())
   {
      b = ~b;
      ++b;
   }
   a ^= b;
   bool isneg = false;
   static const typename cpp_int_backend<MinBits, true, void, true>::local_limb_type mask = static_cast<typename cpp_int_backend<MinBits, true, void, true>::local_limb_type>(1) << (std::numeric_limits<typename cpp_int_backend<MinBits, true, void, true>::local_limb_type>::digits - 1);
   if(a & mask)
   {
      a = ~a;
      ++a;
      isneg = true;
   }
   *result.limbs() = a;
   result.sign(isneg);
}

template <unsigned MinBits>
inline void eval_bitwise_xor(cpp_int_backend<MinBits, false, void, true>& result, const cpp_int_backend<MinBits, false, void, true>& o) BOOST_NOEXCEPT
{
   *result.limbs() ^= *o.limbs();
}

template <unsigned MinBits>
inline void eval_complement(cpp_int_backend<MinBits, true, void, true>& result, const cpp_int_backend<MinBits, true, void, true>& o) BOOST_NOEXCEPT
{
   typename cpp_int_backend<MinBits, true, void, true>::local_limb_type a;
   a = *o.limbs();
   if(o.isneg())
   {
      a = ~a;
      ++a;
   }
   a = ~a;
   bool isneg = false;
   static const typename cpp_int_backend<MinBits, true, void, true>::local_limb_type mask = static_cast<typename cpp_int_backend<MinBits, true, void, true>::local_limb_type>(1) << (std::numeric_limits<typename cpp_int_backend<MinBits, true, void, true>::local_limb_type>::digits - 1);
   if(a & mask)
   {
      a = ~a;
      ++a;
      isneg = true;
   }
   *result.limbs() = a;
   result.sign(isneg);
}

template <unsigned MinBits>
inline void eval_complement(cpp_int_backend<MinBits, false, void, true>& result, const cpp_int_backend<MinBits, false, void, true>& o) BOOST_NOEXCEPT
{
   *result.limbs() = ~*o.limbs();
}

template <unsigned MinBits, bool Signed>
inline void eval_gcd(cpp_int_backend<MinBits, Signed, void, true>& result, const cpp_int_backend<MinBits, Signed, void, true>& a, const cpp_int_backend<MinBits, Signed, void, true>& b) BOOST_NOEXCEPT
{
   *result.limbs() = boost::math::gcd(*a.limbs(), *b.limbs());
}

template <unsigned MinBits, bool Signed>
inline void eval_lcm(cpp_int_backend<MinBits, Signed, void, true>& result, const cpp_int_backend<MinBits, Signed, void, true>& a, const cpp_int_backend<MinBits, Signed, void, true>& b) BOOST_NOEXCEPT
{
   *result.limbs() = boost::math::lcm(*a.limbs(), *b.limbs());
}

#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4127 4018)
#endif
template <class R, unsigned MinBits>
inline void eval_convert_to(R* result, const cpp_int_backend<MinBits, true, void, true>& val)
{
   if(std::numeric_limits<R>::is_specialized && (*val.limbs() > (std::numeric_limits<R>::max)()))
      *result = (std::numeric_limits<R>::max)();
   else
      *result = static_cast<R>(*val.limbs());
   if(val.isneg())
      *result = negate_integer(*result, mpl::bool_<boost::is_signed<R>::value || boost::is_floating_point<R>::value>());
}

template <class R, unsigned MinBits>
inline void eval_convert_to(R* result, const cpp_int_backend<MinBits, false, void, true>& val)
{
   if(std::numeric_limits<R>::is_specialized && (*val.limbs() > (std::numeric_limits<R>::max)()))
      *result = (std::numeric_limits<R>::max)();
   else
      *result = static_cast<R>(*val.limbs());
}
#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

}}} // namespaces

#endif
