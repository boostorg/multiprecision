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
#include <boost/multiprecision/detail/default_ops.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_int/cpp_int_config.hpp>
#include <boost/multiprecision/modular/base_params.hpp>
#include <boost/multiprecision/modular/barrett_params.hpp>

namespace boost {
namespace multiprecision {
namespace backends {

template <typename Backend>
Backend eval_extended_euclidean_algorithm (Backend& a, Backend& b, Backend& x, Backend& y) {
   if (eval_is_zero(a)) {
      typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;
      x = ui_type(0u);
      y = ui_type(1u);
      return b;
   }
   Backend x1, y1, tmp = b;
   eval_modulus(tmp, a);
   Backend d = eval_extended_euclidean_algorithm(tmp, a, x1, y1);
   tmp = b;
   eval_divide(tmp, a);
   eval_multiply(tmp, x1);
   x = y1;
   eval_subtract(x, tmp);
   y = x1;
   return d;
}

template <typename Backend>
Backend eval_inverse_extended_euclidean_algorithm (const Backend& a, const Backend& m) {
   Backend aa = a, mm = m, x, y, g;
   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;
   g = eval_extended_euclidean_algorithm(aa, mm, x, y);
   if (!eval_eq(g, ui_type(1u))) {
      //BOOST_THROW_EXCEPTION(std::invalid_argument("eval_inverse_with_gcd: no inverse element"));
      return ui_type(0u);
   } else {
      eval_modulus(x, m);
      eval_add(x, m);
      eval_modulus(x, m);
      return x;
   }
}

/*
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
/*
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

*/


template <typename Backend>
void eval_monty_inverse(Backend& res, const Backend& a, const Backend& p, const Backend& k)
{

   using default_ops::eval_modulus;
   using default_ops::eval_subtract;
   using default_ops::eval_abs;
   using default_ops::eval_gt;

   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;
   Backend zero = ui_type(0u);
   Backend one  = ui_type(1u);
   Backend two  = ui_type(2u);

   /*
    * From "A New Algorithm for Inversion mod p^k" by Çetin Kaya Koç
    * https://eprint.iacr.org/2017/411.pdf sections 5 and 7.
    */
   Backend c, tmp;

   //a^(-1) mod p:
   c = eval_inverse_extended_euclidean_algorithm(a, p);

   Backend bi         = one, bt, i = zero, k_negone = k, xi, nextp = one;
   eval_subtract(k_negone, one);
   res               = zero;

   ui_type kn = cpp_int(k_negone);

   while(!eval_eq(i, k))
   {
      //xi:
      xi = bi;
      eval_multiply(xi, c);
      eval_modulus(xi, p);

      if (eval_get_sign(xi) < 0) {
         tmp = xi;
         eval_abs(tmp, tmp);
         eval_modulus(tmp, p);
         xi = p;
         eval_subtract(xi, tmp);
      }

      //bi:
      tmp = a;
      eval_multiply(tmp, xi);
      eval_subtract(bi, tmp);
      eval_divide(bi, p);

      //res:
      tmp = xi;
      eval_multiply(tmp, nextp);
      eval_multiply(nextp, p);
      eval_add(res, tmp);
      eval_add(i, one);
   }
}


/*
template <typename Backend>
inline void bigint_shr1(typename mpl::front<typename Backend::unsigned_types>::type x[], size_t x_size,
                        size_t word_shift, size_t bit_shift)
{
   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;

   const size_t top = x_size >= word_shift ? (x_size - word_shift) : 0;

   if (top > 0)
      copy_mem(x, x + word_shift, top);
   clear_mem(x + top, std::min(word_shift, x_size));

   const auto   carry_mask  = CT::Mask<ui_type>::expand(bit_shift);
   const size_t carry_shift = carry_mask.if_set_return(BOTAN_MP_WORD_BITS - bit_shift);

   ui_type carry = 0;

   for (size_t i = 0; i != top; ++i)
   {
      const ui_type w = x[top - i - 1];
      x[top - i - 1]  = (w >> bit_shift) | carry;
      carry           = carry_mask.if_set_return(w << carry_shift);
   }
}

template <typename Backend>
inline typename mpl::front<typename Backend::unsigned_types>::type bigint_add2_nc(
    typename mpl::front<typename Backend::unsigned_types>::type x[], size_t x_size,
    const typename mpl::front<typename Backend::unsigned_types>::type y[], size_t y_size)
{
   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;

   ui_type carry = 0;

   BOOST_ASSERT_MSG(x_size >= y_size, "Expected sizes");

   const size_t blocks = y_size - (y_size % 8);

   for (size_t i = 0; i != blocks; i += 8)
      carry = word8_add2(x + i, y + i, carry);

   for (size_t i = blocks; i != y_size; ++i)
      x[i] = word_add(x[i], y[i], &carry);

   for (size_t i = y_size; i != x_size; ++i)
      x[i] = word_add(x[i], 0, &carry);

   return carry;
}

template <typename Backend>
inline typename mpl::front<typename Backend::unsigned_types>::type bigint_cnd_sub(
    typename mpl::front<typename Backend::unsigned_types>::type cnd,
    typename mpl::front<typename Backend::unsigned_types>::type x[], size_t x_size,
    const typename mpl::front<typename Backend::unsigned_types>::type y[], size_t y_size)
{
   BOOST_ASSERT_MSG(x_size >= y_size, "Expected sizes");

   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;

   const auto mask = CT::Mask<ui_type>::expand(cnd);

   ui_type carry = 0;

   const size_t blocks = y_size - (y_size % 8);
   ui_type      z[8]   = {0};

   for (size_t i = 0; i != blocks; i += 8)
   {
      carry = word8_sub3(z, x + i, y + i, carry);
      mask.select_n(x + i, z, x + i, 8);
   }

   for (size_t i = blocks; i != y_size; ++i)
   {
      z[0] = word_sub(x[i], y[i], &carry);
      x[i] = mask.select(z[0], x[i]);
   }

   for (size_t i = y_size; i != x_size; ++i)
   {
      z[0] = word_sub(x[i], 0, &carry);
      x[i] = mask.select(z[0], x[i]);
   }

   return mask.if_set_return(carry);
}

template <typename Backend>
inline typename mpl::front<typename Backend::unsigned_types>::type bigint_cnd_add(
    typename mpl::front<typename Backend::unsigned_types>::type       cnd,
    typename mpl::front<typename Backend::unsigned_types>::type       x[],
    typename mpl::front<typename Backend::unsigned_types>::type       x_size,
    const typename mpl::front<typename Backend::unsigned_types>::type y[], size_t y_size)
{
   BOTAN_ASSERT(x_size >= y_size, "Expected sizes");

   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;

   const auto mask = CT::Mask<ui_type>::expand(cnd);

   ui_type carry = 0;

   const size_t blocks = y_size - (y_size % 8);
   ui_type      z[8]   = {0};

   for (size_t i = 0; i != blocks; i += 8)
   {
      carry = word8_add3(z, x + i, y + i, carry);
      mask.select_n(x + i, z, x + i, 8);
   }

   for (size_t i = blocks; i != y_size; ++i)
   {
      z[0] = word_add(x[i], y[i], &carry);
      x[i] = mask.select(z[0], x[i]);
   }

   for (size_t i = y_size; i != x_size; ++i)
   {
      z[0] = word_add(x[i], 0, &carry);
      x[i] = mask.select(z[0], x[i]);
   }

   return mask.if_set_return(carry);
}

template <typename Backend>
inline void bigint_cnd_abs(typename mpl::front<typename Backend::unsigned_types>::type cnd,
                           typename mpl::front<typename Backend::unsigned_types>::type x[], size_t size)
{
   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;
   const auto                                                          mask = CT::Mask<ui_type>::expand(cnd);

   ui_type carry = mask.if_set_return(1);
   for (size_t i = 0; i != size; ++i)
   {
      const ui_type z = word_add(~x[i], 0, &carry);
      x[i]            = mask.select(z, x[i]);
   }
}

template <typename Backend>
inline void bigint_cnd_swap(typename mpl::front<typename Backend::unsigned_types>::type cnd,
                            typename mpl::front<typename Backend::unsigned_types>::type x[],
                            typename mpl::front<typename Backend::unsigned_types>::type y[], size_t size)
{
   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;
   const auto                                                          mask = CT::Mask<ui_type>::expand(cnd);

   for (size_t i = 0; i != size; ++i)
   {
      const ui_type a = x[i];
      const ui_type b = y[i];
      x[i]            = mask.select(b, a);
      y[i]            = mask.select(a, b);
   }
}


template <typename Backend>
void eval_inverse_mod_odd_modulus(Backend& res, const Backend& n, const Backend& mod)
{
   typedef typename mpl::front<typename Backend::signed_types>::type   si_type;
   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;

   // Caller should assure these preconditions:
   BOOST_ASSERT(eval_gt(n, 0));
   BOOST_ASSERT(eval_gt(mod, 0));
   BOOST_ASSERT(eval_lt(n, mod));
   BOOST_ASSERT(eval_ge(mod, 3) && eval_modulus(mod, 2) == 1);

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
/*
   const size_t mod_words = mod.size();
   BOOST_ASSERT_MSG(mod_words > 0, "Not empty");

   std::vector<ui_type> tmp_mem(5 * mod_words);

   ui_type* v_w   = &tmp_mem[0];
   ui_type* u_w   = &tmp_mem[1 * mod_words];
   ui_type* b_w   = &tmp_mem[2 * mod_words];
   ui_type* a_w   = &tmp_mem[3 * mod_words];
   ui_type* mp1o2 = &tmp_mem[4 * mod_words];

   //   ct::poison(tmp_mem.data(), tmp_mem.size());

   copy_mem(a_w, n.data(), std::min(n.size(), mod_words));
   copy_mem(b_w, mod.data(), std::min(mod.size(), mod_words));
   u_w[0] = 1;
   // v_w = 0

   // compute (mod + 1) / 2 which [because mod is odd] is equal to
   // (mod / 2) + 1
   copy_mem(mp1o2, mod.data(), std::min(mod.size(), mod_words));
   bigint_shr1(mp1o2, mod_words, 0, 1);
   ui_type carry = bigint_add2_nc(mp1o2, mod_words, u_w, 1);
   BOOST_ASSERT(carry == 0);

   // Only n.bits() + mod.bits() iterations are required, but avoid leaking the size of n
   const size_t execs = 2 * eval_msb(mod);

   for (size_t i = 0; i != execs; ++i)
   {
      const ui_type odd_a = a_w[0] & 1;

      //if(odd_a) a -= b
      ui_type underflow = bigint_cnd_sub(odd_a, a_w, b_w, mod_words);

      //if(underflow) { b -= a; a = abs(a); swap(u, v); }
      bigint_cnd_add(underflow, b_w, a_w, mod_words);
      bigint_cnd_abs(underflow, a_w, mod_words);
      bigint_cnd_swap(underflow, u_w, v_w, mod_words);

      // a >>= 1
      bigint_shr1(a_w, mod_words, 0, 1);

      //if(odd_a) u -= v;
      ui_type borrow = bigint_cnd_sub(odd_a, u_w, v_w, mod_words);

      // if(borrow) u += p
      bigint_cnd_add(borrow, u_w, mod.data(), mod_words);

      const ui_type odd_u = u_w[0] & 1;

      // u >>= 1
      bigint_shr1(u_w, mod_words, 0, 1);

      //if(odd_u) u += mp1o2;
      bigint_cnd_add(odd_u, u_w, mp1o2, mod_words);
   }

   auto a_is_0 = CT::Mask<ui_type>::set();
   for (size_t i = 0; i != mod_words; ++i)
      a_is_0 &= CT::Mask<ui_type>::is_zero(a_w[i]);

   auto b_is_1 = CT::Mask<ui_type>::is_equal(b_w[0], 1);
   for (size_t i = 1; i != mod_words; ++i)
      b_is_1 &= CT::Mask<ui_type>::is_zero(b_w[i]);

   BOOST_ASSERT_MSG(a_is_0.is_set(), "A is zero");

   // if b != 1 then gcd(n,mod) > 1 and inverse does not exist
   // in which case zero out the result to indicate this
   (~b_is_1).if_set_zero_out(v_w, mod_words);

   /*
   * We've placed the result in the lowest words of the temp buffer.
   * So just clear out the other values and then give that buffer to a
   * BigInt.
   */
/*
   clear_mem(&tmp_mem[mod_words], 4 * mod_words);

   CT::unpoison(tmp_mem.data(), tmp_mem.size());

   Backend r;
   r.swap_reg(tmp_mem);
   return r;
}

/*
template <typename Backend, expression_template_option ExpressionTemplates>
void inverse_mod_odd_modulus(number<Backend, ExpressionTemplates>&       res,
                             const number<Backend, ExpressionTemplates>& n,
                             const number<Backend, ExpressionTemplates>& mod)
{
   eval_inverse_mod_odd_modulus(res.backend(), n.backend(), mod.backend());
}
 */

/*
template <typename Backend>
std::size_t eval_almost_montgomery_inverse(Backend& result, const Backend& a,
                                           const Backend& p)
{
   size_t k = 0;

   Backend u = p, v = a, r = 0, s = 1;

   while (eval_gt(v, 0))
   {
      if (eval_integer_modulus(u, 2) == 0)
      {
         eval_right_shift(u, 1);
         eval_left_shift(s, 1);
      }
      else if (eval_integer_modulus(v, 2) == 0)
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

      k++;
   }

   if (!eval_gt(p, r))
   {
      eval_subtract(r, p);
   }

   result = p;

   eval_subtract(result, r);

   return k;
}
*/

/*
template <typename Backend, expression_template_option ExpressionTemplates>
std::size_t almost_montgomery_inverse(number<Backend, ExpressionTemplates>&       result,
                                      const number<Backend, ExpressionTemplates>& a,
                                      const number<Backend, ExpressionTemplates>& p)
{
   return eval_almost_montgomery_inverse(result.backend(), a.backend(), p.backend());
}
*/

/*
template <typename Backend>
Backend eval_normalized_montgomery_inverse(const Backend& a, const Backend& p)
{
   Backend     r;
   std::size_t k = eval_almost_montgomery_inverse(r, a, p);

   for (std::size_t i = 0; i != k; ++i)
   {
      if (eval_integer_modulus(p, 2) == 1)
      {
         eval_add(r, p);
      }
      eval_right_shift(r, 1);
   }

   return r;
}
*/

/*
template <typename Backend, expression_template_option ExpressionTemplates>
number<Backend, ExpressionTemplates> normalized_montgomery_inverse(
    const number<Backend, ExpressionTemplates>& a,
    const number<Backend, ExpressionTemplates>& p)
{
   return number<Backend, ExpressionTemplates>(
       evaL_normalized_montgomery_inverse(a.backned(), p.backend()));
}
 */

/*
template <typename Backend>
Backend eval_inverse_mod_pow2(Backend& a1, size_t k)
{
   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;
   /*
   * From "A New Algorithm for Inversion mod p^k" by Çetin Kaya Koç
   * https://eprint.iacr.org/2017/411.pdf sections 5 and 7.
   */
/*
   if (eval_integer_modulus(a1, 2) == 0)
      return 0;

   Backend a = a1;
   eval_bit_set(a, k);

   Backend b = 1, X = 0, newb;

   const std::size_t a_words = a.sig_words();

   X.grow_to(round_up(k, sizeof(ui_type) * CHAR_BIT) / sizeof(ui_type) * CHAR_BIT);
   b.grow_to(a_words);

   /*
   Hide the exact value of k. k is anyway known to word length
   granularity because of the length of a, so no point in doing more
   than this.
   */
/*

   const std::size_t iter = round_up(k, sizeof(ui_type) * CHAR_BIT);

   for (std::size_t i = 0; i != iter; ++i)
   {
      const bool b0 = eval_bit_test(b, 0);
      X.conditionally_set_bit(i, b0);
      newb = b;
      eval_subtract(newb, a);
      b.ct_cond_assign(b0, newb);
      eval_right_shift(b, 1);
   }
   eval_bit_set(X, k);
   X.const_time_unpoison();
   return X;
}
*/

/*
template <typename Backend, expression_template_option ExpressionTemplates>
number<Backend, ExpressionTemplates> inverse_mod_pow2(
    const number<Backend, ExpressionTemplates>& a1, size_t k)
{
   return number<Backend, ExpressionTemplates>(
       eval_inverse_mod_pow2(a1.backend(), k.backend()));
}
*/

/*
template <typename Backend>
Backend eval_inverse_mod(Backend& res, const Backend& n, const Backend& mod)
{
   if (eval_is_zero(mod))
   {
      BOOST_THROW_EXCEPTION(
          std::invalid_argument("eval_inverse_mod: mod must be non zero"));
   }
   if ((eval_get_sign(mod) < 0) || (eval_get_sign(n) < 0))
   {
      BOOST_THROW_EXCEPTION(
          std::invalid_argument("eval_inverse_mod: arguments must be non-negative"));
   }
   if (eval_is_zero(n) || (eval_integer_modulus(n, 2) == 0 && eval_integer_modulus(mod, 2) == 0))
   {
      return 0;
   }
   if (eval_integer_modulus(n, 2) == 1)
   {
      /*
      Fastpath for common case. This leaks information if n > mod
      but we don't guarantee const time behavior in that case.
      */
/*
      if (eval_gt(mod, n))
         return eval_inverse_mod_odd_modulus(n, mod);
      else
         return eval_inverse_mod_odd_modulus(ct_modulo(n, mod), mod);
   }

   const std::size_t mod_lz = eval_lsb(mod);
   BOOST_ASSERT(mod_lz > 0);
   const std::size_t mod_bits = eval_msb(mod);
   BOOST_ASSERT(mod_bits > mod_lz);

   if (mod_lz == mod_bits - 1)
   {
      // In this case we are performing an inversion modulo 2^k
      return eval_inverse_mod_pow2(n, mod_lz);
   }

   /*
   * In this case we are performing an inversion modulo 2^k*o for
   * some k > 1 and some odd (not necessarily prime) integer.
   * Compute the inversions modulo 2^k and modulo o, then combine them
   * using CRT, which is possible because 2^k and o are relatively prime.
   */
/*
   Backend o = mod;

   eval_right_shift(mod, mod_lz);

   Backend n_redc = ct_modulo(n, o);
   Backend inv_o  = eval_inverse_mod_odd_modulus(n_redc, o);
   Backend inv_2k = eval_inverse_mod_pow2(n, mod_lz);

   // No modular inverse in this case:
   if (eval_is_zero(o) || eval_is_zero(inv_2k))
      return 0;

   Backend m2k = mod_lz;
   eval_multiply(m2k, m2k);
   // Compute the CRT parameter
   Backend c = inverse_mod_pow2(o, mod_lz);

   // Compute h = c*(inv_2k-inv_o) mod 2^k
   Backend h = inv_2k;

   eval_subtract(h, inv_o);
   eval_multiply(h, c);

   const bool h_neg = (eval_get_sign(h) < 0);

   eval_abs(h); // h.set_sign(BigInt::Positive);
   eval_bit_set(h, mod_lz);

   const bool h_nonzero = !eval_is_zero(h);

   eval_subtracr(m2k, h);
   h.ct_cond_assign(h_nonzero && h_neg, m2k);

   // Return result inv_o + h * o
   eval_multiply(h, o);
   eval_add(h, inv_o);

   return h;
}
*/

/*
template <typename Backend, expression_template_option ExpressionTemplates>
number<Backend, ExpressionTemplates> inverse_mod(const number<Backend, ExpressionTemplates>& n,
                                                 const number<Backend, ExpressionTemplates>& mod)
{
   return number<Backend, ExpressionTemplates>(eval_inverse_mod(n.backend(), mod.backend()));
}
*/

/*
template <typename IntegerType, typename = typename enable_if<typename is_trivial_cpp_int<IntegerType>::value>::type>
IntegerType monty_inverse(const IntegerType& a)
{
   return eval_monty_inverse(a);
}
 */

template <typename Backend, expression_template_option ExpressionTemplates>
number<Backend, ExpressionTemplates> inverse_extended_euclidean_algorithm(const number<Backend, ExpressionTemplates>& a, const number<Backend, ExpressionTemplates>& p)
{
   number<Backend, ExpressionTemplates> res;
   return eval_inverse_extended_euclidean_algorithm(a.backend(), p.backend());
}


template <typename Backend, expression_template_option ExpressionTemplates>
number<Backend, ExpressionTemplates> monty_inverse(const number<Backend, ExpressionTemplates>& a, const number<Backend, ExpressionTemplates>& p, const number<Backend, ExpressionTemplates>& k)
{
   number<Backend, ExpressionTemplates> res;
   eval_monty_inverse(res.backend(), a.backend(), p.backend(), k.backend());
   return res;
}


/*
template <typename IntegerType, typename = typename enable_if<!typename is_trivial_cpp_int<IntegerType>::value>::type>
IntegerType monty_inverse(const IntegerType& a)
{
   IntegerType res;
   eval_monty_inverse(res.backend(), a.backend());
   return res;
}
 */
}

}
} // namespace boost::multiprecision::backends

#endif
