//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_MP_MONTGOMERY_INT_MUL_HPP
#define CRYPTO3_MP_MONTGOMERY_INT_MUL_HPP

namespace boost {
namespace multiprecision {
namespace backends {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127) // conditional expression is constant
#endif
template <typename Backend, unsigned MinBits1, unsigned MaxBits1,
          cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE
    typename enable_if_c<!is_trivial_montgomery_int<montgomery_int_backend<
                             MinBits1, MaxBits1, SignType1, Checked1,
                             Allocator1, ParamsBackend1> >::value &&
                         !is_trivial_montgomery_int<Backend>::value>::type
    eval_multiply_helper(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        const Backend &a, const limb_type &val)
        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_montgomery_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   if (!val)
   {
      result = static_cast<limb_type>(0);
      redc(result);
      return;
   }
   if ((void *)&a != (void *)&result)
   {
      result.resize(a.size(), a.size());
   }
   double_limb_type                                                          carry = 0;
   typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1>::limb_pointer p =
       result.limbs();
   typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1>::limb_pointer pe =
       result.limbs() + result.size();
   typename Backend::const_limb_pointer pa = a.limbs();
   while (p != pe)
   {
      carry +=
          static_cast<double_limb_type>(*pa) * static_cast<double_limb_type>(val);
#ifdef __MSVC_RUNTIME_CHECKS
      *p = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
#else
      *p = static_cast<limb_type>(carry);
#endif
      carry >>= montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1>::limb_bits;
      ++p, ++pa;
   }
   if (carry)
   {
      unsigned i = result.size();
      result.resize(i + 1, i + 1);
      if (result.size() > i)
      {
         result.limbs()[i] = static_cast<limb_type>(carry);
      }
   }
   result.sign(a.sign());
   if (!montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1>::variable)
   {
      result.normalize();
   }
   redc(result);
}

//
// resize_for_carry forces a resize of the underlying buffer only if a previous
// request for "required" elements could possibly have failed, *and* we have
// checking enabled. This will cause an overflow error inside resize():
//
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
inline void resize_for_carry(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> & /*result*/,
    unsigned /*required*/) {}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          class Allocator1, typename ParamsBackend1>
inline void
resize_for_carry(montgomery_int_backend<MinBits1, MaxBits1, SignType1, checked,
                                        Allocator1, ParamsBackend1> &result,
                 unsigned                                            required)
{
   if (result.size() < required)
   {
      result.resize(required, required);
   }
}

// SPLIT FOR 2
template <typename Backend1, typename Backend2, unsigned MinBits1,
          unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
BOOST_MP_FORCEINLINE
    typename enable_if_c<!is_trivial_montgomery_int<montgomery_int_backend<
                             MinBits1, MaxBits1, SignType1, Checked1,
                             Allocator1, ParamsBackend1> >::value &&
                         !is_trivial_montgomery_int<Backend2>::value &&
                         !is_trivial_montgomery_int<Backend2>::value>::type
    eval_multiply_helper(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        const Backend1 &a, const Backend2 &b)
        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_montgomery_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))

