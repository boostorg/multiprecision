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

#include <boost/multiprecision/montgomery/inverse.hpp>
#include <boost/multiprecision/montgomery/modular_reduce.hpp>

#include <boost/type_traits/is_integral.hpp>

#include <boost/multiprecision/cpp_int/cpp_int_config.hpp>

namespace boost {
namespace multiprecision {

/**
 * Parameters for Montgomery Reduction
 */
template <typename Backend>
class montgomery_params
{
   //typedef typename std::enable_if<is_number<NumberType>::value>::type number_type;
   typedef number<Backend> number_type;
 public:

   montgomery_params() {}
   /**
   * Initialize a set of Montgomery reduction parameters. These values
   * can be shared by all values in a specific Montgomery domain.
   */
   //TODO: only with cpp_int (or another backends)
   template <typename Number,
             typename = typename boost::enable_if_c<is_number<Number>::value || is_integral<Number>::value>::type >
   explicit montgomery_params(const Number& p) {
      find_const_variables(p);
   }

   const number_type& mod() const { return m_mod; }

   const number_type& p() const { return m_p; }

   const number_type& R1() const { return m_r1; }

   const number_type& R2() const { return m_r2; }

   const number_type& R3() const { return m_r3; }

   limb_type p_dash() const { return m_p_dash; }

   size_t p_words() const { return m_p_words; }

   template <class V>
   montgomery_params& operator=(const V& v)
   {
      find_const_variables(v);
      return *this;
   }

   template <typename BackendT, expression_template_option ExpressionTemplates>
   operator number<BackendT, ExpressionTemplates>()
   {
      return p();
   };


 private:
   template <typename T>
   void find_const_variables(const T& p) {
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
      eval_mod_redc(m_r1.backend(), m_p.backend());
      m_r2 = m_r1 * m_r1;
      eval_mod_redc(m_r2.backend(), m_p.backend());
      m_r3 = m_r2 * m_r1;
      eval_mod_redc(m_r3.backend(), m_p.backend());
   }

   number_type m_p;
   number_type m_r1;
   number_type m_r2;
   number_type m_r3;
   limb_type   m_p_dash;
   size_t      m_p_words;
   number_type m_mod;
};

}
} // namespace boost::multiprecision

#endif
