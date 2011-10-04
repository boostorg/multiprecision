///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MATH_ARITH_BACKEND_HPP
#define BOOST_MATH_ARITH_BACKEND_HPP

#include <iostream>
#include <iomanip>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/multiprecision/mp_number.hpp>

namespace boost{
namespace multiprecision{

template <class Arithmetic>
struct arithmetic_backend
{
   typedef mpl::list<long long>                 signed_types;
   typedef mpl::list<unsigned long long>        unsigned_types;
   typedef mpl::list<long double>               real_types;
   typedef int                                  exponent_type;

   arithmetic_backend(){}
   arithmetic_backend(const arithmetic_backend& o)
   {
      m_value = o.m_value;
   }
   arithmetic_backend(const Arithmetic& o) : m_value(o) {}
#ifndef BOOST_NO_RVALUE_REFERENCES
   arithmetic_backend(arithmetic_backend&& o) : m_value(o.m_value) {}
   arithmetic_backend(Arithmetic&& o) : m_value(o) {}
#endif
   arithmetic_backend& operator = (const arithmetic_backend& o)
   {
      m_value = o.m_value;
      return *this;
   }
   arithmetic_backend& operator = (boost::uintmax_t i)
   {
      m_value = i;
      return *this;
   }
   arithmetic_backend& operator = (boost::intmax_t i)
   {
      m_value = i;
      return *this;
   }
   arithmetic_backend& operator = (long double d)
   {
      m_value = d;
      return *this;
   }
   arithmetic_backend& operator = (const char* s)
   {
      m_value = boost::lexical_cast<double>(s);
      return *this;
   }
   void swap(arithmetic_backend& o)
   {
      std::swap(m_value, o.m_value);
   }
   std::string str(unsigned digits, bool scientific)const
   {
      return boost::lexical_cast<std::string>(m_value);
   }
   void negate()
   {
      m_value = -m_value;
   }
   int compare(const arithmetic_backend& o)const
   {
      return m_value > o.m_value ? 1 : (m_value < o.m_value ? -1 : 0);
   }
   int compare(boost::intmax_t i)const
   {
      return m_value > i ? 1 : (m_value < i ? -1 : 0);
   }
   int compare(boost::uintmax_t i)const
   {
      return m_value > i ? 1 : (m_value < i ? -1 : 0);
   }
   int compare(long double d)const
   {
      return m_value > d ? 1 : (m_value < d ? -1 : 0);
   }
   Arithmetic& data() { return m_value; }
   const Arithmetic& data()const { return m_value; }
private:
   Arithmetic m_value;
};

template <class Arithmetic>
inline void add(arithmetic_backend<Arithmetic>& result, const arithmetic_backend<Arithmetic>& o)
{
   result.data() += o.data();
}
template <class Arithmetic>
inline void subtract(arithmetic_backend<Arithmetic>& result, const arithmetic_backend<Arithmetic>& o)
{
   result.data() -= o.data();
}
template <class Arithmetic>
inline void multiply(arithmetic_backend<Arithmetic>& result, const arithmetic_backend<Arithmetic>& o)
{
   result.data() *= o.data();
}
template <class Arithmetic>
inline void divide(arithmetic_backend<Arithmetic>& result, const arithmetic_backend<Arithmetic>& o)
{
   result.data() /= o.data();
}

}} // namespaces


namespace std{

#ifdef BOOST_NO_NOEXCEPT
#  define noexcept
#endif

template <class Arithmetic>
class numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::arithmetic_backend<Arithmetic> > > : public std::numeric_limits<Arithmetic>
{
   typedef std::numeric_limits<Arithmetic> base_type;
   typedef boost::multiprecision::mp_number<boost::multiprecision::arithmetic_backend<Arithmetic> > number_type;
public:
   BOOST_STATIC_CONSTEXPR number_type (min)() noexcept { return (base_type::min)(); }
   BOOST_STATIC_CONSTEXPR number_type (max)() noexcept { return (base_type::max)(); }
   BOOST_STATIC_CONSTEXPR number_type lowest() noexcept { return -(max)(); }
   BOOST_STATIC_CONSTEXPR number_type epsilon() noexcept { return base_type::epsilon(); }
   BOOST_STATIC_CONSTEXPR number_type round_error() noexcept { return epsilon() / 2; }
   BOOST_STATIC_CONSTEXPR number_type infinity() noexcept { return base_type::infinity(); }
   BOOST_STATIC_CONSTEXPR number_type quiet_NaN() noexcept { return base_type::quiet_NaN(); }
   BOOST_STATIC_CONSTEXPR number_type signaling_NaN() noexcept { return base_type::signaling_NaN(); }
   BOOST_STATIC_CONSTEXPR number_type denorm_min() noexcept { return base_type::denorm_min(); }
};

#ifdef BOOST_NO_NOEXCEPT
#  undef noexcept
#endif

}

#endif
