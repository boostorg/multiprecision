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
   typedef mpl::list<int, long long>                 signed_types;
   typedef mpl::list<unsigned, unsigned long long>   unsigned_types;
   typedef mpl::list<double, long double>            real_types;

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
   big_number_backend_real_architype& operator = (unsigned i)
   {
      m_value = i;
      std::cout << "UInt Assignment (" << i << ")" << std::endl;
      return *this;
   }
   big_number_backend_real_architype& operator = (int i)
   {
      m_value = i;
      std::cout << "Int Assignment (" << i << ")" << std::endl;
      return *this;
   }
   big_number_backend_real_architype& operator = (double d)
   {
      m_value = d;
      std::cout << "double Assignment (" << d << ")" << std::endl;
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
   big_number_backend_real_architype& operator += (const big_number_backend_real_architype& o)
   {
      std::cout << "Addition (" << m_value << " += " << o.m_value << ")" << std::endl;
      m_value += o.m_value;
      return *this;
   }
   big_number_backend_real_architype& operator -= (const big_number_backend_real_architype& o)
   {
      std::cout << "Subtraction (" << m_value << " -= " << o.m_value << ")" << std::endl;
      m_value -= o.m_value;
      return *this;
   }
   big_number_backend_real_architype& operator *= (const big_number_backend_real_architype& o)
   {
      std::cout << "Multiplication (" << m_value << " *= " << o.m_value << ")" << std::endl;
      m_value *= o.m_value;
      return *this;
   }
   big_number_backend_real_architype& operator /= (const big_number_backend_real_architype& o)
   {
      std::cout << "Division (" << m_value << " /= " << o.m_value << ")" << std::endl;
      m_value /= o.m_value;
      return *this;
   }
   big_number_backend_real_architype& operator += (boost::uintmax_t i)
   {
      std::cout << "UIntmax_t Addition (" << m_value << " += " << i << ")" << std::endl;
      m_value += i;
      return *this;
   }
   big_number_backend_real_architype& operator -= (boost::uintmax_t i)
   {
      std::cout << "UIntmax_t Subtraction (" << m_value << " -= " << i << ")" << std::endl;
      m_value -= i;
      return *this;
   }
   big_number_backend_real_architype& operator *= (boost::uintmax_t i)
   {
      std::cout << "UIntmax_t Multiplication (" << m_value << " *= " << i << ")" << std::endl;
      m_value *= i;
      return *this;
   }
   big_number_backend_real_architype& operator /= (boost::uintmax_t i)
   {
      std::cout << "UIntmax_t Division (" << m_value << " /= " << i << ")" << std::endl;
      m_value /= i;
      return *this;
   }
   big_number_backend_real_architype& operator += (boost::intmax_t i)
   {
      std::cout << "Intmax_t Addition (" << m_value << " += " << i << ")" << std::endl;
      m_value += i;
      return *this;
   }
   big_number_backend_real_architype& operator -= (boost::intmax_t i)
   {
      std::cout << "Intmax_t Subtraction (" << m_value << " -= " << i << ")" << std::endl;
      m_value -= i;
      return *this;
   }
   big_number_backend_real_architype& operator *= (boost::intmax_t i)
   {
      std::cout << "Intmax_t Multiplication (" << m_value << " *= " << i << ")" << std::endl;
      m_value *= i;
      return *this;
   }
   big_number_backend_real_architype& operator /= (boost::intmax_t i)
   {
      std::cout << "Intmax_t Division (" << m_value << " /= " << i << ")" << std::endl;
      m_value /= i;
      return *this;
   }
   big_number_backend_real_architype& operator += (unsigned i)
   {
      std::cout << "UInt Addition (" << m_value << " += " << i << ")" << std::endl;
      m_value += i;
      return *this;
   }
   big_number_backend_real_architype& operator -= (unsigned i)
   {
      std::cout << "UInt Subtraction (" << m_value << " -= " << i << ")" << std::endl;
      m_value -= i;
      return *this;
   }
   big_number_backend_real_architype& operator *= (unsigned i)
   {
      std::cout << "UInt Multiplication (" << m_value << " *= " << i << ")" << std::endl;
      m_value *= i;
      return *this;
   }
   big_number_backend_real_architype& operator /= (unsigned i)
   {
      std::cout << "UInt Division (" << m_value << " /= " << i << ")" << std::endl;
      m_value /= i;
      return *this;
   }
   big_number_backend_real_architype& operator += (int i)
   {
      std::cout << "Int Addition (" << m_value << " += " << i << ")" << std::endl;
      m_value += i;
      return *this;
   }
   big_number_backend_real_architype& operator -= (int i)
   {
      std::cout << "Int Subtraction (" << m_value << " -= " << i << ")" << std::endl;
      m_value -= i;
      return *this;
   }
   big_number_backend_real_architype& operator *= (int i)
   {
      std::cout << "Int Multiplication (" << m_value << " *= " << i << ")" << std::endl;
      m_value *= i;
      return *this;
   }
   big_number_backend_real_architype& operator /= (int i)
   {
      std::cout << "Int Division (" << m_value << " /= " << i << ")" << std::endl;
      m_value /= i;
      return *this;
   }
   big_number_backend_real_architype& operator += (double d)
   {
      std::cout << "double Addition (" << m_value << " += " << d << ")" << std::endl;
      m_value += d;
      return *this;
   }
   big_number_backend_real_architype& operator -= (double d)
   {
      std::cout << "double Subtraction (" << m_value << " -= " << d << ")" << std::endl;
      m_value -= d;
      return *this;
   }
   big_number_backend_real_architype& operator *= (double d)
   {
      std::cout << "double Multiplication (" << m_value << " *= " << d << ")" << std::endl;
      m_value *= d;
      return *this;
   }
   big_number_backend_real_architype& operator /= (double d)
   {
      std::cout << "double Division (" << m_value << " /= " << d << ")" << std::endl;
      m_value /= d;
      return *this;
   }
   big_number_backend_real_architype& operator += (long double d)
   {
      std::cout << "long double Addition (" << m_value << " += " << d << ")" << std::endl;
      m_value += d;
      return *this;
   }
   big_number_backend_real_architype& operator -= (long double d)
   {
      std::cout << "long double Subtraction (" << m_value << " -= " << d << ")" << std::endl;
      m_value -= d;
      return *this;
   }
   big_number_backend_real_architype& operator *= (long double d)
   {
      std::cout << "long double Multiplication (" << m_value << " *= " << d << ")" << std::endl;
      m_value *= d;
      return *this;
   }
   big_number_backend_real_architype& operator /= (long double d)
   {
      std::cout << "long double Division (" << m_value << " /= " << d << ")" << std::endl;
      m_value /= d;
      return *this;
   }
   void swap(big_number_backend_real_architype& o)
   {
      std::cout << "Swapping (" << m_value << " with " << o.m_value << ")" << std::endl;
      std::swap(m_value, o.m_value);
   }
   std::string str(unsigned)const
   {
      std::string s(boost::lexical_cast<std::string>(m_value));
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
   int compare(int i)const
   {
      std::cout << "Comparison with int" << std::endl;
      return m_value > i ? 1 : (m_value < i ? -1 : 0);
   }
   int compare(unsigned i)const
   {
      std::cout << "Comparison with unsigned" << std::endl;
      return m_value > i ? 1 : (m_value < i ? -1 : 0);
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
   int compare(double d)const
   {
      std::cout << "Comparison with double" << std::endl;
      return m_value > d ? 1 : (m_value < d ? -1 : 0);
   }
   int compare(long double d)const
   {
      std::cout << "Comparison with long double" << std::endl;
      return m_value > d ? 1 : (m_value < d ? -1 : 0);
   }
private:
   long double m_value;
};

typedef boost::math::big_number<big_number_backend_real_architype> big_number_real_architype;

}}} // namespaces

#endif
