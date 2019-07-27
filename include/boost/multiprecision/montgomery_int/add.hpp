//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MONTGOMERY_INT_ADD_HPP
#define BOOST_MULTIPRECISION_MONTGOMERY_INT_ADD_HPP

namespace boost {
namespace multiprecision {
namespace backends {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127) // conditional expression is constant
#endif

//
// Now the actual functions called by the front end, all of which forward to one of the above:
//
template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          !is_trivial_montgomery_int<
                                              Backend<MinBits2, MaxBits2, SignType2,
                                                      Checked2, Allocator2, ParamsBackend2> >::value &&
                                          is_equal_montgomery_int_backend<Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value>

    ::type eval_add(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                    const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2>&          o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   eval_add(result, result, o);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend1,
          template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend2,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2, unsigned MinBits3, unsigned MaxBits3, cpp_integer_type SignType3, cpp_int_check_type Checked3,
          class Allocator3, typename ParamsBackend3>
inline typename enable_if_c<!is_trivial_montgomery_int<
                                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                            !is_trivial_montgomery_int<
                                Backend1<MinBits2, MaxBits2, SignType2, Checked2,
                                         Allocator2, ParamsBackend2> >::value &&
                            !is_trivial_montgomery_int<
                                Backend2<MinBits3, MaxBits3, SignType3, Checked3, Allocator3, ParamsBackend3> >::value &&
                            is_equal_montgomery_int_backend<Backend1<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value &&
                            is_equal_montgomery_int_backend<Backend2<MinBits3, MaxBits3, SignType3, Checked3, Allocator3, ParamsBackend3> >::value>

    ::type eval_add(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                    const Backend1<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2>&         a,
                    const Backend2<MinBits3, MaxBits3, SignType3, Checked3, Allocator3, ParamsBackend3>&         b)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   if (a.sign() != b.sign())
   {
      subtract_unsigned(result, a, b);
      return;
   }
   add_unsigned(result, a, b);
   eval_divide(result, result.m_params.p().backend());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

    ::type eval_add(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                    const limb_type&                                                                             o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   if (result.sign())
   {
      subtract_unsigned(result, result, o);
   }
   else
   {
      add_unsigned(result, result, o);
   }
   eval_divide(result, result.m_params.p().backend());
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          !is_trivial_montgomery_int<
                                              Backend<MinBits2, MaxBits2, SignType2,
                                                      Checked2, Allocator2, ParamsBackend2> >::value &&
                                          is_equal_montgomery_int_backend<Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value>

    ::type eval_add(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                    const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2>&          a,
                    const limb_type&                                                                             o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   if (a.sign())
   {
      subtract_unsigned(result, a, o);
   }
   else
   {
      add_unsigned(result, a, o);
   }
   eval_divide(result, result.m_params.p().backend());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

    ::type eval_add(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                    const signed_limb_type&                                                                      o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   if (o < 0)
   {
      eval_subtract(result, static_cast<limb_type>(boost::multiprecision::detail::unsigned_abs(o)));
   }
   else if (o > 0)
   {
      eval_add(result, static_cast<limb_type>(o));
   }
   eval_divide(result, result.m_params.p().backend());
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          !is_trivial_montgomery_int<
                                              Backend<MinBits2, MaxBits2, SignType2,
                                                      Checked2, Allocator2, ParamsBackend2> >::value &&
                                          is_equal_montgomery_int_backend<Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value>

    ::type eval_add(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                    const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2>&          a,
                    const signed_limb_type&                                                                      o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   if (o < 0)
   {
      eval_subtract(result, a, static_cast<limb_type>(boost::multiprecision::detail::unsigned_abs(o)));
   }
   else if (o > 0)
   {
      eval_add(result, a, static_cast<limb_type>(o));
   }
   else if (&result != &a)
   {
      result = a;
      eval_divide(result, result.m_params.p().backend());
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

    ::type eval_subtract(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                         const limb_type&                                                                             o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   if (result.sign())
   {
      add_unsigned(result, result, o);
   }
   else
   {
      subtract_unsigned(result, result, o);
   }
   eval_divide(result, result.m_params.p().backend());
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          !is_trivial_montgomery_int<
                                              Backend<MinBits2, MaxBits2, SignType2,
                                                      Checked2, Allocator2, ParamsBackend2> >::value &&
                                          is_equal_montgomery_int_backend<Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value>

    ::type eval_subtract(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                         const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2>&          a,
                         const limb_type&                                                                             o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   if (a.sign())
   {
      add_unsigned(result, a, o);
   }
   else
   {
      subtract_unsigned(result, a, o);
   }
   eval_divide(result, result.m_params.p().backend());
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

    ::type eval_subtract(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                         const signed_limb_type&                                                                      o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   if (o)
   {
      if (o < 0)
      {
         eval_add(result, static_cast<limb_type>(boost::multiprecision::detail::unsigned_abs(o)));
      }
      else
      {
         eval_subtract(result, static_cast<limb_type>(o));
      }
   }
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          !is_trivial_montgomery_int<
                                              Backend<MinBits2, MaxBits2, SignType2,
                                                      Checked2, Allocator2, ParamsBackend2> >::value &&
                                          is_equal_montgomery_int_backend<Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value>

    ::type eval_subtract(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                         const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2>&          a,
                         const signed_limb_type&                                                                      o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   if (o)
   {
      if (o < 0)
      {
         eval_add(result, a, static_cast<limb_type>(boost::multiprecision::detail::unsigned_abs(o)));
      }
      else
      {
         eval_subtract(result, a, static_cast<limb_type>(o));
      }
   }
   else if (&result != &a)
   {
      result = a;
      eval_divide(result, result.m_params.p().backend());
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

    ::type eval_increment(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   static const limb_type one = 1;
   if (!result.sign() && (result.limbs()[0] <
                          montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                                 Allocator1, ParamsBackend1>::max_limb_value))
   {
      ++result.limbs()[0];
      eval_divide(result, result.m_params.p().backend());
   }
   else if (result.sign() && result.limbs()[0])
   {
      --result.limbs()[0];
      eval_divide(result, result.m_params.p().backend());
   }
   else
   {
      eval_add(result, one);
   }
}

template <unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
    montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

    ::type eval_decrement(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   static const limb_type one = 1;
   if (!result.sign() && result.limbs()[0])
   {
      --result.limbs()[0];
      eval_divide(result, result.m_params.p().backend());
   }
   else if (result.sign() && (result.limbs()[0] <
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                                     Allocator1, ParamsBackend1>::max_limb_value))
   {
      ++result.limbs()[0];
      eval_divide(result, result.m_params.p().backend());
   }
   else
   {
      eval_subtract(result, one);
   }
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          !is_trivial_montgomery_int<
                                              Backend<MinBits2, MaxBits2, SignType2,
                                                      Checked2, Allocator2, ParamsBackend2> >::value &&
                                          is_equal_montgomery_int_backend<Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value>

    ::type eval_subtract(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                         const Backend<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2>&          o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   eval_subtract(result, result, o);
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend1,
          template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend2,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1, unsigned MinBits2, unsigned MaxBits2, cpp_integer_type SignType2, cpp_int_check_type Checked2,
          class Allocator2, typename ParamsBackend2, unsigned MinBits3, unsigned MaxBits3, cpp_integer_type SignType3, cpp_int_check_type Checked3,
          class Allocator3, typename ParamsBackend3>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          !is_trivial_montgomery_int<
                                              Backend1<MinBits2, MaxBits2, SignType2,
                                                       Checked2, Allocator2, ParamsBackend2> >::value &&
                                          !is_trivial_montgomery_int<
                                              Backend2<MinBits3, MaxBits3, SignType3,
                                                       Checked3, Allocator3, ParamsBackend3> >::value &&
                                          is_equal_montgomery_int_backend<Backend1<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2> >::value &&
                                          is_equal_montgomery_int_backend<Backend2<MinBits3, MaxBits3, SignType3, Checked3, Allocator3, ParamsBackend3> >::value>

    ::type eval_subtract(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                         const Backend1<MinBits2, MaxBits2, SignType2, Checked2, Allocator2, ParamsBackend2>&         a,
                         const Backend2<MinBits3, MaxBits3, SignType3, Checked3, Allocator3, ParamsBackend3>&         b)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   if (a.sign() != b.sign())
   {
      add_unsigned(result, a, b);
      eval_divide(result, result.m_params.p().backend());
      return;
   }
   subtract_unsigned(result, a, b);
   eval_divide(result, result.m_params.p().backend());
}

//
// Simple addition and subtraction routine for trivial cpp_int's come last:
//
// One of the arguments is signed:
//
template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
inline typename enable_if_c<is_trivial_montgomery_int<
                                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                            is_trivial_montgomery_int<
                                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                                       Allocator1, ParamsBackend1> >::value &&
                            (is_signed_number<
                                 montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value ||
                             is_signed_number<
                                 montgomery_int_backend<
                                     MinBits1, MaxBits1,
                                     SignType1, Checked1,
                                     Allocator1, ParamsBackend1> >::value) &&
                            is_trivial_montgomery_int<
                                Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                            is_trivial_montgomery_int<
                                Backend<MinBits1, MaxBits1, SignType1, Checked1,
                                        Allocator1, ParamsBackend1> >::value &&
                            (is_signed_number<
                                 Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value ||
                             is_signed_number<
                                 Backend<
                                     MinBits1, MaxBits1,
                                     SignType1, Checked1,
                                     Allocator1, ParamsBackend1> >::value) &&
                            is_equal_montgomery_int_backend<Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

    ::type eval_add(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                    const Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>&          o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   if (result.sign() != o.sign())
   {
      if (*o.limbs() > *result.limbs())
      {
         *result.limbs() = detail::checked_subtract(*o.limbs(), *result.limbs(),
                                                    typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                                                                    Allocator1, ParamsBackend1>::checked_type());

         result.negate();
      }
      else
      {
         *result.limbs() = detail::checked_subtract(*result.limbs(), *o.limbs(),
                                                    typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                                                                    Allocator1, ParamsBackend1>::checked_type());
      }
   }
   else
   {
      *result.limbs() = detail::checked_add(*result.limbs(), *o.limbs(),
                                            typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                                                            Allocator1, ParamsBackend1>::checked_type());
   }

   result.normalize();
   eval_divide(result, result.m_params.p().backend());
}

// Simple version for two unsigned arguments:
template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1,
                                                                     Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_unsigned_number<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1,
                                                                     Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_unsigned_number<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1,
                                                                     Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_trivial_montgomery_int<
                                              Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_trivial_montgomery_int<
                                              Backend<MinBits1, MaxBits1, SignType1,
                                                      Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_unsigned_number<
                                              Backend<MinBits1, MaxBits1, SignType1,
                                                      Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_unsigned_number<
                                              Backend<MinBits1, MaxBits1, SignType1,
                                                      Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_equal_montgomery_int_backend<Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

    ::type eval_add(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                    const Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>&          o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   *result.limbs() = detail::checked_add(*result.limbs(), *o.limbs(),
                                         typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                                                         Allocator1, ParamsBackend1>::checked_type());

   result.normalize();
   eval_divide(result, result.m_params.p().backend());
}

// signed subtraction:
template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
inline typename enable_if_c<is_trivial_montgomery_int<
                                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                            is_trivial_montgomery_int<
                                montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                                       Allocator1, ParamsBackend1> >::value &&
                            (is_signed_number<
                                 montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value ||
                             is_signed_number<
                                 montgomery_int_backend<
                                     MinBits1, MaxBits1,
                                     SignType1, Checked1,
                                     Allocator1, ParamsBackend1> >::value) &&
                            is_trivial_montgomery_int<
                                Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                            is_trivial_montgomery_int<
                                Backend<MinBits1, MaxBits1, SignType1, Checked1,
                                        Allocator1, ParamsBackend1> >::value &&
                            (is_signed_number<
                                 Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value ||
                             is_signed_number<
                                 Backend<
                                     MinBits1, MaxBits1,
                                     SignType1, Checked1,
                                     Allocator1, ParamsBackend1> >::value) &&
                            is_equal_montgomery_int_backend<Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

    ::type eval_subtract(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                         const Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>&          o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   if (result.sign() != o.sign())
   {
      *result.limbs() = detail::checked_add(*result.limbs(), *o.limbs(),
                                            typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                                                            Allocator1, ParamsBackend1>::checked_type());
   }
   else if (*result.limbs() < *o.limbs())
   {
      *result.limbs() = detail::checked_subtract(*o.limbs(), *result.limbs(),
                                                 typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                                                                 Allocator1, ParamsBackend1>::checked_type());

      result.negate();
   }
   else
   {
      *result.limbs() = detail::checked_subtract(*result.limbs(), *o.limbs(),
                                                 typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                                                                 Allocator1, ParamsBackend1>::checked_type());
   }

   result.normalize();
   eval_divide(result, result.m_params.p().backend());
}

template <template <unsigned, unsigned, cpp_integer_type, cpp_int_check_type, typename, typename> class Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1,
                                                                     Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_unsigned_number<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1,
                                                                     Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_unsigned_number<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1,
                                                                     Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_trivial_montgomery_int<
                                              Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_trivial_montgomery_int<
                                              Backend<MinBits1, MaxBits1, SignType1,
                                                      Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_unsigned_number<
                                              Backend<MinBits1, MaxBits1, SignType1,
                                                      Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_unsigned_number<
                                              Backend<MinBits1, MaxBits1, SignType1,
                                                      Checked1, Allocator1, ParamsBackend1> >::value &&
                                          is_equal_montgomery_int_backend<Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value>

    ::type eval_subtract(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                         const Backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>&          o)

        BOOST_MP_NOEXCEPT_IF((is_non_throwing_cpp_int<
                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value))
{
   *result.limbs() = detail::checked_subtract(*result.limbs(), *o.limbs(),
                                              typename montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1,
                                                                              Allocator1, ParamsBackend1>::checked_type());

   result.normalize();
   eval_divide(result, result.m_params.p().backend());
}

template <typename Backend,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          !is_trivial_montgomery_int<
                                              Backend>::value>

    ::type eval_subtract(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
                         const Backend&                                                                               o)
{
   int x = 0;
}

template <typename Backend1, typename Backend2,
          unsigned MinBits1, unsigned MaxBits1, cpp_integer_type SignType1, cpp_int_check_type Checked1,
          class Allocator1, typename ParamsBackend1>
BOOST_MP_FORCEINLINE typename enable_if_c<!is_trivial_montgomery_int<
                                              montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1> >::value &&
                                          !is_trivial_montgomery_int<
                                              Backend1>::value>::type
eval_subtract(montgomery_int_backend<MinBits1, MaxBits1, SignType1, Checked1, Allocator1, ParamsBackend1>& result,
              const Backend1& a, const Backend2& b)
{
   int x = 0;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

}
}
} // namespace boost::multiprecision::backends

#endif