{
   // Very simple long multiplication, only usable for small numbers of
   // limb_type's but that's the typical use case for this type anyway:
   //
   // Special cases first:
   //
   unsigned                              as = a.size();
   unsigned                              bs = b.size();
   typename Backend1::const_limb_pointer pa = a.limbs();
   typename Backend2::const_limb_pointer pb = b.limbs();
   if (as == 1)
   {
      bool s = b.sign() != a.sign();
      if (bs == 1)
      {
         result = static_cast<double_limb_type>(*pa) *
                  static_cast<double_limb_type>(*pb);
         redc(result);
      }
      else
      {
         limb_type l = *pa;
         eval_multiply_helper(result, b, l);
      }
      result.sign(s);
      return;
   }
   if (bs == 1)
   {
      bool      s = b.sign() != a.sign();
      limb_type l = *pb;
      eval_multiply_helper(result, a, l);
      result.sign(s);
      return;
   }

   if ((void *)&result == (void *)&a)
   {
      montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                             ParamsBackend1>
          t(a);
      eval_multiply_helper(result, t, b);
      return;
   }
   if ((void *)&result == (void *)&b)
   {
      // montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
      // Allocator1, ParamsBackend1> t(b); //TODO: fix it!!! Very bad if b -
      // cpp_int eval_multiply_helper(result, a, t);
      return;
   }

   result.resize(as + bs, as + bs - 1);
   typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1>::limb_pointer pr =
       result.limbs();

   static const double_limb_type limb_max = ~static_cast<limb_type>(0u);
   static const double_limb_type double_limb_max =
       ~static_cast<double_limb_type>(0u);
   BOOST_STATIC_ASSERT(double_limb_max - 2 * limb_max >= limb_max * limb_max);

   double_limb_type carry = 0;
   std::memset(pr, 0, result.size() * sizeof(limb_type));
   for (unsigned i = 0; i < as; ++i)
   {
      unsigned inner_limit =
          montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                 Allocator1, ParamsBackend1>::variable
              ? bs
              : (std::min)(result.size() - i, bs);
      unsigned j;
      for (j = 0; j < inner_limit; ++j)
      {
         BOOST_ASSERT(i + j < result.size());
#if (!defined(__GLIBCXX__) && !defined(__GLIBCPP__)) || \
    !BOOST_WORKAROUND(BOOST_GCC_VERSION, <= 50100)
         BOOST_ASSERT(
             !std::numeric_limits<double_limb_type>::is_specialized ||
             ((std::numeric_limits<double_limb_type>::max)() - carry >
              static_cast<double_limb_type>(
                  montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                         Allocator1,
                                         ParamsBackend1>::max_limb_value) *
                  static_cast<double_limb_type>(
                      montgomery_int_backend<MinBits1, MaxBits1, SignType1,
                                             Checked1, Allocator1,
                                             ParamsBackend1>::max_limb_value)));
#endif
         carry += static_cast<double_limb_type>(pa[i]) *
                  static_cast<double_limb_type>(pb[j]);
         BOOST_ASSERT(!std::numeric_limits<double_limb_type>::is_specialized ||
                      ((std::numeric_limits<double_limb_type>::max)() - carry >=
                       pr[i + j]));
         carry += pr[i + j];
#ifdef __MSVC_RUNTIME_CHECKS
         pr[i + j] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
#else
         pr[i + j] = static_cast<limb_type>(carry);
#endif
         carry >>= montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                          Allocator1, ParamsBackend1>::limb_bits;
         BOOST_ASSERT(
             carry <=
             (montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1>::max_limb_value));
      }
      if (carry)
      {
         resize_for_carry(result, i + j + 1); // May throw if checking is enabled
         if (i + j < result.size())
#ifdef __MSVC_RUNTIME_CHECKS
            pr[i + j] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
#else
         {
            pr[i + j] = static_cast<limb_type>(carry);
         }
#endif
      }
      carry = 0;
   }
   result.normalize();

   //
   // Set the sign of the result:
   //
   result.sign(a.sign() != b.sign());
   redc(result);
}

template <typename Backend, unsigned MinBits1, unsigned MaxBits1,
          cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE
    typename enable_if_c<!is_trivial_montgomery_int<montgomery_int_backend<
                             MinBits1, MaxBits1, SignType1, Checked1,
                             Allocator1, ParamsBackend1> >::value &&
                         !is_trivial_montgomery_int<Backend>::value>::type
    eval_multiply_helper(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        const Backend &                                     a)
        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_montgomery_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))

{
   eval_multiply_helper(result, result, a);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> >::value>::type
eval_multiply(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &result,
              const limb_type &                                   val)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, result, val);
}

template <typename Backend, unsigned MinBits1, unsigned MaxBits1,
          cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>

BOOST_MP_FORCEINLINE
    typename enable_if_c<!is_trivial_montgomery_int<montgomery_int_backend<
                             MinBits1, MaxBits1, SignType1, Checked1,
                             Allocator1, ParamsBackend1> >::value &&
                         !is_trivial_montgomery_int<Backend>::value>::type
    eval_multiply_helper(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        const Backend &a, const double_limb_type &val)
        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_montgomery_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   if (val <= (std::numeric_limits<limb_type>::max)())
   {
      eval_multiply_helper(result, a, static_cast<limb_type>(val));
   }
   else
   {
#if defined(BOOST_LITTLE_ENDIAN) && !defined(BOOST_MP_TEST_NO_LE)
      montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                             ParamsBackend1>
          t(val);
#else
      montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                             ParamsBackend1>
          t;
      t = val;
#endif
      eval_multiply_helper(result, a, t);
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> >::value>

    ::type eval_multiply(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        const double_limb_type &                            val)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, result, val);
}

