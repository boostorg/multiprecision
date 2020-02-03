///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_MATH_FLOAT_BACKEND_HPP
#define BOOST_MATH_FLOAT_BACKEND_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/cstdint.hpp>
#include <boost/multiprecision/number.hpp>
#include <boost/multiprecision/detail/float_string_cvt.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/type_traits/common_type.hpp>
#include <boost/math/special_functions/trunc.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/math/special_functions/cbrt.hpp>
#include <qd/qd_real.h>

namespace boost {
namespace multiprecision {
namespace backends {

#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable : 4389 4244 4018 4244 4127)
#endif

struct quad_double_backend
{
   typedef mpl::list<short, long long>                                 signed_types;
   typedef mpl::list<unsigned short, unsigned long long>               unsigned_types;
   typedef mpl::list<double, long double>                              float_types;
   typedef int                                                         exponent_type;

   quad_double_backend() : m_value(0) {}
   quad_double_backend(qd_real const& r) : m_value(r) {}
   quad_double_backend(const char* s)
   {
      *this = s;
   }
   quad_double_backend(const quad_double_backend& o) : m_value(o.m_value) {}
   quad_double_backend(short i)
   {
      m_value[0] = i;
      m_value[1] = m_value[2] = m_value[3] = 0;
   }
   quad_double_backend(double i)
   {
      m_value[0] = i;
      m_value[1] = m_value[2] = m_value[3] = 0;
   }
   quad_double_backend(unsigned short i)
   {
      m_value[0] = i;
      m_value[1] = m_value[2] = m_value[3] = 0;
   }
   quad_double_backend(long long i)
   {
      *this = i < 0 ? static_cast<unsigned long long>(-i) : static_cast<unsigned long long>(i);
      if (i < 0)
         this->negate();
   }
   quad_double_backend(unsigned long long i)
   {
      *this = i;
   }
   quad_double_backend& operator=(const quad_double_backend& o)
   {
      m_value = o.m_value;
      return *this;
   }
   quad_double_backend& operator=(const qd_real& o)
   {
      m_value = o;
      return *this;
   }
   quad_double_backend& operator=(unsigned long long i)
   {
      static const unsigned long long mask = (2uLL << 50) - 1;

      m_value[0]                           = static_cast<double>(i & mask);
      m_value[1] = m_value[2] = m_value[3] = 0;
      i >>= 50;
      int shift = 50;
      while (i)
      {
         m_value += std::ldexp(static_cast<double>(i & mask), shift);
         i >>= 50;
         shift += 50;
      }
      return *this;
   }
   quad_double_backend& operator=(long long i)
   {
      *this = static_cast<unsigned long long>(i > 0 ? i : -i);
      if (i < 0)
         this->negate();
      return *this;
   }
   quad_double_backend& operator=(short i)
   {
      m_value[0] = i;
      m_value[1] = m_value[2] = m_value[3] = 0;
      return *this;
   }
   quad_double_backend& operator=(unsigned short i)
   {
      m_value[0] = i;
      m_value[1] = m_value[2] = m_value[3] = 0;
      return *this;
   }
   quad_double_backend& operator=(long double i)
   {
      bool isneg = i < 0;
      if (isneg)
         i = -i;
      m_value[0] = static_cast<double>(i);
      m_value[1] = m_value[2] = m_value[3] = 0;
      i -= m_value[0];
      while (i)
      {
         double d = static_cast<double>(i);
         m_value += i;
         i -= d;
      }
      return *this;
   }
   quad_double_backend& operator=(double i)
   {
      m_value[0] = i;
      m_value[1] = m_value[2] = m_value[3] = 0;
      return *this;
   }
   quad_double_backend& operator=(const char* s)
   {
      boost::multiprecision::detail::convert_from_string(*this, s);
      return *this;
   }
   void swap(quad_double_backend& o)
   {
      std::swap(m_value, o.m_value);
   }
   std::string str(std::streamsize digits, std::ios_base::fmtflags f) const
   {
      if (digits == 0)
         digits = std::numeric_limits<qd_real>::digits10 + 3;
      return boost::multiprecision::detail::convert_to_string(*this, digits, f);
   }
   void negate()
   {
      m_value = -m_value;
   }
   int compare(const quad_double_backend& o) const
   {
      return m_value > o.m_value ? 1 : (m_value < o.m_value ? -1 : 0);
   }
   template <class A>
   typename enable_if<is_arithmetic<A>, int>::type compare(A i) const
   {
      quad_double_backend t(i);
      return compare(t);
   }
   qd_real& data() { return m_value; }
   const qd_real& data() const { return m_value; }

