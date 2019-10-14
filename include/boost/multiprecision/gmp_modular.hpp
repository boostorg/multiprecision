//---------------------------------------------------------------------------//
// Copyright (c) 2019 Nil Foundation AG
// Copyright (c) 2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef _MULTIPRECISION_GMP_MODULAR_HPP
#define _MULTIPRECISION_GMP_MODULAR_HPP

#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/modular/modular_adaptor.hpp>
#include <boost/multiprecision/modular/modular_params_gmp.hpp>

namespace boost {
namespace multiprecision {
typedef modular_params<gmp_int>           mpz_int_mod_params;
typedef number<modular_adaptor<gmp_int> > mpz_int_mod;
}
} // namespace boost::multiprecision
#endif //_MULTIPRECISION_GMP_MODULAR_HPP
