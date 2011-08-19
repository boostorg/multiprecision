///////////////////////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_BN_MPFR_HPP
#define BOOST_MATH_BN_MPFR_HPP

#include <boost/math/big_number.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <mpfr.h>
#include <cmath>
#include <algorithm>

namespace boost{ namespace math{

template <unsigned digits10>
struct mpfr_real_backend;

namespace detail{

template <unsigned digits10>
struct mpfr_real_imp
{
   typedef mpl::list<long, long long>                     signed_types;
   typedef mpl::list<unsigned long, unsigned long long>   unsigned_types;
   typedef mpl::list<double, long double>                 real_types;

   mpfr_real_imp(){}

   mpfr_real_imp(const mpfr_real_imp& o)
   {
      mpfr_init2(m_data, (((digits10 ? digits10 : mpfr_real_backend<0>::default_precision()) + 1) * 1000L) / 301L);
      mpfr_set(m_data, o.m_data, MPFR_RNDN);
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   mpfr_real_imp(mpfr_real_imp&& o)
   {
      m_data[0] = o.m_data[0];
      o.m_data[0]._mpfr_d = 0;
   }
#endif
   mpfr_real_imp& operator = (const mpfr_real_imp& o)
   {
      mpfr_set(m_data, o.m_data, MPFR_RNDN);
      return *this;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   mpfr_real_imp& operator = (mpfr_real_imp&& o)
   {
      mpfr_swap(m_data, o.m_data);
      return *this;
   }
#endif
   mpfr_real_imp& operator = (boost::uintmax_t i)
   {
      boost::uintmax_t mask = ((1uLL << std::numeric_limits<unsigned>::digits) - 1);
      unsigned shift = 0;
      mpfr_t t;
      mpfr_init2(t, (std::max)(static_cast<unsigned>(std::numeric_limits<boost::uintmax_t>::digits), static_cast<unsigned>(((digits10 + 1) * 1000L) / 301L)));
      mpfr_set_ui(m_data, 0, MPFR_RNDN);
      while(i)
      {
         mpfr_set_ui(t, static_cast<unsigned>(i & mask), MPFR_RNDN);
         long e;
         const char* ps = mpfr_get_str (0, &e, 10, 0, t, MPFR_RNDN);
         if(shift)
            mpfr_mul_2exp(t, t, shift, MPFR_RNDN);
         ps = mpfr_get_str (0, &e, 10, 0, t, MPFR_RNDN);
         mpfr_add(m_data, m_data, t, MPFR_RNDN);
         ps = mpfr_get_str (0, &e, 10, 0, m_data, MPFR_RNDN);
         shift += std::numeric_limits<unsigned>::digits;
         i >>= std::numeric_limits<unsigned>::digits;
      }
      mpfr_clear(t);
      return *this;
   }
   mpfr_real_imp& operator = (boost::intmax_t i)
   {
      bool neg = i < 0;
      *this = static_cast<boost::uintmax_t>(std::abs(i));
      if(neg)
         mpfr_neg(m_data, m_data, MPFR_RNDN);
      return *this;
   }
   mpfr_real_imp& operator = (unsigned long i)
   {
      mpfr_set_ui(m_data, i, MPFR_RNDN);
      return *this;
   }
   mpfr_real_imp& operator = (long i)
   {
      mpfr_set_si(m_data, i, MPFR_RNDN);
      return *this;
   }
   mpfr_real_imp& operator = (double d)
   {
      mpfr_set_d(m_data, d, MPFR_RNDN);
      return *this;
   }
   mpfr_real_imp& operator = (long double a)
   {
      using std::frexp;
      using std::ldexp;
      using std::floor;

      if (a == 0) {
         mpfr_set_si(m_data, 0, MPFR_RNDN);
         return *this;
      }

      if (a == 1) {
         mpfr_set_si(m_data, 1, MPFR_RNDN);
         return *this;
      }

      BOOST_ASSERT(!(boost::math::isinf)(a));
      BOOST_ASSERT(!(boost::math::isnan)(a));

      int e;
      long double f, term;
      mpfr_init_set_ui(m_data, 0u, MPFR_RNDN);

      f = frexp(a, &e);

      static const int shift = std::numeric_limits<int>::digits - 1;

      while(f)
      {
         // extract int sized bits from f:
         f = ldexp(f, shift);
         term = floor(f);
         e -= shift;
         mpfr_mul_2exp(m_data, m_data, shift, MPFR_RNDN);
         if(term > 0)
            mpfr_add_ui(m_data, m_data, static_cast<unsigned>(term), MPFR_RNDN);
         else
            mpfr_sub_ui(m_data, m_data, static_cast<unsigned>(-term), MPFR_RNDN);
         f -= term;
      }
      if(e > 0)
         mpfr_mul_2exp(m_data, m_data, e, MPFR_RNDN);
      else if(e < 0)
         mpfr_div_2exp(m_data, m_data, -e, MPFR_RNDN);
      return *this;
   }
   mpfr_real_imp& operator = (const char* s)
   {
      mpfr_set_str(m_data, s, 10, MPFR_RNDN);
      return *this;
   }
   void swap(mpfr_real_imp& o)
   {
      mpfr_swap(m_data, o.m_data);
   }
   std::string str(unsigned digits, bool scientific)const
   {
      std::string result;
      mp_exp_t e;
      void *(*alloc_func_ptr) (size_t);
      void *(*realloc_func_ptr) (void *, size_t, size_t);
      void (*free_func_ptr) (void *, size_t);
      const char* ps = mpfr_get_str (0, &e, 10, digits, m_data, MPFR_RNDN);
      std::ptrdiff_t sl = std::strlen(ps);
      unsigned chars = sl;
      if(sl == 0)
         return "0";
      while(ps[chars-1] == '0')
         --chars;
      if(*ps == '-')
         --chars; // number of digits excluding sign.
      if(chars == 0)
         return "0";
      if(!scientific
         && (chars <= std::numeric_limits<boost::uintmax_t>::digits10 + 1)
         && (e >= (int)chars)
         && (chars <= std::numeric_limits<boost::uintmax_t>::digits10 + 1))
      {
         result.assign(ps, (*ps == '-' ? chars+1 : chars));
         result.append(e-chars, '0');
      }
      else
      {
         result = ps;
         if(ps[0] == '-')
            result.insert(2, 1, '.');
         else
            result.insert(1, 1, '.');
         --e;
         if(e)
            result += "e" + lexical_cast<std::string>(e);
      }
      mp_get_memory_functions(&alloc_func_ptr, &realloc_func_ptr, &free_func_ptr);
      (*free_func_ptr)((void*)ps, sl + 1);
      return result;
   }
   ~mpfr_real_imp()
   {
      if(m_data[0]._mpfr_d)
         mpfr_clear(m_data);
   }
   void negate()
   {
      mpfr_neg(m_data, m_data, MPFR_RNDN);
   }
   int compare(const mpfr_real_backend<digits10>& o)const
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
      mpfr_real_backend<digits10> d;
      d = v;
      return compare(d);
   }
   mpfr_t& data() { return m_data; }
   const mpfr_t& data()const { return m_data; }
protected:
   mpfr_t m_data;
};

} // namespace detail

template <unsigned digits10>
struct mpfr_real_backend : public detail::mpfr_real_imp<digits10>
{
   mpfr_real_backend()
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
   }
   mpfr_real_backend(const mpfr_real_backend& o) : detail::mpfr_real_imp<digits10>(o) {}
#ifndef BOOST_NO_RVALUE_REFERENCES
   mpfr_real_backend(mpfr_real_backend&& o) : detail::mpfr_real_imp<digits10>(o) {}
#endif
   mpfr_real_backend& operator=(const mpfr_real_backend& o)
   {
      *static_cast<detail::mpfr_real_imp<digits10>*>(this) = static_cast<detail::mpfr_real_imp<digits10> const&>(o);
      return *this;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   mpfr_real_backend& operator=(mpfr_real_backend&& o)
   {
      *static_cast<detail::mpfr_real_imp<digits10>*>(this) = static_cast<detail::mpfr_real_imp<digits10>&&>(o);
      return *this;
   }
#endif
   template <class V>
   mpfr_real_backend& operator=(const V& v)
   {
      *static_cast<detail::mpfr_real_imp<digits10>*>(this) = v;
      return *this;
   }
};

template <>
struct mpfr_real_backend<0> : public detail::mpfr_real_imp<0>
{
   mpfr_real_backend()
   {
      mpfr_init2(this->m_data, ((get_default_precision() + 1) * 1000L) / 301L);
   }
   mpfr_real_backend(unsigned digits10)
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
   }
   mpfr_real_backend(const mpfr_real_backend& o) : detail::mpfr_real_imp<0>(o) {}
#ifndef BOOST_NO_RVALUE_REFERENCES
   mpfr_real_backend(mpfr_real_backend&& o) : detail::mpfr_real_imp<0>(o) {}
#endif
   mpfr_real_backend(const mpfr_real_backend& o, unsigned digits10) 
   {
      mpfr_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
      *this = o;
   }

