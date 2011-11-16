///////////////////////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_MP_TOMMATH_BACKEND_HPP
#define BOOST_MATH_MP_TOMMATH_BACKEND_HPP

#include <boost/multiprecision/mp_number.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_array.hpp>
#include <tommath.h>
#include <cmath>
#include <limits>
#include <climits>

namespace boost{ namespace multiprecision{

struct tommath_int
{
   typedef mpl::list<long, long long>                     signed_types;
   typedef mpl::list<unsigned long, unsigned long long>   unsigned_types;
   typedef mpl::list<long double>                 float_types;

   tommath_int()
   {
      mp_init(&m_data);
   }
   tommath_int(const tommath_int& o)
   {
      mp_init_copy(&m_data, const_cast<::mp_int*>(&o.m_data));
   }
   tommath_int& operator = (const tommath_int& o)
   {
      mp_copy(const_cast<::mp_int*>(&o.m_data), &m_data);
      return *this;
   }
   tommath_int& operator = (boost::uintmax_t i)
   {
      boost::uintmax_t mask = ((1uLL << std::numeric_limits<unsigned>::digits) - 1);
      unsigned shift = 0;
      ::mp_int t;
      mp_init(&t);
      mp_zero(&m_data);
      while(i)
      {
         mp_set_int(&t, static_cast<unsigned>(i & mask));
         if(shift)
            mp_mul_2d(&t, shift, &t);
         mp_add(&m_data, &t, &m_data);
         shift += std::numeric_limits<unsigned>::digits;
         i >>= std::numeric_limits<unsigned>::digits;
      }
      mp_clear(&t);
      return *this;
   }
   tommath_int& operator = (boost::intmax_t i)
   {
      bool neg = i < 0;
      *this = static_cast<boost::uintmax_t>(std::abs(i));
      if(neg)
         mp_neg(&m_data, &m_data);
      return *this;
   }
   tommath_int& operator = (unsigned long i)
   {
      mp_set_int(&m_data, i);
      return *this;
   }
   tommath_int& operator = (long i)
   {
      bool neg = i < 0;
      *this = static_cast<unsigned long>(std::abs(i));
      if(neg)
         mp_neg(&m_data, &m_data);
      return *this;
   }
   tommath_int& operator = (long double a)
   {
      using std::frexp;
      using std::ldexp;
      using std::floor;

      if (a == 0) {
         mp_set_int(&m_data, 0);
         return *this;
      }

      if (a == 1) {
         mp_set_int(&m_data, 1);
         return *this;
      }

      BOOST_ASSERT(!(boost::math::isinf)(a));
      BOOST_ASSERT(!(boost::math::isnan)(a));

      int e;
      long double f, term;
      mp_init_set_int(&m_data, 0u);
      ::mp_int t;
      mp_init(&t);

      f = frexp(a, &e);

      static const int shift = std::numeric_limits<int>::digits - 1;

      while(f)
      {
         // extract int sized bits from f:
         f = ldexp(f, shift);
         term = floor(f);
         e -= shift;
         mp_mul_2d(&m_data, shift, &m_data);
         if(term > 0)
         {
            mp_set_int(&t, static_cast<int>(term));
            mp_add(&m_data, &t, &m_data);
         }
         else
         {
            mp_set_int(&t, static_cast<int>(-term));
            mp_sub(&m_data, &t, &m_data);
         }
         f -= term;
      }
      if(e > 0)
         mp_mul_2d(&m_data, e, &m_data);
      else if(e < 0)
      {
         tommath_int t2;
         mp_div_2d(&m_data, -e, &m_data, &t2.data());
      }
      return *this;
   }
   tommath_int& operator = (const char* s)
   {
      mp_read_radix(&m_data, s, 10);
      return *this;
   }
   std::string str(unsigned /*digits*/, bool /*scientific*/)const
   {
      int s;
      mp_radix_size(const_cast<::mp_int*>(&m_data), 10, &s);
      boost::scoped_array<char> a(new char[s+1]);
      mp_toradix_n(const_cast<::mp_int*>(&m_data), a.get(), 10, s+1);
      return a.get();
   }
   ~tommath_int()
   {
      mp_clear(&m_data);
   }
   void negate()
   {
      mp_neg(&m_data, &m_data);
   }
   int compare(const tommath_int& o)const
   {
      return mp_cmp(const_cast<::mp_int*>(&m_data), const_cast<::mp_int*>(&o.m_data));
   }
   template <class V>
   int compare(V v)const
   {
      tommath_int d;
      tommath_int t(*this);
      mp_shrink(&t.data());
      d = v;
      return t.compare(d);
   }
   mp_int& data() { return m_data; }
   const mp_int& data()const { return m_data; }
   void swap(tommath_int& o)
   {
      mp_exch(&m_data, &o.data());
   }
protected:
   mp_int m_data;
};

int get_sign(const tommath_int& val);

inline void add(tommath_int& t, const tommath_int& o)
{
   mp_add(&t.data(), const_cast<::mp_int*>(&o.data()), &t.data());
}
inline void subtract(tommath_int& t, const tommath_int& o)
{
   mp_sub(&t.data(), const_cast<::mp_int*>(&o.data()), &t.data());
}
inline void multiply(tommath_int& t, const tommath_int& o)
{
   mp_mul(&t.data(), const_cast<::mp_int*>(&o.data()), &t.data());
}
inline void divide(tommath_int& t, const tommath_int& o)
{
   tommath_int temp;
   mp_div(&t.data(), const_cast<::mp_int*>(&o.data()), &t.data(), &temp.data());
}
inline void modulus(tommath_int& t, const tommath_int& o)
{
   bool neg = get_sign(t) < 0;
   bool neg2 = get_sign(o) < 0;
   mp_mod(&t.data(), const_cast<::mp_int*>(&o.data()), &t.data());
   if(neg != neg2)
   {
      t.negate();
      mp_add(&t.data(), const_cast<::mp_int*>(&o.data()), &t.data());
      t.negate();
   }
}
template <class UI>
inline void left_shift(tommath_int& t, UI i)
{
   mp_mul_2d(&t.data(), static_cast<unsigned>(i), &t.data());
}
template <class UI>
inline void right_shift(tommath_int& t, UI i)
{
   tommath_int d;
   mp_div_2d(&t.data(), static_cast<unsigned>(i), &t.data(), &d.data());
}
template <class UI>
inline void left_shift(tommath_int& t, const tommath_int& v, UI i)
{
   mp_mul_2d(const_cast<::mp_int*>(&v.data()), static_cast<unsigned>(i), &t.data());
}
template <class UI>
inline void right_shift(tommath_int& t, const tommath_int& v, UI i)
{
   tommath_int d;
   mp_div_2d(const_cast<::mp_int*>(&v.data()), static_cast<unsigned long>(i), &t.data(), &d.data());
}

inline void bitwise_and(tommath_int& result, const tommath_int& v)
{
   mp_and(&result.data(), const_cast<::mp_int*>(&v.data()), &result.data());
}

inline void bitwise_or(tommath_int& result, const tommath_int& v)
{
   mp_or(&result.data(), const_cast<::mp_int*>(&v.data()), &result.data());
}

inline void bitwise_xor(tommath_int& result, const tommath_int& v)
{
   mp_xor(&result.data(), const_cast<::mp_int*>(&v.data()), &result.data());
}

inline void add(tommath_int& t, const tommath_int& p, const tommath_int& o)
{
   mp_add(const_cast<::mp_int*>(&p.data()), const_cast<::mp_int*>(&o.data()), &t.data());
}
inline void subtract(tommath_int& t, const tommath_int& p, const tommath_int& o)
{
   mp_sub(const_cast<::mp_int*>(&p.data()), const_cast<::mp_int*>(&o.data()), &t.data());
}
inline void multiply(tommath_int& t, const tommath_int& p, const tommath_int& o)
{
   mp_mul(const_cast<::mp_int*>(&p.data()), const_cast<::mp_int*>(&o.data()), &t.data());
}
inline void divide(tommath_int& t, const tommath_int& p, const tommath_int& o)
{
   tommath_int d;
   mp_div(const_cast<::mp_int*>(&p.data()), const_cast<::mp_int*>(&o.data()), &t.data(), &d.data());
}
inline void modulus(tommath_int& t, const tommath_int& p, const tommath_int& o)
{
   bool neg = get_sign(p) < 0;
   bool neg2 = get_sign(o) < 0;
   mp_mod(const_cast<::mp_int*>(&p.data()), const_cast<::mp_int*>(&o.data()), &t.data());
   if(neg != neg2)
   {
      t.negate();
      mp_add(&t.data(), const_cast<::mp_int*>(&o.data()), &t.data());
      t.negate();
   }
}

inline void bitwise_and(tommath_int& result, const tommath_int& u, const tommath_int& v)
{
   mp_and(const_cast<::mp_int*>(&u.data()), const_cast<::mp_int*>(&v.data()), &result.data());
}

inline void bitwise_or(tommath_int& result, const tommath_int& u, const tommath_int& v)
{
   mp_or(const_cast<::mp_int*>(&u.data()), const_cast<::mp_int*>(&v.data()), &result.data());
}

inline void bitwise_xor(tommath_int& result, const tommath_int& u, const tommath_int& v)
{
   mp_xor(const_cast<::mp_int*>(&u.data()), const_cast<::mp_int*>(&v.data()), &result.data());
}

inline void complement(tommath_int& result, const tommath_int& u)
{
   result = u;
   for(int i = 0; i < result.data().used; ++i)
   {
      result.data().dp[i] = MP_MASK & ~(result.data().dp[i]);
   }
   //
   // We now need to pad out the left of the value with 1's to round up to a whole number of
   // CHAR_BIT * sizeof(mp_digit) units.  Otherwise we'll end up with a very strange number of
   // bits set!
   //
   unsigned shift = result.data().used * DIGIT_BIT;    // How many bits we're actually using
   // How many bits we actually need, reduced by one to account for a mythical sign bit:
   unsigned padding = result.data().used * std::numeric_limits<mp_digit>::digits - shift - 1; 
   while(padding >= std::numeric_limits<mp_digit>::digits) 
      padding -= std::numeric_limits<mp_digit>::digits;

   // Create a mask providing the extra bits we need and add to result:
   tommath_int mask;
   mask = static_cast<long long>((1u << padding) - 1);
   left_shift(mask, shift);
   add(result, mask);
}

inline bool is_zero(const tommath_int& val)
{
   return mp_iszero(&val.data());
}
inline int get_sign(const tommath_int& val)
{
   return mp_iszero(&val.data()) ? 0 : SIGN(&val.data()) ? -1 : 1;
}
template <class A>
inline void convert_to(A* result, const tommath_int& val)
{
   *result = boost::lexical_cast<A>(val.str(0, false));
}
inline void convert_to(char* result, const tommath_int& val)
{
   *result = static_cast<char>(boost::lexical_cast<int>(val.str(0, false)));
}
inline void convert_to(unsigned char* result, const tommath_int& val)
{
   *result = static_cast<unsigned char>(boost::lexical_cast<unsigned>(val.str(0, false)));
}
inline void convert_to(signed char* result, const tommath_int& val)
{
   *result = static_cast<signed char>(boost::lexical_cast<int>(val.str(0, false)));
}
inline void eval_abs(tommath_int& result, const tommath_int& val)
{
   mp_abs(const_cast<::mp_int*>(&val.data()), &result.data());
}


template<>
struct number_category<tommath_int> : public mpl::int_<number_kind_integer>{};

typedef mp_number<tommath_int >         mp_int;

}}  // namespaces

