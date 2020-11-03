//---------------------------------------------------------------------------//
// Copyright (c) 2020 Mikhail Komarov <nemo@nil.foundation>
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
class modular_params : public backends::montgomery_params<Backend>, public backends::barrett_params<Backend>
{
   typedef number<Backend> number_type;

 public:
   modular_params() : backends::montgomery_params<Backend>(), backends::barrett_params<Backend>() {}

   template <typename Number>
   explicit modular_params(const Number& p) : backends::montgomery_params<Backend>(number_type(p)), backends::barrett_params<Backend>(number_type(p))
   {
   }

   modular_params& operator=(const modular_params<Backend>& v)
   {
      backends::montgomery_params<Backend>::m_mod = v.get_mod();
      backends::barrett_params<Backend>::m_mod    = v.get_mod();

      this->m_mu = v.mu();

      this->m_r2      = v.r2();
      this->m_p_dash  = v.p_dash();
      this->m_p_words = v.p_words();

      return *this;
   }

   template <typename Number>
   modular_params& operator=(const Number& v)
   {
      number_type tmp(v);
      this->initialize_barrett_params(tmp);
      this->initialize_montgomery_params(tmp);
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
      return backends::montgomery_params<Backend>::mod() | backends::barrett_params<Backend>::mod();
   }

   template <typename BackendT, expression_template_option ExpressionTemplates>
   operator number<BackendT, ExpressionTemplates>()
   {
      return get_mod();
   };

   int compare(const modular_params<Backend>& o) const
   {
      // They are either equal or not:
      return (get_mod().compare(o.get_mod()));
   }

   friend std::ostream& operator<<(std::ostream& o, modular_params<Backend> const& a)
   {
      o << a.get_mod();
      return o;
   }
};
}
} // namespace boost::multiprecision
#endif //_MULTIPRECISION_MODULAR_PARAMS_HPP
