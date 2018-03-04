///////////////////////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_BN_MPC_HPP
#define BOOST_MATH_BN_MPC_HPP

#include <boost/multiprecision/number.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/cstdint.hpp>
#include <boost/multiprecision/detail/big_lanczos.hpp>
#include <boost/multiprecision/detail/digits.hpp>
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/logged_adaptor.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/functional/hash_fwd.hpp>
#include <mpc.h>
#include <cmath>
#include <algorithm>

#ifndef BOOST_MULTIPRECISION_MPFI_DEFAULT_PRECISION
#  define BOOST_MULTIPRECISION_MPFI_DEFAULT_PRECISION 20
#endif

namespace boost{
namespace multiprecision{
namespace backends{

template <unsigned digits10>
struct mpc_float_backend;

} // namespace backends

template <unsigned digits10>
struct number_category<backends::mpc_float_backend<digits10> > : public mpl::int_<number_kind_complex>{};

namespace backends{

namespace detail{

template <unsigned digits10>
struct mpc_float_imp
{
#ifdef BOOST_HAS_LONG_LONG
   typedef mpl::list<long, boost::long_long_type>                     signed_types;
   typedef mpl::list<unsigned long, boost::ulong_long_type>   unsigned_types;
#else
   typedef mpl::list<long>                                signed_types;
   typedef mpl::list<unsigned long>                       unsigned_types;
#endif
   typedef mpl::list<double, long double>                 float_types;
   typedef long                                           exponent_type;

   mpc_float_imp()
   {
      mpc_init2(m_data, multiprecision::detail::digits10_2_2(digits10 ? digits10 : get_default_precision()));
      mpc_set_ui(m_data, 0u, GMP_RNDN);
   }
   mpc_float_imp(unsigned prec)
   {
      mpc_init2(m_data, prec);
      mpc_set_ui(m_data, 0u, GMP_RNDN);
   }

