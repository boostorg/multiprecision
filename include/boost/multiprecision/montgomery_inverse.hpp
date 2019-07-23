//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_MONTGOMERY_INVERSE_HPP
#define CRYPTO3_MONTGOMERY_INVERSE_HPP

#include <boost/multiprecision/pow_mod.hpp>

namespace nil {
namespace crypto3 {
template <typename Backend>
inline Backend eval_almost_montgomery_inverse(Backend &result, const Backend &a,
                                              const Backend &b)
{
   typedef
       typename default_ops::double_precision_type<Backend>::type double_type;
   typedef typename boost::multiprecision::detail::canonical<
       unsigned int, double_type>::type ui_type;

   using default_ops::eval_eq;
   using default_ops::eval_get_sign;
   using default_ops::eval_gt;
   using default_ops::eval_integer_modulus;
   using default_ops::eval_is_zero;
   using default_ops::eval_left_shift;
   using default_ops::eval_lt;
   using default_ops::eval_right_shift;

   ui_type k = 0;

   Backend u = b, v = a, r = 0, s = 1;

   while (eval_gt(v, 0))
   {
      if (!(eval_integer_modulus(u, 2)))
      {
         eval_right_shift(u, 1);
         eval_left_shift(s, 1);
      }
      else if (!eval_integer_modulus(v, 2))
      {
         eval_right_shift(v, 1);
         eval_left_shift(r, 1);
      }
      else if (eval_gt(u, v))
      {
         eval_subtract(u, v);
         eval_right_shift(u, 1);
         eval_add(r, s);
         eval_left_shift(s, 1);
      }
      else
      {
         eval_subtract(v, u);
         eval_right_shift(v, 1);
         eval_add(s, r);
         eval_left_shift(r, 1);
      }

      ++k;
   }

   if (!eval_lt(r, b))
   {
      eval_subtract(r, b);
   }

   eval_subtract(result, b, r);

   return k;
}

/*!
 * @brief Sets result to a^-1 * 2^k mod a with n <= k <= 2n
 *
 * "The Montgomery Modular Inverse - Revisited" Çetin Koç, E. Savas
 * https://citeseerx.ist.psu.edu/viewdoc/citations?doi=10.1.1.75.8377
 *
 * A const time implementation of this algorithm is described in
 * "Constant Time Modular Inversion" Joppe W. Bos
 * http://www.joppebos.com/files/CTInversion.pdf
 *
 * @note Non-const time
 *
 * @param result
 * @param a
 * @param b
 * @return a^-1 * 2^k mod b
 */
template <typename Backend, expression_template_option ExpressionTemplates>
inline number<Backend, ExpressionTemplates>
almost_montgomery_inverse(number<Backend, ExpressionTemplates> &      result,
                          const number<Backend, ExpressionTemplates> &a,
                          const number<Backend, ExpressionTemplates> &b)
{
   return number<Backend, ExpressionTemplates>(eval_almost_montgomery_inverse(
       result.backend(), a.backend(), b.backend()));
}

/**
 * Call almost_montgomery_inverse and correct the result to a^-1 mod b
 */
template <typename Backend>
inline Backend eval_normalized_montgomery_inverse(const Backend &a,
                                                  const Backend &p)
{
   using default_ops::eval_add;
   using default_ops::eval_integer_modulus;
   using default_ops::eval_right_shift;

   Backend r, k = eval_almost_montgomery_inverse(r, a, p);

   for (size_t i = 0; i != k; ++i)
   {
      if (eval_integer_modulus(r, 2))
      {
         eval_add(r, p);
      }
      eval_right_shift(r, 1);
   }

   return r;
}

/**
 * @brief Call almost_montgomery_inverse and correct the result to a^-1 mod b
 */
template <typename Backend, expression_template_option ExpressionTemplates>
inline number<Backend, ExpressionTemplates>
normalized_montgomery_inverse(const number<Backend, ExpressionTemplates> &a,
                              const number<Backend, ExpressionTemplates> &p)
{
   return number<Backend, ExpressionTemplates>(
       eval_normalized_montgomery_inverse(a.backend(), p.backend()));
}
}
} // namespace nil::crypto3

#endif // CRYPTO3_MONTGOMERY_INVERSE_HPP