   mpfr_real_backend& operator=(const mpfr_real_backend& o)
   {
      *static_cast<detail::mpfr_real_imp<0>*>(this) = static_cast<detail::mpfr_real_imp<0> const&>(o);
      return *this;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   mpfr_real_backend& operator=(mpfr_real_backend&& o)
   {
      *static_cast<detail::mpfr_real_imp<0>*>(this) = static_cast<detail::mpfr_real_imp<0> &&>(o);
      return *this;
   }
#endif
   template <class V>
   mpfr_real_backend& operator=(const V& v)
   {
      *static_cast<detail::mpfr_real_imp<0>*>(this) = v;
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
private:
   static unsigned& get_default_precision()
   {
      static unsigned val = 50;
      return val;
   }
};

template <unsigned digits10>
inline void add(mpfr_real_backend<digits10>& result, const mpfr_real_backend<digits10>& o)
{
   mpfr_add(result.data(), result.data(), o.data(), MPFR_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_real_backend<digits10>& result, const mpfr_real_backend<digits10>& o)
{
   mpfr_sub(result.data(), result.data(), o.data(), MPFR_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_real_backend<digits10>& result, const mpfr_real_backend<digits10>& o)
{
   mpfr_mul(result.data(), result.data(), o.data(), MPFR_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_real_backend<digits10>& result, const mpfr_real_backend<digits10>& o)
{
   mpfr_div(result.data(), result.data(), o.data(), MPFR_RNDN);
}
template <unsigned digits10>
inline void add(mpfr_real_backend<digits10>& result, unsigned long i)
{
   mpfr_add_ui(result.data(), result.data(), i, MPFR_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_real_backend<digits10>& result, unsigned long i)
{
   mpfr_sub_ui(result.data(), result.data(), i, MPFR_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_real_backend<digits10>& result, unsigned long i)
{
   mpfr_mul_ui(result.data(), result.data(), i, MPFR_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_real_backend<digits10>& result, unsigned long i)
{
   mpfr_div_ui(result.data(), result.data(), i, MPFR_RNDN);
}
template <unsigned digits10>
inline void add(mpfr_real_backend<digits10>& result, long i)
{
   if(i > 0)
      mpfr_add_ui(result.data(), result.data(), i, MPFR_RNDN);
   else
      mpfr_sub_ui(result.data(), result.data(), std::abs(i), MPFR_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_real_backend<digits10>& result, long i)
{
   if(i > 0)
      mpfr_sub_ui(result.data(), result.data(), i, MPFR_RNDN);
   else
      mpfr_add_ui(result.data(), result.data(), std::abs(i), MPFR_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_real_backend<digits10>& result, long i)
{
   mpfr_mul_ui(result.data(), result.data(), std::abs(i), MPFR_RNDN);
   if(i < 0)
      mpfr_neg(result.data(), result.data(), MPFR_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_real_backend<digits10>& result, long i)
{
   mpfr_div_ui(result.data(), result.data(), std::abs(i), MPFR_RNDN);
   if(i < 0)
      mpfr_neg(result.data(), result.data(), MPFR_RNDN);
}
//
// Specialised 3 arg versions of the basic operators:
//
template <unsigned digits10>
inline void add(mpfr_real_backend<digits10>& a, const mpfr_real_backend<digits10>& x, const mpfr_real_backend<digits10>& y)
{
   mpfr_add(a.data(), x.data(), y.data(), MPFR_RNDN);
}
template <unsigned digits10>
inline void add(mpfr_real_backend<digits10>& a, const mpfr_real_backend<digits10>& x, unsigned long y)
{
   mpfr_add_ui(a.data(), x.data(), y, MPFR_RNDN);
}
template <unsigned digits10>
inline void add(mpfr_real_backend<digits10>& a, const mpfr_real_backend<digits10>& x, long y)
{
   if(y < 0)
      mpfr_sub_ui(a.data(), x.data(), -y, MPFR_RNDN);
   else
      mpfr_add_ui(a.data(), x.data(), y, MPFR_RNDN);
}
template <unsigned digits10>
inline void add(mpfr_real_backend<digits10>& a, unsigned long x, const mpfr_real_backend<digits10>& y)
{
   mpfr_add_ui(a.data(), y.data(), x, MPFR_RNDN);
}
template <unsigned digits10>
inline void add(mpfr_real_backend<digits10>& a, long x, const mpfr_real_backend<digits10>& y)
{
   if(x < 0)
   {
      mpfr_ui_sub(a.data(), -x, y.data(), MPFR_RNDN);
      mpfr_neg(a.data(), a.data(), MPFR_RNDN);
   }
   else
      mpfr_add_ui(a.data(), y.data(), x, MPFR_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_real_backend<digits10>& a, const mpfr_real_backend<digits10>& x, const mpfr_real_backend<digits10>& y)
{
   mpfr_sub(a.data(), x.data(), y.data(), MPFR_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_real_backend<digits10>& a, const mpfr_real_backend<digits10>& x, unsigned long y)
{
   mpfr_sub_ui(a.data(), x.data(), y, MPFR_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_real_backend<digits10>& a, const mpfr_real_backend<digits10>& x, long y)
{
   if(y < 0)
      mpfr_add_ui(a.data(), x.data(), -y, MPFR_RNDN);
   else
      mpfr_sub_ui(a.data(), x.data(), y, MPFR_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_real_backend<digits10>& a, unsigned long x, const mpfr_real_backend<digits10>& y)
{
   mpfr_ui_sub(a.data(), x, y.data(), MPFR_RNDN);
}
template <unsigned digits10>
inline void subtract(mpfr_real_backend<digits10>& a, long x, const mpfr_real_backend<digits10>& y)
{
   if(x < 0)
   {
      mpfr_add_ui(a.data(), y.data(), -x, MPFR_RNDN);
      mpfr_neg(a.data(), a.data(), MPFR_RNDN);
   }
   else
      mpfr_ui_sub(a.data(), x, y.data(), MPFR_RNDN);
}

template <unsigned digits10>
inline void multiply(mpfr_real_backend<digits10>& a, const mpfr_real_backend<digits10>& x, const mpfr_real_backend<digits10>& y)
{
   mpfr_mul(a.data(), x.data(), y.data(), MPFR_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_real_backend<digits10>& a, const mpfr_real_backend<digits10>& x, unsigned long y)
{
   mpfr_mul_ui(a.data(), x.data(), y, MPFR_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_real_backend<digits10>& a, const mpfr_real_backend<digits10>& x, long y)
{
   if(y < 0)
   {
      mpfr_mul_ui(a.data(), x.data(), -y, MPFR_RNDN);
      a.negate();
   }
   else
      mpfr_mul_ui(a.data(), x.data(), y, MPFR_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_real_backend<digits10>& a, unsigned long x, const mpfr_real_backend<digits10>& y)
{
   mpfr_mul_ui(a.data(), y.data(), x, MPFR_RNDN);
}
template <unsigned digits10>
inline void multiply(mpfr_real_backend<digits10>& a, long x, const mpfr_real_backend<digits10>& y)
{
   if(x < 0)
   {
      mpfr_mul_ui(a.data(), y.data(), -x, MPFR_RNDN);
      mpfr_neg(a.data(), a.data(), MPFR_RNDN);
   }
   else
      mpfr_mul_ui(a.data(), y.data(), x, MPFR_RNDN);
}

template <unsigned digits10>
inline void divide(mpfr_real_backend<digits10>& a, const mpfr_real_backend<digits10>& x, const mpfr_real_backend<digits10>& y)
{
   mpfr_div(a.data(), x.data(), y.data(), MPFR_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_real_backend<digits10>& a, const mpfr_real_backend<digits10>& x, unsigned long y)
{
   mpfr_div_ui(a.data(), x.data(), y, MPFR_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_real_backend<digits10>& a, const mpfr_real_backend<digits10>& x, long y)
{
   if(y < 0)
   {
      mpfr_div_ui(a.data(), x.data(), -y, MPFR_RNDN);
      a.negate();
   }
   else
      mpfr_div_ui(a.data(), x.data(), y, MPFR_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_real_backend<digits10>& a, unsigned long x, const mpfr_real_backend<digits10>& y)
{
   mpfr_ui_div(a.data(), x, y.data(), MPFR_RNDN);
}
template <unsigned digits10>
inline void divide(mpfr_real_backend<digits10>& a, long x, const mpfr_real_backend<digits10>& y)
{
   if(x < 0)
   {
      mpfr_ui_div(a.data(), -x, y.data(), MPFR_RNDN);
      mpfr_neg(a.data(), a.data(), MPFR_RNDN);
   }
   else
      mpfr_ui_div(a.data(), x, y.data(), MPFR_RNDN);
}

//
// Native non-member operations:
//
template <unsigned Digits10>
inline void sqrt(mpfr_real_backend<Digits10>* result, const mpfr_real_backend<Digits10>& val)
{
   mpfr_sqrt(result->data(), val.data(), MPFR_RNDN);
}

template <unsigned Digits10>
inline void abs(mpfr_real_backend<Digits10>* result, const mpfr_real_backend<Digits10>& val)
{
   mpfr_abs(result->data(), val.data(), MPFR_RNDN);
}

template <unsigned Digits10>
inline void fabs(mpfr_real_backend<Digits10>* result, const mpfr_real_backend<Digits10>& val)
{
   mpfr_abs(result->data(), val.data(), MPFR_RNDN);
}
template <unsigned Digits10>
inline void ceil(mpfr_real_backend<Digits10>* result, const mpfr_real_backend<Digits10>& val)
{
   mpfr_ceil(result->data(), val.data());
}
template <unsigned Digits10>
inline void floor(mpfr_real_backend<Digits10>* result, const mpfr_real_backend<Digits10>& val)
{
   mpfr_floor(result->data(), val.data());
}
template <unsigned Digits10>
inline void trunc(mpfr_real_backend<Digits10>* result, const mpfr_real_backend<Digits10>& val)
{
   mpfr_trunc(result->data(), val.data());
}
template <unsigned Digits10>
inline void ldexp(mpfr_real_backend<Digits10>* result, const mpfr_real_backend<Digits10>& val, long e)
{
   if(e > 0)
      mpfr_mul_2exp(result->data(), val.data(), e, MPFR_RNDN);
   else if(e < 0)
      mpfr_div_2exp(result->data(), val.data(), -e, MPFR_RNDN);
}
template <unsigned Digits10>
inline void frexp(mpfr_real_backend<Digits10>* result, const mpfr_real_backend<Digits10>& val, int* e)
{
   long v;
   mpfr_get_d_2exp(&v, val.data(), MPFR_RNDN);
   *e = v;
   return ldexp(result, val, -v);
}
template <unsigned Digits10>
inline void frexp(mpfr_real_backend<Digits10>* result, const mpfr_real_backend<Digits10>& val, long* e)
{
   mpfr_get_d_2exp(e, val.data(), MPFR_RNDN);
   return ldexp(result, val, -*e);
}

typedef big_number<mpfr_real_backend<50> >    mpfr_real_50;
typedef big_number<mpfr_real_backend<100> >   mpfr_real_100;
typedef big_number<mpfr_real_backend<500> >   mpfr_real_500;
typedef big_number<mpfr_real_backend<1000> >  mpfr_real_1000;
typedef big_number<mpfr_real_backend<0> >     mpfr_real;

}}  // namespaces

#endif
