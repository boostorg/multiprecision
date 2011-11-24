///////////////////////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_BN_MPFR_HPP
#define BOOST_MATH_BN_MPFR_HPP

#include <boost/multiprecision/mp_number.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/bindings/detail/big_lanczos.hpp>
#include <mpfr.h>
#include <cmath>
#include <algorithm>

namespace boost{ namespace multiprecision{

template <unsigned digits10>
struct mpfr_float_backend;

namespace detail{

long get_default_precision() { return 50; }

template <unsigned digits10>
struct mpfr_float_imp
{
   typedef mpl::list<long, long long>                     signed_types;
   typedef mpl::list<unsigned long, unsigned long long>   unsigned_types;
   typedef mpl::list<double, long double>                 float_types;
   typedef long                                           exponent_type;

   mpfr_float_imp(){}

   mpfr_float_imp(const mpfr_float_imp& o)
   {
      mpfr_init2(m_data, (((digits10 ? digits10 : get_default_precision()) + 1) * 1000L) / 301L);
      mpfr_set(m_data, o.m_data, GMP_RNDN);
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   mpfr_float_imp(mpfr_float_imp&& o)
   {
      m_data[0] = o.m_data[0];
      o.m_data[0]._mpfr_d = 0;
   }
#endif
   mpfr_float_imp& operator = (const mpfr_float_imp& o)
   {
      mpfr_set(m_data, o.m_data, GMP_RNDN);
      return *this;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   mpfr_float_imp& operator = (mpfr_float_imp&& o)
   {
      mpfr_swap(m_data, o.m_data);
      return *this;
   }
#endif
   mpfr_float_imp& operator = (boost::uintmax_t i)
   {
      boost::uintmax_t mask = ((1uLL << std::numeric_limits<unsigned>::digits) - 1);
      unsigned shift = 0;
      mpfr_t t;
      mpfr_init2(t, (std::max)(static_cast<unsigned>(std::numeric_limits<boost::uintmax_t>::digits), static_cast<unsigned>(((digits10 + 1) * 1000L) / 301L)));
      mpfr_set_ui(m_data, 0, GMP_RNDN);
      while(i)
      {
         mpfr_set_ui(t, static_cast<unsigned>(i & mask), GMP_RNDN);
         if(shift)
            mpfr_mul_2exp(t, t, shift, GMP_RNDN);
         mpfr_add(m_data, m_data, t, GMP_RNDN);
         shift += std::numeric_limits<unsigned>::digits;
         i >>= std::numeric_limits<unsigned>::digits;
      }
      mpfr_clear(t);
      return *this;
   }
   mpfr_float_imp& operator = (boost::intmax_t i)
   {
      bool neg = i < 0;
      *this = static_cast<boost::uintmax_t>(std::abs(i));
      if(neg)
         mpfr_neg(m_data, m_data, GMP_RNDN);
      return *this;
   }
   mpfr_float_imp& operator = (unsigned long i)
   {
      mpfr_set_ui(m_data, i, GMP_RNDN);
      return *this;
   }
   mpfr_float_imp& operator = (long i)
   {
      mpfr_set_si(m_data, i, GMP_RNDN);
      return *this;
   }
   mpfr_float_imp& operator = (double d)
   {
      mpfr_set_d(m_data, d, GMP_RNDN);
      return *this;
   }
   mpfr_float_imp& operator = (long double a)
   {
      using std::frexp;
      using std::ldexp;
      using std::floor;

      if (a == 0) {
         mpfr_set_si(m_data, 0, GMP_RNDN);
         return *this;
      }

      if (a == 1) {
         mpfr_set_si(m_data, 1, GMP_RNDN);
         return *this;
      }

      BOOST_ASSERT(!(boost::math::isinf)(a));
      BOOST_ASSERT(!(boost::math::isnan)(a));

      int e;
      long double f, term;
      mpfr_init_set_ui(m_data, 0u, GMP_RNDN);

      f = frexp(a, &e);

      static const int shift = std::numeric_limits<int>::digits - 1;

      while(f)
      {
         // extract int sized bits from f:
         f = ldexp(f, shift);
         term = floor(f);
         e -= shift;
         mpfr_mul_2exp(m_data, m_data, shift, GMP_RNDN);
         if(term > 0)
            mpfr_add_ui(m_data, m_data, static_cast<unsigned>(term), GMP_RNDN);
         else
            mpfr_sub_ui(m_data, m_data, static_cast<unsigned>(-term), GMP_RNDN);
         f -= term;
      }
      if(e > 0)
         mpfr_mul_2exp(m_data, m_data, e, GMP_RNDN);
      else if(e < 0)
         mpfr_div_2exp(m_data, m_data, -e, GMP_RNDN);
      return *this;
   }
   mpfr_float_imp& operator = (const char* s)
   {
      mpfr_set_str(m_data, s, 10, GMP_RNDN);
      return *this;
   }
   void swap(mpfr_float_imp& o)
   {
      mpfr_swap(m_data, o.m_data);
   }
   std::string str(std::streamsize digits, std::ios_base::fmtflags f)const
   {
      bool scientific = (f & std::ios_base::scientific) == std::ios_base::scientific;
      bool fixed      = (f & std::ios_base::fixed) == std::ios_base::fixed;

      if(scientific)
         ++digits;

      std::string result;
      mp_exp_t e;
      if(mpfr_zero_p(m_data))
      {
         e = 0;
         result = "0";
         if(fixed)
            ++digits;
      }
      else
      {
         char* ps = mpfr_get_str (0, &e, 10, static_cast<std::size_t>(digits), m_data, GMP_RNDN);
        --e;  // To match with what our formatter expects.
         if(fixed && e != -1)
         {
            // Oops we actually need a different number of digits to what we asked for:
            mpfr_free_str(ps);
            digits += e + 1;
            ps = ps = mpfr_get_str (0, &e, 10, static_cast<std::size_t>(digits), m_data, GMP_RNDN);
            --e;  // To match with what our formatter expects.
         }
         result = ps;
         mpfr_free_str(ps);
      }
      boost::multiprecision::detail::format_float_string(result, e, digits, f);
      return result;
   }
   ~mpfr_float_imp()
   {
      if(m_data[0]._mpfr_d)
         mpfr_clear(m_data);
   }
   void negate()
   {
      mpfr_neg(m_data, m_data, GMP_RNDN);
   }
   int compare(const mpfr_float_backend<digits10>& o)const
   {
      return mpfr_cmp(m_data, o.m_data);
   }
   int compare(long i)const
   {
      return mpfr_cmp_si(m_data, i);
   }
   int compare(unsigned long i)const
   {
      return mpfr_cmp_ui(m_data, i);
   }
   template <class V>
   int compare(V v)const
   {
      mpfr_float_backend<digits10> d;
      d = v;
      return compare(d);
   }
   mpfr_t& data() { return m_data; }
   const mpfr_t& data()const { return m_data; }
protected:
   mpfr_t m_data;
   static unsigned& get_default_precision()
   {
      static unsigned val = 50;
      return val;
   }
};

} // namespace detail

template <unsigned digits10>
struct mpfr_float_backend : public detail::mpfr_float_imp<digits10>
{
   mpfr_float_backend()
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
   }
   mpfr_float_backend(const mpfr_float_backend& o) : detail::mpfr_float_imp<digits10>(o) {}
#ifndef BOOST_NO_RVALUE_REFERENCES
   mpfr_float_backend(mpfr_float_backend&& o) : detail::mpfr_float_imp<digits10>(o) {}
#endif
   template <unsigned D>
   mpfr_float_backend(const mpfr_float_backend<D>& val)
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
      mpfr_set(this->m_data, val.data(), GMP_RNDN);
   }
   template <unsigned D>
   mpfr_float_backend(const gmp_float<D>& val)
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
      mpfr_set_f(this->m_data, val.data(), GMP_RNDN);
   }
   mpfr_float_backend(const gmp_int& val)
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
      mpfr_set_z(this->m_data, val.data(), GMP_RNDN);
   }
   mpfr_float_backend(const gmp_rational& val)
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
      mpfr_set_q(this->m_data, val.data(), GMP_RNDN);
   }
   mpfr_float_backend(mpfr_t val)
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
      mpfr_set(this->m_data, val, GMP_RNDN);
   }
   mpfr_float_backend(mpf_t val)
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
      mpfr_set_f(this->m_data, val, GMP_RNDN);
   }
   mpfr_float_backend(mpz_t val)
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
      mpfr_set_z(this->m_data, val, GMP_RNDN);
   }
   mpfr_float_backend(mpq_t val)
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
      mpfr_set_q(this->m_data, val, GMP_RNDN);
   }
   mpfr_float_backend& operator=(const mpfr_float_backend& o)
   {
      *static_cast<detail::mpfr_float_imp<digits10>*>(this) = static_cast<detail::mpfr_float_imp<digits10> const&>(o);
      return *this;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   mpfr_float_backend& operator=(mpfr_float_backend&& o)
   {
      *static_cast<detail::mpfr_float_imp<digits10>*>(this) = static_cast<detail::mpfr_float_imp<digits10>&&>(o);
      return *this;
   }
#endif
   template <class V>
   mpfr_float_backend& operator=(const V& v)
   {
      *static_cast<detail::mpfr_float_imp<digits10>*>(this) = v;
      return *this;
   }
   mpfr_float_backend& operator=(const mpfr_t& val)
   {
      mpfr_set(this->m_data, val, GMP_RNDN);
      return *this;
   }
   mpfr_float_backend& operator=(const mpf_t& val)
   {
      mpfr_set_f(this->m_data, val, GMP_RNDN);
      return *this;
   }
   mpfr_float_backend& operator=(const mpz_t& val)
   {
      mpfr_set_z(this->m_data, val, GMP_RNDN);
      return *this;
   }
   mpfr_float_backend& operator=(const mpq_t& val)
   {
      mpfr_set_q(this->m_data, val, GMP_RNDN);
      return *this;
   }
   template <unsigned D>
   mpfr_float_backend& operator=(const mpfr_float_backend<D>& val)
   {
      mpfr_set(this->m_data, val.data(), GMP_RNDN);
      return *this;
   }
   template <unsigned D>
   mpfr_float_backend& operator=(const gmp_float<D>& val)
   {
      mpfr_set_f(this->m_data, val.data(), GMP_RNDN);
      return *this;
   }
   mpfr_float_backend& operator=(const gmp_int& val)
   {
      mpfr_set_z(this->m_data, val.data(), GMP_RNDN);
      return *this;
   }
   mpfr_float_backend& operator=(const gmp_rational& val)
   {
      mpfr_set_q(this->m_data, val.data(), GMP_RNDN);
      return *this;
   }
};