namespace std{

template<> 
class numeric_limits<boost::multiprecision::mp_int >
{
   typedef boost::multiprecision::mp_int number_type;
public:
   BOOST_STATIC_CONSTEXPR bool is_specialized = true;
   //
   // Largest and smallest numbers are bounded only by available memory, set
   // to zero:
   //
   BOOST_STATIC_CONSTEXPR number_type (min)() BOOST_MP_NOEXCEPT
   { 
      return number_type();
   }
   BOOST_STATIC_CONSTEXPR number_type (max)() BOOST_MP_NOEXCEPT 
   { 
      return number_type();
   }
   BOOST_STATIC_CONSTEXPR number_type lowest() BOOST_MP_NOEXCEPT { return (min)(); }
   BOOST_STATIC_CONSTEXPR int digits = INT_MAX;
   BOOST_STATIC_CONSTEXPR int digits10 = (INT_MAX / 1000) * 301L;
   BOOST_STATIC_CONSTEXPR int max_digits10 = digits10 + 2;
   BOOST_STATIC_CONSTEXPR bool is_signed = true;
   BOOST_STATIC_CONSTEXPR bool is_integer = true;
   BOOST_STATIC_CONSTEXPR bool is_exact = true;
   BOOST_STATIC_CONSTEXPR int radix = 2;
   BOOST_STATIC_CONSTEXPR number_type epsilon() BOOST_MP_NOEXCEPT { return number_type(); }
   BOOST_STATIC_CONSTEXPR number_type round_error() BOOST_MP_NOEXCEPT { return number_type(); }
   BOOST_STATIC_CONSTEXPR int min_exponent = 0;
   BOOST_STATIC_CONSTEXPR int min_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent = 0;
   BOOST_STATIC_CONSTEXPR int max_exponent10 = 0;
   BOOST_STATIC_CONSTEXPR bool has_infinity = false;
   BOOST_STATIC_CONSTEXPR bool has_quiet_NaN = false;
   BOOST_STATIC_CONSTEXPR bool has_signaling_NaN = false;
   BOOST_STATIC_CONSTEXPR float_denorm_style has_denorm = denorm_absent;
   BOOST_STATIC_CONSTEXPR bool has_denorm_loss = false;
   BOOST_STATIC_CONSTEXPR number_type infinity() BOOST_MP_NOEXCEPT { return number_type(); }
   BOOST_STATIC_CONSTEXPR number_type quiet_NaN() BOOST_MP_NOEXCEPT { return number_type(); }
   BOOST_STATIC_CONSTEXPR number_type signaling_NaN() BOOST_MP_NOEXCEPT { return number_type(); }
   BOOST_STATIC_CONSTEXPR number_type denorm_min() BOOST_MP_NOEXCEPT { return number_type(); }
   BOOST_STATIC_CONSTEXPR bool is_iec559 = false;
   BOOST_STATIC_CONSTEXPR bool is_bounded = false;
   BOOST_STATIC_CONSTEXPR bool is_modulo = false;
   BOOST_STATIC_CONSTEXPR bool traps = false;
   BOOST_STATIC_CONSTEXPR bool tinyness_before = false;
   BOOST_STATIC_CONSTEXPR float_round_style round_style = round_toward_zero;
};

}

#endif
