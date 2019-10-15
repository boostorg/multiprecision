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

#include <boost/multiprecision/tommath.hpp>
#include <boost/multiprecision/modular/modular_adaptor.hpp>
#include <boost/multiprecision/modular/base_params.hpp>

namespace boost {
namespace multiprecision {

template <typename Backend>
class modular_params;

template <>
class modular_params<tommath_int> : public backends::base_params<tommath_int>
{
   typedef number<tommath_int> number_type;

 public:
   modular_params() : backends::base_params<tommath_int>(){};

   template <typename Number>
   explicit modular_params(const Number& p) : backends::base_params<tommath_int>(number_type(p))
   {
   }

   modular_params& operator=(const modular_params<tommath_int>& v) {
      backends::base_params<tommath_int>::m_mod = v.get_mod();
      return *this;
   }

   template <class Number>
   modular_params& operator=(const Number& v)
   {
      number_type tmp(v);
      this->initialize_base_params(tmp);
      return *this;
   }

   void reduce(tommath_int& result) const
   {
      backends::detail::check_tommath_result(mp_mod(const_cast< ::mp_int*>(&result.data()), const_cast< ::mp_int*>(&get_mod().backend().data()), &result.data()));
   }

   void adjust_modular(tommath_int& result)
   {
      backends::detail::check_tommath_result(mp_mod(const_cast< ::mp_int*>(&result.data()), const_cast< ::mp_int*>(&get_mod().backend().data()), &result.data()));
   }

   void adjust_regular(tommath_int& result, const tommath_int& input) const
   {
      result = input;
   }

   number_type get_mod() const
   {
      return base_params<tommath_int>::mod();
   }

   template <typename BackendT, expression_template_option ExpressionTemplates>
   operator number<BackendT, ExpressionTemplates>()
   {
      return this->m_mod;
   };

   int compare(const modular_params<tommath_int>& o) const
   {
      // They are either equal or not:
      return (get_mod().compare(o.mod()));
   }

   friend std::ostream& operator<<(std::ostream& o, modular_params<tommath_int> const& a)
   {
      o << a.get_mod();
      return o;
   }
};
}
} // namespace boost::multiprecision
#endif //_MULTIPRECISION_MODULAR_ADAPTOR_TOMMATH_HPP