template <>
struct mpfr_float_backend<0> : public detail::mpfr_float_imp<0>
{
   mpfr_float_backend()
   {
      mpfr_init2(this->m_data, ((get_default_precision() + 1) * 1000L) / 301L);
   }
   mpfr_float_backend(unsigned digits10)
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
   }
   mpfr_float_backend(mpfr_t val)
   {
      mpfr_init2(this->m_data, ((get_default_precision() + 1) * 1000L) / 301L);
      mpfr_set(this->m_data, val, GMP_RNDN);
   }
   mpfr_float_backend(mpf_t val)
   {
      mpfr_init2(this->m_data, ((get_default_precision() + 1) * 1000L) / 301L);
      mpfr_set_f(this->m_data, val, GMP_RNDN);
   }
   mpfr_float_backend(mpz_t val)
   {
      mpfr_init2(this->m_data, ((get_default_precision() + 1) * 1000L) / 301L);
      mpfr_set_z(this->m_data, val, GMP_RNDN);
   }
   mpfr_float_backend(mpq_t val)
   {
      mpfr_init2(this->m_data, ((get_default_precision() + 1) * 1000L) / 301L);
      mpfr_set_q(this->m_data, val, GMP_RNDN);
   }
   mpfr_float_backend(const mpfr_float_backend& o) : detail::mpfr_float_imp<0>(o) {}
