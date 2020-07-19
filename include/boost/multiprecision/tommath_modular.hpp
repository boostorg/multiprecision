//---------------------------------------------------------------------------//
// Copyright (c) 2020 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef _MULTIPRECISION_TOMMATH_MODULAR_HPP
#define _MULTIPRECISION_TOMMATH_MODULAR_HPP

#include <boost/multiprecision/tommath.hpp>
#include <boost/multiprecision/modular/modular_adaptor.hpp>
#include <boost/multiprecision/modular/modular_params_tommath.hpp>

namespace boost {
namespace multiprecision {
typedef modular_params<tommath_int>           tom_int_mod_params;
typedef number<modular_adaptor<tommath_int> > tom_int_mod;
}
} // namespace boost::multiprecision
#endif //_MULTIPRECISION_TOMMATH_MODULAR_HPP
