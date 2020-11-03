//---------------------------------------------------------------------------//
// Copyright (c) 2020 Mikhail Komarov <nemo@nil.foundation>
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
#include <boost/multiprecision/modular/base_params.hpp>

namespace boost {
namespace multiprecision {

template <typename Backend>
class modular_params;

template <>
class modular_params<gmp_int> : public backends::base_params<gmp_int>
{
   typedef number<gmp_int> number_type;

 public:
   modular_params() : backends::base_params<gmp_int>(){};

   template <typename Number>
   explicit modular_params(const Number& p) : backends::base_params<gmp_int>(number_type(p))
   {
   }

   modular_params& operator=(const modular_params<gmp_int>& v) {
      backends::base_params<gmp_int>::m_mod = v.get_mod();
      return *this;
   }

   template <class Number>
   modular_params& operator=(const Number& v)
   {
      number_type tmp(v);
      this->initialize_base_params(tmp);
      return *this;
   }

   inline void reduce(gmp_int& result) const
   {
      mpz_mod(result.data(), result.data(), get_mod().backend().data());
   }

   inline void adjust_modular(gmp_int& result)
   {
      mpz_mod(result.data(), result.data(), get_mod().backend().data());
   }

   inline void adjust_regular(gmp_int& result, const gmp_int& input) const
   {
      result = input;
   }

   inline number_type get_mod() const
   {
      return base_params<gmp_int>::mod();
   }

   template <typename BackendType, expression_template_option ExpressionTemplates>
   inline operator number<BackendType, ExpressionTemplates>()
   {
      return this->m_mod;
   };

   inline int compare(const modular_params<gmp_int>& o) const
   {
      // They are either equal or not:
      return (get_mod().compare(o.mod()));
   }

   friend std::ostream& operator<<(std::ostream& o, modular_params<gmp_int> const& a)
   {
      o << a.get_mod();
      return o;
   }
};
}
} // namespace boost::multiprecision
#endif //BOOST_MULTIPRECISION_MODULAR_ADAPTOR_GMP_HPP
