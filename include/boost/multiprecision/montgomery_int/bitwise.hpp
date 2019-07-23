//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_MP_MONTGOMERY_INT_BIT_HPP
#define CRYPTO3_MP_MONTGOMERY_INT_BIT_HPP

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4319)
#endif

namespace boost {
namespace multiprecision {
namespace backends {

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
void is_valid_bitwise_op(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &                  result,
    const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                 Allocator2, ParamsBackend2> &o,
    const mpl::int_<checked> &)
{
   if (result.sign() || o.sign())
   {
      BOOST_THROW_EXCEPTION(
          std::range_error("Bitwise operations on negative values results in "
                           "undefined behavior."));
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
void is_valid_bitwise_op(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &,
    const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                 Allocator2, ParamsBackend2> &,
    const mpl::int_<unchecked> &) {}

template <unsigned MinBits1, unsigned MaxBits1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
void is_valid_bitwise_op(
    const montgomery_int_backend<MinBits1, MaxBits1, signed_magnitude, Checked1,
                                 Allocator1, ParamsBackend1> &result,
    const mpl::int_<checked> &)
{
   if (result.sign())
   {
      BOOST_THROW_EXCEPTION(
          std::range_error("Bitwise operations on negative values results in "
                           "undefined behavior."));
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
void is_valid_bitwise_op(
    const montgomery_int_backend<MinBits1, MaxBits1, unsigned_magnitude,
                                 Checked1, Allocator1, ParamsBackend1> &,
    const mpl::int_<checked> &) {}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
void is_valid_bitwise_op(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &,
    const mpl::int_<unchecked> &) {}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value>

    ::type eval_bitwise_and(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                     Allocator2, ParamsBackend2> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   bitwise_op(
       result, o, bit_and(),
       mpl::bool_ < std::numeric_limits<number<montgomery_int_backend<
                        MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                        ParamsBackend1> > >::is_signed ||
           std::numeric_limits<
               number<montgomery_int_backend<MinBits2, MaxBits2, SignType2,
                                             Checked2, Allocator2> > >::is_signed >
               ());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value>

    ::type eval_bitwise_or(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                     Allocator2, ParamsBackend2> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   bitwise_op(result, o, bit_or(),
              mpl::bool_ < std::numeric_limits<number<montgomery_int_backend<
                               MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> > >::is_signed ||
                  std::numeric_limits<number<montgomery_int_backend<
                          MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                          ParamsBackend2> > >::is_signed > ());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value>

    ::type eval_bitwise_xor(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                     Allocator2, ParamsBackend2> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   bitwise_op(
       result, o, bit_xor(),
       mpl::bool_ < std::numeric_limits<number<montgomery_int_backend<
                        MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                        ParamsBackend1> > >::is_signed ||
           std::numeric_limits<
               number<montgomery_int_backend<MinBits2, MaxBits2, SignType2,
                                             Checked2, Allocator2> > >::is_signed >
               ());
}

//
// Again for operands which are single limbs:
//
template <unsigned MinBits1, unsigned MaxBits1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, unsigned_magnitude, Checked1,
                           Allocator1, ParamsBackend1> >::value>::type
eval_bitwise_and(
    montgomery_int_backend<MinBits1, MaxBits1, unsigned_magnitude, Checked1,
                           Allocator1, ParamsBackend1> &result,
    limb_type                                           l) BOOST_NOEXCEPT
{
   result.limbs()[0] &= l;
   result.resize(1, 1);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, unsigned_magnitude, Checked1,
                           Allocator1, ParamsBackend1> >::value>

    ::type eval_bitwise_or(
        montgomery_int_backend<MinBits1, MaxBits1, unsigned_magnitude, Checked1,
                               Allocator1, ParamsBackend1> &result,
        limb_type                                           l)

        BOOST_NOEXCEPT
{
   result.limbs()[0] |= l;
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, unsigned_magnitude, Checked1,
                           Allocator1, ParamsBackend1> >::value>

    ::type eval_bitwise_xor(
        montgomery_int_backend<MinBits1, MaxBits1, unsigned_magnitude, Checked1,
                               Allocator1, ParamsBackend1> &result,
        limb_type                                           l)

        BOOST_NOEXCEPT
{
   result.limbs()[0] ^= l;
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<
    is_signed_number<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value>

    ::type eval_complement(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                     Allocator2, ParamsBackend2> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   BOOST_STATIC_ASSERT_MSG(((Checked1 != checked) || (Checked2 != checked)),
                           "Attempt to take the complement of a signed type "
                           "results in undefined behavior.");
   // Increment and negate:
   result = o;
   eval_increment(result);
   result.negate();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<
    is_unsigned_number<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value>

    ::type eval_complement(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   unsigned os = o.size();
   result.resize(UINT_MAX, os);
   for (unsigned i = 0; i < os; ++i)
   {
      result.limbs()[i] = ~o.limbs()[i];
   }

   for (unsigned i = os; i < result.size(); ++i)
   {
      result.limbs()[i] = ~static_cast<limb_type>(0);
   }

   result.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
inline typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> >::value>

    ::type eval_left_shift(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        double_limb_type                                    s)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   is_valid_bitwise_op(
       result,

       typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1,
                                       ParamsBackend1>::checked_type()

   );
   if (!s)
   {
      return;
   }

#if defined(BOOST_LITTLE_ENDIAN) && defined(BOOST_MP_USE_LIMB_SHIFT)
   static const limb_type limb_shift_mask =
       montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                              Allocator1, ParamsBackend1>::limb_bits -
       1;
   static const limb_type byte_shift_mask = CHAR_BIT - 1;
   if ((s & limb_shift_mask) == 0)
   {
      left_shift_limb(result, s);
   }
   else if ((s & byte_shift_mask) == 0)
   {
      left_shift_byte(result, s);
   }
#elif defined(BOOST_LITTLE_ENDIAN)
   static const limb_type byte_shift_mask = CHAR_BIT - 1;
   if ((s & byte_shift_mask) == 0)
   {
      left_shift_byte(result, s);
   }
#else
   static const limb_type limb_shift_mask =
       montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                              Allocator1, ParamsBackend1>::limb_bits -
       1;
   if ((s & limb_shift_mask) == 0)
   {
      left_shift_limb(result, s);
   }
#endif
   else
   {
      left_shift_generic(result, s);
   }
   //
   // We may have shifted off the end and have leading zeros:
   //
   result.normalize();
}
template <unsigned MinBits1, unsigned MaxBits1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
inline typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, unsigned_magnitude, Checked1,
                           Allocator1, ParamsBackend1> >::value>

    ::type eval_right_shift(
        montgomery_int_backend<MinBits1, MaxBits1, unsigned_magnitude, Checked1,
                               Allocator1, ParamsBackend1> &result,
        double_limb_type                                    s)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<montgomery_int_backend<
                 MinBits1, MaxBits1, unsigned_magnitude, Checked1, Allocator1,
                 ParamsBackend1> >::value))
{
   is_valid_bitwise_op(
       result,

       typename montgomery_int_backend<MinBits1, MaxBits1, unsigned_magnitude,
                                       Checked1, Allocator1>::checked_type()

   );
   if (!s)
   {
      return;
   }

#if defined(BOOST_LITTLE_ENDIAN) && defined(BOOST_MP_USE_LIMB_SHIFT)
   static const limb_type limb_shift_mask =
       montgomery_int_backend<MinBits1, MaxBits1, signed_magnitude, Checked1,
                              Allocator1, ParamsBackend1>::limb_bits -
       1;
   static const limb_type byte_shift_mask = CHAR_BIT - 1;
   if ((s & limb_shift_mask) == 0)
      right_shift_limb(result, s);
   else if ((s & byte_shift_mask) == 0)
      right_shift_byte(result, s);
#elif defined(BOOST_LITTLE_ENDIAN)
   static const limb_type byte_shift_mask = CHAR_BIT - 1;
   if ((s & byte_shift_mask) == 0)
   {
      right_shift_byte(result, s);
   }
#else
   static const limb_type limb_shift_mask =
       montgomery_int_backend<MinBits1, MaxBits1, signed_magnitude, Checked1,
                              Allocator1, ParamsBackend1>::limb_bits -
       1;
   if ((s & limb_shift_mask) == 0)
   {
      right_shift_limb(result, s);
   }
#endif
   else
   {
      right_shift_generic(result, s);
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
inline typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, signed_magnitude, Checked1,
                           Allocator1, ParamsBackend1> >::value>

    ::type eval_right_shift(
        montgomery_int_backend<MinBits1, MaxBits1, signed_magnitude, Checked1,
                               Allocator1, ParamsBackend1> &result,
        double_limb_type                                    s)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<montgomery_int_backend<
                 MinBits1, MaxBits1, signed_magnitude, Checked1, Allocator1,
                 ParamsBackend1> >::value))
{
   is_valid_bitwise_op(
       result,
       typename montgomery_int_backend<MinBits1, MaxBits1, signed_magnitude,
                                       Checked1, Allocator1>::checked_type());
   if (!s)
   {
      return;
   }

   bool is_neg = result.sign();
   if (is_neg)
   {
      eval_increment(result);
   }

#if defined(BOOST_LITTLE_ENDIAN) && defined(BOOST_MP_USE_LIMB_SHIFT)
   static const limb_type limb_shift_mask =
       montgomery_int_backend<MinBits1, MaxBits1, signed_magnitude, Checked1,
                              Allocator1, ParamsBackend1>::limb_bits -
       1;
   static const limb_type byte_shift_mask = CHAR_BIT - 1;
   if ((s & limb_shift_mask) == 0)
      right_shift_limb(result, s);
   else if ((s & byte_shift_mask) == 0)
      right_shift_byte(result, s);
#elif defined(BOOST_LITTLE_ENDIAN)
   static const limb_type byte_shift_mask = CHAR_BIT - 1;
   if ((s & byte_shift_mask) == 0)
   {
      right_shift_byte(result, s);
   }
#else
   static const limb_type limb_shift_mask =
       montgomery_int_backend<MinBits1, MaxBits1, signed_magnitude, Checked1,
                              Allocator1, ParamsBackend1>::limb_bits -
       1;
   if ((s & limb_shift_mask) == 0)
   {
      right_shift_limb(result, s);
   }
#endif
   else
   {
      right_shift_generic(result, s);
   }
   if (is_neg)
   {
      eval_decrement(result);
   }
}

//
// Over again for trivial cpp_int's:
//
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, class T>
BOOST_MP_FORCEINLINE
    typename enable_if<is_trivial_montgomery_int<montgomery_int_backend<
        MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> > >

    ::type eval_left_shift(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        T                                                   s)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   is_valid_bitwise_op(
       result, typename montgomery_int_backend<MinBits1, MaxBits1, SignType1,
                                               Checked1, Allocator1,
                                               ParamsBackend1>::checked_type());
   *result.limbs() = detail::checked_left_shift(
       *result.limbs(), s,
       typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1,
                                       ParamsBackend1>::checked_type());

   result.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, class T>
BOOST_MP_FORCEINLINE
    typename enable_if<is_trivial_montgomery_int<montgomery_int_backend<
        MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> > >

    ::type eval_right_shift(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        T                                                   s)
        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   // Nothing to check here... just make sure we don't invoke undefined behavior:
   is_valid_bitwise_op(
       result,
       typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1>::checked_type());
   *result.limbs() =
       (static_cast<unsigned>(s) >= sizeof(*result.limbs()) * CHAR_BIT)
           ? 0
           : (result.sign() ? ((--*result.limbs()) >> s) + 1
                            : *result.limbs() >> s);

   if (result.sign() && (*result.limbs() == 0))
   {
      result = static_cast<signed_limb_type>(-1);
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
inline typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value &&
    (is_signed_number<
         montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                Allocator1, ParamsBackend1> >::value ||
     is_signed_number<
         montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                Allocator2, ParamsBackend2> >::value)>

    ::type eval_complement(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                     Allocator2, ParamsBackend2> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   BOOST_STATIC_ASSERT_MSG(((Checked1 != checked) || (Checked2 != checked)),
                           "Attempt to take the complement of a signed type "
                           "results in undefined behavior.");
   //
   // If we're not checked then emulate 2's complement behavior:
   //
   if (o.sign())
   {
      *result.limbs() = *o.limbs() - 1;

      result.sign(false);
   }
   else
   {
      *result.limbs() = 1 + *o.limbs();

      result.sign(true);
   }
   result.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
inline typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value &&
    is_unsigned_number<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_unsigned_number<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value>

    ::type eval_complement(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                     Allocator2, ParamsBackend2> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   *result.limbs() = ~*o.limbs();

   result.normalize();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
inline typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value &&
    is_unsigned_number<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_unsigned_number<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value>

    ::type eval_bitwise_and(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                     Allocator2, ParamsBackend2> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   *result.limbs() &= *o.limbs();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
inline typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value &&
    (is_signed_number<
         montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                Allocator1, ParamsBackend1> >::value ||
     is_signed_number<
         montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                Allocator2, ParamsBackend2> >::value)>

    ::type eval_bitwise_and(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                     Allocator2, ParamsBackend2> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   is_valid_bitwise_op(
       result, o,

       typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1,
                                       ParamsBackend1>::checked_type()

   );

   using default_ops::eval_bit_test;
   using default_ops::eval_increment;

   if (result.sign() || o.sign())
   {
      static const unsigned m = static_unsigned_max<
          static_unsigned_max<MinBits1, MinBits2>::value,
          static_unsigned_max<MaxBits1, MaxBits2>::value>::value;
      montgomery_int_backend<m + 1, m + 1, unsigned_magnitude, unchecked, void>
          t1(result);
      montgomery_int_backend<m + 1, m + 1, unsigned_magnitude, unchecked, void>
          t2(o);
      eval_bitwise_and(t1, t2);
      bool s = eval_bit_test(t1, m + 1);
      if (s)
      {
         eval_complement(t1, t1);
         eval_increment(t1);
      }
      result = t1;
      result.sign(s);
   }
   else
   {
      *result.limbs() &= *o.limbs();
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
inline typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value &&
    is_unsigned_number<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_unsigned_number<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value>

    ::type eval_bitwise_or(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                     Allocator2, ParamsBackend2> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   *result.limbs() |= *o.limbs();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
inline typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value &&
    (is_signed_number<
         montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                Allocator1, ParamsBackend1> >::value ||
     is_signed_number<
         montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                Allocator2, ParamsBackend2> >::value)>

    ::type eval_bitwise_or(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                     Allocator2, ParamsBackend2> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   is_valid_bitwise_op(
       result, o,

       typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1,
                                       ParamsBackend1>::checked_type()

   );

   using default_ops::eval_bit_test;
   using default_ops::eval_increment;

   if (result.sign() || o.sign())
   {
      static const unsigned m = static_unsigned_max<
          static_unsigned_max<MinBits1, MinBits2>::value,
          static_unsigned_max<MaxBits1, MaxBits2>::value>::value;
      montgomery_int_backend<m + 1, m + 1, unsigned_magnitude, unchecked, void>
          t1(result);
      montgomery_int_backend<m + 1, m + 1, unsigned_magnitude, unchecked, void>
          t2(o);
      eval_bitwise_or(t1, t2);
      bool s = eval_bit_test(t1, m + 1);
      if (s)
      {
         eval_complement(t1, t1);
         eval_increment(t1);
      }
      result = t1;
      result.sign(s);
   }
   else
   {
      *result.limbs() |= *o.limbs();
      result.normalize();
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
inline typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value &&
    is_unsigned_number<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_unsigned_number<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value>

    ::type eval_bitwise_xor(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                     Allocator2, ParamsBackend2> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   *result.limbs() ^= *o.limbs();
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
inline typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                               Allocator2, ParamsBackend2> >::value &&
    (is_signed_number<
         montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                Allocator1, ParamsBackend1> >::value ||
     is_signed_number<
         montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                Allocator2, ParamsBackend2> >::value)>

    ::type eval_bitwise_xor(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &      result,
        const montgomery_int_backend<MinBits2, MaxBits2, SignType2, Checked2,
                                     Allocator2, ParamsBackend2> &o)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   is_valid_bitwise_op(
       result, o,
       typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1>::checked_type());

   using default_ops::eval_bit_test;
   using default_ops::eval_increment;

   if (result.sign() || o.sign())
   {
      static const unsigned m = static_unsigned_max<
          static_unsigned_max<MinBits1, MinBits2>::value,
          static_unsigned_max<MaxBits1, MaxBits2>::value>::value;
      montgomery_int_backend<m + 1, m + 1, unsigned_magnitude, unchecked, void>
          t1(result);
      montgomery_int_backend<m + 1, m + 1, unsigned_magnitude, unchecked, void>
          t2(o);
      eval_bitwise_xor(t1, t2);
      bool s = eval_bit_test(t1, m + 1);
      if (s)
      {
         eval_complement(t1, t1);
         eval_increment(t1);
      }
      result = t1;
      result.sign(s);
   }
   else
   {
      *result.limbs() ^= *o.limbs();
   }
}
}
}
} // namespace boost::multiprecision::backends

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
