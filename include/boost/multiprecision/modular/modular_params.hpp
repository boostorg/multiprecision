//---------------------------------------------------------------------------//
// Copyright (c) 2019 Nil Foundation AG
// Copyright (c) 2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MODULAR_PARAMS_HPP
#define BOOST_MULTIPRECISION_MODULAR_PARAMS_HPP

#include <boost/multiprecision/modular/montgomery_params.hpp>
#include <boost/multiprecision/modular/barrett_params.hpp>

namespace boost {
namespace multiprecision {
template <typename Backend>
class modular_params : public montgomery_params<Backend>, public barrett_params<Backend>
{
   typedef number<Backend> number_type;

 public:
   modular_params() : montgomery_params<Backend>(), barrett_params<Backend>() {}

   template <typename Number>
   explicit modular_params(const Number& p) : montgomery_params<Backend>(p), barrett_params<Backend>(p)
   {
   }

   template <class V>
   modular_params& operator=(const V& v)
   {
      this->initialize_barrett_params(v);
      this->initialize_montgomery_params(v);
      return *this;
   }

   void reduce(Backend& result) const
   {
      if (get_mod() % 2 == 0)
      {
         this->eval_barret_reduce(result);
      }
      else
      {
         this->eval_montgomery_reduce(result);
      }
   }

   void adjust_modular(Backend& result)
   {
      this->eval_barret_reduce(result);
      if (get_mod() % 2 != 0)
      {
         eval_multiply(result, this->r2().backend());
         this->eval_montgomery_reduce(result);
      }
   }

   void adjust_regular(Backend& result, const Backend& input) const
   {
      result = input;
      if (get_mod() % 2 != 0)
      {
         this->eval_montgomery_reduce(result);
      }
   }

   number_type get_mod() const
   {
      return montgomery_params<Backend>::mod() | barrett_params<Backend>::mod();
   }

   template <typename BackendT, expression_template_option ExpressionTemplates>
   operator number<BackendT, ExpressionTemplates>()
   {
      return this->m_mod;
   };
};
}
} // namespace boost::multiprecision
#endif //_MULTIPRECISION_MODULAR_PARAMS_HPP
