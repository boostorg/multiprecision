//---------------------------------------------------------------------------//
// Copyright (c) 2019 Nil Foundation AG
// Copyright (c) 2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MODULAR_ADAPTOR_TOMMATH_HPP
#define BOOST_MULTIPRECISION_MODULAR_ADAPTOR_TOMMATH_HPP

#include <boost/multiprecision/modular/modular_adaptor.hpp>
#include <boost/multiprecision/tommath.hpp>

namespace boost {
namespace multiprecision {

template <typename T>
class modular_params;

template <>
class modular_params<tommath_int>
{
   typedef number<tommath_int> number_type;

 public:
   modular_params() {}

   template <typename Number>
   explicit modular_params(const Number& p)
   {
      initialize_params(p);
   }

   template <class V>
   modular_params& operator=(const V& v)
   {
      initialize_params(v);
      return *this;
   }

   void reduce(tommath_int& result) const
   {
      mp_montgomery_reduce(&result.data(), const_cast< ::mp_int*>(&m_mod.backend().data()), m_tho);
   }

   void adjust_modular(tommath_int& result)
   {
      mp_montgomery_reduce(&result.data(), const_cast< ::mp_int*>(&m_mod.backend().data()), m_tho);
   }

   tommath_int adjust_regular(tommath_int& result, const tommath_int& input) const
   {
      return result;
   }

   number_type get_mod() const
   {
      return m_mod;
   }

   template <typename BackendT, expression_template_option ExpressionTemplates>
   operator number<BackendT, ExpressionTemplates>()
   {
      return m_mod;
   };

   template <typename T>
   void initialize_params(const T& p)
   {
      m_mod = p;
      mp_montgomery_setup(const_cast< ::mp_int*>(&m_mod.backend().data()), &m_tho);
   }

 private:
   ::mp_digit  m_tho;
   number_type m_mod;
};

typedef number<modular_adaptor<tommath_int> > tom_int_mod;
typedef modular_params<tommath_int> tom_int_mod_params;

}
} // namespace boost::multiprecision

#endif //_MULTIPRECISION_MODULAR_ADAPTOR_TOMMATH_HPP