#ifndef BOOST_NO_RVALUE_REFERENCES
   mpfr_float_backend(mpfr_float_backend&& o) : detail::mpfr_float_imp<0>(o) {}
#endif
   mpfr_float_backend(const mpfr_float_backend& o, unsigned digits10) 
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
      *this = o;
   }
   template <unsigned D>
   mpfr_float_backend(const mpfr_float_backend<D>& val)
   {
      mpfr_init2(this->m_data, ((get_default_precision() + 1) * 1000L) / 301L);
      mpfr_set(this->m_data, val.data(), GMP_RNDN);
   }
   template <unsigned D>
   mpfr_float_backend(const gmp_float<D>& val)
   {
      mpfr_init2(this->m_data, ((get_default_precision() + 1) * 1000L) / 301L);
      mpfr_set_f(this->m_data, val.data(), GMP_RNDN);
   }
   mpfr_float_backend(const gmp_int& val)
   {
      mpfr_init2(this->m_data, ((get_default_precision() + 1) * 1000L) / 301L);
      mpfr_set_z(this->m_data, val.data(), GMP_RNDN);
   }
   mpfr_float_backend(const gmp_rational& val)
   {
      mpfr_init2(this->m_data, ((get_default_precision() + 1) * 1000L) / 301L);
      mpfr_set_q(this->m_data, val.data(), GMP_RNDN);
   }

   mpfr_float_backend& operator=(const mpfr_float_backend& o)
   {
      *static_cast<detail::mpfr_float_imp<0>*>(this) = static_cast<detail::mpfr_float_imp<0> const&>(o);
      return *this;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   mpfr_float_backend& operator=(mpfr_float_backend&& o)
   {
      *static_cast<detail::mpfr_float_imp<0>*>(this) = static_cast<detail::mpfr_float_imp<0> &&>(o);
      return *this;
   }
#endif
   template <class V>
   mpfr_float_backend& operator=(const V& v)
   {
      *static_cast<detail::mpfr_float_imp<0>*>(this) = v;
      return *this;
   }
   mpfr_float_backend& operator=(const mpfr_t& val)
   {
      mpfr_set(this->m_data, val, GMP_RNDN);
      return *this;
   }
   mpfr_float_backend& operator=(const mpf_t& val)
   {
      mpfr_set_f(this->m_data, val, GMP_RNDN);
      return *this;
   }
   mpfr_float_backend& operator=(const mpz_t& val)
   {
      mpfr_set_z(this->m_data, val, GMP_RNDN);
      return *this;
   }
   mpfr_float_backend& operator=(const mpq_t& val)
   {
      mpfr_set_q(this->m_data, val, GMP_RNDN);
      return *this;
   }
   template <unsigned D>
   mpfr_float_backend& operator=(const mpfr_float_backend<D>& val)
   {
      mpfr_set(this->m_data, val.data(), GMP_RNDN);
      return *this;
   }
   template <unsigned D>
   mpfr_float_backend& operator=(const gmp_float<D>& val)
   {
      mpfr_set_f(this->m_data, val.data(), GMP_RNDN);
      return *this;
   }
   mpfr_float_backend& operator=(const gmp_int& val)
   {
      mpfr_set_z(this->m_data, val.data(), GMP_RNDN);
      return *this;
   }
   mpfr_float_backend& operator=(const gmp_rational& val)
   {
      mpfr_set_q(this->m_data, val.data(), GMP_RNDN);
      return *this;
   }
   static unsigned default_precision()
   {
      return get_default_precision();
   }
   static void default_precision(unsigned v)
   {
      get_default_precision() = v;
   }
   unsigned precision()const
   {
      return mpfr_get_prec(this->m_data) * 301L / 1000 - 1;
   }
   void precision(unsigned digits10)
   {
      mpfr_set_prec(this->m_data, (digits10 + 1) * 1000L / 301);
   }
};

