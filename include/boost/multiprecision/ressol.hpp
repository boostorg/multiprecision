//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_RESSOL_HPP
#define BOOST_MULTIPRECISION_RESSOL_HPP

#include <boost/multiprecision/jacobi.hpp>

#include <boost/multiprecision/modular/modular_adaptor.hpp>

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
   using default_ops::eval_subtract;
   using default_ops::eval_add;
   using default_ops::eval_divide;

   Backend zero, posone, negone;
   eval_subtract(zero, a, a);
   eval_add(posone, zero, 1);
   eval_subtract(negone, zero, posone);

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
   else if (eval_integer_modulus(p, 2))
   {
      BOOST_THROW_EXCEPTION(std::invalid_argument("ressol: invalid prime"));
   }

   if (eval_jacobi(a, p) != 1)
   { // not a quadratic residue
      return negone;
   }

   modular_adaptor<Backend> res, a_mod(a, p);

   Backend shift, p_posone;

   eval_add(p_posone, p, 1);

   shift = p_posone;

   if (eval_integer_modulus(p, 4) == 3)
   {
      eval_divide(shift, shift, 4);
      eval_pow(res, a_mod, shift);
      return res.base_data();
   }
   
   Backend p_negone;
   eval_subtract(p_negone, p, posone);
   size_t s = eval_lsb(p_negone);

   Backend q = p, two;
   eval_add(two, posone, posone);

   modular_adaptor<Backend> p_mod(p, p_posone), two_s_mod(posone, p), two_mod(two, p); 

   //eval_right_shift(p_mod, s);
   size_t i = 0;
   while (i < s) {
      eval_multiply(two_s_mod, two_mod);
      ++i;
   }
   eval_divide(p_mod, two_s_mod);

   eval_subtract(q, posone);

   modular_adaptor<Backend>  q_mod(q, p_posone);
   //eval_right_shift(q_mod, posone);
   eval_divide(q_mod, two_mod);

   modular_adaptor <Backend> r = a_mod, n = a_mod, r_sq;
   eval_pow(r, a_mod, q_mod.base_data());
   eval_pow(r_sq, r, two);
   eval_multiply(n, r_sq);
   eval_multiply(r, a_mod);

   if (eval_eq(n.base_data(), posone))
   {
      return r.base_data();
   }

   // find random non quadratic residue z
   modular_adaptor<Backend>  z_mod(two, p);
   while (eval_jacobi(z_mod.base_data(), p) == 1)
   { // while z quadratic residue
      eval_add(z_mod, z_mod, posone);
   }

   res = z_mod;
   //eval_left_shift(res, posone);
   eval_multiply(res, two_mod);
   eval_add(res, res, posone);
   eval_pow(r, z_mod, res.base_data());
   Backend c = r.base_data();

   while (eval_gt(n.base_data(), posone))
   {
      q_mod = n;

      size_t i = 0;
      while (!eval_eq(q_mod.base_data(), posone))
      {  
         eval_pow(q_mod, q_mod, two_mod);
         ++i;

         if (i >= s)
         {
            return negone;
         }
      }

      Backend p2;
      eval_bit_set(p2, s - i - 1);


      modular_adaptor <Backend> c_mod(c, p), p2_mod(p2, p);
      eval_pow(c_mod, c_mod, p2);
      eval_multiply(r, c_mod);
      eval_pow(c_mod, c_mod, two_mod);
      eval_multiply(n, c_mod);
      s = i;
   }

   return r.base_data();
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
