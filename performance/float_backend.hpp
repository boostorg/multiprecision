///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MATH_FLOAT_BACKEND_HPP
#define BOOST_MATH_FLOAT_BACKEND_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/concepts/real_concept.hpp>
#include <boost/multiprecision/mp_number.hpp>

namespace boost{
namespace multiprecision{
namespace backends{

template <class Arithmetic>
struct float_backend
{
   typedef mpl::list<int, long long>                   signed_types;
   typedef mpl::list<unsigned, unsigned long long>      unsigned_types;
   typedef mpl::list<double, long double>               float_types;
   typedef int                                          exponent_type;

   float_backend(){}
   float_backend(const float_backend& o)
   {
      m_value = o.m_value;
   }
   float_backend(const Arithmetic& o) : m_value(o) {}
#ifndef BOOST_NO_RVALUE_REFERENCES
   float_backend(float_backend&& o) : m_value(o.m_value) {}
   float_backend(Arithmetic&& o) : m_value(o) {}
#endif
   float_backend& operator = (const float_backend& o)
   {
      m_value = o.m_value;
      return *this;
   }
   float_backend& operator = (unsigned long long i)
   {
      m_value = i;
      return *this;
   }
   float_backend& operator = (unsigned i)
   {
      m_value = i;
      return *this;
   }
   float_backend& operator = (long long i)
   {
      m_value = i;
      return *this;
   }
   float_backend& operator = (int i)
   {
      m_value = i;
      return *this;
   }
   float_backend& operator = (long double d)
   {
      m_value = d;
      return *this;
   }
   float_backend& operator = (double d)
   {
      m_value = d;
      return *this;
   }
   float_backend& operator = (const char* s)
   {
      m_value = boost::lexical_cast<double>(s);
      return *this;
   }
   void swap(float_backend& o)
   {
      std::swap(m_value, o.m_value);
   }
   std::string str(std::streamsize digits, std::ios_base::fmtflags f)const
   {
      std::stringstream ss;
      ss.flags(f);
      ss << std::setprecision(digits) << m_value;
      return ss.str();
   }
   void negate()
   {
      m_value = -m_value;
   }
   int compare(const float_backend& o)const
   {
      return m_value > o.m_value ? 1 : (m_value < o.m_value ? -1 : 0);
   }
   int compare(long long i)const
   {
      return m_value > i ? 1 : (m_value < i ? -1 : 0);
   }
   int compare(int i)const
   {
      return m_value > i ? 1 : (m_value < i ? -1 : 0);
   }
   int compare(unsigned long long i)const
   {
      return m_value > i ? 1 : (m_value < i ? -1 : 0);
   }
   int compare(unsigned i)const
   {
      return m_value > i ? 1 : (m_value < i ? -1 : 0);
   }
   int compare(long double d)const
   {
      return m_value > d ? 1 : (m_value < d ? -1 : 0);
   }
   int compare(double d)const
   {
      return m_value > d ? 1 : (m_value < d ? -1 : 0);
   }
   Arithmetic& data() { return m_value; }
   const Arithmetic& data()const { return m_value; }
private:
   Arithmetic m_value;
};

template <class Arithmetic>
inline void eval_add(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   result.data() += o.data();
}
template <class Arithmetic>
inline void eval_subtract(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   result.data() -= o.data();
}
template <class Arithmetic>
inline void eval_multiply(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   result.data() *= o.data();
}
template <class Arithmetic>
inline void eval_divide(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   result.data() /= o.data();
}

template <class Arithmetic, class A2>
inline typename enable_if<is_arithmetic<A2> >::type eval_add(float_backend<Arithmetic>& result, const A2& o)
{
   result.data() += o;
}
template <class Arithmetic, class A2>
inline typename enable_if<is_arithmetic<A2> >::type eval_subtract(float_backend<Arithmetic>& result, const A2& o)
{
   result.data() -= o;
}
template <class Arithmetic, class A2>
inline typename enable_if<is_arithmetic<A2> >::type eval_multiply(float_backend<Arithmetic>& result, const A2& o)
{
   result.data() *= o;
}
template <class Arithmetic, class A2>
inline typename enable_if<is_arithmetic<A2> >::type eval_divide(float_backend<Arithmetic>& result, const A2& o)
{
   result.data() /= o;
}

template <class Arithmetic>
inline void eval_add(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& a, const float_backend<Arithmetic>& b)
{
   result.data() = a.data() + b.data();
}
template <class Arithmetic>
inline void eval_subtract(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& a, const float_backend<Arithmetic>& b)
{
   result.data() = a.data() - b.data();
}
template <class Arithmetic>
inline void eval_multiply(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& a, const float_backend<Arithmetic>& b)
{
   result.data() = a.data() * b.data();
}
template <class Arithmetic>
inline void eval_divide(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& a, const float_backend<Arithmetic>& b)
{
   result.data() = a.data() / b.data();
}

template <class Arithmetic, class A2>
inline typename enable_if<is_arithmetic<A2>>::type eval_add(float_backend<Arithmetic>& result, const A2& a, const float_backend<Arithmetic>& b)
{
   result.data() = a + b.data();
}
template <class Arithmetic, class A2>
inline typename enable_if<is_arithmetic<A2>>::type eval_subtract(float_backend<Arithmetic>& result, const A2& a, const float_backend<Arithmetic>& b)
{
   result.data() = a - b.data();
}
template <class Arithmetic, class A2>
inline typename enable_if<is_arithmetic<A2>>::type eval_multiply(float_backend<Arithmetic>& result, const A2& a, const float_backend<Arithmetic>& b)
{
   result.data() = a * b.data();
}
template <class Arithmetic, class A2>
inline typename enable_if<is_arithmetic<A2>>::type eval_divide(float_backend<Arithmetic>& result, const A2& a, const float_backend<Arithmetic>& b)
{
   result.data() = a / b.data();
}

template <class Arithmetic>
inline bool eval_is_zero(const float_backend<Arithmetic>& val)
{
   return val.data() == 0;
}

template <class Arithmetic>
inline int eval_get_sign(const float_backend<Arithmetic>& val)
{
   return val.data() == 0 ? 0 : val.data() < 0 ? -1 : 1;
}

template <class Arithmetic>
inline void eval_abs(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   result.data() = std::abs(o.data());
}

template <class Arithmetic>
inline void eval_fabs(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   result.data() = std::abs(o.data());
}

template <class Arithmetic>
inline void eval_floor(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = floor(o.data());
}

template <class Arithmetic>
inline void eval_ceil(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = ceil(o.data());
}

template <class Arithmetic>
inline void eval_sqrt(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = sqrt(o.data());
}

template <class Arithmetic>
inline int eval_fpclassify(const float_backend<Arithmetic>& o)
{
   return (boost::math::fpclassify)(o.data());
}

template <class Arithmetic>
inline void eval_trunc(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = trunc(o.data());
}

template <class Arithmetic>
inline void eval_round(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = round(o.data());
}

template <class Arithmetic>
inline void eval_exp(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = exp(o.data());
}

template <class Arithmetic>
inline void eval_log(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = log(o.data());
}

template <class Arithmetic>
inline void eval_log10(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = log10(o.data());
}

template <class Arithmetic>
inline void eval_sin(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = sin(o.data());
}

template <class Arithmetic>
inline void eval_cos(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = cos(o.data());
}

template <class Arithmetic>
inline void eval_tan(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = tan(o.data());
}

template <class Arithmetic>
inline void eval_acos(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = acos(o.data());
}

template <class Arithmetic>
inline void eval_asin(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = asin(o.data());
}

template <class Arithmetic>
inline void eval_atan(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = atan(o.data());
}

template <class Arithmetic>
inline void eval_sinh(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = sinh(o.data());
}

template <class Arithmetic>
inline void eval_cosh(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = cosh(o.data());
}

template <class Arithmetic>
inline void eval_tanh(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& o)
{
   BOOST_MATH_STD_USING
   result.data() = tanh(o.data());
}

template <class Arithmetic>
inline void eval_fmod(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& a, const float_backend<Arithmetic>& b)
{
   BOOST_MATH_STD_USING
   result.data() = fmod(a.data(), b.data());
}

template <class Arithmetic>
inline void eval_pow(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& a, const float_backend<Arithmetic>& b)
{
   BOOST_MATH_STD_USING
   result.data() = pow(a.data(), b.data());
}

template <class Arithmetic>
inline void eval_atan2(float_backend<Arithmetic>& result, const float_backend<Arithmetic>& a, const float_backend<Arithmetic>& b)
{
   BOOST_MATH_STD_USING
   result.data() = atan2(a.data(), b.data());
}

} // namespace backends

using boost::multiprecision::backends::float_backend;

}} // namespaces

