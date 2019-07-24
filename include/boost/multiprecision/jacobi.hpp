//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_JACOBI_HPP
#define BOOST_MULTIPRECISION_JACOBI_HPP

#include <boost/multiprecision/cpp_int.hpp>

namespace boost {
namespace multiprecision {

template <typename Backend>
inline limb_type eval_jacobi(const Backend &a, const Backend &n)
{
   using default_ops::eval_get_sign;
   using default_ops::eval_gt;
   using default_ops::eval_integer_modulus;
   using default_ops::eval_is_zero;
   using default_ops::eval_lsb;
   using default_ops::eval_lt;
   using default_ops::eval_modulus;
   using default_ops::eval_right_shift;

   if (eval_get_sign(a) < 0)
   {
      BOOST_THROW_EXCEPTION(std::invalid_argument("jacobi: first argument must be non-negative"));
   }
   if (!eval_integer_modulus(n, 2) || eval_lt(n, 2))
   {
      BOOST_THROW_EXCEPTION(std::invalid_argument("jacobi: second argument must be odd and > 1"));
   }

   Backend   x = a, y = n;
   limb_type J = 1;

   while (eval_gt(y, 1))
   {
      eval_modulus(x, y);

      Backend yd2 = y;
      eval_divide(yd2, 2);

      if (eval_gt(x, yd2))
      {
         eval_subtract(x, y, x);
         if (eval_integer_modulus(y, 4) == 3)
         {
            J = -J;
         }
      }
      if (eval_is_zero(x))
      {
         return 0;
      }

      size_t shifts = eval_lsb(x);
      eval_right_shift(x, shifts);
      if (eval_integer_modulus(shifts, 2))
      {
         limb_type y_mod_8 = eval_integer_modulus(y, 8);
         if (y_mod_8 == 3 || y_mod_8 == 5)
         {
            J = -J;
         }
      }

      if (eval_integer_modulus(x, 4) == 3 && eval_integer_modulus(y, 4) == 3)
      {
         J = -J;
      }

      std::swap(x, y);
   }
   return J;
}

/**
         * Compute the Jacobi symbol. If n is prime, this is equivalent
         * to the Legendre symbol.
         * @see http://mathworld.wolfram.com/JacobiSymbol.html
         *
         * @param a is a non-negative integer
         * @param n is an odd integer > 1
         * @return (n / m)
         */
template <typename Backend, expression_template_option ExpressionTemplates>
inline typename std::enable_if<number_category<Backend>::value == number_kind_integer, limb_type>::type jacobi(
    const number<Backend, ExpressionTemplates> &a, const number<Backend, ExpressionTemplates> &n)
{
   return number<Backend, ExpressionTemplates>(eval_jacobi(a.backend(), n.backend()));
}
}
} // namespace boost::multiprecision

#endif //BOOST_MULTIPRECISION_JACOBI_HPP