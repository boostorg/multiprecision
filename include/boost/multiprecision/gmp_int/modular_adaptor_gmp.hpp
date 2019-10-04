//---------------------------------------------------------------------------//
// Copyright (c) 2019 Nil Foundation AG
// Copyright (c) 2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MODULAR_ADAPTOR_GMP_HPP
#define BOOST_MULTIPRECISION_MODULAR_ADAPTOR_GMP_HPP

#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/modular/modular_adaptor.hpp>

namespace boost {
namespace multiprecision {
namespace backends {

template <typename T>
class modular_adapter_base;

template <>
class modular_adapter_base<gmp_int>
{
 protected:
   gmp_int m_base, m_mod;

 public:
   inline gmp_int& mod_data() { return m_mod; }

   inline const gmp_int& mod_data() const { return m_mod; }
};

inline void eval_redc(gmp_int& result, const gmp_int& mod)
{
   mpz_mod(result.data(), result.data(), mod.data());
}

template <class T, class V>
inline void assign_components(modular_adaptor<gmp_int>& result, const T& a, const V& b)
{
   result.base_data() = a;
   result.mod_data()  = b;
   eval_redc(result.base_data(), result.mod_data());
}

} // namespace backends

typedef number<modular_adaptor<gmp_int> > mpz_int_mod;

}
} // namespace boost::multiprecision

#endif //BOOST_MULTIPRECISION_MODULAR_ADAPTOR_GMP_HPP