   mpc_float_imp(const mpc_float_imp& o)
   {
      mpc_init2(m_data, multiprecision::detail::digits10_2_2(digits10 ? digits10 : get_default_precision()));
      if(o.m_data[0].re[0]._mpfr_d)
         mpc_set(m_data, o.m_data, GMP_RNDN);
   }
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   mpc_float_imp(mpc_float_imp&& o) BOOST_NOEXCEPT
   {
      m_data[0] = o.m_data[0];
      o.m_data[0].re[0]._mpfr_d = 0;
   }
#endif
   mpc_float_imp& operator = (const mpc_float_imp& o)
   {
      if(m_data[0].re[0]._mpfr_d == 0)
         mpc_init2(m_data, multiprecision::detail::digits10_2_2(digits10 ? digits10 : get_default_precision()));
      if(o.m_data[0].re[0]._mpfr_d)
         mpc_set(m_data, o.m_data, GMP_RNDD);
      return *this;
   }
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   mpc_float_imp& operator = (mpc_float_imp&& o) BOOST_NOEXCEPT
   {
      mpc_swap(m_data, o.m_data);
      return *this;
   }
#endif
#ifdef BOOST_HAS_LONG_LONG
#ifdef _MPFR_H_HAVE_INTMAX_T
   mpc_float_imp& operator = (boost::ulong_long_type i)
   {
      if(m_data[0].re[0]._mpfr_d == 0)
         mpc_init2(m_data, multiprecision::detail::digits10_2_2(digits10 ? digits10 : get_default_precision()));
      mpc_set_uj(data(), i, GMP_RNDD);
      return *this;
   }
   mpc_float_imp& operator = (boost::long_long_type i)
   {
      if(m_data[0].re[0]._mpfr_d == 0)
         mpc_init2(m_data, multiprecision::detail::digits10_2_2(digits10 ? digits10 : get_default_precision()));
      mpc_set_sj(data(), i, GMP_RNDD);
      return *this;
   }
#else
   mpc_float_imp& operator = (boost::ulong_long_type i)
   {
      if(m_data[0].re[0]._mpfr_d == 0)
         mpc_init2(m_data, multiprecision::detail::digits10_2_2(digits10 ? digits10 : get_default_precision()));
      boost::ulong_long_type mask = ((((1uLL << (std::numeric_limits<unsigned long>::digits - 1)) - 1) << 1) | 1u);
      unsigned shift = 0;
      mpc_t t;
      mpc_init2(t, (std::max)(static_cast<unsigned long>(std::numeric_limits<boost::ulong_long_type>::digits), static_cast<unsigned long>(multiprecision::detail::digits10_2_2(digits10))));
      mpc_set_ui(m_data, 0, GMP_RNDN);
      while(i)
      {
         mpc_set_ui(t, static_cast<unsigned long>(i & mask), GMP_RNDN);
         if(shift)
            mpc_mul_2exp(t, t, shift);
         mpc_add(m_data, m_data, t);
         shift += std::numeric_limits<unsigned long>::digits;
         i >>= std::numeric_limits<unsigned long>::digits;
      }
      mpc_clear(t);
      return *this;
   }
   mpc_float_imp& operator = (boost::long_long_type i)
   {
      if(m_data[0].re[0]._mpfr_d == 0)
         mpc_init2(m_data, multiprecision::detail::digits10_2_2(digits10 ? digits10 : get_default_precision()));
      bool neg = i < 0;
      *this = boost::multiprecision::detail::unsigned_abs(i);
      if(neg)
         mpc_neg(m_data, m_data, GMP_RNDD);
      return *this;
   }
#endif
#endif
   mpc_float_imp& operator = (unsigned long i)
   {
      if(m_data[0].re[0]._mpfr_d == 0)
         mpc_init2(m_data, multiprecision::detail::digits10_2_2(digits10 ? digits10 : get_default_precision()));
      mpc_set_ui(m_data, i, GMP_RNDN);
      return *this;
   }
   mpc_float_imp& operator = (long i)
   {
      if(m_data[0].re[0]._mpfr_d == 0)
         mpc_init2(m_data, multiprecision::detail::digits10_2_2(digits10 ? digits10 : get_default_precision()));
      mpc_set_si(m_data, i, GMP_RNDN);
      return *this;
   }
   mpc_float_imp& operator = (double d)
   {
      if(m_data[0].re[0]._mpfr_d == 0)
         mpc_init2(m_data, multiprecision::detail::digits10_2_2(digits10 ? digits10 : get_default_precision()));
      mpc_set_d(m_data, d, GMP_RNDN);
      return *this;
   }
   mpc_float_imp& operator = (long double a)
   {
      if(m_data[0].re[0]._mpfr_d == 0)
         mpc_init2(m_data, multiprecision::detail::digits10_2_2(digits10 ? digits10 : get_default_precision()));
      mpfr_set_ld(left_data(), a, GMP_RNDD);
      mpfr_set_ld(right_data(), a, GMP_RNDU);
      return *this;
   }
   mpc_float_imp& operator = (const char* s)
   {
      using default_ops::eval_fpclassify;
   
      if(m_data[0].re[0]._mpfr_d == 0)
         mpc_init2(m_data, multiprecision::detail::digits10_2_2(digits10 ? digits10 : get_default_precision()));

      if(s && (*s == '('))
      {
         mpfr_float_backend<digits10> a, b;
         std::string part;
         const char* p = ++s;
         while(*p && (*p != ',') && (*p != ')'))
            ++p;
         part.assign(s + 1, p);
         a = part.c_str();
         s = p;
         if(*p && (*p != '}'))
         {
            ++p;
            while(*p && (*p != ',') && (*p != ')'))
               ++p;
            part.assign(s + 1, p);
         }
         else
            part.erase();
         b = part.c_str();

         if(eval_fpclassify(a) == (int)FP_NAN)
         {
            mpc_set_fr(this->data(), a.data(), GMP_RNDN);
         }
         else if(eval_fpclassify(b) == (int)FP_NAN)
         {
            mpc_set_fr(this->data(), b.data(), GMP_RNDN);
         }
         else
         {
            mpc_set_fr_fr(m_data, a.data(), b.data(), GMP_RNDN);
         }
      }
      else if(mpc_set_str(m_data, s, 10, GMP_RNDN) != 0)
      {
         BOOST_THROW_EXCEPTION(std::runtime_error(std::string("Unable to parse string \"") + s + std::string("\"as a valid floating point number.")));
      }
      return *this;
   }
   void swap(mpc_float_imp& o) BOOST_NOEXCEPT
   {
      mpc_swap(m_data, o.m_data);
   }
   std::string str(std::streamsize digits, std::ios_base::fmtflags f)const
   {
      BOOST_ASSERT(m_data[0].re[0]._mpfr_d);

      mpfr_float_backend<digits10> a, b;

      mpc_real(a.data(), m_data, GMP_RNDD);
      mpc_imag(b.data(), m_data, GMP_RNDD);

      if(eval_is_zero(b))
         return a.str(digits, f);

      return "(" + a.str(digits, f) + "," + b.str(digits, f) + ")";
   }
   ~mpc_float_imp() BOOST_NOEXCEPT
   {
      if(m_data[0].re[0]._mpfr_d)
         mpc_clear(m_data);
   }
   void negate() BOOST_NOEXCEPT
   {
      BOOST_ASSERT(m_data[0].re[0]._mpfr_d);
      mpc_neg(m_data, m_data, GMP_RNDD);
   }
   int compare(const mpc_float_imp& o)const BOOST_NOEXCEPT
   {
      BOOST_ASSERT(m_data[0].re[0]._mpfr_d && o.m_data[0].re[0]._mpfr_d);
      return mpc_cmp(m_data, o.m_data);
   }
   template <class V>
   int compare(V v)const BOOST_NOEXCEPT
   {
      mpc_float_imp d;
      d = v;
      return compare(d);
   }
   mpc_t& data() BOOST_NOEXCEPT
   {
      BOOST_ASSERT(m_data[0].re[0]._mpfr_d);
      return m_data;
   }
   const mpc_t& data()const BOOST_NOEXCEPT
   {
      BOOST_ASSERT(m_data[0].re[0]._mpfr_d);
      return m_data;
   }
protected:
   mpc_t m_data;
   static unsigned& get_default_precision() BOOST_NOEXCEPT
   {
      static unsigned val = BOOST_MULTIPRECISION_MPFI_DEFAULT_PRECISION;
      return val;
   }
};

} // namespace detail

template <unsigned digits10>
struct mpc_float_backend : public detail::mpc_float_imp<digits10>
{
   mpc_float_backend() : detail::mpc_float_imp<digits10>() {}
   mpc_float_backend(const mpc_float_backend& o) : detail::mpc_float_imp<digits10>(o) {}
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   mpc_float_backend(mpc_float_backend&& o) : detail::mpc_float_imp<digits10>(static_cast<detail::mpc_float_imp<digits10>&&>(o)) {}
#endif
   template <unsigned D>
   mpc_float_backend(const mpc_float_backend<D>& val, typename enable_if_c<D <= digits10>::type* = 0)
       : detail::mpc_float_imp<digits10>()
   {
      mpc_set(this->m_data, val.data());
   }
   template <unsigned D>
   explicit mpc_float_backend(const mpc_float_backend<D>& val, typename disable_if_c<D <= digits10>::type* = 0)
       : detail::mpc_float_imp<digits10>()
   {
      mpc_set(this->m_data, val.data());
   }
   mpc_float_backend(const mpc_t val)
       : detail::mpc_float_imp<digits10>()
   {
      mpc_set(this->m_data, val);
   }
   mpc_float_backend& operator=(const mpc_float_backend& o)
   {
      *static_cast<detail::mpc_float_imp<digits10>*>(this) = static_cast<detail::mpc_float_imp<digits10> const&>(o);
      return *this;
   }
   template <unsigned D>
   mpc_float_backend(const mpfr_float_backend<D>& val, typename enable_if_c<D <= digits10>::type* = 0)
       : detail::mpc_float_imp<digits10>()
   {
      mpc_set_fr(this->m_data, val.data(), GMP_RNDN);
   }
   template <unsigned D>
   mpc_float_backend& operator=(const mpfr_float_backend<D>& val)
   {
      mpc_set_fr(this->m_data, val.data(), GMP_RNDN);
      return *this;
   }
   template <unsigned D>
   explicit mpc_float_backend(const mpfr_float_backend<D>& val, typename disable_if_c<D <= digits10>::type* = 0)
       : detail::mpc_float_imp<digits10>()
   {
      mpc_set_fr(this->m_data, val.data(), GMP_RNDN);
   }
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   mpc_float_backend& operator=(mpc_float_backend&& o) BOOST_NOEXCEPT
   {
      *static_cast<detail::mpc_float_imp<digits10>*>(this) = static_cast<detail::mpc_float_imp<digits10>&&>(o);
      return *this;
   }
#endif
   template <class V>
   mpc_float_backend& operator=(const V& v)
   {
      *static_cast<detail::mpc_float_imp<digits10>*>(this) = v;
      return *this;
   }
   mpc_float_backend& operator=(const mpc_t val)
   {
      mpc_set(this->m_data, val);
      return *this;
   }
   // We don't change our precision here, this is a fixed precision type:
   template <unsigned D>
   mpc_float_backend& operator=(const mpc_float_backend<D>& val)
   {
      mpc_set(this->m_data, val.data());
      return *this;
   }
};

template <>
struct mpc_float_backend<0> : public detail::mpc_float_imp<0>
{
   mpc_float_backend() : detail::mpc_float_imp<0>() {}
   mpc_float_backend(const mpc_t val)
      : detail::mpc_float_imp<0>(mpc_get_prec(val))
   {
      mpc_set(this->m_data, val, GMP_RNDN);
   }
   mpc_float_backend(const mpc_float_backend& o) : detail::mpc_float_imp<0>(o) {}
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   mpc_float_backend(mpc_float_backend&& o) BOOST_NOEXCEPT : detail::mpc_float_imp<0>(static_cast<detail::mpc_float_imp<0>&&>(o)) {}
#endif
   mpc_float_backend(const mpc_float_backend& o, unsigned digits10)
      : detail::mpc_float_imp<0>(digits10)
   {
      *this = o;
   }
   template <unsigned D>
   mpc_float_backend(const mpc_float_backend<D>& val)
      : detail::mpc_float_imp<0>(mpc_get_prec(val.data()))
   {
      mpc_set(this->m_data, val.data());
   }
   mpc_float_backend& operator=(const mpc_float_backend& o)
   {
      mpc_set_prec(this->m_data, mpc_get_prec(o.data()));
      mpc_set(this->m_data, o.data(), GMP_RNDN);
      return *this;
   }
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   mpc_float_backend& operator=(mpc_float_backend&& o) BOOST_NOEXCEPT
   {
      *static_cast<detail::mpc_float_imp<0>*>(this) = static_cast<detail::mpc_float_imp<0> &&>(o);
      return *this;
   }
#endif
   template <class V>
   mpc_float_backend& operator=(const V& v)
   {
      *static_cast<detail::mpc_float_imp<0>*>(this) = v;
      return *this;
   }
   mpc_float_backend& operator=(const mpc_t val)
   {
      mpc_set_prec(this->m_data, mpc_get_prec(val));
      mpc_set(this->m_data, val, GMP_RNDN);
      return *this;
   }
   template <unsigned D>
   mpc_float_backend& operator=(const mpc_float_backend<D>& val)
   {
      mpc_set_prec(this->m_data, mpc_get_prec(val.data()));
      mpc_set(this->m_data, val.data());
      return *this;
   }
   static unsigned default_precision() BOOST_NOEXCEPT
   {
      return get_default_precision();
   }
   static void default_precision(unsigned v) BOOST_NOEXCEPT
   {
      get_default_precision() = v;
   }
   unsigned precision()const BOOST_NOEXCEPT
   {
      return multiprecision::detail::digits2_2_10(mpc_get_prec(this->m_data));
   }
   void precision(unsigned digits10) BOOST_NOEXCEPT
   {
      mpc_set_prec(this->m_data, multiprecision::detail::digits2_2_10((digits10)));
   }
};

template <unsigned digits10, class T>
inline typename enable_if<is_arithmetic<T>, bool>::type eval_eq(const mpc_float_backend<digits10>& a, const T& b) BOOST_NOEXCEPT
{
   return a.compare(b) == 0;
}
template <unsigned digits10, class T>
inline typename enable_if<is_arithmetic<T>, bool>::type eval_lt(const mpc_float_backend<digits10>& a, const T& b) BOOST_NOEXCEPT
{
   return a.compare(b) < 0;
}
template <unsigned digits10, class T>
inline typename enable_if<is_arithmetic<T>, bool>::type eval_gt(const mpc_float_backend<digits10>& a, const T& b) BOOST_NOEXCEPT
{
   return a.compare(b) > 0;
}

template <unsigned D1, unsigned D2>
inline void eval_add(mpc_float_backend<D1>& result, const mpc_float_backend<D2>& o)
{
   mpc_add(result.data(), result.data(), o.data(), GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_subtract(mpc_float_backend<D1>& result, const mpc_float_backend<D2>& o)
{
   mpc_sub(result.data(), result.data(), o.data(), GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_multiply(mpc_float_backend<D1>& result, const mpc_float_backend<D2>& o)
{
   if((void*)&result == (void*)&o)
      mpc_sqr(result.data(), o.data(), GMP_RNDN);
   else
      mpc_mul(result.data(), result.data(), o.data(), GMP_RNDN);
}
template <unsigned D1, unsigned D2>
inline void eval_divide(mpc_float_backend<D1>& result, const mpc_float_backend<D2>& o)
{
   mpc_div(result.data(), result.data(), o.data(), GMP_RNDD);
}
template <unsigned digits10>
inline void eval_add(mpc_float_backend<digits10>& result, unsigned long i)
{
   mpc_add_ui(result.data(), result.data(), i, GMP_RNDN);
}
template <unsigned digits10>
inline void eval_subtract(mpc_float_backend<digits10>& result, unsigned long i)
{
   mpc_sub_ui(result.data(), result.data(), i, GMP_RNDN);
}
template <unsigned digits10>
inline void eval_multiply(mpc_float_backend<digits10>& result, unsigned long i)
{
   mpc_mul_ui(result.data(), result.data(), i, GMP_RNDN);
}
template <unsigned digits10>
inline void eval_divide(mpc_float_backend<digits10>& result, unsigned long i)
{
   mpc_div_ui(result.data(), result.data(), i, GMP_RNDN);
}
template <unsigned digits10>
inline void eval_add(mpc_float_backend<digits10>& result, long i)
{
   if(i > 0)
      mpc_add_ui(result.data(), result.data(), i, GMP_RNDN);
   else
      mpc_sub_ui(result.data(), result.data(), boost::multiprecision::detail::unsigned_abs(i), GMP_RNDN);
}
template <unsigned digits10>
inline void eval_subtract(mpc_float_backend<digits10>& result, long i)
{
   if(i > 0)
      mpc_sub_ui(result.data(), result.data(), i, GMP_RNDN);
   else
      mpc_add_ui(result.data(), result.data(), boost::multiprecision::detail::unsigned_abs(i), GMP_RNDN);
}
template <unsigned digits10>
inline void eval_multiply(mpc_float_backend<digits10>& result, long i)
{
   mpc_mul_ui(result.data(), result.data(), boost::multiprecision::detail::unsigned_abs(i), GMP_RNDN);
   if(i < 0)
      mpc_neg(result.data(), result.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void eval_divide(mpc_float_backend<digits10>& result, long i)
{
   mpc_div_ui(result.data(), result.data(), boost::multiprecision::detail::unsigned_abs(i), GMP_RNDN);
   if(i < 0)
      mpc_neg(result.data(), result.data(), GMP_RNDN);
}
//
// Specialised 3 arg versions of the basic operators:
//
template <unsigned D1, unsigned D2, unsigned D3>
inline void eval_add(mpc_float_backend<D1>& a, const mpc_float_backend<D2>& x, const mpc_float_backend<D3>& y)
{
   mpc_add(a.data(), x.data(), y.data(), GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_add(mpc_float_backend<D1>& a, const mpc_float_backend<D2>& x, unsigned long y)
{
   mpc_add_ui(a.data(), x.data(), y, GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_add(mpc_float_backend<D1>& a, const mpc_float_backend<D2>& x, long y)
{
   if(y < 0)
      mpc_sub_ui(a.data(), x.data(), boost::multiprecision::detail::unsigned_abs(y), GMP_RNDD);
   else
      mpc_add_ui(a.data(), x.data(), y, GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_add(mpc_float_backend<D1>& a, unsigned long x, const mpc_float_backend<D2>& y)
{
   mpc_add_ui(a.data(), y.data(), x, GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_add(mpc_float_backend<D1>& a, long x, const mpc_float_backend<D2>& y)
{
   if(x < 0)
   {
      mpc_ui_sub(a.data(), boost::multiprecision::detail::unsigned_abs(x), y.data(), GMP_RNDN);
      mpc_neg(a.data(), a.data(), GMP_RNDD);
   }
   else
      mpc_add_ui(a.data(), y.data(), x, GMP_RNDD);
}
template <unsigned D1, unsigned D2, unsigned D3>
inline void eval_subtract(mpc_float_backend<D1>& a, const mpc_float_backend<D2>& x, const mpc_float_backend<D3>& y)
{
   mpc_sub(a.data(), x.data(), y.data(), GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_subtract(mpc_float_backend<D1>& a, const mpc_float_backend<D2>& x, unsigned long y)
{
   mpc_sub_ui(a.data(), x.data(), y, GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_subtract(mpc_float_backend<D1>& a, const mpc_float_backend<D2>& x, long y)
{
   if(y < 0)
      mpc_add_ui(a.data(), x.data(), boost::multiprecision::detail::unsigned_abs(y), GMP_RNDD);
   else
      mpc_sub_ui(a.data(), x.data(), y, GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_subtract(mpc_float_backend<D1>& a, unsigned long x, const mpc_float_backend<D2>& y)
{
   mpc_ui_sub(a.data(), x, y.data(), GMP_RNDN);
}
template <unsigned D1, unsigned D2>
inline void eval_subtract(mpc_float_backend<D1>& a, long x, const mpc_float_backend<D2>& y)
{
   if(x < 0)
   {
      mpc_add_ui(a.data(), y.data(), boost::multiprecision::detail::unsigned_abs(x), GMP_RNDD);
      mpc_neg(a.data(), a.data(), GMP_RNDD);
   }
   else
      mpc_ui_sub(a.data(), x, y.data(), GMP_RNDN);
}

template <unsigned D1, unsigned D2, unsigned D3>
inline void eval_multiply(mpc_float_backend<D1>& a, const mpc_float_backend<D2>& x, const mpc_float_backend<D3>& y)
{
   if((void*)&x == (void*)&y)
      mpc_sqr(a.data(), x.data(), GMP_RNDD);
   else
      mpc_mul(a.data(), x.data(), y.data(), GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_multiply(mpc_float_backend<D1>& a, const mpc_float_backend<D2>& x, unsigned long y)
{
   mpc_mul_ui(a.data(), x.data(), y, GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_multiply(mpc_float_backend<D1>& a, const mpc_float_backend<D2>& x, long y)
{
   if(y < 0)
   {
      mpc_mul_ui(a.data(), x.data(), boost::multiprecision::detail::unsigned_abs(y), GMP_RNDD);
      a.negate();
   }
   else
      mpc_mul_ui(a.data(), x.data(), y, GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_multiply(mpc_float_backend<D1>& a, unsigned long x, const mpc_float_backend<D2>& y)
{
   mpc_mul_ui(a.data(), y.data(), x, GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_multiply(mpc_float_backend<D1>& a, long x, const mpc_float_backend<D2>& y)
{
   if(x < 0)
   {
      mpc_mul_ui(a.data(), y.data(), boost::multiprecision::detail::unsigned_abs(x), GMP_RNDD);
      mpc_neg(a.data(), a.data(), GMP_RNDD);
   }
   else
      mpc_mul_ui(a.data(), y.data(), x, GMP_RNDD);
}

template <unsigned D1, unsigned D2, unsigned D3>
inline void eval_divide(mpc_float_backend<D1>& a, const mpc_float_backend<D2>& x, const mpc_float_backend<D3>& y)
{
   mpc_div(a.data(), x.data(), y.data(), GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_divide(mpc_float_backend<D1>& a, const mpc_float_backend<D2>& x, unsigned long y)
{
   mpc_div_ui(a.data(), x.data(), y, GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_divide(mpc_float_backend<D1>& a, const mpc_float_backend<D2>& x, long y)
{
   if(y < 0)
   {
      mpc_div_ui(a.data(), x.data(), boost::multiprecision::detail::unsigned_abs(y), GMP_RNDD);
      a.negate();
   }
   else
      mpc_div_ui(a.data(), x.data(), y, GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_divide(mpc_float_backend<D1>& a, unsigned long x, const mpc_float_backend<D2>& y)
{
   mpc_ui_div(a.data(), x, y.data(), GMP_RNDD);
}
template <unsigned D1, unsigned D2>
inline void eval_divide(mpc_float_backend<D1>& a, long x, const mpc_float_backend<D2>& y)
{
   if(x < 0)
   {
      mpc_ui_div(a.data(), boost::multiprecision::detail::unsigned_abs(x), y.data(), GMP_RNDD);
      mpc_neg(a.data(), a.data(), GMP_RNDD);
   }
   else
      mpc_ui_div(a.data(), x, y.data(), GMP_RNDD);
}

template <unsigned digits10>
inline bool eval_is_zero(const mpc_float_backend<digits10>& val) BOOST_NOEXCEPT
{
   return (0 != mpfr_zero_p(mpc_realref(val.data()))) && (0 != mpfr_zero_p(mpc_imagref(val.data())));
}
template <unsigned digits10>
inline int eval_get_sign(const mpc_float_backend<digits10>& val)
{
   BOOST_STATIC_ASSERT_MSG(digits10 == UINT_MAX, "Complex numbers have no sign bit."); // designed to always fail
   return 0;
}

template <unsigned digits10>
inline void eval_convert_to(unsigned long* result, const mpc_float_backend<digits10>& val)
{
   if (0 == mpfr_zero_p(mpc_imagref(val.data())))
   {
      BOOST_THROW_EXCEPTION(std::runtime_error("Could not convert imaginary number to scalar."));
   }
   mpfr_float_backend<digits10> t;
   mpc_real(t.data(), val.data(), GMP_RNDN);
   eval_convert_to(result, t);
}
template <unsigned digits10>
inline void eval_convert_to(long* result, const mpc_float_backend<digits10>& val)
{
   if (0 == mpfr_zero_p(mpc_imagref(val.data())))
   {
      BOOST_THROW_EXCEPTION(std::runtime_error("Could not convert imaginary number to scalar."));
   }
   mpfr_float_backend<digits10> t;
   mpc_real(t.data(), val.data(), GMP_RNDN);
   eval_convert_to(result, t);
}
#ifdef _MPFR_H_HAVE_INTMAX_T
template <unsigned digits10>
inline void eval_convert_to(boost::ulong_long_type* result, const mpc_float_backend<digits10>& val)
{
   if (0 == mpfr_zero_p(mpc_imagref(val.data())))
   {
      BOOST_THROW_EXCEPTION(std::runtime_error("Could not convert imaginary number to scalar."));
   }
   mpfr_float_backend<digits10> t;
   mpc_real(t.data(), val.data(), GMP_RNDN);
   eval_convert_to(result, t);
}
template <unsigned digits10>
inline void eval_convert_to(boost::long_long_type* result, const mpc_float_backend<digits10>& val)
{
   if (0 == mpfr_zero_p(mpc_imagref(val.data())))
   {
      BOOST_THROW_EXCEPTION(std::runtime_error("Could not convert imaginary number to scalar."));
   }
   mpfr_float_backend<digits10> t;
   mpc_real(t.data(), val.data(), GMP_RNDN);
   eval_convert_to(result, t);
}
#endif
template <unsigned digits10>
inline void eval_convert_to(double* result, const mpc_float_backend<digits10>& val) BOOST_NOEXCEPT
{
   if (0 == mpfr_zero_p(mpc_imagref(val.data())))
   {
      BOOST_THROW_EXCEPTION(std::runtime_error("Could not convert imaginary number to scalar."));
   }
   mpfr_float_backend<digits10> t;
   mpc_real(t.data(), val.data(), GMP_RNDN);
   eval_convert_to(result, t);
}
template <unsigned digits10>
inline void eval_convert_to(long double* result, const mpc_float_backend<digits10>& val) BOOST_NOEXCEPT
{
   if (0 == mpfr_zero_p(mpc_imagref(val.data())))
   {
      BOOST_THROW_EXCEPTION(std::runtime_error("Could not convert imaginary number to scalar."));
   }
   mpfr_float_backend<digits10> t;
   mpc_real(t.data(), val.data(), GMP_RNDN);
   eval_convert_to(result, t);
}

template <unsigned D1, unsigned D2, mpfr_allocation_type AllocationType>
inline void assign_components(mpc_float_backend<D1>& result, const mpfr_float_backend<D2, AllocationType>& a, const mpfr_float_backend<D2, AllocationType>& b)
{
   using default_ops::eval_fpclassify;
   if(eval_fpclassify(a) == (int)FP_NAN)
   {
      mpc_set_fr(result.data(), a.data(), GMP_RNDN);
   }
   else if(eval_fpclassify(b) == (int)FP_NAN)
   {
      mpc_set_fr(result.data(), b.data(), GMP_RNDN);
   }
   else
   {
      if(a.compare(b) > 0)
      {
         BOOST_THROW_EXCEPTION(std::runtime_error("Attempt to create interval with invalid range (start is greater than end)."));
      }
      mpc_interv_fr(result.data(), a.data(), b.data());
   }
}

template <unsigned Digits10, class V>
inline typename enable_if_c<is_convertible<V, number<mpfr_float_backend<Digits10, allocate_dynamic>, et_on> >::value >::type 
   assign_components(mpc_float_backend<Digits10>& result, const V& a, const V& b)
{
   number<mpfr_float_backend<Digits10, allocate_dynamic>, et_on> x(a), y(b);
   assign_components(result, x.backend(), y.backend());
}

//
// Native non-member operations:
//
template <unsigned Digits10>
inline void eval_sqrt(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& val)
{
   mpc_sqrt(result.data(), val.data());
}

template <unsigned Digits10>
inline void eval_abs(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& val)
{
   mpc_abs(result.data(), val.data());
}

template <unsigned Digits10>
inline void eval_fabs(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& val)
{
   mpc_abs(result.data(), val.data());
}
template <unsigned Digits10>
inline void eval_ceil(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& val)
{
   mpfr_float_backend<Digits10> a, b;
   mpfr_set(a.data(), val.left_data(), GMP_RNDN);
   mpfr_set(b.data(), val.right_data(), GMP_RNDN);
   eval_ceil(a, a);
   eval_ceil(b, b);
   if(a.compare(b) != 0)
   {
      BOOST_THROW_EXCEPTION(interval_error("Attempt to take the ceil of a value that straddles an integer boundary."));
   }
   mpc_set_fr(result.data(), a.data(), GMP_RNDN);
}
template <unsigned Digits10>
inline void eval_floor(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& val)
{
   mpfr_float_backend<Digits10> a, b;
   mpfr_set(a.data(), val.left_data(), GMP_RNDN);
   mpfr_set(b.data(), val.right_data(), GMP_RNDN);
   eval_floor(a, a);
   eval_floor(b, b);
   if(a.compare(b) != 0)
   {
      BOOST_THROW_EXCEPTION(interval_error("Attempt to take the floor of a value that straddles an integer boundary."));
   }
   mpc_set_fr(result.data(), a.data(), GMP_RNDN);
}
template <unsigned Digits10>
inline void eval_ldexp(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& val, long e)
{
   if(e > 0)
      mpc_mul_2exp(result.data(), val.data(), e);
   else if(e < 0)
      mpc_div_2exp(result.data(), val.data(), -e);
   else
      result = val;
}
template <unsigned Digits10>
inline void eval_frexp(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& val, int* e)
{
   mpfr_float_backend<Digits10> t, rt;
   mpc_mid(t.data(), val.data());
   eval_frexp(rt, t, e);
   eval_ldexp(result, val, -*e);
}
template <unsigned Digits10>
inline void eval_frexp(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& val, long* e)
{
   mpfr_float_backend<Digits10> t, rt;
   mpc_mid(t.data(), val.data());
   eval_frexp(rt, t, e);
   eval_ldexp(result, val, -*e);
}

template <unsigned Digits10>
inline int eval_fpclassify(const mpc_float_backend<Digits10>& val) BOOST_NOEXCEPT
{
   return mpc_inf_p(val.data()) ? FP_INFINITE : mpc_nan_p(val.data()) ? FP_NAN : mpc_is_zero(val.data()) ? FP_ZERO : FP_NORMAL;
}

template <unsigned Digits10>
inline void eval_pow(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& b, const mpc_float_backend<Digits10>& e)
{
   typedef typename boost::multiprecision::detail::canonical<unsigned, mpc_float_backend<Digits10> >::type ui_type;
   using default_ops::eval_get_sign;
   int s = eval_get_sign(b);
   if(s == 0)
   {
      if(eval_get_sign(e) == 0)
      {
         result = ui_type(1);
      }
      else
      {
         result = ui_type(0);
      }
      return;
   }
   if(s < 0)
   {
      if(eval_get_sign(e) < 0)
      {
         mpc_float_backend<Digits10> t1, t2;
         t1 = e;
         t1.negate();
         eval_pow(t2, b, t1);
         t1 = ui_type(1);
         eval_divide(result, t1, t2);
         return;
      }
      typename boost::multiprecision::detail::canonical<boost::uintmax_t, mpc_float_backend<Digits10> >::type an;
#ifndef BOOST_NO_EXCEPTIONS
      try
      {
#endif
         using default_ops::eval_convert_to;
         eval_convert_to(&an, e);
         if(e.compare(an) == 0)
         {
            mpc_float_backend<Digits10> pb(b);
            pb.negate();
            eval_pow(result, pb, e);
            if(an & 1u)
               result.negate();
            return;
         }
#ifndef BOOST_NO_EXCEPTIONS
      }
      catch(const std::exception&)
      {
         // conversion failed, just fall through, value is not an integer.
      }
#endif
      result = std::numeric_limits<number<mpc_float_backend<Digits10>, et_on> >::quiet_NaN().backend();
      return;
   }
   mpc_log(result.data(), b.data());
   mpc_mul(result.data(), result.data(), e.data());
   mpc_exp(result.data(), result.data());
}

template <unsigned Digits10>
inline void eval_exp(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_exp(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_exp2(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_exp2(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_log(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_log(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_log10(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_log10(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_sin(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_sin(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_cos(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_cos(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_tan(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_tan(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_asin(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_asin(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_acos(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_acos(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_atan(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_atan(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_atan2(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg1, const mpc_float_backend<Digits10>& arg2)
{
   mpc_atan2(result.data(), arg1.data(), arg2.data());
}

template <unsigned Digits10>
inline void eval_sinh(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_sinh(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_cosh(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_cosh(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_tanh(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_tanh(result.data(), arg.data());
}

template <unsigned Digits10>
inline void eval_log2(mpc_float_backend<Digits10>& result, const mpc_float_backend<Digits10>& arg)
{
   mpc_log2(result.data(), arg.data());
}

template <unsigned Digits10>
inline std::size_t hash_value(const mpc_float_backend<Digits10>& val)
{
   std::size_t result = 0;
   std::size_t len = val.left_data()[0]._mpfr_prec / mp_bits_per_limb;
   if(val.left_data()[0]._mpfr_prec % mp_bits_per_limb)
      ++len;
   for(std::size_t i = 0; i < len; ++i)
      boost::hash_combine(result, val.left_data()[0]._mpfr_d[i]);
   boost::hash_combine(result, val.left_data()[0]._mpfr_exp);
   boost::hash_combine(result, val.left_data()[0]._mpfr_sign);

   len = val.right_data()[0]._mpfr_prec / mp_bits_per_limb;
   if(val.right_data()[0]._mpfr_prec % mp_bits_per_limb)
      ++len;
   for(std::size_t i = 0; i < len; ++i)
      boost::hash_combine(result, val.right_data()[0]._mpfr_d[i]);
   boost::hash_combine(result, val.right_data()[0]._mpfr_exp);
   boost::hash_combine(result, val.right_data()[0]._mpfr_sign);
   return result;
}

template <class To, unsigned D>
void generic_interconvert(To& to, const mpc_float_backend<D>& from, const mpl::int_<number_kind_integer>& to_type, const mpl::int_<number_kind_floating_point>& from_type)
{
   using boost::multiprecision::detail::generic_interconvert;
   mpfr_float_backend<D> t;
   mpc_mid(t.data(), from.data());
   generic_interconvert(to, t, to_type, from_type);
}

template <class To, unsigned D>
void generic_interconvert(To& to, const mpc_float_backend<D>& from, const mpl::int_<number_kind_rational>& to_type, const mpl::int_<number_kind_floating_point>& from_type)
{
   using boost::multiprecision::detail::generic_interconvert;
   mpfr_float_backend<D> t;
   mpc_mid(t.data(), from.data());
   generic_interconvert(to, t, to_type, from_type);
}

template <class To, unsigned D>
void generic_interconvert(To& to, const mpc_float_backend<D>& from, const mpl::int_<number_kind_floating_point>& to_type, const mpl::int_<number_kind_floating_point>& from_type)
{
   using boost::multiprecision::detail::generic_interconvert;
   mpfr_float_backend<D> t;
   mpc_mid(t.data(), from.data());
   generic_interconvert(to, t, to_type, from_type);
}

} // namespace backends

#ifdef BOOST_NO_SFINAE_EXPR

namespace detail{

template<unsigned D1, unsigned D2>
struct is_explicitly_convertible<backends::mpc_float_backend<D1>, backends::mpc_float_backend<D2> > : public mpl::true_ {};

}
#endif

template<>
struct number_category<detail::canonical<mpc_t, backends::mpc_float_backend<0> >::type> : public mpl::int_<number_kind_floating_point>{};
template <unsigned Digits10>
struct is_interval_number<backends::mpc_float_backend<Digits10> > : public mpl::true_ {};

using boost::multiprecision::backends::mpc_float_backend;

typedef number<mpc_float_backend<50> >    mpc_float_50;
typedef number<mpc_float_backend<100> >   mpc_float_100;
typedef number<mpc_float_backend<500> >   mpc_float_500;
typedef number<mpc_float_backend<1000> >  mpc_float_1000;
typedef number<mpc_float_backend<0> >     mpc_float;

//
// Special interval specific functions:
//
template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline number<mpfr_float_backend<Digits10>, ExpressionTemplates> lower(const number<mpc_float_backend<Digits10>, ExpressionTemplates>& val)
{
   number<mpfr_float_backend<Digits10> > result;
   mpfr_set(result.backend().data(), val.backend().left_data(), GMP_RNDN);
   return result;
}

template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline number<mpfr_float_backend<Digits10>, ExpressionTemplates> upper(const number<mpc_float_backend<Digits10>, ExpressionTemplates>& val)
{
   number<mpfr_float_backend<Digits10> > result;
   mpfr_set(result.backend().data(), val.backend().right_data(), GMP_RNDN);
   return result;
}

template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline number<mpfr_float_backend<Digits10>, ExpressionTemplates> median(const number<mpc_float_backend<Digits10>, ExpressionTemplates>& val)
{
   number<mpfr_float_backend<Digits10> > result;
   mpc_mid(result.backend().data(), val.backend().data());
   return result;
}

template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline number<mpfr_float_backend<Digits10>, ExpressionTemplates> width(const number<mpc_float_backend<Digits10>, ExpressionTemplates>& val)
{
   number<mpfr_float_backend<Digits10> > result;
   mpc_diam_abs(result.backend().data(), val.backend().data());
   return result;
}

template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline number<mpc_float_backend<Digits10>, ExpressionTemplates> intersect(const number<mpc_float_backend<Digits10>, ExpressionTemplates>& a, const number<mpc_float_backend<Digits10>, ExpressionTemplates>&  b)
{
   number<mpc_float_backend<Digits10>, ExpressionTemplates> result;
   mpc_intersect(result.backend().data(), a.backend().data(), b.backend().data());
   return result;
}

template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline number<mpc_float_backend<Digits10>, ExpressionTemplates> hull(const number<mpc_float_backend<Digits10>, ExpressionTemplates>& a, const number<mpc_float_backend<Digits10>, ExpressionTemplates>&  b)
{
   number<mpc_float_backend<Digits10>, ExpressionTemplates> result;
   mpc_union(result.backend().data(), a.backend().data(), b.backend().data());
   return result;
}

template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline bool overlap(const number<mpc_float_backend<Digits10>, ExpressionTemplates>& a, const number<mpc_float_backend<Digits10>, ExpressionTemplates>&  b)
{
  return (lower(a) <= lower(b) && lower(b) <= upper(a)) ||
         (lower(b) <= lower(a) && lower(a) <= upper(b));
}

template <unsigned Digits10, expression_template_option ExpressionTemplates1, expression_template_option ExpressionTemplates2>
inline bool in(const number<mpfr_float_backend<Digits10>, ExpressionTemplates1>& a, const number<mpc_float_backend<Digits10>, ExpressionTemplates2>&  b)
{
  return mpc_is_inside_fr(a.backend().data(), b.backend().data()) != 0;
}

template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline bool zero_in(const number<mpc_float_backend<Digits10>, ExpressionTemplates>&  a)
{
  return mpc_has_zero(a.backend().data()) != 0;
}

template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline bool subset(const number<mpc_float_backend<Digits10>, ExpressionTemplates>& a, const number<mpc_float_backend<Digits10>, ExpressionTemplates>&  b)
{
  return mpc_is_inside(a.backend().data(), b.backend().data()) != 0;
}

template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline bool proper_subset(const number<mpc_float_backend<Digits10>, ExpressionTemplates>& a, const number<mpc_float_backend<Digits10>, ExpressionTemplates>&  b)
{
  return mpc_is_strictly_inside(a.backend().data(), b.backend().data()) != 0;
}

template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline bool empty(const number<mpc_float_backend<Digits10>, ExpressionTemplates>&  a)
{
  return mpc_is_empty(a.backend().data()) != 0;
}

template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline bool singleton(const number<mpc_float_backend<Digits10>, ExpressionTemplates>&  a)
{
  return mpfr_cmp(a.backend().left_data(), a.backend().right_data()) == 0;
}

template <unsigned Digits10, expression_template_option ExpressionTemplates>
struct component_type<number<mpc_float_backend<Digits10>, ExpressionTemplates> >
{
   typedef number<mpfr_float_backend<Digits10>, ExpressionTemplates> type;
};

//
// Overloaded special functions which call native mpfr routines:
//
template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> asinh BOOST_PREVENT_MACRO_SUBSTITUTION(const boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates>& arg)
{
   boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> result;
   mpc_asinh(result.backend().data(), arg.backend().data());
   return BOOST_MP_MOVE(result);
}
template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> acosh BOOST_PREVENT_MACRO_SUBSTITUTION(const boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates>& arg)
{
   boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> result;
   mpc_acosh(result.backend().data(), arg.backend().data());
   return BOOST_MP_MOVE(result);
}
template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> atanh BOOST_PREVENT_MACRO_SUBSTITUTION(const boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates>& arg)
{
   boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> result;
   mpc_atanh(result.backend().data(), arg.backend().data());
   return BOOST_MP_MOVE(result);
}
template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> cbrt BOOST_PREVENT_MACRO_SUBSTITUTION(const boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates>& arg)
{
   boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> result;
   mpc_cbrt(result.backend().data(), arg.backend().data());
   return BOOST_MP_MOVE(result);
}
template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> expm1 BOOST_PREVENT_MACRO_SUBSTITUTION(const boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates>& arg)
{
   boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> result;
   mpc_expm1(result.backend().data(), arg.backend().data());
   return BOOST_MP_MOVE(result);
}
template <unsigned Digits10, expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> log1p BOOST_PREVENT_MACRO_SUBSTITUTION(const boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates>& arg)
{
   boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> result;
   mpc_log1p(result.backend().data(), arg.backend().data());
   return BOOST_MP_MOVE(result);
}


} // namespace multiprecision

namespace math{

namespace tools{

template <>
inline int digits<boost::multiprecision::mpc_float>()
#ifdef BOOST_MATH_NOEXCEPT
   BOOST_NOEXCEPT
#endif
{
   return  multiprecision::detail::digits10_2_2(boost::multiprecision::mpc_float::default_precision());
}
template <>
inline int digits<boost::multiprecision::number<boost::multiprecision::mpc_float_backend<0>, boost::multiprecision::et_off> >()
#ifdef BOOST_MATH_NOEXCEPT
   BOOST_NOEXCEPT
#endif
{
   return  multiprecision::detail::digits10_2_2(boost::multiprecision::mpc_float::default_precision());
}


// mpfi gets used with logged_adaptor fairly often, so specialize for that use case as well:
typedef boost::multiprecision::number<boost::multiprecision::backends::logged_adaptor<boost::multiprecision::mpc_float::backend_type>, boost::multiprecision::et_on> logged_type1;
typedef boost::multiprecision::number<boost::multiprecision::backends::logged_adaptor<boost::multiprecision::mpc_float::backend_type>, boost::multiprecision::et_off> logged_type2;

template <>
inline int digits<logged_type1>()
#ifdef BOOST_MATH_NOEXCEPT
BOOST_NOEXCEPT
#endif
{
   return  multiprecision::detail::digits10_2_2(logged_type1::default_precision());
}
template <>
inline int digits<logged_type2 >()
#ifdef BOOST_MATH_NOEXCEPT
BOOST_NOEXCEPT
#endif
{
   return  multiprecision::detail::digits10_2_2(logged_type1::default_precision());
}

} // namespace tools

namespace constants{ namespace detail{

template <class T> struct constant_pi;
template <class T> struct constant_ln_two;
template <class T> struct constant_euler;
template <class T> struct constant_catalan;

//
// Initializer: ensure all our constants are initialized prior to the first call of main:
//
template <class T>
struct mpc_initializer
{
   struct init
   {
      init()
      {
         boost::math::constants::pi<T>();
         boost::math::constants::ln_two<T>();
         boost::math::constants::euler<T>();
         boost::math::constants::catalan<T>();
      }
      void force_instantiate()const{}
   };
   static const init initializer;
   static void force_instantiate()
   {
      initializer.force_instantiate();
   }
};

template <class T>
const typename mpc_initializer<T>::init mpc_initializer<T>::initializer;

template<unsigned Digits10, boost::multiprecision::expression_template_option ExpressionTemplates>
struct constant_pi<boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> >
{
   typedef boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> result_type;
   template<int N>
   static inline const result_type& get(const mpl::int_<N>&)
   {
      mpc_initializer<result_type>::force_instantiate();
      static result_type result;
      static bool has_init = false;
      if(!has_init)
      {
         has_init = true;
         mpc_const_pi(result.backend().data());
      }
      return result;
   }
   static inline result_type get(const mpl::int_<0>&)
   {
      result_type result;
      mpc_const_pi(result.backend().data());
      return result;
   }
};
template<unsigned Digits10, boost::multiprecision::expression_template_option ExpressionTemplates>
struct constant_ln_two<boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> >
{
   typedef boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> result_type;
   template<int N>
   static inline const result_type& get(const mpl::int_<N>&)
   {
      mpc_initializer<result_type>::force_instantiate();
      static result_type result;
      static bool has_init = false;
      if(!has_init)
      {
         has_init = true;
         mpc_const_log2(result.backend().data());
      }
      return result;
   }
   static inline result_type get(const mpl::int_<0>&)
   {
      result_type result;
      mpc_const_log2(result.backend().data());
      return result;
   }
};
template<unsigned Digits10, boost::multiprecision::expression_template_option ExpressionTemplates>
struct constant_euler<boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> >
{
   typedef boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> result_type;
   template<int N>
   static inline result_type const& get(const mpl::int_<N>&)
   {
      mpc_initializer<result_type>::force_instantiate();
      static result_type result;
      static bool has_init = false;
      if(!has_init)
      {
         has_init = true;
         mpc_const_euler(result.backend().data());
      }
      return result;
   }
   static inline result_type get(const mpl::int_<0>&)
   {
      result_type result;
      mpc_const_euler(result.backend().data());
      return result;
   }
};
template<unsigned Digits10, boost::multiprecision::expression_template_option ExpressionTemplates>
struct constant_catalan<boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> >
{
   typedef boost::multiprecision::number<boost::multiprecision::mpc_float_backend<Digits10>, ExpressionTemplates> result_type;
   template<int N>
   static inline result_type const& get(const mpl::int_<N>&)
   {
      mpc_initializer<result_type>::force_instantiate();
      static result_type result;
      static bool has_init = false;
      if(!has_init)
      {
         has_init = true;
         mpc_const_catalan(result.backend().data());
      }
      return result;
   }
   static inline result_type get(const mpl::int_<0>&)
   {
      result_type result;
      mpc_const_catalan(result.backend().data());
      return result;
   }
};

}} // namespaces

}}  // namespaces

#endif
