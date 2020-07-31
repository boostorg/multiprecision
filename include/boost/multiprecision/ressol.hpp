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
#include <boost/multiprecision/pow_mod.hpp>

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

   if (eval_is_zero(a))
   {
      return Backend(0);
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
      return Backend(-1);
   }

   if (eval_integer_modulus(p, 4) == 3)
   {
      return power_mod(a, ((p + 1) >> 2), p);
   }

   size_t s = eval_lsb(p - 1);

   Backend q = p;

   eval_right_shift(p, s);
   eval_subtract(q, 1);
   eval_right_shift(q, 1);

   modular_reducer mod_p(p);

   Backend r = eval_power_mod(a, q, p);
   Backend n = mod_p.multiply(a, mod_p.square(r));
   r         = mod_p.multiply(r, a);

   if (eval_eq(n, 1))
   {
      return r;
   }

   // find random non quadratic residue z
   Backend z = 2;
   while (eval_jacobi(z, p) == 1)
   { // while z quadratic residue
      ++z;
   }

   Backend c = eval_power_mod(z, (q << 1) + 1, p);

   while (eval_gt(n, 1))
   {
      q = n;

      size_t i = 0;
      while (q != 1)
      {
         q = mod_p.square(q);
         ++i;

         if (i >= s)
         {
            return -Backend(1);
         }
      }

      Backend p2;
      eval_bit_set(p2, s - i - 1);

      c = power_mod(c, p2, p);
      r = mod_p.multiply(r, c);
      c = mod_p.square(c);
      n = mod_p.multiply(n, c);
      s = i;
   }

   return r;
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
