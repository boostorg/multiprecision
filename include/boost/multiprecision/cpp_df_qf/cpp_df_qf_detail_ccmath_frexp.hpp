///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2023.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_FREXP_2023_01_07_HPP
#define BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_FREXP_2023_01_07_HPP

#include <cstdint>

#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_fabs.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_isinf.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_isnan.hpp>
#include <boost/multiprecision/cpp_df_qf/cpp_df_qf_detail_ccmath_limits.hpp>

namespace boost { namespace multiprecision { namespace backends { namespace cpp_df_qf_detail { namespace ccmath {

namespace detail
{

template <typename Real>
inline constexpr Real frexp_zero_impl(Real arg, int* exp)
{
   if (exp != nullptr)
   {
      *exp = 0;
   }

   return arg;
}

template <typename Real>
inline constexpr Real frexp_impl(Real arg, int* expptr)
{
   Real f = arg;
   int e2 = 0;
   constexpr Real two_pow_n_big = Real(UINT32_C(65536));

   while (f >= two_pow_n_big)
   {
       f = f / two_pow_n_big;
       e2 += 16;
   }

   constexpr Real two_pow_8 = Real(UINT16_C(256));

   while (f >= two_pow_8)
   {
       f = f / two_pow_8;
       e2 += 8;
   }

   while(f >= Real(1))
   {
      f = f / Real(2);
      ++e2;
   }
   
   if(expptr != nullptr)
   {
      *expptr = e2;
   }

   return f;
}

template <typename Real>
inline constexpr Real frexp_impl_lt_half(Real arg, int* expptr)
{
   Real f = arg;
   int e2 = 0;
   constexpr Real two_pow_n_big = Real(UINT32_C(65536));

   while (f < Real(0.00000762939453125L))
   {
      f = f * two_pow_n_big;
      e2 -= 16;
   }

   constexpr Real two_pow_8 = Real(UINT16_C(256));

   while (f < 0.001953125L)
   {
      f = f * two_pow_8;
      e2 -= 8;
   }

   while(f < Real(0.5))
   {
      f = f * Real(2);
      --e2;
   }
   
   if(expptr != nullptr)
   {
      *expptr = e2;
   }

   return f;
}

} // namespace detail

template <typename Real>
inline constexpr Real frexp(Real arg, int* expptr)
{
   if (   (arg == Real(0))
       || (arg == Real(-0))
       || cpp_df_qf_detail::ccmath::isinf(arg)
       || cpp_df_qf_detail::ccmath::isnan(arg))
   {
      return cpp_df_qf_detail::ccmath::detail::frexp_zero_impl(arg, expptr);
   }
   else
   {
      const auto negative_arg = (arg < Real(0));

      const Real f = (negative_arg ? -arg : arg);

      Real result { };

      if(arg >= Real(1.0))
      {
         result = cpp_df_qf_detail::ccmath::detail::frexp_impl(f, expptr);
      }
      else if(f < Real(0.5))
      {
         result = cpp_df_qf_detail::ccmath::detail::frexp_impl_lt_half(f, expptr);
      }
      else
      {
         result = cpp_df_qf_detail::ccmath::detail::frexp_zero_impl(f, expptr);
      }

      return ((!negative_arg) ? result : -result);
   }
}

} } } } } // namespace boost::multiprecision::backends::cpp_df_qf_detail::ccmath

#endif // BOOST_MP_CPP_DF_QF_DETAIL_CCMATH_FREXP_2023_01_07_HPP
