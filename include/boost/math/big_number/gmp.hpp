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
   gmp_real_imp& operator = (const gmp_real_imp& o)
   {
      mpf_set(m_data, o.m_data);
      return *this;
   }
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
   gmp_real_imp& operator += (const gmp_real<digits10>& o)
   {
      mpf_add(m_data, m_data, o.m_data);
      return *this;
   }
   template <class V>
   gmp_real_imp& operator += (V v)
   {
      gmp_real<digits10> d;
      d = v;
      return *this += d;
   }
   gmp_real_imp& operator -= (const gmp_real<digits10>& o)
   {
      mpf_sub(m_data, m_data, o.m_data);
      return *this;
   }
   template <class V>
   gmp_real_imp& operator -= (V v)
   {
      gmp_real<digits10> d;
      d = v;
      return *this -= d;
   }
   gmp_real_imp& operator *= (const gmp_real<digits10>& o)
   {
      mpf_mul(m_data, m_data, o.m_data);
      return *this;
   }
   template <class V>
   gmp_real_imp& operator *= (V v)
   {
      gmp_real<digits10> d;
      d = v;
      return *this *= d;
   }
   gmp_real_imp& operator /= (const gmp_real<digits10>& o)
   {
      mpf_div(m_data, m_data, o.m_data);
      return *this;
   }
   template <class V>
   gmp_real_imp& operator /= (V v)
   {
      gmp_real<digits10> d;
      d = v;
      return *this /= d;
   }
   gmp_real_imp& operator += (unsigned long i)
   {
      mpf_add_ui(m_data, m_data, i);
      return *this;
   }
   gmp_real_imp& operator -= (unsigned long i)
   {
      mpf_sub_ui(m_data, m_data, i);
      return *this;
   }
   gmp_real_imp& operator *= (unsigned long i)
   {
      mpf_mul_ui(m_data, m_data, i);
      return *this;
   }
   gmp_real_imp& operator /= (unsigned long i)
   {
      mpf_div_ui(m_data, m_data, i);
      return *this;
   }
   gmp_real_imp& operator += (long i)
   {
      if(i > 0)
         mpf_add_ui(m_data, m_data, i);
      else
         mpf_sub_ui(m_data, m_data, std::abs(i));
      return *this;
   }
   gmp_real_imp& operator -= (long i)
   {
      if(i > 0)
         mpf_sub_ui(m_data, m_data, i);
      else
         mpf_add_ui(m_data, m_data, std::abs(i));
      return *this;
   }
   gmp_real_imp& operator *= (long i)
   {
      mpf_mul_ui(m_data, m_data, std::abs(i));
      if(i < 0)
         mpf_neg(m_data, m_data);
      return *this;
   }
   gmp_real_imp& operator /= (long i)
   {
      mpf_div_ui(m_data, m_data, std::abs(i));
      if(i < 0)
         mpf_neg(m_data, m_data);
      return *this;
   }
   void swap(gmp_real_imp& o)
   {
      mpf_swap(m_data, o.m_data);
   }
   std::string str()const
   {
      mp_exp_t e;
      void *(*alloc_func_ptr) (size_t);
      void *(*realloc_func_ptr) (void *, size_t, size_t);
      void (*free_func_ptr) (void *, size_t);
      const char* ps = mpf_get_str (0, &e, 10, 0, m_data);
      std::string s("0.");
      if(ps[0] == '-')
      {
         s.insert(0, ps, 1);
         s += ps + 1;
      }
      else
      {
         s += ps;
      }
      s += "e";
      s += boost::lexical_cast<std::string>(e);
      mp_get_memory_functions(&alloc_func_ptr, &realloc_func_ptr, &free_func_ptr);
      (*free_func_ptr)((void*)ps, std::strlen(ps) + 1);
      return s;
   }
   ~gmp_real_imp()
   {
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
   gmp_real(const gmp_real& o) : gmp_real_imp(o) {}

   gmp_real& operator=(const gmp_real& o)
   {
      *static_cast<detail::gmp_real_imp<digits10>*>(this) = static_cast<detail::gmp_real_imp<digits10> const&>(o);
      return *this;
   }

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
   gmp_real(const gmp_real& o) : gmp_real_imp(o) {}
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
   gmp_int& operator += (const gmp_int& o)
   {
      mpz_add(m_data, m_data, o.m_data);
      return *this;
   }
   template <class V>
   gmp_int& operator += (V v)
   {
      gmp_int d;
      d = v;
      return *this += d;
   }
   gmp_int& operator -= (const gmp_int& o)
   {
      mpz_sub(m_data, m_data, o.m_data);
      return *this;
   }
   template <class V>
   gmp_int& operator -= (V v)
   {
      gmp_int d;
      d = v;
      return *this -= d;
   }
   gmp_int& operator *= (const gmp_int& o)
   {
      mpz_mul(m_data, m_data, o.m_data);
      return *this;
   }
   template <class V>
   gmp_int& operator *= (V v)
   {
      gmp_int d;
      d = v;
      return *this *= d;
   }
   gmp_int& operator /= (const gmp_int& o)
   {
      mpz_div(m_data, m_data, o.m_data);
      return *this;
   }
   template <class V>
   gmp_int& operator /= (V v)
   {
      gmp_int d;
      d = v;
      return *this /= d;
   }
   gmp_int& operator %= (const gmp_int& o)
   {
      bool neg = mpz_sgn(m_data) < 0;
      bool neg2 = mpz_sgn(o.m_data) < 0;
      mpz_mod(m_data, m_data, o.m_data);
      if(neg)
      {
         if(!neg2)
            negate();
         mpz_add(m_data, m_data, o.m_data);
         if(!neg2)
            negate();
      }
      return *this;
   }
   template <class V>
   gmp_int& operator %= (V v)
   {
      gmp_int d;
      d = v;
      return *this %= d;
   }
   gmp_int& operator += (unsigned long i)
   {
      mpz_add_ui(m_data, m_data, i);
      return *this;
   }
   gmp_int& operator -= (unsigned long i)
   {
      mpz_sub_ui(m_data, m_data, i);
      return *this;
   }
   gmp_int& operator *= (unsigned long i)
   {
      mpz_mul_ui(m_data, m_data, i);
      return *this;
   }
   gmp_int& operator %= (unsigned long i)
   {
      bool neg = mpz_sgn(m_data) < 0;
      mpz_mod_ui(m_data, m_data, i);
      if(neg)
      {
         negate();
         mpz_add_ui(m_data, m_data, i);
         negate();
      }
      return *this;
   }
   gmp_int& operator /= (unsigned long i)
   {
      mpz_div_ui(m_data, m_data, i);
      return *this;
   }
   gmp_int& operator += (long i)
   {
      if(i > 0)
         mpz_add_ui(m_data, m_data, i);
      else
         mpz_sub_ui(m_data, m_data, std::abs(i));
      return *this;
   }
   gmp_int& operator -= (long i)
   {
      if(i > 0)
         mpz_sub_ui(m_data, m_data, i);
      else
         mpz_add_ui(m_data, m_data, std::abs(i));
      return *this;
   }
   gmp_int& operator *= (long i)
   {
      mpz_mul_ui(m_data, m_data, std::abs(i));
      if(i < 0)
         mpz_neg(m_data, m_data);
      return *this;
   }
   gmp_int& operator %= (long i)
   {
      bool neg = mpz_sgn(m_data) < 0;
      bool neg2 = i < 0;
      mpz_mod_ui(m_data, m_data, std::abs(i));
      if(neg)
      {
         if(!neg2)
         {
            negate();
            mpz_add_ui(m_data, m_data, std::abs(i));
            negate();
         }
         else
         {
            mpz_sub_ui(m_data, m_data, std::abs(i));
         }
      }
      return *this;
   }
   gmp_int& operator /= (long i)
   {
      mpz_div_ui(m_data, m_data, std::abs(i));
      if(i < 0)
         mpz_neg(m_data, m_data);
      return *this;
   }
   void swap(gmp_int& o)
   {
      mpz_swap(m_data, o.m_data);
   }
   std::string str()const
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
protected:
   mpz_t m_data;
};

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