template <typename Backend, unsigned MinBits1, unsigned MaxBits1,
          cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE
    typename enable_if_c<!is_trivial_montgomery_int<montgomery_int_backend<
                             MinBits1, MaxBits1, SignType1, Checked1,
                             Allocator1, ParamsBackend1> >::value &&
                         !is_trivial_montgomery_int<Backend>::value>::type
    eval_multiply_helper(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        const Backend &a, const signed_limb_type &val)
        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_montgomery_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   if (val > 0)
   {
      eval_multiply_helper(result, a, static_cast<limb_type>(val));
   }
   else
   {
      eval_multiply_helper(result, a,
                           static_cast<limb_type>(
                               boost::multiprecision::detail::unsigned_abs(val)));
      result.negate();
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> >::value>

    ::type eval_multiply(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        const signed_limb_type &                            val)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, result, val);
}

template <typename Backend, unsigned MinBits1, unsigned MaxBits1,
          cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
inline typename enable_if_c<!is_trivial_montgomery_int<montgomery_int_backend<
                                MinBits1, MaxBits1, SignType1, Checked1,
                                Allocator1, ParamsBackend1> >::value &&
                            !is_trivial_montgomery_int<Backend>::value>::type
eval_multiply_helper(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &result,
    const Backend &a, const signed_double_limb_type &val)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   if (val > 0)
   {
      if (val <= (std::numeric_limits<limb_type>::max)())
      {
         eval_multiply_helper(result, a, static_cast<limb_type>(val));
         return;
      }
   }
   else if (val >= -static_cast<signed_double_limb_type>(
                       (std::numeric_limits<limb_type>::max)()))
   {
      eval_multiply_helper(result, a,
                           static_cast<limb_type>(
                               boost::multiprecision::detail::unsigned_abs(val)));
      result.negate();

      return;
   }
#if defined(BOOST_LITTLE_ENDIAN) && !defined(BOOST_MP_TEST_NO_LE)
   montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                          ParamsBackend1>
       t(val);
#else
   montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                          ParamsBackend1>
       t;
   t = val;
#endif
   eval_multiply_helper(result, a, t);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> >::value>

    ::type eval_multiply(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        const signed_double_limb_type &                     val)

        BOOST_MP_NOEXCEPT_IF(
            (is_non_throwing_cpp_int<
                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, result, val);
}

//
// Now over again for trivial cpp_int's:
//
template <typename Backend, unsigned MinBits1, unsigned MaxBits1,
          cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<Backend>::value &&
    (is_signed_number<
         montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                Allocator1, ParamsBackend1> >::value ||
     is_signed_number<Backend>::value)>::type
eval_multiply_helper(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &result,
    const Backend &                         o)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))

{
   *result.limbs() = detail::checked_multiply(
       *result.limbs(), *o.limbs(),
       typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1,
                                       ParamsBackend1>::checked_type());

   result.sign(result.sign() != o.sign());
   result.normalize();
   redc(result);
}

template <typename Backend, unsigned MinBits1, unsigned MaxBits1,
          cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_unsigned_number<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<Backend>::value &&
    is_unsigned_number<Backend>::value>::type
eval_multiply_helper(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &result,
    const Backend &                         o)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   *result.limbs() = detail::checked_multiply(
       *result.limbs(), *o.limbs(),
       typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1,
                                       ParamsBackend1>::checked_type());

   result.normalize();
   redc(result);
}

template <typename Backend, unsigned MinBits1, unsigned MaxBits1,
          cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<Backend>::value &&
    (is_signed_number<
         montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                Allocator1, ParamsBackend1> >::value ||
     is_signed_number<Backend>::value)>::type
eval_multiply_helper(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &result,
    const Backend &a, const Backend &b)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   *result.limbs() = detail::checked_multiply(
       *a.limbs(), *b.limbs(),
       typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1,
                                       ParamsBackend1>::checked_type());

   result.sign(a.sign() != b.sign());
   result.normalize();
   redc(result);
}

template <typename Backend, unsigned MinBits1, unsigned MaxBits1,
          cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_unsigned_number<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<Backend>::value &&
    is_unsigned_number<Backend>::value>::type
