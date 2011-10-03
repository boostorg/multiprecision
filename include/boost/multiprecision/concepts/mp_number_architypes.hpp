///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MATH_CONCEPTS_ER_HPP
#define BOOST_MATH_CONCEPTS_ER_HPP

#include <iostream>
#include <iomanip>
#include <cmath>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/multiprecision/mp_number.hpp>
#include <boost/math/special_functions/trunc.hpp>

namespace boost{
namespace multiprecision{
namespace concepts{

#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4244)
#endif

struct mp_number_backend_real_architype
{
   typedef mpl::list<long long>                 signed_types;
   typedef mpl::list<unsigned long long>        unsigned_types;
   typedef mpl::list<long double>               real_types;

   mp_number_backend_real_architype()
   {
      std::cout << "Default construct" << std::endl;
   }
   mp_number_backend_real_architype(const mp_number_backend_real_architype& o)
   {
      std::cout << "Copy construct" << std::endl;
      m_value = o.m_value;
   }
   mp_number_backend_real_architype& operator = (const mp_number_backend_real_architype& o)
   {
      m_value = o.m_value;
      std::cout << "Assignment (" << m_value << ")" << std::endl;
      return *this;
   }
   mp_number_backend_real_architype& operator = (boost::uintmax_t i)
   {
      m_value = i;
      std::cout << "UInt Assignment (" << i << ")" << std::endl;
      return *this;
   }
   mp_number_backend_real_architype& operator = (boost::intmax_t i)
   {
      m_value = i;
      std::cout << "Int Assignment (" << i << ")" << std::endl;
      return *this;
   }
   mp_number_backend_real_architype& operator = (long double d)
   {
      m_value = d;
      std::cout << "long double Assignment (" << d << ")" << std::endl;
      return *this;
   }
   mp_number_backend_real_architype& operator = (const char* s)
   {
      m_value = boost::lexical_cast<long double>(s);
      std::cout << "const char* Assignment (" << s << ")" << std::endl;
      return *this;
   }
   void swap(mp_number_backend_real_architype& o)
   {
      std::cout << "Swapping (" << m_value << " with " << o.m_value << ")" << std::endl;
      std::swap(m_value, o.m_value);
   }
   std::string str(unsigned digits, bool scientific)const
   {
      std::stringstream ss;
      if(scientific)
         ss.setf(ss.scientific);
      if(digits)
         ss.precision(digits);
      else
         ss.precision(std::numeric_limits<long double>::digits10 + 2);
      boost::intmax_t i = m_value;
      boost::uintmax_t u = m_value;
      if(!scientific && m_value == i)
         ss << i;
      else if(!scientific && m_value == u)
         ss << u;
      else
         ss << m_value;
      std::string s = ss.str();
      std::cout << "Converting to string (" << s << ")" << std::endl;
      return s;
   }
   void negate()
   {
      std::cout << "Negating (" << m_value << ")" << std::endl;
      m_value = -m_value;
   }
   int compare(const mp_number_backend_real_architype& o)const
   {
      std::cout << "Comparison" << std::endl;
      return m_value > o.m_value ? 1 : (m_value < o.m_value ? -1 : 0);
   }
   int compare(boost::intmax_t i)const
   {
      std::cout << "Comparison with int" << std::endl;
      return m_value > i ? 1 : (m_value < i ? -1 : 0);
   }
   int compare(boost::uintmax_t i)const
   {
      std::cout << "Comparison with unsigned" << std::endl;
      return m_value > i ? 1 : (m_value < i ? -1 : 0);
   }
   int compare(long double d)const
   {
      std::cout << "Comparison with long double" << std::endl;
      return m_value > d ? 1 : (m_value < d ? -1 : 0);
   }
   long double m_value;
};

inline void add(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& o)
{
   std::cout << "Addition (" << result.m_value << " += " << o.m_value << ")" << std::endl;
   result.m_value += o.m_value;
}
inline void subtract(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& o)
{
   std::cout << "Subtraction (" << result.m_value << " -= " << o.m_value << ")" << std::endl;
   result.m_value -= o.m_value;
}
inline void multiply(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& o)
{
   std::cout << "Multiplication (" << result.m_value << " *= " << o.m_value << ")" << std::endl;
   result.m_value *= o.m_value;
}
inline void divide(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& o)
{
   std::cout << "Division (" << result.m_value << " /= " << o.m_value << ")" << std::endl;
   result.m_value /= o.m_value;
}

inline void convert_to(boost::uintmax_t* result, const mp_number_backend_real_architype& val)
{
   *result = static_cast<boost::uintmax_t>(val.m_value);
}
inline void convert_to(boost::intmax_t* result, const mp_number_backend_real_architype& val)
{
   *result = static_cast<boost::intmax_t>(val.m_value);
}
inline void convert_to(long double* result, mp_number_backend_real_architype& val)
{
   *result = val.m_value;
}

inline void eval_frexp(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg, int* exp)
{
   result = std::frexp(arg.m_value, exp);
}

inline void eval_ldexp(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg, int exp)
{
   result = std::ldexp(arg.m_value, exp);
}

inline void eval_floor(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::floor(arg.m_value);
}

inline void eval_ceil(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::ceil(arg.m_value);
}

inline void eval_trunc(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = boost::math::trunc(arg.m_value);
}

inline void eval_sqrt(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::sqrt(arg.m_value);
}

inline void eval_abs(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::abs(arg.m_value);
}

inline void eval_fabs(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::fabs(arg.m_value);
}

inline int eval_fpclassify(const mp_number_backend_real_architype& arg)
{
   return boost::math::fpclassify(arg.m_value);
}

inline void eval_pow(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& b, const mp_number_backend_real_architype& e)
{
   result = std::pow(b.m_value, e.m_value);
}

inline void eval_pow(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& b, int e)
{
   result = std::pow(b.m_value, e);
}

inline void eval_exp(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::exp(arg.m_value);
}

inline void eval_log(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::log(arg.m_value);
}

inline void eval_sin(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::sin(arg.m_value);
}

inline void eval_cos(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::cos(arg.m_value);
}

inline void eval_tan(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::tan(arg.m_value);
}

inline void eval_asin(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::asin(arg.m_value);
}

inline void eval_acos(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::acos(arg.m_value);
}

inline void eval_atan(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::atan(arg.m_value);
}

inline void eval_sinh(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::sinh(arg.m_value);
}

inline void eval_cosh(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::cosh(arg.m_value);
}

inline void eval_tanh(mp_number_backend_real_architype& result, const mp_number_backend_real_architype& arg)
{
   result = std::tanh(arg.m_value);
}

typedef boost::multiprecision::mp_number<mp_number_backend_real_architype> mp_number_real_architype;

}}} // namespaces

namespace std{

#ifdef BOOST_NO_NOEXCEPT
#  define noexcept
#endif

template <>
class numeric_limits<boost::multiprecision::concepts::mp_number_real_architype> : public std::numeric_limits<long double>
{
   typedef std::numeric_limits<long double> base_type;
   typedef boost::multiprecision::concepts::mp_number_real_architype number_type;
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

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#endif