 private:
   qd_real m_value;
};

template <class R>
typename boost::disable_if_c<boost::is_unsigned<R>::value, R>::type minus_max()
{
   return -(std::numeric_limits<R>::max)();
}
template <class R>
typename boost::enable_if_c<boost::is_unsigned<R>::value, R>::type minus_max()
{
   return 0;
}

template <class R>
inline typename enable_if_c<boost::is_integral<R>::value>::type eval_convert_to(R* result, const quad_double_backend& backend)
{
   typedef typename boost::common_type<R, double>::type c_type;
   
   if ((backend.data()[0] < 0) && !std::numeric_limits<R>::is_signed)
      BOOST_THROW_EXCEPTION(std::range_error("Attempt to convert negative number to unsigned type."));

   BOOST_CONSTEXPR const c_type                             max = static_cast<c_type>((std::numeric_limits<R>::max)());
   BOOST_CONSTEXPR const c_type                             min = static_cast<c_type>((std::numeric_limits<R>::min)());
   c_type                                                   ct  = std::fabs(backend.data()[0]);

   if (ct > max)
      if (!boost::is_unsigned<R>::value)
         *result = backend.data()[0] >= 0 ? (std::numeric_limits<R>::max)() : minus_max<R>();
      else
         *result = (std::numeric_limits<R>::max)();
   else
   {
      *result = backend.data()[0];
      *result += backend.data()[1];
      *result += backend.data()[2];
      *result += backend.data()[3];
   }
}

template <class R>
inline typename disable_if_c<boost::is_integral<R>::value>::type eval_convert_to(R* result, const quad_double_backend& backend)
{
   *result = backend.data()[0];
   *result += backend.data()[1];
   *result += backend.data()[2];
   *result += backend.data()[3];
}


inline bool eval_eq(const quad_double_backend& a, const quad_double_backend& b)
{
   return a.data() == b.data();
}

inline bool eval_lt(const quad_double_backend& a, const quad_double_backend& b)
{
   return a.data() < b.data();
}

inline bool eval_gt(const quad_double_backend& a, const quad_double_backend& b)
{
   return a.data() > b.data();
}


inline void eval_add(quad_double_backend& result, const quad_double_backend& o)
{
   if ((boost::math::isinf)(result.data()[0])){}
   else if ((boost::math::isinf)(o.data()[0]))
      result.data() = o.data();
   else
      result.data() += o.data();
}

inline void eval_subtract(quad_double_backend& result, const quad_double_backend& o)
{
   if ((boost::math::isinf)(result.data()[0])) {}
   else if ((boost::math::isinf)(o.data()[0]))
      result.data() = -o.data();
   else
      result.data() -= o.data();
}

inline void eval_multiply(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() *= o.data();
}

inline void eval_divide(quad_double_backend& result, const quad_double_backend& o)
{
   bool i1 = (boost::math::isinf)(result.data()[0]);
   bool i2 = (boost::math::isinf)(o.data()[0]);
   if (i1 && !i2) {}
   else if (i2 && !i1)
      result.data() = 0.0;
   else
      result.data() /= o.data();
}

template <class A2>
inline typename enable_if_c<is_floating_point<A2>::value >::type eval_add(quad_double_backend& result, const A2& o)
{
   if ((boost::math::isinf)(result.data()[0])) {}
   else if ((boost::math::isinf)(o))
      result = o;
   else
      result.data() += o;
}
template <class A2>
inline typename enable_if_c<is_arithmetic<A2>::value && !is_floating_point<A2>::value>::type eval_add(quad_double_backend& result, const A2& o)
{
   if (!(boost::math::isinf)(result.data()[0]))
      result.data() += o;
}
template <class A2>
inline typename enable_if_c<is_floating_point<A2>::value>::type eval_subtract(quad_double_backend& result, const A2& o)
{
   if ((boost::math::isinf)(result.data()[0])) {}
   else if ((boost::math::isinf)(o))
      result.data() = -o;
   else
      result.data() -= o;
}
template <class A2>
inline typename enable_if_c<is_arithmetic<A2>::value && !is_floating_point<A2>::value>::type eval_subtract(quad_double_backend& result, const A2& o)
{
   if (!(boost::math::isinf)(result.data()[0]))
      result.data() -= o;
}
template <class A2>
inline typename enable_if<is_arithmetic<A2> >::type eval_multiply(quad_double_backend& result, const A2& o)
{
   result.data() *= o;
}
template <class A2>
inline typename enable_if_c<is_floating_point<A2>::value>::type
eval_divide(quad_double_backend& result, const A2& o)
{
   bool i1 = (boost::math::isinf)(result.data()[0]);
   bool i2 = (boost::math::isinf)(o);
   if (i1 && !i2) {}
   else if (i2 && !i1)
      result.data() = 0.0;
   else
      result.data() /= o;
}
template <class A2>
inline typename enable_if_c<is_arithmetic<A2>::value && !is_floating_point<A2>::value>::type
eval_divide(quad_double_backend& result, const A2& o)
{
   bool i1 = (boost::math::isinf)(result.data()[0]);
   if (!i1)
      result.data() /= o;
}


inline void eval_add(quad_double_backend& result, const quad_double_backend& a, const quad_double_backend& b)
{
   if((boost::math::isinf)(a.data()[0]))
      result.data() = a.data();
   else if((boost::math::isinf)(b.data()[0]))
      result.data() = b.data();
   else
      result.data() = a.data() + b.data();
}

inline void eval_subtract(quad_double_backend& result, const quad_double_backend& a, const quad_double_backend& b)
{
   if ((boost::math::isinf)(a.data()[0]))
      result.data() = a.data();
   else if ((boost::math::isinf)(b.data()[0]))
      result.data() = -b.data();
   else
      result.data() = a.data() - b.data();
}

inline void eval_multiply(quad_double_backend& result, const quad_double_backend& a, const quad_double_backend& b)
{
   result.data() = a.data() * b.data();
}

inline void eval_divide(quad_double_backend& result, const quad_double_backend& a, const quad_double_backend& b)
{
   bool i1 = (boost::math::isinf)(a.data()[0]);
   bool i2 = (boost::math::isinf)(b.data()[0]);
   if (i1 && !i2)
      result.data() = a.data();
   else if (i2 && !i1)
      result.data() = 0.0;
   else
      result.data() = a.data() / b.data();
}


template <class A2>
inline typename enable_if_c<is_floating_point<A2>::value>::type eval_add(quad_double_backend& result, const quad_double_backend& a, const A2& b)
{
   if ((boost::math::isinf)(a.data()[0]))
      result.data() = a.data();
   else if ((boost::math::isinf)(b))
      result = b;
   else
      result.data() = a.data() + b;
}
template <class A2>
inline typename enable_if_c<is_arithmetic<A2>::value && !is_floating_point<A2>::value>::type eval_add(quad_double_backend& result, const quad_double_backend& a, const A2& b)
{
   if ((boost::math::isinf)(a.data()[0]))
      result.data() = a.data();
   else
      result.data() = a.data() + b;
}
template <class A2>
inline typename enable_if_c<is_floating_point<A2>::value>::type eval_subtract(quad_double_backend& result, const quad_double_backend& a, const A2& b)
{
   if ((boost::math::isinf)(a.data()[0]))
      result.data() = a.data();
   else if ((boost::math::isinf)(b))
      result.data() = -b;
   else
      result.data() = a.data() - b;
}
template <class A2>
inline typename enable_if_c<is_arithmetic<A2>::value && !is_floating_point<A2>::value>::type eval_subtract(quad_double_backend& result, const quad_double_backend& a, const A2& b)
{
   if ((boost::math::isinf)(a.data()[0]))
      result.data() = a.data();
   else
      result.data() = a.data() - b;
}
template <class A2>
inline typename enable_if<is_arithmetic<A2> >::type eval_multiply(quad_double_backend& result, const quad_double_backend& a, const A2& b)
{
   result.data() = a.data() * b;
}
template <class A2>
inline typename enable_if_c<is_floating_point<A2>::value>::type
eval_divide(quad_double_backend& result, const quad_double_backend& a, const A2& b)
{
   bool i1 = (boost::math::isinf)(a.data()[0]);
   bool i2 = (boost::math::isinf)(b);
   if (i1 && !i2)
      result.data() = a.data();
   else if (i2 && !i1)
      result.data() = 0.0;
   else
      result.data() = a.data() / b;
}
template <class A2>
inline typename enable_if_c<is_arithmetic<A2>::value && !is_floating_point<A2>::value>::type
eval_divide(quad_double_backend& result, const quad_double_backend& a, const A2& b)
{
   bool i1 = (boost::math::isinf)(a.data()[0]);
   if (i1)
      result.data() = a.data();
   else
      result.data() = a.data() / b;
}


inline bool eval_is_zero(const quad_double_backend& val)
{
   return val.data() == 0;
}


inline int eval_get_sign(const quad_double_backend& val)
{
   return val.data() == 0 ? 0 : val.data() < 0 ? -1 : 1;
}

inline void eval_abs(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = abs(o.data());
}


inline void eval_fabs(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = abs(o.data());
}


inline void eval_floor(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = floor(o.data());
}


inline void eval_ceil(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = ceil(o.data());
}


inline void eval_sqrt(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = sqrt(o.data());
}


inline int eval_fpclassify(const quad_double_backend& o)
{
   return (boost::math::fpclassify)(o.data()[0]);
}


inline void eval_trunc(quad_double_backend& result, const quad_double_backend& o)
{
   if (o.data()[0] < 0)
      result.data() = ceil(o.data());
   else
      result.data() = floor(o.data());
}


inline void eval_round(quad_double_backend& result, const quad_double_backend& o)
{
   qd_real const& v = o.data();
   //
   // The logic here is rather convoluted, but avoids a number of traps,
   // see discussion here https://github.com/boostorg/math/pull/8
   //
   if (-0.5 < v && v < 0.5)
   {
      // special case to avoid rounding error on the direct
      // predecessor of +0.5 resp. the direct successor of -0.5 in
      // IEEE floating point types
      result = 0uLL;
   }
   else if (v > 0)
   {
      // subtract v from ceil(v) first in order to avoid rounding
      // errors on largest representable integer numbers
      qd_real c(ceil(v));
      result = 0.5 < c - v ? c - 1 : c;
   }
   else
   {
      // see former branch
      qd_real f(floor(v));
      result = 0.5 < v - f ? f + 1 : f;
   }
}


inline void eval_frexp(quad_double_backend& result, const quad_double_backend& a, int* v)
{
   result.data()[0] = std::frexp(a.data()[0], v);
   result.data()[1] = std::ldexp(a.data()[1], -*v);
   result.data()[2] = std::ldexp(a.data()[2], -*v);
   result.data()[3] = std::ldexp(a.data()[3], -*v);
}


inline void eval_ldexp(quad_double_backend& result, const quad_double_backend& a, int v)
{
   result.data() = ldexp(a.data(), v);
}


inline void eval_exp(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = exp(o.data());
}


inline void eval_log(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = log(o.data());
}


inline void eval_log10(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = log10(o.data());
}


inline void eval_sin(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = sin(o.data());
}


inline void eval_cos(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = cos(o.data());
}


inline void eval_tan(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = tan(o.data());
}


inline void eval_acos(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = acos(o.data());
}


inline void eval_asin(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = asin(o.data());
}


inline void eval_atan(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = atan(o.data());
}


inline void eval_sinh(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = sinh(o.data());
}


inline void eval_cosh(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = cosh(o.data());
}


inline void eval_tanh(quad_double_backend& result, const quad_double_backend& o)
{
   result.data() = tanh(o.data());
}


inline void eval_fmod(quad_double_backend& result, const quad_double_backend& a, const quad_double_backend& b)
{
   result.data() = fmod(a.data(), b.data());
}


inline void eval_pow(quad_double_backend& result, const quad_double_backend& a, const quad_double_backend& b)
{
   result.data() = pow(a.data(), b.data());
}


inline void eval_atan2(quad_double_backend& result, const quad_double_backend& a, const quad_double_backend& b)
{
   result.data() = atan2(a.data(), b.data());
}

inline std::size_t hash_value(const quad_double_backend& a)
{
   boost::hash<double> hasher;
   std::size_t result = hasher(a.data()[0]);
   boost::hash_combine(result, hasher(a.data()[1]));
   boost::hash_combine(result, hasher(a.data()[2]));
   boost::hash_combine(result, hasher(a.data()[3]));
   return result;
}

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

} // namespace backends

using boost::multiprecision::backends::quad_double_backend;

typedef number<quad_double_backend, et_off> quad_double;

template<>
struct number_category<quad_double_backend > : public mpl::int_<number_kind_floating_point>
{};

template <boost::multiprecision::expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<quad_double_backend, ExpressionTemplates> asinh BOOST_PREVENT_MACRO_SUBSTITUTION(const boost::multiprecision::number<quad_double_backend, ExpressionTemplates>& arg)
{
   return asinh(arg.backend().data());
}
template <boost::multiprecision::expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<quad_double_backend, ExpressionTemplates> acosh BOOST_PREVENT_MACRO_SUBSTITUTION(const boost::multiprecision::number<quad_double_backend, ExpressionTemplates>& arg)
{
   return acosh(arg.backend().data());
}
template <boost::multiprecision::expression_template_option ExpressionTemplates>
inline boost::multiprecision::number<quad_double_backend, ExpressionTemplates> atanh BOOST_PREVENT_MACRO_SUBSTITUTION(const boost::multiprecision::number<quad_double_backend, ExpressionTemplates>& arg)
{
   return atanh(arg.backend().data());
}

}} // namespace boost::multiprecision