eval_multiply_helper(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &result,
    const Backend &a, const Backend &b)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   *result.limbs() = detail::checked_multiply(
       *a.limbs(), *b.limbs(),
       typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                       Allocator1,
                                       ParamsBackend1>::checked_type());

   result.normalize();
   redc(result);
}

//
// Special routines for multiplying two integers to obtain a multiprecision
// result:
//
template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> >::value>::type
eval_multiply(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &result,
              signed_double_limb_type a, signed_double_limb_type b)
{
   static const signed_double_limb_type mask      = ~static_cast<limb_type>(0);
   static const unsigned                limb_bits = sizeof(limb_type) * CHAR_BIT;
   bool                                 s         = false;
   double_limb_type                     w, x, y, z;
   if (a < 0)
   {
      a = -a;
      s = true;
   }
   if (b < 0)
   {
      b = -b;
      s = !s;
   }
   w = a & mask;
   x = a >> limb_bits;
   y = b & mask;
   z = b >> limb_bits;

   result.resize(4, 4);
   limb_type *pr = result.limbs();

   double_limb_type carry = w * y;
#ifdef __MSVC_RUNTIME_CHECKS
   pr[0] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   carry >>= limb_bits;
   carry += w * z + x * y;
   pr[1] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   carry >>= limb_bits;
   carry += x * z;
   pr[2] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   pr[3] = static_cast<limb_type>(carry >> limb_bits);
#else
   pr[0] = static_cast<limb_type>(carry);
   carry >>= limb_bits;
   carry += w * z + x * y;
   pr[1] = static_cast<limb_type>(carry);
   carry >>= limb_bits;
   carry += x * z;
   pr[2] = static_cast<limb_type>(carry);
   pr[3] = static_cast<limb_type>(carry >> limb_bits);
#endif
   result.sign(s);
   result.normalize();
   redc(result);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> >::value>

    ::type eval_multiply(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        double_limb_type a, double_limb_type b)
{
   static const signed_double_limb_type mask      = ~static_cast<limb_type>(0);
   static const unsigned                limb_bits = sizeof(limb_type) * CHAR_BIT;

   double_limb_type w, x, y, z;
   w = a & mask;
   x = a >> limb_bits;
   y = b & mask;
   z = b >> limb_bits;

   result.resize(4, 4);
   limb_type *pr = result.limbs();

   double_limb_type carry = w * y;
#ifdef __MSVC_RUNTIME_CHECKS
   pr[0] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   carry >>= limb_bits;
   carry += w * z;
   pr[1] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   carry >>= limb_bits;
   pr[2] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   carry = x * y + pr[1];
   pr[1] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   carry >>= limb_bits;
   carry += pr[2] + x * z;
   pr[2] = static_cast<limb_type>(carry & ~static_cast<limb_type>(0));
   pr[3] = static_cast<limb_type>(carry >> limb_bits);
#else
   pr[0] = static_cast<limb_type>(carry);
   carry >>= limb_bits;
   carry += w * z;
   pr[1] = static_cast<limb_type>(carry);
   carry >>= limb_bits;
   pr[2] = static_cast<limb_type>(carry);
   carry = x * y + pr[1];
   pr[1] = static_cast<limb_type>(carry);
   carry >>= limb_bits;
   carry += pr[2] + x * z;
   pr[2] = static_cast<limb_type>(carry);
   pr[3] = static_cast<limb_type>(carry >> limb_bits);
#endif
   result.sign(false);
   result.normalize();
   redc(result);
}

template <typename Backend, unsigned MinBits1, unsigned MaxBits1,
          cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>

BOOST_MP_FORCEINLINE
    typename enable_if_c<!is_trivial_montgomery_int<montgomery_int_backend<
                             MinBits1, MaxBits1, SignType1, Checked1,
                             Allocator1, ParamsBackend1> >::value &&
                         is_trivial_montgomery_int<Backend>::value &&
                         is_trivial_montgomery_int<Backend>::value>::type
    eval_multiply_helper(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> &result,
        Backend const &a, Backend const &b)
{
   typedef typename boost::multiprecision::detail::canonical<
       typename Backend::local_limb_type,
       montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                              Allocator1, ParamsBackend1> >::type canonical_type;
   eval_multiply_helper(result, static_cast<canonical_type>(*a.limbs()),
                        static_cast<canonical_type>(*b.limbs()));
   result.sign(a.sign() != b.sign());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, class SI>
BOOST_MP_FORCEINLINE typename enable_if_c<
    is_signed<SI>::value &&
    (sizeof(SI) <= sizeof(signed_double_limb_type) / 2)>::type
eval_multiply(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &result,
              SI a, SI b)
{
   result = static_cast<signed_double_limb_type>(a) *
            static_cast<signed_double_limb_type>(b);
   redc(result);
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, class UI>
BOOST_MP_FORCEINLINE typename enable_if_c<
    is_unsigned<UI>::value &&
    (sizeof(UI) <= sizeof(signed_double_limb_type) / 2)>::type
eval_multiply(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &result,
              UI a, UI b)
{
   result = static_cast<double_limb_type>(a) * static_cast<double_limb_type>(b);
   redc(result);
}

//====================================================//
//====================================================//
//====================================================//

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename, typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<Backend<MinBits2, MaxBits2, SignType2, Checked2,
                                       Allocator2, ParamsBackend2> >::value &&
    is_equal_montgomery_int_field<
        Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                ParamsBackend2> >::value>::type
eval_multiply(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &result,
              const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                            ParamsBackend2> &                     a,
              const limb_type &                                   val)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, a, val);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value &&
    is_equal_montgomery_int_backend<Backend<MinBits2, MaxBits2, SignType2,
                                            Checked2, Allocator2> >::value>::type
eval_multiply(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &                            result,
    const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> &a,
    const limb_type &                                                   val)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, a, val);
}

