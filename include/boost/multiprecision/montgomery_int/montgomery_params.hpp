//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MONTY_INT_HPP
#define BOOST_MULTIPRECISION_MONTY_INT_HPP

#include <boost/multiprecision/montgomery_int/inverse.hpp>
#include <boost/multiprecision/montgomery_int/modular_reduce.hpp>

using namespace boost::multiprecision;

namespace nil {
namespace crypto3 {

class modular_reducer;

/**
         * Parameters for Montgomery Reduction
         */
template <typename NumberType>
class montgomery_params
{
 public:
   typedef NumberType number_type;

   /**
            * Initialize a set of Montgomery reduction parameters. These values
            * can be shared by all values in a specific Montgomery domain.
            */

   //work_only_with_cpp_int
   template <typename Backend, expression_template_option ExpressionTemplates>
   explicit montgomery_params(const number<Backend, ExpressionTemplates>& p,
                              const number<Backend, ExpressionTemplates>& redc_module)
   {

      if (p < 0 || !(p % 2))
      {
         throw std::invalid_argument("montgomery_params invalid modulus");
      }

      m_p = p;

      m_mod = redc_module;

      m_p_words = m_p.backend().size();

      m_p_dash = monty_inverse(m_p.backend().limbs()[0]);

      number_type r;

      boost::multiprecision::backends::eval_bit_set(r.backend(), m_p_words * CRYPTO3_MP_WORD_BITS);

      m_r1 = r;
      mod_redc(m_r1.backend(), redc_module.backend());
      m_r2 = m_r1;
      eval_multiply(m_r2.backend(), m_r1.backend());
      mod_redc(m_r2.backend(), redc_module.backend());
      m_r3 = m_r2;
      eval_multiply(m_r3.backend(), m_r1.backend());
      mod_redc(m_r3.backend(), redc_module.backend());
   }

   BOOST_MP_FORCEINLINE BOOST_CONSTEXPR montgomery_params() BOOST_NOEXCEPT
   {
   }

   template <typename Backend, expression_template_option ExpressionTemplates>
   explicit montgomery_params()
   {
   }

   /**
            * Initialize a set of Montgomery reduction parameters. These values
            * can be shared by all values in a specific Montgomery domain.
            */
   template <typename Backend, expression_template_option ExpressionTemplates>
   explicit montgomery_params(const number<Backend, ExpressionTemplates>& p)
   {

      if (p < 0 || !(p % 2))
      {
         throw std::invalid_argument("montgomery_params invalid modulus");
      }

      m_p = p;

      m_mod = p;

      m_p_words = m_p.backend().size();

      m_p_dash = monty_inverse(m_p.backend().limbs()[0]);

      number_type r;

      boost::multiprecision::backends::eval_bit_set(r.backend(), m_p_words * CRYPTO3_MP_WORD_BITS);

      m_r1 = r;
      mod_redc(m_r1.backend(), p.backend());
      m_r2 = m_r1;
      eval_multiply(m_r2.backend(), m_r1.backend());
      mod_redc(m_r2.backend(), p.backend());
      m_r3 = m_r2;
      eval_multiply(m_r3.backend(), m_r1.backend());
      mod_redc(m_r3.backend(), p.backend());
   }

   const number_type& mod() const
   {
      return m_mod;
   }

   const number_type& p() const
   {
      return m_p;
   }

   const number_type& R1() const
   {
      return m_r1;
   }

   const number_type& R2() const
   {
      return m_r2;
   }

   const number_type& R3() const
   {
      return m_r3;
   }

   limb_type p_dash() const
   {
      return m_p_dash;
   }

   size_t p_words() const
   {
      return m_p_words;
   }

   /*
            operator number_type() {
                return m_p;
            };
            */

   template <typename Backend, expression_template_option ExpressionTemplates>
   operator number<Backend, ExpressionTemplates>()
   {
      number<Backend, ExpressionTemplates> x = 0;
      return x;
   };

 private:
   NumberType m_p;
   NumberType m_r1;
   NumberType m_r2;
   NumberType m_r3;
   limb_type  m_p_dash;
   size_t     m_p_words;
   NumberType m_mod;
};

/**
* The Montgomery representation of an integer
*/

}
} // namespace nil::crypto3

#endif
