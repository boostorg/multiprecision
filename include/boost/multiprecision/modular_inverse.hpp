//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_MOD_INVERSE_HPP
#define CRYPTO3_MOD_INVERSE_HPP

#include <exception>

#include <boost/multiprecision/cpp_int.hpp>

#include <boost/multiprecision/inverse_euclid.hpp>

namespace nil {
namespace crypto3 {
using namespace boost::multiprecision;

template <typename Backend>
inline Backend eval_ct_inverse_mod_odd_modulus(const Backend &n,
                                               const Backend &mod)
{

   typedef
       typename default_ops::double_precision_type<Backend>::type double_type;
   typedef typename boost::multiprecision::detail::canonical<
       unsigned int, double_type>::type ui_type;

   using default_ops::eval_add;
   using default_ops::eval_eq;
   using default_ops::eval_get_sign;
   using default_ops::eval_integer_modulus;
   using default_ops::eval_is_zero;
   using default_ops::eval_lt;
   using default_ops::eval_msb;
   using default_ops::eval_multiply;
   using default_ops::eval_multiply_add;
   using default_ops::eval_multiply_subtract;
   using default_ops::eval_right_shift;

   typedef cpp_int_backend<0, 0, Backend::integer_type, Backend::checked_type,
                           secure_allocator<limb_type> >
       secure_cpp_int_backend;

   if (eval_get_sign(n) < 0 || eval_get_sign(mod) < 0)
   {
      BOOST_THROW_EXCEPTION(
          std::invalid_argument("ct_inverse_mod_odd_modulus: arguments must be "
                                "non-negative"));
   }

   if (eval_lt(mod, 3) || !eval_integer_modulus(mod, 2))
   {
      BOOST_THROW_EXCEPTION(
          std::invalid_argument("Bad modulus to ct_inverse_mod_odd_modulus"));
   }
   if (!eval_lt(n, mod))
   {
      BOOST_THROW_EXCEPTION(std::invalid_argument(
          "ct_inverse_mod_odd_modulus n >= mod not supported"));
   }

   /*
  This uses a modular inversion algorithm designed by Niels Möller
  and implemented in Nettle. The same algorithm was later also
  adapted to GMP in mpn_sec_invert.

  It can be easily implemented in a way that does not depend on
  secret branches or memory lookups, providing resistance against
  some forms of side channel attack.

  There is also a description of the algorithm in Appendix 5 of "Fast
  Software Polynomial Multiplication on ARM Processors using the NEON Engine"
  by Danilo Câmara, Conrado P. L. Gouvêa, Julio López, and Ricardo
  Dahab in LNCS 8182
     https://conradoplg.cryptoland.net/files/2010/12/mocrysen13.pdf

  Thanks to Niels for creating the algorithm, explaining some things
  about it, and the reference to the paper.
  */

   secure_cpp_int_backend mp1, mp1o2;
   eval_add(mod, 1);
   eval_right_shift(mp1o2, mp1, 1);

   // todo allow this to be pre-calculated and passed in as arg

   const size_t mod_words = mod.size();
   CRYPTO3_ASSERT(mod_words > 0, "Not empty");

   secure_cpp_int_backend a = n;
   secure_cpp_int_backend b = mod;
   secure_cpp_int_backend u = 1, v = 0;

   ct::poison(a.limbs(), a.size());
   ct::poison(b.limbs(), b.size());
   ct::poison(u.limbs(), u.size());
   ct::poison(v.limbs(), v.size());

   // Only n.bits() + mod.bits() iterations are required, but avoid leaking the
   // size of n
   ui_type bits = 2ul * eval_msb(mod);

   while (bits--)
   {
      const bool odd = eval_integer_modulus(a, 2);
      eval_multiply_subtract(a, odd, b);

      const bool underflow = eval_get_sign(a) < 0;
      eval_multiply_add(b, a, underflow);

      if (eval_get_sign(a) < 0)
      {
         eval_multiply(a, -1);
      }

      eval_right_shift(a, 1);

      if (underflow)
      {
         std::swap(u, v);
      }

      eval_multiply_subtract(u, odd, v);
      eval_multiply_add(u, eval_get_sign(u) < 0, mod);

      eval_right_shift(u, 1);
      eval_multiply_add(u, mp1o2, eval_integer_modulus(u, 2));
   }

   ct::unpoison(a.limbs(), a.size());
   ct::unpoison(b.limbs(), b.size());
   ct::unpoison(u.limbs(), u.size());
   ct::unpoison(v.limbs(), v.size());

   CRYPTO3_ASSERT(eval_is_zero(a), "A is zero");

   if (!eval_eq(b, 1))
   {
      return 0;
   }

   return v;
}

template <typename Backend>
inline Backend eval_inverse_mod(const Backend &x, const Backend &mod)
{
   using default_ops::eval_get_sign;
   using default_ops::eval_is_zero;
   using default_ops::eval_lt;

   if (eval_is_zero(mod))
   {
      BOOST_THROW_EXCEPTION(
          std::invalid_argument("eval_inverse_mod: zero division"));
   }
   if (eval_get_sign(mod) < 0 || eval_get_sign(x) < 0)
   {
      BOOST_THROW_EXCEPTION(std::invalid_argument(
          "eval_inverse_mod: arguments must be non-negative"));
   }

   if (eval_is_zero(x) || (!eval_is_zero(eval_integer_modulus(x, 2)) &&
                           !eval_is_zero(eval_integer_modulus(mod, 2))))
   {
      return 0;
   } // fast fail checks

   if (!eval_is_zero(eval_integer_modulus(mod, 2)) && eval_lt(x, mod))
   {
      return ct_inverse_mod_odd_modulus(x, mod);
   }

   return eval_inverse_euclid(x, mod);
}

/**
 * @brief Modular inversion
 * @param x a positive integer
 * @param mod a positive integer
 * @return y st (x*y) % modulus == 1 or 0 if no such value
 * @note Non-const time
 */
template <typename Backend, expression_template_option ExpressionTemplates>
inline number<Backend, ExpressionTemplates>
inverse_mod(const number<Backend, ExpressionTemplates> &x,
            const number<Backend, ExpressionTemplates> &mod)
{
   return eval_inverse_mod(x.backend(), mod.backend());
}

/**
 * @brief Const time modular inversion
 *
 * @param n
 * @param mod
 *
 * @note Requires the modulus be odd
 */
template <typename Backend, expression_template_option ExpressionTemplates>
inline number<Backend, ExpressionTemplates>
ct_inverse_mod_odd_modulus(const number<Backend, ExpressionTemplates> &n,
                           const number<Backend, ExpressionTemplates> &mod)
{
   return number<Backend, ExpressionTemplates>(
       eval_ct_inverse_mod_odd_modulus(n.backend(), mod.backend()));
}
}
} // namespace nil::crypto3

#endif // CRYPTO3_MOD_INVERSE_HPP