//
// resize_for_carry forces a resize of the underlying buffer only if a previous
// request for "required" elements could possibly have failed, *and* we have
// checking enabled. This will cause an overflow error inside resize():
//

template <
    template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
              typename, typename>
    class Backend1,
    template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
              typename, typename>
    class Backend2,
    unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
    cpp_int_check_type Checked1, class Allocator1, typename ParamsBackend1,
    unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2,
    cpp_int_check_type Checked2, class Allocator2, typename ParamsBackend2,
    unsigned MinBits3, unsigned MaxBits3, cpp_integer_type SignType3,
    cpp_int_check_type Checked3, class Allocator3, typename ParamsBackend3>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<Backend1<MinBits2, MaxBits2, SignType2, Checked2,
                                        Allocator2, ParamsBackend2> >::value &&
    !is_trivial_montgomery_int<Backend2<MinBits3, MaxBits3, SignType3, Checked3,
                                        Allocator3, ParamsBackend3> >::value &&
    is_equal_montgomery_int_field<
        Backend1<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                 ParamsBackend2> >::value &&
    is_equal_montgomery_int_field<
        Backend2<MinBits3, MaxBits3, SignType3, Checked3, Allocator3,
                 ParamsBackend3> >::value>::type
eval_multiply(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &result,
              const Backend1<MinBits2, MaxBits2, SignType2, Checked2,
                             Allocator2, ParamsBackend2> &        a,
              const Backend2<MinBits3, MaxBits3, SignType3, Checked3,
                             Allocator3, ParamsBackend3> &        b)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))

{
   eval_multiply_helper(result, a, b);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename, typename>
          class Backend1,
          template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename>
          class Backend2,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2, unsigned MinBits3,
          unsigned MaxBits3, cpp_integer_type SignType3,
          cpp_int_check_type Checked3, class Allocator3>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<Backend1<MinBits2, MaxBits2, SignType2, Checked2,
                                        Allocator2, ParamsBackend2> >::value &&
    !is_trivial_montgomery_int<
        Backend2<MinBits3, MaxBits3, SignType3, Checked3, Allocator3> >::value &&
    is_equal_montgomery_int_field<
        Backend1<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                 ParamsBackend2> >::value &&
    is_equal_montgomery_int_backend<Backend2<
        MinBits3, MaxBits3, SignType3, Checked3, Allocator3> >::value>::type
eval_multiply(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &                             result,
    const Backend1<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                   ParamsBackend2> &                                     a,
    const Backend2<MinBits3, MaxBits3, SignType3, Checked3, Allocator3> &b)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))

