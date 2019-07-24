#ifndef BOOST_MULTIPRECISION_INVERSE_EUCLID_HPP
#define BOOST_MULTIPRECISION_INVERSE_EUCLID_HPP

#include <boost/multiprecision/cpp_int.hpp>

namespace boost {
namespace multiprecision {

template <typename Backend, expression_template_option ExpressionTemplates>
inline Backend eval_inverse_euclid(const Backend &n, const Backend &mod)
{

   typedef typename default_ops::double_precision_type<Backend>::type                         double_type;
   typedef typename boost::multiprecision::detail::canonical<unsigned int, double_type>::type ui_type;

   using default_ops::eval_add;
   using default_ops::eval_eq;
   using default_ops::eval_get_sign;
   using default_ops::eval_integer_modulus;
   using default_ops::eval_is_zero;
   using default_ops::eval_lt;
   using default_ops::eval_right_shift;
   using default_ops::eval_subtract;

   if (eval_is_zero(mod))
   {
      BOOST_THROW_EXCEPTION(std::invalid_argument("inverse_mod: arguments must be non-zero"));
   }
   if (eval_get_sign(mod) || eval_get_sign(n))
   {
      BOOST_THROW_EXCEPTION(std::invalid_argument("inverse_mod: arguments must be non-negative"));
   }

   if (eval_is_zero(n) || (!eval_integer_modulus(n, 2) && !eval_integer_modulus(mod, 2)))
   {
      return 0;
   } // fast fail checks

   number<Backend, ExpressionTemplates> u = mod, v = n;
   number<Backend, ExpressionTemplates> a = 1, b = 0, c = 0, d = 1;

   while (!eval_is_zero(u))
   {
      const ui_type u_zero_bits = lsb(u);
      eval_right_shift(u, u_zero_bits);

      for (std::size_t i = 0; i != u_zero_bits; ++i)
      {
         if (eval_integer_modulus(a, 2) || eval_integer_modulus(b, 2))
         {
            eval_add(a, n);
            eval_subtract(b, mod);
         }
         eval_right_shift(a, 1);
         eval_right_shift(b, 1);
      }

      const ui_type v_zero_bits = lsb(v);
      eval_right_shift(v, v_zero_bits);
      for (size_t i = 0; i != v_zero_bits; ++i)
      {
         if (eval_integer_modulus(c, 2) || eval_integer_modulus(d, 2))
         {
            eval_add(c, n);
            eval_subtract(d, mod);
         }
         eval_right_shift(c, 1);
         eval_right_shift(d, 1);
      }

      if (!eval_lt(u, v))
      {
         eval_subtract(u, v);
         eval_subtract(a, c);
         eval_subtract(b, d);
      }
      else
      {
         eval_subtract(v, u);
         eval_subtract(c, a);
         eval_subtract(d, b);
      }
   }

   if (!eval_eq(v, 1))
   {
      return 0;
   } // no modular inverse

   while (eval_lt(d, 0))
   {
      eval_add(d, mod);
   }
   while (d >= mod)
   {
      eval_subtract(d, mod);
   }

   return d;
}

/**
         * Modular inversion using extended binary Euclidian algorithm
         * @param x a positive integer
         * @param modulus a positive integer
         * @return y st (x*y) % modulus == 1 or 0 if no such value
         * @note Non-const time
         */
template <typename Backend, expression_template_option ExpressionTemplates>
inline number<Backend, ExpressionTemplates> inverse_euclid(const number<Backend, ExpressionTemplates> &n,
                                                           const number<Backend, ExpressionTemplates> &mod)
{
   return number<Backend, ExpressionTemplates>(eval_inverse_euclid(n.backend(), mod.backend()));
}
}
} // namespace boost::multiprecision

#endif //BOOST_MULTIPRECISION_INVERSE_EUCLID_HPP