namespace boost{ namespace math{ namespace tools{

template <>
inline double real_cast<double, concepts::real_concept>(concepts::real_concept r)
{
   return static_cast<double>(r.value());
}

}}}


namespace std{

template <class Arithmetic, bool ExpressionTemplates>
class numeric_limits<boost::multiprecision::mp_number<boost::multiprecision::float_backend<Arithmetic>, ExpressionTemplates > > : public std::numeric_limits<Arithmetic>
{
   typedef std::numeric_limits<Arithmetic> base_type;
   typedef boost::multiprecision::mp_number<boost::multiprecision::float_backend<Arithmetic>, ExpressionTemplates> number_type;
public:
   BOOST_STATIC_CONSTEXPR number_type (min)() BOOST_NOEXCEPT { return (base_type::min)(); }
   BOOST_STATIC_CONSTEXPR number_type (max)() BOOST_NOEXCEPT { return (base_type::max)(); }
   BOOST_STATIC_CONSTEXPR number_type lowest() BOOST_NOEXCEPT { return -(max)(); }
   BOOST_STATIC_CONSTEXPR number_type epsilon() BOOST_NOEXCEPT { return base_type::epsilon(); }
   BOOST_STATIC_CONSTEXPR number_type round_error() BOOST_NOEXCEPT { return epsilon() / 2; }
   BOOST_STATIC_CONSTEXPR number_type infinity() BOOST_NOEXCEPT { return base_type::infinity(); }
   BOOST_STATIC_CONSTEXPR number_type quiet_NaN() BOOST_NOEXCEPT { return base_type::quiet_NaN(); }
   BOOST_STATIC_CONSTEXPR number_type signaling_NaN() BOOST_NOEXCEPT { return base_type::signaling_NaN(); }
   BOOST_STATIC_CONSTEXPR number_type denorm_min() BOOST_NOEXCEPT { return base_type::denorm_min(); }
};

template<>
class numeric_limits<boost::math::concepts::real_concept> : public std::numeric_limits<long double>
{
   typedef std::numeric_limits<long double> base_type;
   typedef boost::math::concepts::real_concept number_type;
public:
   BOOST_STATIC_CONSTEXPR number_type (min)() BOOST_NOEXCEPT { return (base_type::min)(); }
   BOOST_STATIC_CONSTEXPR number_type (max)() BOOST_NOEXCEPT { return (base_type::max)(); }
   BOOST_STATIC_CONSTEXPR number_type lowest() BOOST_NOEXCEPT { return -(max)(); }
   BOOST_STATIC_CONSTEXPR number_type epsilon() BOOST_NOEXCEPT { return base_type::epsilon(); }
   BOOST_STATIC_CONSTEXPR number_type round_error() BOOST_NOEXCEPT { return epsilon() / 2; }
   BOOST_STATIC_CONSTEXPR number_type infinity() BOOST_NOEXCEPT { return base_type::infinity(); }
   BOOST_STATIC_CONSTEXPR number_type quiet_NaN() BOOST_NOEXCEPT { return base_type::quiet_NaN(); }
   BOOST_STATIC_CONSTEXPR number_type signaling_NaN() BOOST_NOEXCEPT { return base_type::signaling_NaN(); }
   BOOST_STATIC_CONSTEXPR number_type denorm_min() BOOST_NOEXCEPT { return base_type::denorm_min(); }
};

}

#endif
