///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_
//
// Comparison operators for cpp_int_backend:
//
#ifndef BOOST_MP_CPP_INT_MISC_HPP
#define BOOST_MP_CPP_INT_MISC_HPP

#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4702)
#endif

namespace boost{ namespace multiprecision{ namespace backends{

template <class R, class CppInt>
void check_in_range(const CppInt& val, const mpl::int_<checked>&)
{
   typedef typename boost::multiprecision::detail::canonical<R, CppInt>::type cast_type;
   if(val.sign())
   {
      if(val.compare(static_cast<cast_type>((std::numeric_limits<R>::min)())) < 0)
         BOOST_THROW_EXCEPTION(std::overflow_error("Could not convert to the target type - -value is out of range."));
   }
   else
   {
      if(val.compare(static_cast<cast_type>((std::numeric_limits<R>::max)())) > 0)
         BOOST_THROW_EXCEPTION(std::overflow_error("Could not convert to the target type - -value is out of range."));
   }
}
template <class R, class CppInt>
inline void check_in_range(const CppInt& /*val*/, const mpl::int_<unchecked>&) BOOST_NOEXCEPT {}

inline void check_is_negative(const mpl::true_&) BOOST_NOEXCEPT {}
inline void check_is_negative(const mpl::false_&)
{
   BOOST_THROW_EXCEPTION(std::range_error("Attempt to assign a negative value to an unsigned type."));
}

template <class Integer>
inline Integer negate_integer(Integer i, const mpl::true_&) BOOST_NOEXCEPT
{
   return -i;
}
template <class Integer>
inline Integer negate_integer(Integer i, const mpl::false_&) BOOST_NOEXCEPT
{
   return ~(i-1);
}

template <class R, unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline typename enable_if_c<is_integral<R>::value && !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, void>::type
   eval_convert_to(R* result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& backend) BOOST_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   typedef mpl::int_<Checked1> checked_type;
   check_in_range<R>(backend, checked_type());