namespace std {

template <boost::multiprecision::expression_template_option ExpressionTemplates>
class numeric_limits<boost::multiprecision::number<boost::multiprecision::quad_double_backend, ExpressionTemplates> > : public std::numeric_limits<qd_real>
{
   typedef std::numeric_limits<qd_real>                                                                           base_type;
   typedef boost::multiprecision::number<boost::multiprecision::quad_double_backend, ExpressionTemplates>         number_type;

 public:
   BOOST_STATIC_CONSTEXPR int  max_digits10 = digits10 + 3;
   BOOST_STATIC_CONSTEXPR number_type(min)() BOOST_NOEXCEPT 
   { 
      static const qd_real value = { 1.6259745436952323e-260 };
      return (base_type::min)(); 
   }
   BOOST_STATIC_CONSTEXPR number_type(max)() BOOST_NOEXCEPT 
   {
      static const qd_real value = { 1.79769313486231570815e+308, 9.97920154767359795037e+291, 5.53956966280111259858e+275, 3.07507889307840487279e+259 };
      return value; 
   }
   BOOST_STATIC_CONSTEXPR number_type lowest() BOOST_NOEXCEPT { return -(max)(); }
   BOOST_STATIC_CONSTEXPR number_type epsilon() BOOST_NOEXCEPT { return 1.21543267145725e-63; }
   BOOST_STATIC_CONSTEXPR number_type round_error() BOOST_NOEXCEPT { return epsilon() / 2; }
   BOOST_STATIC_CONSTEXPR number_type infinity() BOOST_NOEXCEPT { return std::numeric_limits<double>::infinity(); }
   BOOST_STATIC_CONSTEXPR number_type quiet_NaN() BOOST_NOEXCEPT { return std::numeric_limits<double>::quiet_NaN(); }
   BOOST_STATIC_CONSTEXPR number_type signaling_NaN() BOOST_NOEXCEPT { return std::numeric_limits<double>::signaling_NaN(); }
   BOOST_STATIC_CONSTEXPR number_type denorm_min() BOOST_NOEXCEPT { return std::numeric_limits<double>::denorm_min(); }
};

template <boost::multiprecision::expression_template_option ExpressionTemplates>
BOOST_CONSTEXPR_OR_CONST int numeric_limits<boost::multiprecision::number<boost::multiprecision::quad_double_backend, ExpressionTemplates> >::max_digits10;

} // namespace std

#endif
