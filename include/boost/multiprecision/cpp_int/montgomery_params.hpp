//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MONTGOMERY_PARAMS_HPP
#define BOOST_MULTIPRECISION_MONTGOMERY_PARAMS_HPP

#include <boost/multiprecision/modular/modular_reduce.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/multiprecision/cpp_int/cpp_int_config.hpp>
#include <boost/multiprecision/tommath.hpp>

namespace boost {
namespace multiprecision {

/**
 * Parameters for Montgomery Reduction
 */
template <typename Backend>
class montgomery_params
{
   typedef number<Backend> number_type;
 public:

   montgomery_params() {}
   /**
   * Initialize a set of Montgomery reduction parameters. These values
   * can be shared by all values in a specific Montgomery domain.
   */
   //TODO: only with cpp_int (or another backends)
   template <typename Number>
   explicit montgomery_params(const Number & p) {
      find_const_variables(p);
   }

   const number_type& mod() const { return m_mod; }

   const number_type& r1() const { return m_r1; }

   const number_type& r2() const { return m_r2; }

   const number_type& r3() const { return m_r3; }

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
      return mod();
   };


 private:
   limb_type monty_inverse(limb_type a)
   {
      if (a % 2 == 0)
      {
         throw std::invalid_argument("Monty_inverse only valid for odd integers");
      }

      limb_type b = 1;
      limb_type r = 0;

      for (size_t i = 0; i != Backend::limb_bits; ++i)
      {
         const limb_type bi = b % 2;
         r >>= 1;
         r += bi << (Backend::limb_bits - 1);

         b -= a * bi;
         b >>= 1;
      }

      // Now invert in addition space
      r = (~static_cast<limb_type>(0) - r) + 1;

      return r;
   }

   template <typename T>
   typename boost::enable_if_c<is_number<T>::value || is_integral<T>::value, void>::type find_const_variables(const T& p) {
      if (p < 0 || !(p % 2))
      {
         throw std::invalid_argument("montgomery_params invalid modulus");
      }

      m_mod = p;

      m_p_words = m_mod.backend().size();

      m_p_dash = monty_inverse(m_mod.backend().limbs()[0]);

      number_type r;

      default_ops::eval_bit_set(r.backend(), m_p_words * Backend::limb_bits);

      m_r1 = r;
      eval_mod_redc(m_r1.backend(), m_mod.backend());
      m_r2 = m_r1 * m_r1;
      eval_mod_redc(m_r2.backend(), m_mod.backend());
      m_r3 = m_r2 * m_r1;
      eval_mod_redc(m_r3.backend(), m_mod.backend());
   }

   number_type m_r1;
   number_type m_r2;
   number_type m_r3;
   limb_type   m_p_dash;
   size_t      m_p_words;
   number_type m_mod;
};


/**
 * Parameters for Montgomery Reduction for tommath
 */
template <>
class montgomery_params<tommath_int>
{
   typedef number<tommath_int> number_type;
 public:

   montgomery_params() {}
   /**
   * Initialize a set of Montgomery reduction parameters. These values
   * can be shared by all values in a specific Montgomery domain.
   */
   //TODO: only with cpp_int (or another backends)
   template <typename Number>
   explicit montgomery_params(const Number & p) {
      find_const_variables(p);
   }

   const number_type& mod() const { return m_mod; }

   const number_type& r1() const { return m_r1; }

   const ::mp_digit& tho() const { return m_tho; }

   template <typename T>
   typename boost::enable_if_c<is_number<T>::value || is_integral<T>::value, void>::type find_const_variables(const T& p)
   {
      m_mod = p;
      mp_montgomery_setup(const_cast< ::mp_int*>(&m_mod.backend().data()), &m_tho);
   }

   template <class V>
   montgomery_params& operator=(const V& v)
   {
      find_const_variables(v);
      return *this;
   }

   template <typename BackendT, expression_template_option ExpressionTemplates>
   operator number<BackendT, ExpressionTemplates>()
   {
      return mod();
   };


 private:
   number_type m_r1;
   ::mp_digit m_tho;
   number_type m_mod;
};

}
} // namespace boost::multiprecision

#endif