template <unsigned digits10>
inline void add(mpfr_float_backend<digits10>& result, const mpfr_float_backend<digits10>& o)
{
   mpfr_add(result.data(), result.data(), o.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_float_backend<digits10>& result, const mpfr_float_backend<digits10>& o)
{
   mpfr_sub(result.data(), result.data(), o.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_float_backend<digits10>& result, const mpfr_float_backend<digits10>& o)
{
   mpfr_mul(result.data(), result.data(), o.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_float_backend<digits10>& result, const mpfr_float_backend<digits10>& o)
{
   mpfr_div(result.data(), result.data(), o.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void add(mpfr_float_backend<digits10>& result, unsigned long i)
{
   mpfr_add_ui(result.data(), result.data(), i, GMP_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_float_backend<digits10>& result, unsigned long i)
{
   mpfr_sub_ui(result.data(), result.data(), i, GMP_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_float_backend<digits10>& result, unsigned long i)
{
   mpfr_mul_ui(result.data(), result.data(), i, GMP_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_float_backend<digits10>& result, unsigned long i)
{
   mpfr_div_ui(result.data(), result.data(), i, GMP_RNDN);
}
template <unsigned digits10>
inline void add(mpfr_float_backend<digits10>& result, long i)
{
   if(i > 0)
      mpfr_add_ui(result.data(), result.data(), i, GMP_RNDN);
   else
      mpfr_sub_ui(result.data(), result.data(), std::abs(i), GMP_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_float_backend<digits10>& result, long i)
{
   if(i > 0)
      mpfr_sub_ui(result.data(), result.data(), i, GMP_RNDN);
   else
      mpfr_add_ui(result.data(), result.data(), std::abs(i), GMP_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_float_backend<digits10>& result, long i)
{
   mpfr_mul_ui(result.data(), result.data(), std::abs(i), GMP_RNDN);
   if(i < 0)
      mpfr_neg(result.data(), result.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_float_backend<digits10>& result, long i)
{
   mpfr_div_ui(result.data(), result.data(), std::abs(i), GMP_RNDN);
   if(i < 0)
      mpfr_neg(result.data(), result.data(), GMP_RNDN);
}
//
// Specialised 3 arg versions of the basic operators:
//
template <unsigned digits10>
inline void add(mpfr_float_backend<digits10>& a, const mpfr_float_backend<digits10>& x, const mpfr_float_backend<digits10>& y)
{
   mpfr_add(a.data(), x.data(), y.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void add(mpfr_float_backend<digits10>& a, const mpfr_float_backend<digits10>& x, unsigned long y)
{
   mpfr_add_ui(a.data(), x.data(), y, GMP_RNDN);
}
template <unsigned digits10>
inline void add(mpfr_float_backend<digits10>& a, const mpfr_float_backend<digits10>& x, long y)
{
   if(y < 0)
      mpfr_sub_ui(a.data(), x.data(), -y, GMP_RNDN);
   else
      mpfr_add_ui(a.data(), x.data(), y, GMP_RNDN);
}
template <unsigned digits10>
inline void add(mpfr_float_backend<digits10>& a, unsigned long x, const mpfr_float_backend<digits10>& y)
{
   mpfr_add_ui(a.data(), y.data(), x, GMP_RNDN);
}
template <unsigned digits10>
inline void add(mpfr_float_backend<digits10>& a, long x, const mpfr_float_backend<digits10>& y)
{
   if(x < 0)
   {
      mpfr_ui_sub(a.data(), -x, y.data(), GMP_RNDN);
      mpfr_neg(a.data(), a.data(), GMP_RNDN);
   }
   else
      mpfr_add_ui(a.data(), y.data(), x, GMP_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_float_backend<digits10>& a, const mpfr_float_backend<digits10>& x, const mpfr_float_backend<digits10>& y)
{
   mpfr_sub(a.data(), x.data(), y.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_float_backend<digits10>& a, const mpfr_float_backend<digits10>& x, unsigned long y)
{
   mpfr_sub_ui(a.data(), x.data(), y, GMP_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_float_backend<digits10>& a, const mpfr_float_backend<digits10>& x, long y)
{
   if(y < 0)
      mpfr_add_ui(a.data(), x.data(), -y, GMP_RNDN);
   else
      mpfr_sub_ui(a.data(), x.data(), y, GMP_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_float_backend<digits10>& a, unsigned long x, const mpfr_float_backend<digits10>& y)
{
   mpfr_ui_sub(a.data(), x, y.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_float_backend<digits10>& a, long x, const mpfr_float_backend<digits10>& y)
{
   if(x < 0)
   {
      mpfr_add_ui(a.data(), y.data(), -x, GMP_RNDN);
      mpfr_neg(a.data(), a.data(), GMP_RNDN);
   }
   else
      mpfr_ui_sub(a.data(), x, y.data(), GMP_RNDN);
}

template <unsigned digits10>
inline void multiply(mpfr_float_backend<digits10>& a, const mpfr_float_backend<digits10>& x, const mpfr_float_backend<digits10>& y)
{
   mpfr_mul(a.data(), x.data(), y.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_float_backend<digits10>& a, const mpfr_float_backend<digits10>& x, unsigned long y)
{
   mpfr_mul_ui(a.data(), x.data(), y, GMP_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_float_backend<digits10>& a, const mpfr_float_backend<digits10>& x, long y)
{
   if(y < 0)
   {
      mpfr_mul_ui(a.data(), x.data(), -y, GMP_RNDN);
      a.negate();
   }
   else
      mpfr_mul_ui(a.data(), x.data(), y, GMP_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_float_backend<digits10>& a, unsigned long x, const mpfr_float_backend<digits10>& y)
{
   mpfr_mul_ui(a.data(), y.data(), x, GMP_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_float_backend<digits10>& a, long x, const mpfr_float_backend<digits10>& y)
{
   if(x < 0)
   {
      mpfr_mul_ui(a.data(), y.data(), -x, GMP_RNDN);
      mpfr_neg(a.data(), a.data(), GMP_RNDN);
   }
   else
      mpfr_mul_ui(a.data(), y.data(), x, GMP_RNDN);
}

template <unsigned digits10>
inline void divide(mpfr_float_backend<digits10>& a, const mpfr_float_backend<digits10>& x, const mpfr_float_backend<digits10>& y)
{
   mpfr_div(a.data(), x.data(), y.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_float_backend<digits10>& a, const mpfr_float_backend<digits10>& x, unsigned long y)
{
   mpfr_div_ui(a.data(), x.data(), y, GMP_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_float_backend<digits10>& a, const mpfr_float_backend<digits10>& x, long y)
{
   if(y < 0)
   {
      mpfr_div_ui(a.data(), x.data(), -y, GMP_RNDN);
      a.negate();
   }
   else
      mpfr_div_ui(a.data(), x.data(), y, GMP_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_float_backend<digits10>& a, unsigned long x, const mpfr_float_backend<digits10>& y)
{
   mpfr_ui_div(a.data(), x, y.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_float_backend<digits10>& a, long x, const mpfr_float_backend<digits10>& y)
{
   if(x < 0)
   {
      mpfr_ui_div(a.data(), -x, y.data(), GMP_RNDN);
      mpfr_neg(a.data(), a.data(), GMP_RNDN);
   }
   else
      mpfr_ui_div(a.data(), x, y.data(), GMP_RNDN);
}

template <unsigned digits10>
inline bool is_zero(const mpfr_float_backend<digits10>& val)
{
   return 0 != mpfr_zero_p(val.data());
}
template <unsigned digits10>
inline int get_sign(const mpfr_float_backend<digits10>& val)
{
   return mpfr_sgn(val.data());
}

template <unsigned digits10>
inline void convert_to(unsigned long* result, const mpfr_float_backend<digits10>& val)
{
   *result = mpfr_get_ui(val.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void convert_to(long* result, const mpfr_float_backend<digits10>& val)
{
   *result = mpfr_get_si(val.data(), GMP_RNDN);
}
#ifdef _MPFR_H_HAVE_INTMAX_T
template <unsigned digits10>
inline void convert_to(boost::uintmax_t* result, const mpfr_float_backend<digits10>& val)
{
   *result = mpfr_get_uj(val.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void convert_to(boost::intmax_t* result, const mpfr_float_backend<digits10>& val)
{
   *result = mpfr_get_sj(val.data(), GMP_RNDN);
}
#endif
template <unsigned digits10>
inline void convert_to(double* result, const mpfr_float_backend<digits10>& val)
{
   *result = mpfr_get_d(val.data(), GMP_RNDN);
}
template <unsigned digits10>
inline void convert_to(long double* result, const mpfr_float_backend<digits10>& val)
{
   *result = mpfr_get_ld(val.data(), GMP_RNDN);
}

//
// Native non-member operations:
//
template <unsigned Digits10>
inline void eval_sqrt(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& val)
{
   mpfr_sqrt(result.data(), val.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_abs(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& val)
{
   mpfr_abs(result.data(), val.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_fabs(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& val)
{
   mpfr_abs(result.data(), val.data(), GMP_RNDN);
}
template <unsigned Digits10>
inline void eval_ceil(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& val)
{
   mpfr_ceil(result.data(), val.data());
}
template <unsigned Digits10>
inline void eval_floor(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& val)
{
   mpfr_floor(result.data(), val.data());
}
template <unsigned Digits10>
inline void eval_trunc(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& val)
{
   if(0 == mpfr_number_p(val.data()))
   {
      result = boost::math::policies::raise_rounding_error("boost::multiprecision::trunc<%1%>(%1%)", 0, mp_number<mpfr_float_backend<Digits10> >(val), 0, boost::math::policies::policy<>()).backend();
      return;
   }
   mpfr_trunc(result.data(), val.data());
}
template <unsigned Digits10>
inline void eval_ldexp(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& val, long e)
{
   if(e > 0)
      mpfr_mul_2exp(result.data(), val.data(), e, GMP_RNDN);
   else if(e < 0)
      mpfr_div_2exp(result.data(), val.data(), -e, GMP_RNDN);
   else
      result = val;
}
template <unsigned Digits10>
inline void eval_frexp(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& val, int* e)
{
   long v;
   mpfr_get_d_2exp(&v, val.data(), GMP_RNDN);
   *e = v;
   eval_ldexp(result, val, -v);
}
template <unsigned Digits10>
inline void eval_frexp(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& val, long* e)
{
   mpfr_get_d_2exp(e, val.data(), GMP_RNDN);
   return eval_ldexp(result, val, -*e);
}

template <unsigned Digits10>
inline int eval_fpclassify(const mpfr_float_backend<Digits10>& val)
{
   return mpfr_inf_p(val.data()) ? FP_INFINITE : mpfr_nan_p(val.data()) ? FP_NAN : mpfr_zero_p(val.data()) ? FP_ZERO : FP_NORMAL;
}

template <unsigned Digits10>
inline void eval_pow(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& b, const mpfr_float_backend<Digits10>& e)
{
   mpfr_pow(result.data(), b.data(), e.data(), GMP_RNDN);
}

template <unsigned Digits10, class Integer>
inline typename enable_if<mpl::and_<is_signed<Integer>, mpl::bool_<sizeof(Integer) <= sizeof(long)> > >::type eval_pow(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& b, const Integer& e)
{
   mpfr_pow_si(result.data(), b.data(), e, GMP_RNDN);
}

template <unsigned Digits10, class Integer>
inline typename enable_if<mpl::and_<is_unsigned<Integer>, mpl::bool_<sizeof(Integer) <= sizeof(long)> > >::type eval_pow(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& b, const Integer& e)
{
   mpfr_pow_ui(result.data(), b.data(), e, GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_exp(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg)
{
   mpfr_exp(result.data(), arg.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_log(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg)
{
   mpfr_log(result.data(), arg.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_log10(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg)
{
   mpfr_log10(result.data(), arg.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_sin(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg)
{
   mpfr_sin(result.data(), arg.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_cos(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg)
{
   mpfr_cos(result.data(), arg.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_tan(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg)
{
   mpfr_tan(result.data(), arg.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_asin(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg)
{
   mpfr_asin(result.data(), arg.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_acos(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg)
{
   mpfr_acos(result.data(), arg.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_atan(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg)
{
   mpfr_atan(result.data(), arg.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_atan2(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg1, const mpfr_float_backend<Digits10>& arg2)
{
   mpfr_atan2(result.data(), arg1.data(), arg2.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_sinh(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg)
{
   mpfr_sinh(result.data(), arg.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_cosh(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg)
{
   mpfr_cosh(result.data(), arg.data(), GMP_RNDN);
}

template <unsigned Digits10>
inline void eval_tanh(mpfr_float_backend<Digits10>& result, const mpfr_float_backend<Digits10>& arg)
{
   mpfr_tanh(result.data(), arg.data(), GMP_RNDN);
}

typedef mp_number<mpfr_float_backend<50> >    mpfr_float_50;
typedef mp_number<mpfr_float_backend<100> >   mpfr_float_100;
typedef mp_number<mpfr_float_backend<500> >   mpfr_float_500;
typedef mp_number<mpfr_float_backend<1000> >  mpfr_float_1000;
typedef mp_number<mpfr_float_backend<0> >     mpfr_float;

} // namespace boost

namespace math{

namespace tools{

template <>
inline int digits<boost::multiprecision::mpfr_float>()
{
   return boost::multiprecision::detail::get_default_precision();
}

}

namespace lanczos{

template<unsigned Digits10, class Policy>
struct lanczos<multiprecision::mp_number<multiprecision::mpfr_float_backend<Digits10> >, Policy>
{
   typedef typename mpl::if_c<
      Digits10 <= 36,
      lanczos22UDT,
      typename mpl::if_c<
         Digits10 <= 50,
         lanczos31UDT,
         typename mpl::if_c<
            Digits10 <= 110,
            lanczos61UDT,
            undefined_lanczos
         >::type
      >::type
   >::type type;
};

} // namespace lanczos


}}  // namespaces

namespace std{

//
// numeric_limits [partial] specializations for the types declared in this header:
//
template<unsigned Digits10> 
class numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<Digits10> > >
{
   typedef boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<Digits10> > number_type;
public:
   BOOST_STATIC_CONSTEXPR bool is_specialized = true;
   BOOST_STATIC_CONSTEXPR number_type (min)() BOOST_MP_NOEXCEPT
   { 
      initializer.do_nothing();
      static std::pair<bool, number_type> value;
      if(!value.first)
      {
         value.first = true;
         value.second = 0.5;
         mpfr_div_2exp(value.second.backend().data(), value.second.backend().data(), -mpfr_get_emin(), GMP_RNDN);
      }
      return value.second;
   }
   BOOST_STATIC_CONSTEXPR number_type (max)() BOOST_MP_NOEXCEPT
   { 
      initializer.do_nothing();
      static std::pair<bool, number_type> value;
      if(!value.first)
      {
         value.first = true;
         value.second = 0.5;
         mpfr_mul_2exp(value.second.backend().data(), value.second.backend().data(), mpfr_get_emax(), GMP_RNDN);
      }
      return value.second;
   }
   BOOST_STATIC_CONSTEXPR number_type lowest() BOOST_MP_NOEXCEPT
   {
      return -(max)();
   }
   BOOST_STATIC_CONSTEXPR int digits = static_cast<int>(((Digits10 + 1) * 1000L) / 301L);
   BOOST_STATIC_CONSTEXPR int digits10 = Digits10;
   // Is this really correct???
   BOOST_STATIC_CONSTEXPR int max_digits10 = Digits10 + 1;
   BOOST_STATIC_CONSTEXPR bool is_signed = true;
   BOOST_STATIC_CONSTEXPR bool is_integer = false;
   BOOST_STATIC_CONSTEXPR bool is_exact = false;
   BOOST_STATIC_CONSTEXPR int radix = 2;
   BOOST_STATIC_CONSTEXPR number_type epsilon() BOOST_MP_NOEXCEPT 
   { 
      initializer.do_nothing();
      static std::pair<bool, number_type> value;
      if(!value.first)
      {
         value.first = true;
         value.second = 1;
         mpfr_div_2exp(value.second.backend().data(), value.second.backend().data(), std::numeric_limits<number_type>::digits - 1, GMP_RNDN);
      }
      return value.second;
   }
   // What value should this be????
   BOOST_STATIC_CONSTEXPR number_type round_error() BOOST_MP_NOEXCEPT 
   { 
      // returns epsilon/2
      initializer.do_nothing();
      static std::pair<bool, number_type> value;
      if(!value.first)
      {
         value.first = true;
         value.second = 1;
         mpfr_div_2exp(value.second.backend().data(), value.second.backend().data(), digits, GMP_RNDN);
      }
      return value.second;
   }
   BOOST_STATIC_CONSTEXPR long min_exponent = MPFR_EMIN_DEFAULT;
   BOOST_STATIC_CONSTEXPR long min_exponent10 = (MPFR_EMIN_DEFAULT / 1000) * 301L;
   BOOST_STATIC_CONSTEXPR long max_exponent = MPFR_EMAX_DEFAULT;
   BOOST_STATIC_CONSTEXPR long max_exponent10 = (MPFR_EMAX_DEFAULT / 1000) * 301L;
   BOOST_STATIC_CONSTEXPR bool has_infinity = true;
   BOOST_STATIC_CONSTEXPR bool has_quiet_NaN = true;
   BOOST_STATIC_CONSTEXPR bool has_signaling_NaN = false;
   BOOST_STATIC_CONSTEXPR float_denorm_style has_denorm = denorm_absent;
   BOOST_STATIC_CONSTEXPR bool has_denorm_loss = false;
   BOOST_STATIC_CONSTEXPR number_type infinity() BOOST_MP_NOEXCEPT 
   { 
      // returns epsilon/2
      initializer.do_nothing();
      static std::pair<bool, number_type> value;
      if(!value.first)
      {
         value.first = true;
         value.second = 1;
         mpfr_set_inf(value.second.backend().data(), 1);
      }
      return value.second;
   }
   BOOST_STATIC_CONSTEXPR number_type quiet_NaN() BOOST_MP_NOEXCEPT 
   { 
      // returns epsilon/2
      initializer.do_nothing();
      static std::pair<bool, number_type> value;
      if(!value.first)
      {
         value.first = true;
         value.second = 1;
         mpfr_set_nan(value.second.backend().data());
      }
      return value.second;
   }
   BOOST_STATIC_CONSTEXPR number_type signaling_NaN() BOOST_MP_NOEXCEPT 
   { 
      return number_type(0); 
   }
   BOOST_STATIC_CONSTEXPR number_type denorm_min() BOOST_MP_NOEXCEPT { return number_type(0); }
   BOOST_STATIC_CONSTEXPR bool is_iec559 = false;
   BOOST_STATIC_CONSTEXPR bool is_bounded = true;
   BOOST_STATIC_CONSTEXPR bool is_modulo = false;
   BOOST_STATIC_CONSTEXPR bool traps = true;
   BOOST_STATIC_CONSTEXPR bool tinyness_before = false;
   BOOST_STATIC_CONSTEXPR float_round_style round_style = round_to_nearest;

private:
   struct data_initializer
   {
      data_initializer()
      {
         std::numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<digits10> > >::epsilon();
         std::numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<digits10> > >::round_error();
         (std::numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<digits10> > >::min)();
         (std::numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<digits10> > >::max)();
         std::numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<digits10> > >::infinity();
         std::numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<digits10> > >::quiet_NaN();
      }
      void do_nothing()const{}
   };
   static const data_initializer initializer;
};

template<unsigned Digits10> 
const typename numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<Digits10> > >::data_initializer numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<Digits10> > >::initializer;

template<> 
class numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<0> > >
{
   typedef boost::multiprecision::mp_number<boost::multiprecision::mpfr_float_backend<0> > number_type;
public:
   BOOST_STATIC_CONSTEXPR bool is_specialized = false;
   BOOST_STATIC_CONSTEXPR number_type (min)() BOOST_MP_NOEXCEPT { return number_type(0); }
   BOOST_STATIC_CONSTEXPR number_type (max)() BOOST_MP_NOEXCEPT { return number_type(0); }
   BOOST_STATIC_CONSTEXPR number_type lowest() BOOST_MP_NOEXCEPT { return number_type(0); }
   BOOST_STATIC_CONSTEXPR int digits = 0;
   BOOST_STATIC_CONSTEXPR int digits10 = 0;
   BOOST_STATIC_CONSTEXPR int max_digits10 = 0;
   BOOST_STATIC_CONSTEXPR bool is_signed = false;
   BOOST_STATIC_CONSTEXPR bool is_integer = false;
   BOOST_STATIC_CONSTEXPR bool is_exact = false;
   BOOST_STATIC_CONSTEXPR int radix = 0;
   BOOST_STATIC_CONSTEXPR number_type epsilon() BOOST_MP_NOEXCEPT { return number_type(0); }
   BOOST_STATIC_CONSTEXPR number_type round_error() BOOST_MP_NOEXCEPT { return number_type(0); }
   BOOST_STATIC_CONSTEXPR int min_exponent = 0;
   BOOST_STATIC_CONSTEXPR int min_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR bool has_infinity = false;
   BOOST_STATIC_CONSTEXPR bool has_quiet_NaN = false;
   BOOST_STATIC_CONSTEXPR bool has_signaling_NaN = false;
   BOOST_STATIC_CONSTEXPR float_denorm_style has_denorm = denorm_absent;
   BOOST_STATIC_CONSTEXPR bool has_denorm_loss = false;
   BOOST_STATIC_CONSTEXPR number_type infinity() BOOST_MP_NOEXCEPT { return number_type(0); }
   BOOST_STATIC_CONSTEXPR number_type quiet_NaN() BOOST_MP_NOEXCEPT { return number_type(0); }
   BOOST_STATIC_CONSTEXPR number_type signaling_NaN() BOOST_MP_NOEXCEPT { return number_type(0); }
   BOOST_STATIC_CONSTEXPR number_type denorm_min() BOOST_MP_NOEXCEPT { return number_type(0); }
   BOOST_STATIC_CONSTEXPR bool is_iec559 = false;
   BOOST_STATIC_CONSTEXPR bool is_bounded = false;
   BOOST_STATIC_CONSTEXPR bool is_modulo = false;
   BOOST_STATIC_CONSTEXPR bool traps = false;
   BOOST_STATIC_CONSTEXPR bool tinyness_before = false;
   BOOST_STATIC_CONSTEXPR float_round_style round_style = round_toward_zero;
};

} // namespace std
#endif
