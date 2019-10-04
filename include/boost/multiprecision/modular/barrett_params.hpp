//---------------------------------------------------------------------------//
// Copyright (c) 2019 Nil Foundation AG
// Copyright (c) 2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_BARRETT_PARAMS_HPP
#define BOOST_MULTIPRECISION_BARRETT_PARAMS_HPP

#include <boost/multiprecision/modular/base_params.hpp>
#include <boost/multiprecision/modular/mask_bits.hpp>

namespace boost {
namespace multiprecision {

/**
 * Parameters for Montgomery Reduction
 */
template <typename Backend>
class barrett_params : public base_params<Backend>
{
   typedef number<Backend> number_type;

 public:
   barrett_params() : base_params<Backend>() {}

   template <typename Number>
   void initialize_barrett_params(const Number& p)
   {
      this->initialize_base_params(p);
      m_mu = 0;

      eval_bit_set(m_mu.backend(), 2 * Backend::limb_bits * m_mu.backend().size());
      eval_divide(m_mu.backend(), this->m_mod.backend());
   }

   template <typename Number>
   explicit barrett_params(const Number& p)
   {
      initialize_barrett_params(p);
   }

   const number_type& mu() const { return m_mu; }

   template <class V>
   barrett_params& operator=(const V& v)
   {
      initialize_barrett_params(v);
      return *this;
   }

   inline void eval_barret_reduce(Backend& result) const
   {
      using default_ops::eval_add;
      using default_ops::eval_bit_set;
      using default_ops::eval_lt;
      using default_ops::eval_multiply;

      if (result.size() < this->m_mod.backend().size() || eval_lt(result, this->m_mod.backend()))
      {
         if (eval_lt(result, 0))
         {
            eval_add(result, this->m_mod.backend());
            return;
         }
         return;
      }
      else if (result.size() < 2 * this->m_mod.backend().size())
      {

         Backend t1(result);

         eval_multiply(t1, m_mu.backend());
         eval_right_shift(t1, (2 * Backend::limb_bits * (this->m_mod.backend().size())));

         eval_multiply(t1, this->m_mod.backend());

         eval_mask_bits(t1, Backend::limb_bits * (this->m_mod.backend().size() + 1));

         eval_subtract(t1, result, t1);

         if (eval_lt(t1, 0))
         {
            Backend p2;
            eval_bit_set(p2, Backend::limb_bits * (this->m_mod.backend().size() + 1));
            eval_add(t1, p2);
         }

         while (eval_lt(this->m_mod.backend(), t1))
         {
            eval_subtract(t1, this->m_mod.backend());
         }

         if (eval_lt(result, 0))
         {
            eval_add(t1, this->m_mod.backend());
         }

         result = t1;
         return;
      }
      else
      {
         Backend t2;
         divide_unsigned_helper(&result, result, this->m_mod.backend(), t2);
         result = t2;
         return;
      }
   }

   number_type m_mu;
};
}
} // namespace boost::multiprecision

#endif //_MULTIPRECISION_BARRETT_PARAMS_HPP
