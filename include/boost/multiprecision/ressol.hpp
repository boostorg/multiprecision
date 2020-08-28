//---------------------------------------------------------------------------//
// Copyright (c) 2018-2020 Nil Foundation AG
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2020 Alexey Moskvin
// Copyright (c) 2018-2020 Pavel Kharitonov <ipavrus@nilfoundation.org>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_RESSOL_HPP
#define BOOST_MULTIPRECISION_RESSOL_HPP

#include <boost/multiprecision/jacobi.hpp>
#include <boost/multiprecision/detail/default_ops.hpp>

#include <boost/multiprecision/modular/modular_adaptor.hpp>
#include <boost/multiprecision/modular/modular_params.hpp>

namespace boost {
namespace multiprecision {

template <typename Backend>
inline Backend eval_ressol(const Backend& a, const Backend& p)
{

   using default_ops::eval_bit_set;
   using default_ops::eval_eq;
   using default_ops::eval_get_sign;
   using default_ops::eval_gt;
   using default_ops::eval_integer_modulus;
   using default_ops::eval_is_zero;
   using default_ops::eval_lsb;
   using default_ops::eval_lt;
   using default_ops::eval_right_shift;
   using default_ops::eval_left_shift;
   using default_ops::eval_subtract;
   using default_ops::eval_add;

   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;

   Backend zero = ui_type(0u);
   Backend posone = ui_type(1u);
   Backend two = ui_type(2u);
   Backend negone;

   eval_subtract(negone, posone);

   if (eval_is_zero(a))
   {
      return zero;
   }
   else if (eval_get_sign(a) < 0)
   {
      BOOST_THROW_EXCEPTION(
          std::invalid_argument("ressol: value to solve for must be positive"));
   }
   else if (!eval_lt(a, p))
   {
      BOOST_THROW_EXCEPTION(std::invalid_argument(
          "ressol: value to solve for must be less than p"));
   }

   if (eval_eq(p, 2))
   {
      return a;
   }
   else if (!eval_gt(p, 1))
   {
      BOOST_THROW_EXCEPTION(std::invalid_argument("ressol: prime must be > 1 a"));
   }
   else if (eval_integer_modulus(p, 2) == 0)
   {
      BOOST_THROW_EXCEPTION(std::invalid_argument("ressol: invalid prime"));
   }

   if (eval_jacobi(a, p) != 1)
   { // not a quadratic residue
      return negone;
   }


   modular_adaptor<Backend> a_mod;

   assign_components(a_mod, a, p);
   
   if (eval_integer_modulus(p, 4) == 3)
   {     
      Backend exp = p, res;

      eval_add(exp, posone);

      eval_right_shift(exp, 2);

      modular_adaptor<Backend> res_mod(zero, p);

      eval_pow(res_mod, a_mod, exp);

      res_mod.mod_data().adjust_regular(res, res_mod.base_data());

      return res;
   }
   
   Backend p_negone = p, q = p;
   eval_add(p_negone, negone);
   size_t s = eval_lsb(p_negone);

   eval_right_shift(q, s);
   eval_subtract(q, posone);
   eval_right_shift(q, 1);

   modular_adaptor <Backend> r_mod, n_mod = a_mod, r_sq_mod;

   eval_pow(r_mod, a_mod, q);
   eval_pow(r_sq_mod, r_mod, two);
   eval_multiply(n_mod, r_sq_mod);
   eval_multiply(r_mod, a_mod);

   if (eval_eq(n_mod.base_data(), posone))
   {
      return r_mod.base_data();
   }

   // find random non quadratic residue z
   Backend z = two;
   while (eval_jacobi(z, p) == 1)
   { // while z quadratic residue
      eval_add(z, posone);
   }

   eval_left_shift(q, 1);
   eval_add(q, posone);

   modular_adaptor <Backend> z_mod, c_mod, q_mod; 

   assign_components(z_mod, z, p);

   eval_pow(c_mod, z_mod, q);

   while (eval_gt(n_mod.base_data(), posone))
   {
      q_mod = n_mod;

      size_t i = 0;
      while (!eval_eq(q_mod.base_data(), posone))
      {  
         eval_pow(q_mod, q_mod, two);
         ++i;

         if (i >= s)
         {
            return negone;
         }
      }

      Backend p2;

      eval_bit_set(p2, s - i - 1);
      eval_pow(c_mod, c_mod, p2);
      eval_multiply(r_mod, c_mod);
      eval_pow(c_mod, c_mod, two);
      eval_multiply(n_mod, c_mod);

      s = i;
   }

   return r_mod.base_data();
}

/**
 * Compute the square root of x modulo a prime using the
 * Shanks-Tonnelli algorithm
 *
 * @param x the input
 * @param p the prime
 * @return y such that (y*y)%p == x, or -1 if no such integer
 */
template <typename Backend, expression_template_option ExpressionTemplates>
inline number<Backend, ExpressionTemplates>
ressol(const number<Backend, ExpressionTemplates>& a,
       const number<Backend, ExpressionTemplates>& p)
{
   return number<Backend, ExpressionTemplates>(
       eval_ressol(a.backend(), p.backend()));
}

}
} // namespace boost::multiprecision

#endif // BOOST_MULTIPRECISION_RESSOL_HPP