{
   eval_multiply_helper(result, a, b);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename>
          class Backend1,
          template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename, typename>
          class Backend2,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, unsigned MinBits3, unsigned MaxBits3,
          cpp_integer_type SignType3, cpp_int_check_type Checked3,
          class Allocator3, typename ParamsBackend3>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        Backend1<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value &&
    !is_trivial_montgomery_int<Backend2<MinBits3, MaxBits3, SignType3, Checked3,
                                        Allocator3, ParamsBackend3> >::value &&
    is_equal_montgomery_int_field<
        Backend2<MinBits3, MaxBits3, SignType3, Checked3, Allocator3,
                 ParamsBackend3> >::value &&
    is_equal_montgomery_int_backend<Backend1<
        MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value>::type
eval_multiply(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &                             result,
    const Backend1<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> &a,
    const Backend2<MinBits3, MaxBits3, SignType3, Checked3, Allocator3,
                   ParamsBackend3> &                                     b)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))

{
   eval_multiply_helper(result, a, b);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename>
          class Backend1,
          template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename>
          class Backend2,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, unsigned MinBits3, unsigned MaxBits3,
          cpp_integer_type SignType3, cpp_int_check_type Checked3,
          class Allocator3>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        Backend1<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value &&
    !is_trivial_montgomery_int<
        Backend2<MinBits3, MaxBits3, SignType3, Checked3, Allocator3> >::value &&
    is_equal_montgomery_int_backend<
        Backend1<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value &&
    is_equal_montgomery_int_backend<Backend2<
        MinBits3, MaxBits3, SignType3, Checked3, Allocator3> >::value>::type
eval_multiply(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &                             result,
    const Backend1<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> &a,
    const Backend2<MinBits3, MaxBits3, SignType3, Checked3, Allocator3> &b)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))

{
   eval_multiply_helper(result, a, b);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename, typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<Backend<MinBits2, MaxBits2, SignType2, Checked2,
                                       Allocator2, ParamsBackend2> >::value &&
    is_equal_montgomery_int_field<
        Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                ParamsBackend2> >::value>::type
eval_multiply(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &result,
              const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                            ParamsBackend2> &                     a)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))

{
   eval_multiply(result, result, a);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value &&
    is_equal_montgomery_int_backend<Backend<MinBits2, MaxBits2, SignType2,
                                            Checked2, Allocator2> >::value>::type
eval_multiply(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &                            result,
    const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> &a)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))

{
   eval_multiply(result, result, a);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename, typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>

BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<Backend<MinBits2, MaxBits2, SignType2, Checked2,
                                       Allocator2, ParamsBackend2> >::value &&
    is_equal_montgomery_int_field<
        Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                ParamsBackend2> >::value>::type
eval_multiply(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &result,
              const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                            ParamsBackend2> &                     a,
              const double_limb_type &                            val)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, a, val);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2>

BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value &&
    is_equal_montgomery_int_backend<Backend<MinBits2, MaxBits2, SignType2,
                                            Checked2, Allocator2> >::value>::type
eval_multiply(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &                            result,
    const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> &a,
    const double_limb_type &                                            val)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, a, val);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename, typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<Backend<MinBits2, MaxBits2, SignType2, Checked2,
                                       Allocator2, ParamsBackend2> >::value &&
    is_equal_montgomery_int_field<
        Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                ParamsBackend2> >::value>::type
eval_multiply(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &result,
              const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                            ParamsBackend2> &                     a,
              const signed_limb_type &                            val)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, a, val);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2>
BOOST_MP_FORCEINLINE typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value &&
    is_equal_montgomery_int_backend<Backend<MinBits2, MaxBits2, SignType2,
                                            Checked2, Allocator2> >::value>::type
eval_multiply(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &                            result,
    const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> &a,
    const signed_limb_type &                                            val)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, a, val);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename, typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
inline typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<Backend<MinBits2, MaxBits2, SignType2, Checked2,
                                       Allocator2, ParamsBackend2> >::value &&
    is_equal_montgomery_int_field<
        Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                ParamsBackend2> >::value>::type
eval_multiply(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &result,
              const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2,
                            ParamsBackend2> &                     a,
              const signed_double_limb_type &                     val)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, a, val);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2,
          cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2>
inline typename enable_if_c<
    !is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    !is_trivial_montgomery_int<
        Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> >::value &&
    is_equal_montgomery_int_backend<Backend<MinBits2, MaxBits2, SignType2,
                                            Checked2, Allocator2> >::value>::type
