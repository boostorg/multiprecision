///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MATH_RATIONAL_ADAPTER_HPP
#define BOOST_MATH_RATIONAL_ADAPTER_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/multiprecision/mp_number.hpp>
#include <boost/rational.hpp>

namespace boost{
namespace multiprecision{

template <class IntBackend>
struct rational_adapter
{
   typedef mp_number<IntBackend>                integer_type;
   typedef boost::rational<integer_type>        rational_type;

   typedef typename IntBackend::signed_types    signed_types;
   typedef typename IntBackend::unsigned_types  unsigned_types;
   typedef typename IntBackend::float_types     float_types;

   rational_adapter(){}
   rational_adapter(const rational_adapter& o)
   {
      m_value = o.m_value;
   }
   rational_adapter(const IntBackend& o) : m_value(o) {}
#ifndef BOOST_NO_RVALUE_REFERENCES
   rational_adapter(rational_adapter&& o) : m_value(o.m_value) {}
   rational_adapter(IntBackend&& o) : m_value(o) {}
#endif
   rational_adapter& operator = (const rational_adapter& o)
   {
      m_value = o.m_value;
      return *this;
   }
   template <class Arithmetic>
   typename enable_if<is_arithmetic<Arithmetic>, rational_adapter&>::type operator = (Arithmetic i)
   {
      m_value = i;
      return *this;
   }
   rational_adapter& operator = (const char* s)
   {
      std::string s1;
      multiprecision::mp_number<IntBackend> v1, v2;
      char c;
      bool have_hex = false;

      while((0 != (c = *s)) && (c == 'x' || c == 'X' || c == '-' || c == '+' || (c >= '0' && c <= '9') || (have_hex && (c >= 'a' && c <= 'f')) || (have_hex && (c >= 'A' && c <= 'F'))))
      {
         if(c == 'x' || c == 'X')
            have_hex = true;
         s1.append(1, c);
         ++s;
      }
      v1 = s1;
      s1.erase();
      if(c == '/')
      {
         ++s;
         while((0 != (c = *s)) && (c == 'x' || c == 'X' || c == '-' || c == '+' || (c >= '0' && c <= '9') || (have_hex && (c >= 'a' && c <= 'f')) || (have_hex && (c >= 'A' && c <= 'F'))))
         {
            if(c == 'x' || c == 'X')
               have_hex = true;
            s1.append(1, c);
            ++s;
         }
         v2 = s1;
      }
      else
         v2 = 1;
      data().assign(v1, v2);
      return *this;
   }
   void swap(rational_adapter& o)
   {
      std::swap(m_value, o.m_value);
   }
   std::string str(std::streamsize digits, std::ios_base::fmtflags f)const
   {
      //
      // We format the string ourselves so we can match what GMP's mpq type does:
      //
      std::string result = data().numerator().str(digits, f);
      if(data().denominator() != 1)
      {
         result.append(1, '/');
         result.append(data().denominator().str(digits, f));
      }
      return result;
   }
   void negate()
   {
      m_value = -m_value;
   }
   int compare(const rational_adapter& o)const
   {
      return m_value > o.m_value ? 1 : (m_value < o.m_value ? -1 : 0);
   }
   template <class Arithmatic>
   typename enable_if<is_arithmetic<Arithmatic>, int>::type compare(Arithmatic i)const
   {
      return m_value > i ? 1 : (m_value < i ? -1 : 0);
   }
   rational_type& data() { return m_value; }
   const rational_type& data()const { return m_value; }
private:
   rational_type m_value;
};

template <class IntBackend>
inline void add(rational_adapter<IntBackend>& result, const rational_adapter<IntBackend>& o)
{
   result.data() += o.data();
}
template <class IntBackend>
inline void subtract(rational_adapter<IntBackend>& result, const rational_adapter<IntBackend>& o)
{
   result.data() -= o.data();
}
template <class IntBackend>
inline void multiply(rational_adapter<IntBackend>& result, const rational_adapter<IntBackend>& o)
{
   result.data() *= o.data();
}
template <class IntBackend>
inline void divide(rational_adapter<IntBackend>& result, const rational_adapter<IntBackend>& o)
{
   result.data() /= o.data();
}

template <class R, class IntBackend>
inline void convert_to(R* result, const rational_adapter<IntBackend>& backend)
{
   *result = backend.data().numerator().template convert_to<R>();
   *result /= backend.data().denominator().template convert_to<R>();
}

template <class IntBackend>
inline bool is_zero(const rational_adapter<IntBackend>& val)
{
   return is_zero(val.data().numerator().backend());
}
template <class IntBackend>
inline int get_sign(const rational_adapter<IntBackend>& val)
{
   return get_sign(val.data().numerator().backend());
}

template <class IntBackend>
inline mp_number<IntBackend> numerator(const mp_number<rational_adapter<IntBackend> >& val)
{
   return val.backend().data().numerator();
}
template <class IntBackend>
inline mp_number<IntBackend> denominator(const mp_number<rational_adapter<IntBackend> >& val)
{
   return val.backend().data().denominator();
}

template<class IntBackend>
struct number_category<rational_adapter<IntBackend> > : public mpl::int_<number_kind_rational>{};

}} // namespaces


namespace std{

template <class IntBackend>
class numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::rational_adapter<IntBackend> > > : public std::numeric_limits<IntBackend>
{
   typedef std::numeric_limits<IntBackend> base_type;
   typedef boost::multiprecision::mp_number<boost::multiprecision::rational_adapter<IntBackend> > number_type;
public:
   BOOST_STATIC_CONSTEXPR bool is_integer = false;
   BOOST_STATIC_CONSTEXPR bool is_exact = true;
   BOOST_STATIC_CONSTEXPR number_type (min)() BOOST_MP_NOEXCEPT { return (base_type::min)(); }
   BOOST_STATIC_CONSTEXPR number_type (max)() BOOST_MP_NOEXCEPT { return (base_type::max)(); }
   BOOST_STATIC_CONSTEXPR number_type lowest() BOOST_MP_NOEXCEPT { return -(max)(); }
   BOOST_STATIC_CONSTEXPR number_type epsilon() BOOST_MP_NOEXCEPT { return base_type::epsilon(); }
   BOOST_STATIC_CONSTEXPR number_type round_error() BOOST_MP_NOEXCEPT { return epsilon() / 2; }
   BOOST_STATIC_CONSTEXPR number_type infinity() BOOST_MP_NOEXCEPT { return base_type::infinity(); }
   BOOST_STATIC_CONSTEXPR number_type quiet_NaN() BOOST_MP_NOEXCEPT { return base_type::quiet_NaN(); }
   BOOST_STATIC_CONSTEXPR number_type signaling_NaN() BOOST_MP_NOEXCEPT { return base_type::signaling_NaN(); }
   BOOST_STATIC_CONSTEXPR number_type denorm_min() BOOST_MP_NOEXCEPT { return base_type::denorm_min(); }
};

}

#endif
