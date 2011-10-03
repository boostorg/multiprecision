///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MATH_EFX_BACKEND_HPP
#define BOOST_MATH_EFX_BACKEND_HPP

#include <boost/multiprecision/arithmetic_backend.hpp>
#include <boost/e_float/e_float.hpp>
#include <boost/e_float/e_float_complex.hpp>
#include <boost/e_float/e_float_elementary_math.hpp>
#include <fstream>

namespace boost{
namespace multiprecision{

typedef mp_number<arithmetic_backend<efx::e_float> > e_float;

template<>
inline void arithmetic_backend<efx::e_float>::negate()
{
   m_value.negate();
}
template<>
inline std::string arithmetic_backend<efx::e_float>::str(unsigned digits, bool scientific)const
{
   std::fstream os;
   os << std::setprecision(digits ? digits : efx::e_float::ef_digits + 5);
   if(scientific)
      os << std::scientific;
   std::string result;
   this->data().wr_string(result, os);
   return result;
}

inline void eval_abs(arithmetic_backend<efx::e_float>& result, const arithmetic_backend<efx::e_float>& arg)
{
   result.data() = ef::fabs(arg.data());
}

inline void eval_fabs(arithmetic_backend<efx::e_float>& result, const arithmetic_backend<efx::e_float>& arg)
{
   result.data() = ef::fabs(arg.data());
}

inline bool is_zero(const arithmetic_backend<efx::e_float>& val)
{
   return val.data().iszero();
}
inline int get_sign(const arithmetic_backend<efx::e_float>& val)
{
   return val.data().isneg() ? -1 : val.data().iszero() ? 0 : 1;
}

inline void convert_to(boost::uintmax_t* result, const arithmetic_backend<efx::e_float>& val)
{
   *result = val.data().extract_unsigned_long_long();
}
inline void convert_to(boost::intmax_t* result, const arithmetic_backend<efx::e_float>& val)
{
   *result = val.data().extract_signed_long_long();
}
inline void convert_to(double* result, const arithmetic_backend<efx::e_float>& val)
{
   *result = val.data().extract_double();
}
inline void convert_to(long double* result, const arithmetic_backend<efx::e_float>& val)
{
   *result = val.data().extract_long_double();
}
inline int eval_fpclassify(const arithmetic_backend<efx::e_float>& val)
{
   return val.data().isinf() ? FP_INFINITE : val.data().isnan() ? FP_NAN : val.data().iszero() ? FP_ZERO : FP_NORMAL;
}


}} // namespaces

#endif