   *result = static_cast<R>(backend.limbs()[0]);
   unsigned shift = cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   for(unsigned i = 1; (i < backend.size()) && (shift < static_cast<unsigned>(std::numeric_limits<R>::digits)); ++i)
   {
      *result += static_cast<R>(backend.limbs()[i]) << shift;
      shift += cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   }
   if(backend.sign())
   {
      check_is_negative(boost::is_signed<R>());
      *result = negate_integer(*result, boost::is_signed<R>());
   }
}

template <class R, unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline typename enable_if_c<is_floating_point<R>::value && !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, void>::type
   eval_convert_to(R* result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& backend) BOOST_NOEXCEPT
{
   typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::const_limb_pointer p = backend.limbs();
   unsigned shift = cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   *result = static_cast<R>(*p);
   for(unsigned i = 1; i < backend.size(); ++i)
   {
      *result += static_cast<R>(std::ldexp(static_cast<long double>(p[i]), shift));
      shift += cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   }
   if(backend.sign())
      *result = -*result;
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, bool>::type
   eval_is_zero(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val) BOOST_NOEXCEPT
{
   return (val.size() == 1) && (val.limbs()[0] == 0);
}
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, int>::type
   eval_get_sign(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val) BOOST_NOEXCEPT
{
   return eval_is_zero(val) ? 0 : val.sign() ? -1 : 1;
}
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
   eval_abs(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val) BOOST_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   result = val;
   result.sign(false);
}

//
// Get the location of the least-significant-bit:
//
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, unsigned>::type
   eval_lsb(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a) BOOST_NOEXCEPT
{
   using default_ops::eval_get_sign;
   if(eval_get_sign(a) == 0)
   {
      BOOST_THROW_EXCEPTION(std::range_error("No bits were set in the operand."));
   }
   if(a.sign())
   {
      BOOST_THROW_EXCEPTION(std::range_error("Testing individual bits in negative values is not supported - results are undefined."));
   }

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

   return result + index * cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, bool>::type
   eval_bit_test(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val, unsigned index) BOOST_NOEXCEPT
{
   unsigned offset = index / cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   unsigned shift = index % cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   limb_type mask = shift ? limb_type(1u) << shift : limb_type(1u);
   if(offset >= val.size())
      return false;
   return val.limbs()[offset] & mask ? true : false;
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
   eval_bit_set(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val, unsigned index)
{
   unsigned offset = index / cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   unsigned shift = index % cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   limb_type mask = shift ? limb_type(1u) << shift : limb_type(1u);
   if(offset >= val.size())
   {
      unsigned os = val.size();
      val.resize(offset + 1, offset + 1);
      if(offset >= val.size())
         return;  // fixed precision overflow
      for(unsigned i = os; i <= offset; ++i)
         val.limbs()[i] = 0;
   }
   val.limbs()[offset] |= mask;
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
   eval_bit_unset(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val, unsigned index) BOOST_NOEXCEPT
{
   unsigned offset = index / cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   unsigned shift = index % cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   limb_type mask = shift ? limb_type(1u) << shift : limb_type(1u);
   if(offset >= val.size())
      return;
   val.limbs()[offset] &= ~mask;
   val.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
   eval_bit_flip(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val, unsigned index)
{
   unsigned offset = index / cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   unsigned shift = index % cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::limb_bits;
   limb_type mask = shift ? limb_type(1u) << shift : limb_type(1u);
   if(offset >= val.size())
   {
      unsigned os = val.size();
      val.resize(offset + 1, offset + 1);
      if(offset >= val.size())
         return;  // fixed precision overflow
      for(unsigned i = os; i <= offset; ++i)
         val.limbs()[i] = 0;
   }
   val.limbs()[offset] ^= mask;
   val.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline typename enable_if_c<!is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
   eval_qr(
      const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& x,
      const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& y,
      cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& q,
      cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& r) BOOST_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   divide_unsigned_helper(&q, x, y, r);
   q.sign(x.sign() != y.sign());
   r.sign(x.sign());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, class Integer>
inline typename enable_if_c<is_unsigned<Integer>::value && !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, Integer>::type
   eval_integer_modulus(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& x, Integer val)
{
   if((sizeof(Integer) <= sizeof(limb_type)) || (val <= (std::numeric_limits<limb_type>::max)()))
   {
      cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> d;
      divide_unsigned_helper(static_cast<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>*>(0), x, static_cast<limb_type>(val), d);
      return d.limbs()[0];
   }
   else
   {
      return default_ops::eval_integer_modulus(x, val);
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1, class Integer>
BOOST_MP_FORCEINLINE typename enable_if_c<is_signed<Integer>::value && !is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value, Integer>::type
   eval_integer_modulus(const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& x, Integer val)
{
   BOOST_MP_USING_ABS
   typedef typename make_unsigned<Integer>::type unsigned_type;
   return eval_integer_modulus(x, static_cast<unsigned_type>(abs(val)));
}

//
// Now again for trivial backends:
//
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE typename enable_if_c<is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value>::type
   eval_gcd(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& b) BOOST_NOEXCEPT
{
   *result.limbs() = boost::math::gcd(*a.limbs(), *b.limbs());
}
// This one is only enabled for unchecked cpp_int's, for checked int's we need the checking in the default version:
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
BOOST_MP_FORCEINLINE typename enable_if_c<is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value && (Checked1 == unchecked)>::type
   eval_lcm(cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& a, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& b) BOOST_NOEXCEPT_IF((is_non_throwing_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value))
{
   *result.limbs() = boost::math::lcm(*a.limbs(), *b.limbs());
   result.normalize(); // result may overflow the specified number of bits
}

inline void conversion_overflow(const mpl::int_<checked>&)
{
   BOOST_THROW_EXCEPTION(std::overflow_error("Overflow in conversion to narrower type"));
}
inline void conversion_overflow(const mpl::int_<unchecked>&){}

template <class R, unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline typename enable_if_c<
            is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value
            && is_signed_number<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value
         >::type
   eval_convert_to(R* result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val)
{
   typedef typename common_type<R, typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::local_limb_type>::type common_type;
   if(std::numeric_limits<R>::is_specialized && (static_cast<common_type>(*val.limbs()) > static_cast<common_type>((std::numeric_limits<R>::max)())))
   {
      conversion_overflow(typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::checked_type());
      *result = (std::numeric_limits<R>::max)();
   }
   else
      *result = static_cast<R>(*val.limbs());
   if(val.isneg())
   {
      check_is_negative(mpl::bool_<boost::is_signed<R>::value || boost::is_floating_point<R>::value>());
      *result = negate_integer(*result, mpl::bool_<boost::is_signed<R>::value || boost::is_floating_point<R>::value>());
   }
}

template <class R, unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1>
inline typename enable_if_c<
            is_trivial_cpp_int<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value
            && is_unsigned_number<cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value
         >::type
   eval_convert_to(R* result, const cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>& val)
{
   typedef typename common_type<R, typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::local_limb_type>::type common_type;
   if(std::numeric_limits<R>::is_specialized && (static_cast<common_type>(*val.limbs()) > static_cast<common_type>((std::numeric_limits<R>::max)())))
   {
      conversion_overflow(typename cpp_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1>::checked_type());
      *result = (std::numeric_limits<R>::max)();
   }
   else
      *result = static_cast<R>(*val.limbs());
}

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

}}} // namespaces

#endif
