///////////////////////////////////////////////////////////////
//  Copyright 2011 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_

#ifndef BOOST_MATH_EFX_BACKEND_HPP
#define BOOST_MATH_EFX_BACKEND_HPP

#include <boost/math/big_number/arithmetic_backend.hpp>
#include <boost/e_float/e_float.hpp>
#include <boost/e_float/e_float_complex.hpp>
#include <boost/e_float/e_float_elementary_math.hpp>

namespace boost{
namespace math{

typedef big_number<arithmetic_backend<efx::e_float> > e_float;

template<>
inline void arithmetic_backend<efx::e_float>::negate()
{
   m_value.negate();
}

inline void abs(arithmetic_backend<efx::e_float>* result, const arithmetic_backend<efx::e_float>& arg)
{
   result->data() = ef::fabs(arg.data());
}


}} // namespaces

#endif