eval_multiply(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &                            result,
    const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2> &a,
    const signed_double_limb_type &                                     val)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, a, val);
}

//
// Now over again for trivial cpp_int's:
//
template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename, typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<Backend<MinBits1, MaxBits1, SignType1, Checked1,
                                      Allocator1, ParamsBackend1> >::value &&
    (is_signed_number<
         montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                Allocator1, ParamsBackend1> >::value ||
     is_signed_number<Backend<MinBits1, MaxBits1, SignType1, Checked1,
                              Allocator1, ParamsBackend1> >::value) &&
    is_equal_montgomery_int_field<
        Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                ParamsBackend1> >::value>::type
eval_multiply(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &result,
              const Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                            ParamsBackend1> &                     o)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))

{
   eval_multiply_helper(result, o);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<
        Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value &&
    (is_signed_number<
         montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                Allocator1, ParamsBackend1> >::value ||
     is_signed_number<Backend<MinBits1, MaxBits1, SignType1, Checked1,
                              Allocator1> >::value) &&
    is_equal_montgomery_int_backend<Backend<MinBits1, MaxBits1, SignType1,
                                            Checked1, Allocator1> >::value>::type
eval_multiply(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &                            result,
    const Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> &o)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))

{
   eval_multiply_helper(result, o);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename, typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_unsigned_number<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<Backend<MinBits1, MaxBits1, SignType1, Checked1,
                                      Allocator1, ParamsBackend1> >::value &&
    is_unsigned_number<Backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_equal_montgomery_int_field<
        Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                ParamsBackend1> >::value>::type
eval_multiply(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                     Allocator1, ParamsBackend1> &result,
              const Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                            ParamsBackend1> &                     o)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, o);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
                    typename>
          class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1,
          cpp_int_check_type Checked1, class Allocator1,
          typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<
    is_trivial_montgomery_int<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_unsigned_number<
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                               Allocator1, ParamsBackend1> >::value &&
    is_trivial_montgomery_int<
        Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value &&
    is_unsigned_number<
        Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> >::value &&
    is_equal_montgomery_int_backend<Backend<MinBits1, MaxBits1, SignType1,
                                            Checked1, Allocator1> >::value>::type
eval_multiply(
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1,
                           ParamsBackend1> &                            result,
    const Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1> &o)
    BOOST_MP_NOEXCEPT_IF(
        (is_non_throwing_montgomery_int<
            montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                   Allocator1, ParamsBackend1> >::value))
{
   eval_multiply_helper(result, o);
}

