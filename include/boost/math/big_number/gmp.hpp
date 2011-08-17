///////////////////////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_ER_GMP_BACKEND_HPP
#define BOOST_MATH_ER_GMP_BACKEND_HPP

#include <boost/math/big_number.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <gmp.h>
#include <cmath>

namespace boost{ namespace math{

template <unsigned digits10>
struct gmp_real;

namespace detail{

template <unsigned digits10>
struct gmp_real_imp
{
   typedef mpl::list<long, long long>                 signed_types;
   typedef mpl::list<unsigned long, unsigned long long>   unsigned_types;
   typedef mpl::list<double, long double>            real_types;

   gmp_real_imp(){}

   gmp_real_imp(const gmp_real_imp& o)
   {
      mpf_init_set(m_data, o.m_data);
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   gmp_real_imp(gmp_real_imp&& o)
   {
      m_data[0] = o.m_data[0];
      o.m_data[0]._mp_d = 0;
   }
#endif
   gmp_real_imp& operator = (const gmp_real_imp& o)
   {
      mpf_set(m_data, o.m_data);
      return *this;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   gmp_real_imp& operator = (gmp_real_imp&& o)
   {
      mpf_swap(m_data, o.m_data);
      return *this;
   }
#endif
   gmp_real_imp& operator = (boost::uintmax_t i)
   {
      boost::uintmax_t mask = ((1uLL << std::numeric_limits<unsigned>::digits) - 1);
      unsigned shift = 0;
      mpf_t t;
      mpf_init2(m_data, ((digits10 + 1) * 1000L) / 301L);
      mpf_init2(t, ((digits10 + 1) * 1000L) / 301L);
      while(i)
      {
         mpf_set_ui(t, static_cast<unsigned>(i & mask));
         if(shift)
            mpf_mul_2exp(t, t, shift);
         mpf_add(m_data, m_data, t);
         shift += std::numeric_limits<unsigned>::digits;
         i >>= std::numeric_limits<unsigned>::digits;
      }
      mpf_clear(t);
      return *this;
   }
   gmp_real_imp& operator = (boost::intmax_t i)
   {
      bool neg = i < 0;
      *this = static_cast<boost::uintmax_t>(std::abs(i));
      if(neg)
         mpf_neg(m_data, m_data);
      return *this;
   }
   gmp_real_imp& operator = (unsigned long i)
   {
      mpf_set_ui(m_data, i);
      return *this;
   }
   gmp_real_imp& operator = (long i)
   {
      mpf_set_si(m_data, i);
      return *this;
   }
   gmp_real_imp& operator = (double d)
   {
      mpf_set_d(m_data, d);
      return *this;
   }
   gmp_real_imp& operator = (long double a)
   {
      using std::frexp;
      using std::ldexp;
      using std::floor;

      if (a == 0) {
         mpf_set_si(m_data, 0);
         return *this;
      }

      if (a == 1) {
         mpf_set_si(m_data, 1);
         return *this;
      }

      BOOST_ASSERT(!(boost::math::isinf)(a));
      BOOST_ASSERT(!(boost::math::isnan)(a));

      int e;
      long double f, term;
      mpf_init_set_ui(m_data, 0u);

      f = frexp(a, &e);

      static const int shift = std::numeric_limits<int>::digits - 1;

      while(f)
      {
         // extract int sized bits from f:
         f = ldexp(f, shift);
         term = floor(f);
         e -= shift;
         mpf_mul_2exp(m_data, m_data, shift);
         if(term > 0)
            mpf_add_ui(m_data, m_data, static_cast<unsigned>(term));
         else
            mpf_sub_ui(m_data, m_data, static_cast<unsigned>(-term));
         f -= term;
      }
      if(e > 0)
         mpf_mul_2exp(m_data, m_data, e);
      else if(e < 0)
         mpf_div_2exp(m_data, m_data, -e);
      return *this;
   }
   gmp_real_imp& operator = (const char* s)
   {
      mpf_set_str(m_data, s, 10);
      return *this;
   }
   void swap(gmp_real_imp& o)
   {
      mpf_swap(m_data, o.m_data);
   }
   std::string str(unsigned digits, bool scientific)const
   {
      std::string result;
      mp_exp_t e;
      void *(*alloc_func_ptr) (size_t);
      void *(*realloc_func_ptr) (void *, size_t, size_t);
      void (*free_func_ptr) (void *, size_t);
      const char* ps = mpf_get_str (0, &e, 10, digits, m_data);
      std::ptrdiff_t sl = std::strlen(ps);
      if(sl == 0)
         return "0";
      if(*ps == '-')
         --sl; // number of digits excluding sign.
      if(!scientific
         && (sl <= std::numeric_limits<boost::uintmax_t>::digits10 + 1)
         && (e >= sl)
         && (sl <= std::numeric_limits<boost::uintmax_t>::digits10 + 1))
      {
         result = ps;
         result.append(e-sl, '0');
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
      (*free_func_ptr)((void*)ps, std::strlen(ps) + 1);
      return result;
   }
   ~gmp_real_imp()
   {
      if(m_data[0]._mp_d)
         mpf_clear(m_data);
   }
   void negate()
   {
      mpf_neg(m_data, m_data);
   }
   int compare(const gmp_real<digits10>& o)const
   {
      return mpf_cmp(m_data, o.m_data);
   }
   int compare(long i)const
   {
      return mpf_cmp_si(m_data, i);
   }
   int compare(unsigned long i)const
   {
      return mpf_cmp_ui(m_data, i);
   }
   template <class V>
   int compare(V v)const
   {
      gmp_real<digits10> d;
      d = v;
      return compare(d);
   }
   mpf_t& data() { return m_data; }
   const mpf_t& data()const { return m_data; }
protected:
   mpf_t m_data;
};

} // namespace detail

template <unsigned digits10>
struct gmp_real : public detail::gmp_real_imp<digits10>
{
   gmp_real()
   {
      mpf_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
   }
   gmp_real(const gmp_real& o) : detail::gmp_real_imp<digits10>(o) {}
#ifndef BOOST_NO_RVALUE_REFERENCES
   gmp_real(gmp_real&& o) : detail::gmp_real_imp<digits10>(o) {}
#endif
   gmp_real& operator=(const gmp_real& o)
   {
      *static_cast<detail::gmp_real_imp<digits10>*>(this) = static_cast<detail::gmp_real_imp<digits10> const&>(o);
      return *this;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   gmp_real& operator=(gmp_real&& o)
   {
      *static_cast<detail::gmp_real_imp<digits10>*>(this) = static_cast<detail::gmp_real_imp<digits10>&&>(o);
      return *this;
   }
#endif
   template <class V>
   gmp_real& operator=(const V& v)
   {
      *static_cast<detail::gmp_real_imp<digits10>*>(this) = v;
      return *this;
   }
};

template <>
struct gmp_real<0> : public detail::gmp_real_imp<0>
{
   gmp_real()
   {
      mpf_init2(this->m_data, ((get_default_precision() + 1) * 1000L) / 301L);
   }
   gmp_real(unsigned digits10)
   {
      mpf_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
   }
   gmp_real(const gmp_real& o) : detail::gmp_real_imp<0>(o) {}
#ifndef BOOST_NO_RVALUE_REFERENCES
   gmp_real(gmp_real&& o) : detail::gmp_real_imp<0>(o) {}
#endif
   gmp_real(const gmp_real& o, unsigned digits10) 
   {
      mpf_init2(this->m_data, ((digits10 + 1) * 1000L) / 301L);
      *this = o;
   }

   gmp_real& operator=(const gmp_real& o)
   {
      *static_cast<detail::gmp_real_imp<0>*>(this) = static_cast<detail::gmp_real_imp<0> const&>(o);
      return *this;
   }
#ifndef BOOST_NO_RVALUE_REFERENCES
   gmp_real& operator=(gmp_real&& o)
   {
      *static_cast<detail::gmp_real_imp<0>*>(this) = static_cast<detail::gmp_real_imp<0> &&>(o);
      return *this;
   }
#endif
   template <class V>
   gmp_real& operator=(const V& v)
   {
      *static_cast<detail::gmp_real_imp<0>*>(this) = v;
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
      return mpf_get_prec(this->m_data) * 301L / 1000 - 1;
   }
   void precision(unsigned digits10)
   {
      mpf_set_prec(this->m_data, (digits10 + 1) * 1000L / 301);
   }
private:
   static unsigned& get_default_precision()
   {
      static unsigned val = 50;
      return val;
   }
};

template <unsigned digits10>
inline void add(gmp_real<digits10>& result, const gmp_real<digits10>& o)
{
   mpf_add(result.data(), result.data(), o.data());
}
template <unsigned digits10>
inline void subtract(gmp_real<digits10>& result, const gmp_real<digits10>& o)
{
   mpf_sub(result.data(), result.data(), o.data());
}
template <unsigned digits10>
inline void multiply(gmp_real<digits10>& result, const gmp_real<digits10>& o)
{
   mpf_mul(result.data(), result.data(), o.data());
}
template <unsigned digits10>
inline void divide(gmp_real<digits10>& result, const gmp_real<digits10>& o)
{
   mpf_div(result.data(), result.data(), o.data());
}
template <unsigned digits10>
inline void add(gmp_real<digits10>& result, unsigned long i)
{
   mpf_add_ui(result.data(), result.data(), i);
}
template <unsigned digits10>
inline void subtract(gmp_real<digits10>& result, unsigned long i)
{
   mpf_sub_ui(result.data(), result.data(), i);
}
template <unsigned digits10>
inline void multiply(gmp_real<digits10>& result, unsigned long i)
{
   mpf_mul_ui(result.data(), result.data(), i);
}
template <unsigned digits10>
inline void divide(gmp_real<digits10>& result, unsigned long i)
{
   mpf_div_ui(result.data(), result.data(), i);
}
template <unsigned digits10>
inline void add(gmp_real<digits10>& result, long i)
{
   if(i > 0)
      mpf_add_ui(result.data(), result.data(), i);
   else
      mpf_sub_ui(result.data(), result.data(), std::abs(i));
}
template <unsigned digits10>
inline void subtract(gmp_real<digits10>& result, long i)
{
   if(i > 0)
      mpf_sub_ui(result.data(), result.data(), i);
   else
      mpf_add_ui(result.data(), result.data(), std::abs(i));
}
template <unsigned digits10>
inline void multiply(gmp_real<digits10>& result, long i)
{
   mpf_mul_ui(result.data(), result.data(), std::abs(i));
   if(i < 0)
      mpf_neg(result.data(), result.data());
}
template <unsigned digits10>
inline void divide(gmp_real<digits10>& result, long i)
{
   mpf_div_ui(result.data(), result.data(), std::abs(i));
   if(i < 0)
      mpf_neg(result.data(), result.data());
}
//
// Specialised 3 arg versions of the basic operators:
//
template <unsigned digits10>
inline void add(gmp_real<digits10>& a, const gmp_real<digits10>& x, const gmp_real<digits10>& y)
{
   mpf_add(a.data(), x.data(), y.data());
}
template <unsigned digits10>
inline void add(gmp_real<digits10>& a, const gmp_real<digits10>& x, unsigned long y)
{
   mpf_add_ui(a.data(), x.data(), y);
}
template <unsigned digits10>
inline void add(gmp_real<digits10>& a, const gmp_real<digits10>& x, long y)
{
   if(y < 0)
      mpf_sub_ui(a.data(), x.data(), -y);
   else
      mpf_add_ui(a.data(), x.data(), y);
}
template <unsigned digits10>
inline void add(gmp_real<digits10>& a, unsigned long x, const gmp_real<digits10>& y)
{
   mpf_add_ui(a.data(), y.data(), x);
}
template <unsigned digits10>
inline void add(gmp_real<digits10>& a, long x, const gmp_real<digits10>& y)
{
   if(x < 0)
   {
      mpf_ui_sub(a.data(), -x, y.data());
      mpf_neg(a.data(), a.data());
   }
   else
      mpf_add_ui(a.data(), y.data(), x);
}
template <unsigned digits10>
inline void subtract(gmp_real<digits10>& a, const gmp_real<digits10>& x, const gmp_real<digits10>& y)
{
   mpf_sub(a.data(), x.data(), y.data());
}
template <unsigned digits10>
inline void subtract(gmp_real<digits10>& a, const gmp_real<digits10>& x, unsigned long y)
{
   mpf_sub_ui(a.data(), x.data(), y);
}
template <unsigned digits10>
inline void subtract(gmp_real<digits10>& a, const gmp_real<digits10>& x, long y)
{
   if(y < 0)
      mpf_add_ui(a.data(), x.data(), -y);
   else
      mpf_sub_ui(a.data(), x.data(), y);
}
template <unsigned digits10>
inline void subtract(gmp_real<digits10>& a, unsigned long x, const gmp_real<digits10>& y)
{
   mpf_ui_sub(a.data(), x, y.data());
}
template <unsigned digits10>
inline void subtract(gmp_real<digits10>& a, long x, const gmp_real<digits10>& y)
{
   if(x < 0)
   {
      mpf_add_ui(a.data(), y.data(), -x);
      mpf_neg(a.data(), a.data());
   }
   else
      mpf_ui_sub(a.data(), x, y.data());
}

template <unsigned digits10>
inline void multiply(gmp_real<digits10>& a, const gmp_real<digits10>& x, const gmp_real<digits10>& y)
{
   mpf_mul(a.data(), x.data(), y.data());
}
template <unsigned digits10>
inline void multiply(gmp_real<digits10>& a, const gmp_real<digits10>& x, unsigned long y)
{
   mpf_mul_ui(a.data(), x.data(), y);
}
template <unsigned digits10>
inline void multiply(gmp_real<digits10>& a, const gmp_real<digits10>& x, long y)
{
   if(y < 0)
   {
      mpf_mul_ui(a.data(), x.data(), -y);
      a.negate();
   }
   else
      mpf_mul_ui(a.data(), x.data(), y);
}
template <unsigned digits10>
inline void multiply(gmp_real<digits10>& a, unsigned long x, const gmp_real<digits10>& y)
{
   mpf_mul_ui(a.data(), y.data(), x);
}
template <unsigned digits10>
inline void multiply(gmp_real<digits10>& a, long x, const gmp_real<digits10>& y)
{
   if(x < 0)
   {
      mpf_mul_ui(a.data(), y.data(), -x);
      mpf_neg(a.data(), a.data());
   }
   else
      mpf_mul_ui(a.data(), y.data(), x);
}

template <unsigned digits10>
inline void divide(gmp_real<digits10>& a, const gmp_real<digits10>& x, const gmp_real<digits10>& y)
{
   mpf_div(a.data(), x.data(), y.data());
}
template <unsigned digits10>
inline void divide(gmp_real<digits10>& a, const gmp_real<digits10>& x, unsigned long y)
{
   mpf_div_ui(a.data(), x.data(), y);
}
template <unsigned digits10>
inline void divide(gmp_real<digits10>& a, const gmp_real<digits10>& x, long y)
{
   if(y < 0)
   {
      mpf_div_ui(a.data(), x.data(), -y);
      a.negate();
   }
   else
      mpf_div_ui(a.data(), x.data(), y);
}
template <unsigned digits10>
inline void divide(gmp_real<digits10>& a, unsigned long x, const gmp_real<digits10>& y)
{
   mpf_ui_div(a.data(), x, y.data());
}
template <unsigned digits10>
inline void divide(gmp_real<digits10>& a, long x, const gmp_real<digits10>& y)
{
   if(x < 0)
   {
      mpf_ui_div(a.data(), -x, y.data());
      mpf_neg(a.data(), a.data());
   }
   else
      mpf_ui_div(a.data(), x, y.data());
}

//
// Native non-member operations:
//
template <unsigned Digits10>
inline void sqrt(gmp_real<Digits10>* result, const gmp_real<Digits10>& val)
{
   mpf_sqrt(result->data(), val.data());
}

template <unsigned Digits10>
inline void abs(gmp_real<Digits10>* result, const gmp_real<Digits10>& val)
{
   mpf_abs(result->data(), val.data());
}

template <unsigned Digits10>
inline void fabs(gmp_real<Digits10>* result, const gmp_real<Digits10>& val)
{
   mpf_abs(result->data(), val.data());
}
template <unsigned Digits10>
inline void ceil(gmp_real<Digits10>* result, const gmp_real<Digits10>& val)
{
   mpf_ceil(result->data(), val.data());
}
template <unsigned Digits10>
inline void floor(gmp_real<Digits10>* result, const gmp_real<Digits10>& val)
{
   mpf_floor(result->data(), val.data());
}
template <unsigned Digits10>
inline void trunc(gmp_real<Digits10>* result, const gmp_real<Digits10>& val)
{
   mpf_trunc(result->data(), val.data());
}
template <unsigned Digits10>
inline void ldexp(gmp_real<Digits10>* result, const gmp_real<Digits10>& val, long e)
{
   if(e > 0)
      mpf_mul_2exp(result->data(), val.data(), e);
   else if(e < 0)
      mpf_div_2exp(result->data(), val.data(), -e);
}
template <unsigned Digits10>
inline void frexp(gmp_real<Digits10>* result, const gmp_real<Digits10>& val, int* e)
{
   long v;
   mpf_get_d_2exp(&v, val.data());
   *e = v;
   return ldexp(result, val, -v);
}
template <unsigned Digits10>
inline void frexp(gmp_real<Digits10>* result, const gmp_real<Digits10>& val, long* e)
{
   mpf_get_d_2exp(e, val.data());
   return ldexp(result, val, -*e);
}

struct gmp_int
{
   typedef mpl::list<long, long long>                 signed_types;
   typedef mpl::list<unsigned long, unsigned long long>   unsigned_types;
   typedef mpl::list<double, long double>            real_types;

   gmp_int()
   {
      mpz_init(this->m_data);
   }
   gmp_int(const gmp_int& o)
   {
      mpz_init_set(m_data, o.m_data);
   }
   gmp_int& operator = (const gmp_int& o)
   {
      mpz_set(m_data, o.m_data);
      return *this;
   }
   gmp_int& operator = (boost::uintmax_t i)
   {
      boost::uintmax_t mask = ((1uLL << std::numeric_limits<unsigned>::digits) - 1);
      unsigned shift = 0;
      mpz_t t;
      mpz_init(m_data);
      mpz_init(t);
      while(i)
      {
         mpz_set_ui(t, static_cast<unsigned>(i & mask));
         if(shift)
            mpz_mul_2exp(t, t, shift);
         mpz_add(m_data, m_data, t);
         shift += std::numeric_limits<unsigned>::digits;
         i >>= std::numeric_limits<unsigned>::digits;
      }
      mpz_clear(t);
      return *this;
   }
   gmp_int& operator = (boost::intmax_t i)
   {
      bool neg = i < 0;
      *this = static_cast<boost::uintmax_t>(std::abs(i));
      if(neg)
         mpz_neg(m_data, m_data);
      return *this;
   }
   gmp_int& operator = (unsigned long i)
   {
      mpz_set_ui(m_data, i);
      return *this;
   }
   gmp_int& operator = (long i)
   {
      mpz_set_si(m_data, i);
      return *this;
   }
   gmp_int& operator = (double d)
   {
      mpz_set_d(m_data, d);
      return *this;
   }
   gmp_int& operator = (long double a)
   {
      using std::frexp;
      using std::ldexp;
      using std::floor;

      if (a == 0) {
         mpz_set_si(m_data, 0);
         return *this;
      }

      if (a == 1) {
         mpz_set_si(m_data, 1);
         return *this;
      }

      BOOST_ASSERT(!(boost::math::isinf)(a));
      BOOST_ASSERT(!(boost::math::isnan)(a));

      int e;
      long double f, term;
      mpz_init_set_ui(m_data, 0u);

      f = frexp(a, &e);

      static const int shift = std::numeric_limits<int>::digits - 1;

      while(f)
      {
         // extract int sized bits from f:
         f = ldexp(f, shift);
         term = floor(f);
         e -= shift;
         mpz_mul_2exp(m_data, m_data, shift);
         if(term > 0)
            mpz_add_ui(m_data, m_data, static_cast<unsigned>(term));
         else
            mpz_sub_ui(m_data, m_data, static_cast<unsigned>(-term));
         f -= term;
      }
      if(e > 0)
         mpz_mul_2exp(m_data, m_data, e);
      else if(e < 0)
         mpz_div_2exp(m_data, m_data, -e);
      return *this;
   }
   gmp_int& operator = (const char* s)
   {
      mpz_set_str(m_data, s, 10);
      return *this;
   }
   void swap(gmp_int& o)
   {
      mpz_swap(m_data, o.m_data);
   }
   std::string str(unsigned /*digits*/, bool /*scientific*/)const
   {
      void *(*alloc_func_ptr) (size_t);
      void *(*realloc_func_ptr) (void *, size_t, size_t);
      void (*free_func_ptr) (void *, size_t);
      const char* ps = mpz_get_str (0, 10, m_data);
      std::string s = ps;
      mp_get_memory_functions(&alloc_func_ptr, &realloc_func_ptr, &free_func_ptr);
      (*free_func_ptr)((void*)ps, std::strlen(ps) + 1);
      return s;
   }
   ~gmp_int()
   {
      mpz_clear(m_data);
   }
   void negate()
   {
      mpz_neg(m_data, m_data);
   }
   int compare(const gmp_int& o)const
   {
      return mpz_cmp(m_data, o.m_data);
   }
   int compare(long i)const
   {
      return mpz_cmp_si(m_data, i);
   }
   int compare(unsigned long i)const
   {
      return mpz_cmp_ui(m_data, i);
   }
   template <class V>
   int compare(V v)const
   {
      gmp_int d;
      d = v;
      return compare(d);
   }
   mpz_t& data() { return m_data; }
   const mpz_t& data()const { return m_data; }
protected:
   mpz_t m_data;
};

inline void add(gmp_int& t, const gmp_int& o)
{
   mpz_add(t.data(), t.data(), o.data());
}
inline void subtract(gmp_int& t, const gmp_int& o)
{
   mpz_sub(t.data(), t.data(), o.data());
}
inline void multiply(gmp_int& t, const gmp_int& o)
{
   mpz_mul(t.data(), t.data(), o.data());
}
inline void divide(gmp_int& t, const gmp_int& o)
{
   mpz_div(t.data(), t.data(), o.data());
}
inline void modulus(gmp_int& t, const gmp_int& o)
{
   bool neg = mpz_sgn(t.data()) < 0;
   bool neg2 = mpz_sgn(o.data()) < 0;
   mpz_mod(t.data(), t.data(), o.data());
   if(neg)
   {
      if(!neg2)
         t.negate();
      mpz_add(t.data(), t.data(), o.data());
      if(!neg2)
         t.negate();
   }
}
inline void add(gmp_int& t, unsigned long i)
{
   mpz_add_ui(t.data(), t.data(), i);
}
inline void subtract(gmp_int& t, unsigned long i)
{
   mpz_sub_ui(t.data(), t.data(), i);
}
inline void multiply(gmp_int& t, unsigned long i)
{
   mpz_mul_ui(t.data(), t.data(), i);
}
inline void modulus(gmp_int& t, unsigned long i)
{
   bool neg = mpz_sgn(t.data()) < 0;
   mpz_mod_ui(t.data(), t.data(), i);
   if(neg)
   {
      t.negate();
      mpz_add_ui(t.data(), t.data(), i);
      t.negate();
   }
}
inline void divide(gmp_int& t, unsigned long i)
{
   mpz_div_ui(t.data(), t.data(), i);
}
inline void add(gmp_int& t, long i)
{
   if(i > 0)
      mpz_add_ui(t.data(), t.data(), i);
   else
      mpz_sub_ui(t.data(), t.data(), -i);
}
inline void subtract(gmp_int& t, long i)
{
   if(i > 0)
      mpz_sub_ui(t.data(), t.data(), i);
   else
      mpz_add_ui(t.data(), t.data(), -i);
}
inline void multiply(gmp_int& t, long i)
{
   mpz_mul_ui(t.data(), t.data(), std::abs(i));
   if(i < 0)
      mpz_neg(t.data(), t.data());
}
inline void modulus(gmp_int& t, long i)
{
   bool neg = mpz_sgn(t.data()) < 0;
   bool neg2 = i < 0;
   mpz_mod_ui(t.data(), t.data(), std::abs(i));
   if(neg)
   {
      if(!neg2)
      {
         t.negate();
         mpz_add_ui(t.data(), t.data(), std::abs(i));
         t.negate();
      }
      else
      {
         mpz_sub_ui(t.data(), t.data(), std::abs(i));
      }
   }
}
inline void divide(gmp_int& t, long i)
{
   mpz_div_ui(t.data(), t.data(), std::abs(i));
   if(i < 0)
      mpz_neg(t.data(), t.data());
}

inline void add(gmp_int& t, const gmp_int& p, const gmp_int& o)
{
   mpz_add(t.data(), p.data(), o.data());
}
inline void subtract(gmp_int& t, const gmp_int& p, const gmp_int& o)
{
   mpz_sub(t.data(), p.data(), o.data());
}
inline void multiply(gmp_int& t, const gmp_int& p, const gmp_int& o)
{
   mpz_mul(t.data(), p.data(), o.data());
}
inline void divide(gmp_int& t, const gmp_int& p, const gmp_int& o)
{
   mpz_div(t.data(), p.data(), o.data());
}
inline void modulus(gmp_int& t, const gmp_int& p, const gmp_int& o)
{
   bool neg = mpz_sgn(p.data()) < 0;
   bool neg2 = mpz_sgn(o.data()) < 0;
   mpz_mod(t.data(), p.data(), o.data());
   if(neg)
   {
      if(!neg2)
         t.negate();
      mpz_add(t.data(), t.data(), o.data());
      if(!neg2)
         t.negate();
   }
}
inline void add(gmp_int& t, const gmp_int& p, unsigned long i)
{
   mpz_add_ui(t.data(), p.data(), i);
}
inline void subtract(gmp_int& t, const gmp_int& p, unsigned long i)
{
   mpz_sub_ui(t.data(), p.data(), i);
}
inline void multiply(gmp_int& t, const gmp_int& p, unsigned long i)
{
   mpz_mul_ui(t.data(), p.data(), i);
}
inline void modulus(gmp_int& t, const gmp_int& p, unsigned long i)
{
   bool neg = mpz_sgn(p.data()) < 0;
   mpz_mod_ui(t.data(), p.data(), i);
   if(neg)
   {
      t.negate();
      mpz_add_ui(t.data(), t.data(), i);
      t.negate();
   }
}
inline void divide(gmp_int& t, const gmp_int& p, unsigned long i)
{
   mpz_div_ui(t.data(), p.data(), i);
}
inline void add(gmp_int& t, const gmp_int& p, long i)
{
   if(i > 0)
      mpz_add_ui(t.data(), p.data(), i);
   else
      mpz_sub_ui(t.data(), p.data(), -i);
}
inline void subtract(gmp_int& t, const gmp_int& p, long i)
{
   if(i > 0)
      mpz_sub_ui(t.data(), p.data(), i);
   else
      mpz_add_ui(t.data(), p.data(), -i);
}
inline void multiply(gmp_int& t, const gmp_int& p, long i)
{
   mpz_mul_ui(t.data(), p.data(), std::abs(i));
   if(i < 0)
      mpz_neg(t.data(), t.data());
}
inline void modulus(gmp_int& t, const gmp_int& p, long i)
{
   bool neg = mpz_sgn(p.data()) < 0;
   bool neg2 = i < 0;
   mpz_mod_ui(t.data(), p.data(), std::abs(i));
   if(neg)
   {
      if(!neg2)
      {
         t.negate();
         mpz_add_ui(t.data(), t.data(), std::abs(i));
         t.negate();
      }
      else
      {
         mpz_sub_ui(t.data(), t.data(), std::abs(i));
      }
   }
}
inline void divide(gmp_int& t, const gmp_int& p, long i)
{
   mpz_div_ui(t.data(), p.data(), std::abs(i));
   if(i < 0)
      mpz_neg(t.data(), t.data());
}
   
inline void abs(gmp_int* result, const gmp_int& val)
{
   mpz_abs(result->data(), val.data());
}

template<>
struct is_extended_integer<gmp_int> : public mpl::true_ {};

typedef big_number<gmp_real<50> >    mpf_real_50;
typedef big_number<gmp_real<100> >   mpf_real_100;
typedef big_number<gmp_real<500> >   mpf_real_500;
typedef big_number<gmp_real<1000> >  mpf_real_1000;
typedef big_number<gmp_real<0> >     mpf_real;
typedef big_number<gmp_int >         mpz_int;

}}  // namespaces

#endif
