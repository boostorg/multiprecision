///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MATH_CONCEPTS_ER_HPP
#define BOOST_MATH_CONCEPTS_ER_HPP

#include <iostream>
#include <iomanip>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/big_number.hpp>

namespace boost{
namespace math{
namespace concepts{

struct big_number_backend_real_architype
{
   typedef mpl::list<long long>                 signed_types;
   typedef mpl::list<unsigned long long>        unsigned_types;
   typedef mpl::list<long double>               real_types;

   big_number_backend_real_architype()
   {
      std::cout << "Default construct" << std::endl;
   }
   big_number_backend_real_architype(const big_number_backend_real_architype& o)
   {
      std::cout << "Copy construct" << std::endl;
      m_value = o.m_value;
   }
   big_number_backend_real_architype& operator = (const big_number_backend_real_architype& o)
   {
      m_value = o.m_value;
      std::cout << "Assignment (" << m_value << ")" << std::endl;
      return *this;
   }
   big_number_backend_real_architype& operator = (boost::uintmax_t i)
   {
      m_value = i;
      std::cout << "UInt Assignment (" << i << ")" << std::endl;
      return *this;
   }
   big_number_backend_real_architype& operator = (boost::intmax_t i)
   {
      m_value = i;
      std::cout << "Int Assignment (" << i << ")" << std::endl;
      return *this;
   }
   big_number_backend_real_architype& operator = (long double d)
   {
      m_value = d;
      std::cout << "long double Assignment (" << d << ")" << std::endl;
      return *this;
   }
   big_number_backend_real_architype& operator = (const char* s)
   {
      m_value = boost::lexical_cast<double>(s);
      std::cout << "const char* Assignment (" << s << ")" << std::endl;
      return *this;
   }
   void swap(big_number_backend_real_architype& o)
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
   int compare(const big_number_backend_real_architype& o)const
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

inline void add(big_number_backend_real_architype& result, const big_number_backend_real_architype& o)
{
   std::cout << "Addition (" << result.m_value << " += " << o.m_value << ")" << std::endl;
   result.m_value += o.m_value;
}
inline void subtract(big_number_backend_real_architype& result, const big_number_backend_real_architype& o)
{
   std::cout << "Subtraction (" << result.m_value << " -= " << o.m_value << ")" << std::endl;
   result.m_value -= o.m_value;
}
inline void multiply(big_number_backend_real_architype& result, const big_number_backend_real_architype& o)
{
   std::cout << "Multiplication (" << result.m_value << " *= " << o.m_value << ")" << std::endl;
   result.m_value *= o.m_value;
}
inline void divide(big_number_backend_real_architype& result, const big_number_backend_real_architype& o)
{
   std::cout << "Division (" << result.m_value << " /= " << o.m_value << ")" << std::endl;
   result.m_value /= o.m_value;
}

typedef boost::math::big_number<big_number_backend_real_architype> big_number_real_architype;

}}} // namespaces

#endif