/*!

template<template< unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
typename, typename> class Backend, unsigned MinBits1, unsigned MaxBits1,
cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1,
typename ParamsBackend1> BOOST_MP_FORCEINLINE typename enable_if_c<
        is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1>>::value
        && is_trivial_montgomery_int<Backend<MinBits1, MaxBits1, SignType1,
Checked1, Allocator1, ParamsBackend1> >::value
        && (is_signed_number<montgomery_int_backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1> >::value
            || is_signed_number<Backend<MinBits1, MaxBits1, SignType1, Checked1,
Allocator1, ParamsBackend1>>::value)
>::type
eval_multiply(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
Allocator1, ParamsBackend1>& result, const Backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1>& a, const Backend<MinBits1,
MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& b)
BOOST_MP_NOEXCEPT_IF((is_non_throwing_montgomery_int<montgomery_int_backend<MinBits1,
MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
    eval_multiply_helper(result, a, b);
}

emplate<template< unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
typename, typename> class Backend, unsigned MinBits1, unsigned MaxBits1,
cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1,
typename ParamsBackend1> BOOST_MP_FORCEINLINE typename enable_if_c<
        is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1>>::value
        && is_trivial_montgomery_int<Backend<MinBits1, MaxBits1, SignType1,
Checked1, Allocator1, ParamsBackend1> >::value
        && (is_signed_number<montgomery_int_backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1> >::value
            || is_signed_number<Backend<MinBits1, MaxBits1, SignType1, Checked1,
Allocator1, ParamsBackend1>>::value)
>::type
eval_multiply(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
Allocator1, ParamsBackend1>& result, const Backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1>& a, const Backend<MinBits1,
MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& b)
BOOST_MP_NOEXCEPT_IF((is_non_throwing_montgomery_int<montgomery_int_backend<MinBits1,
MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
    eval_multiply_helper(result, a, b);
}

emplate<template< unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
typename, typename> class Backend, unsigned MinBits1, unsigned MaxBits1,
cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1,
typename ParamsBackend1> BOOST_MP_FORCEINLINE typename enable_if_c<
        is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1>>::value
        && is_trivial_montgomery_int<Backend<MinBits1, MaxBits1, SignType1,
Checked1, Allocator1, ParamsBackend1> >::value
        && (is_signed_number<montgomery_int_backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1> >::value
            || is_signed_number<Backend<MinBits1, MaxBits1, SignType1, Checked1,
Allocator1, ParamsBackend1>>::value)
>::type
eval_multiply(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
Allocator1, ParamsBackend1>& result, const Backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1>& a, const Backend<MinBits1,
MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& b)
BOOST_MP_NOEXCEPT_IF((is_non_throwing_montgomery_int<montgomery_int_backend<MinBits1,
MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
    eval_multiply_helper(result, a, b);
}

emplate<template< unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
typename, typename> class Backend, unsigned MinBits1, unsigned MaxBits1,
cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1,
typename ParamsBackend1> BOOST_MP_FORCEINLINE typename enable_if_c<
        is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1>>::value
        && is_trivial_montgomery_int<Backend<MinBits1, MaxBits1, SignType1,
Checked1, Allocator1, ParamsBackend1> >::value
        && (is_signed_number<montgomery_int_backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1> >::value
            || is_signed_number<Backend<MinBits1, MaxBits1, SignType1, Checked1,
Allocator1, ParamsBackend1>>::value)
>::type
eval_multiply(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
Allocator1, ParamsBackend1>& result, const Backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1>& a, const Backend<MinBits1,
MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& b)
BOOST_MP_NOEXCEPT_IF((is_non_throwing_montgomery_int<montgomery_int_backend<MinBits1,
MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
    eval_multiply_helper(result, a, b);
}
-----------

template<template< unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
typename, typename> class Backend, unsigned MinBits1, unsigned MaxBits1,
cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1,
typename ParamsBackend1> BOOST_MP_FORCEINLINE typename enable_if_c<
        is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1>>::value
        && is_unsigned_number<montgomery_int_backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1>>::value
        && is_trivial_montgomery_int<Backend<MinBits1, MaxBits1, SignType1,
Checked1, Allocator1, ParamsBackend1>>::value
        && is_unsigned_number<Backend<MinBits1, MaxBits1, SignType1, Checked1,
Allocator1, ParamsBackend1>>::value
>::type
eval_multiply(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
Allocator1, ParamsBackend1>& result, const Backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1>& a, const Backend<MinBits1,
MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& b)
BOOST_MP_NOEXCEPT_IF((is_non_throwing_montgomery_int<montgomery_int_backend<MinBits1,
MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>>::value))
{
    eval_multiply_helper(result, a, b);
}


template<template< unsigned, unsigned, cpp_integer_type, cpp_int_check_type,
typename, typename> class Backend, unsigned MinBits1, unsigned MaxBits1,
cpp_integer_type SignType1, cpp_int_check_type Checked1, class Allocator1,
typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type
SignType2, cpp_int_check_type Checked2, class Allocator2, typename
ParamsBackend2>

BOOST_MP_FORCEINLINE typename enable_if_c<
        !is_trivial_montgomery_int<montgomery_int_backend<MinBits1, MaxBits1,
SignType1, Checked1, Allocator1, ParamsBackend1> >::value
        && is_trivial_montgomery_int<Backend<MinBits2, MaxBits2, SignType2,
Checked2, Allocator2, ParamsBackend2> >::value
        && is_trivial_montgomery_int<Backend<MinBits2, MaxBits2, SignType2,
Checked2, Allocator2, ParamsBackend2>>::value
>::type
eval_multiply(
        montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
Allocator1, ParamsBackend1>& result, Backend<MinBits2, MaxBits2, SignType2,
Checked2, Allocator2, ParamsBackend2> const& a, Backend<MinBits2, MaxBits2,
SignType2, Checked2, Allocator2, ParamsBackend2> const& b)
{
    eval_multiply_helper(result, a, b);
}

*/
#ifdef _MSC_VER
#pragma warning(pop)
#endif

}
}
} // namespace boost::multiprecision::backends

#endif