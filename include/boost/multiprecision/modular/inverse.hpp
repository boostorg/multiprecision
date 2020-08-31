//---------------------------------------------------------------------------//
// Copyright (c) 2020 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MODULAR_INVERSE_HPP
#define BOOST_MULTIPRECISION_MODULAR_INVERSE_HPP

#include <boost/container/vector.hpp>

#include <boost/type_traits/is_integral.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_int/cpp_int_config.hpp>
#include <boost/multiprecision/modular/base_params.hpp>
#include <boost/multiprecision/modular/barrett_params.hpp>
#include <boost/multiprecisio/gmp.hpp>

namespace boost {
namespace multiprecision {
namespace backends {

template <typename Backend>
typename mpl::front<typename Backend::signed_types>::type eval_monty_inverse(typename mpl::front<typename Backend::signed_types>::type a)
{
   typedef typename mpl::front<typename Backend::signed_types>::type si_type;

   if (a % 2 == 0)
   {
      throw std::invalid_argument("monty_inverse only valid for odd integers");
   }

   /*
    * From "A New Algorithm for Inversion mod p^k" by Çetin Kaya Koç
    * https://eprint.iacr.org/2017/411.pdf sections 5 and 7.
    */

   si_type b = 1;
   si_type r = 0;

   for (size_t i = 0; i != sizeof(si_type) * CHAR_BIT; ++i)
   {
      const si_type bi = b % 2;
      r >>= 1;
      r += bi << (sizeof(si_type) * CHAR_BIT - 1);

      b -= a * bi;
      b >>= 1;
   }

   // Now invert in addition space
   r = (~static_cast<si_type>(0) - r) + 1;

   return r;
}

template <typename Backend>
void eval_monty_inverse(Backend& res, const Backend& a)
{
   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;
   if (eval_modulus(a, 2) == 0)
   {
      throw std::invalid_argument("monty_inverse only valid for odd integers");
   }

   /*
    * From "A New Algorithm for Inversion mod p^k" by Çetin Kaya Koç
    * https://eprint.iacr.org/2017/411.pdf sections 5 and 7.
    */

   Backend b         = ui_type(1u), bi, bt;
   res               = ui_type(0u);
   std::size_t abits = eval_msb(a);

   for (std::size_t i = 0; i != abits; ++i)
   {
      eval_modulus(bi, b, 2);
      eval_right_shift(res, 1);
      eval_right_shift(bt, bi, abits - 1);
      eval_add(res, res, bt);
      eval_multiply(bt, a, bi);
      eval_subtract(b, bt);
      eval_right_shift(b, 1);
   }

   b = ui_type(0u);

   // Now invert in addition space
   eval_complement(b, b);
   eval_subtract(b, res);
   eval_add(res, 1);
}

template <typename Backend>
void inverse_mod_odd_modulus(Backend& res, const Backend& n, const Backend& mod)
{
  eval_inverse_mod_odd_modulus(res, n, mod);
}

template <typename Backend, expression_template_option ExpressionTemplates>
std::size_t almost_montgomery_inverse(number<Backend, ExpressionTemplates>&       result,
                                      const number<Backend, ExpressionTemplates>& a,
                                      const number<Backend, ExpressionTemplates>& p)
{
   eval_almost_montgomery_inverse(result, a, p);
   return result;
}

template <typename Backend, expression_template_option ExpressionTemplates>
number<Backend, ExpressionTemplates> normalized_montgomery_inverse(const number<Backend, ExpressionTemplates>& a, const number<Backend, ExpressionTemplates>& p)
{
   return normalized_montgomery_inverse(a, p);
}

template <typename Backend, expression_template_option ExpressionTemplates>
number<Backend, ExpressionTemplates> inverse_mod_pow2(const number<Backend, ExpressionTemplates>& a1, size_t k)
{
   return eval_inverse_mod_pow2(a1, k);
}

number<Backend, ExpressionTemplates> inverse_mod(const number<Backend, ExpressionTemplates>& n, const number<Backend, ExpressionTemplates>& mod)
{
   return eval_inverse_mod(n, mod);
}

template <typename IntegerType>
IntegerType monty_inverse(const IntegerType& a)
{
   number<IntegerType> res;
   if (IntgerType == number)
   {
      eval_monty_inverse(res, a);
   } else
   {
      res = eval_monty_inverse(a);
   }

   return res;
}
}

} // namespace boost::multiprecision

#endif
