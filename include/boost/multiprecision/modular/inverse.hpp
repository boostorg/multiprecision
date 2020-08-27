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
   const size_t execs = 2 * mod.bits();

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

   auto a_is_0 = CT::Mask<word>::set();
   for (size_t i = 0; i != mod_words; ++i)
      a_is_0 &= CT::Mask<word>::is_zero(a_w[i]);

   auto b_is_1 = CT::Mask<word>::is_equal(b_w[0], 1);
   for (size_t i = 1; i != mod_words; ++i)
      b_is_1 &= CT::Mask<word>::is_zero(b_w[i]);

   BOOST_ASSERT_MSG(a_is_0.is_set(), "A is zero");

   // if b != 1 then gcd(n,mod) > 1 and inverse does not exist
   // in which case zero out the result to indicate this
   (~b_is_1).if_set_zero_out(v_w, mod_words);

   /*
   * We've placed the result in the lowest words of the temp buffer.
   * So just clear out the other values and then give that buffer to a
   * BigInt.
   */
   clear_mem(&tmp_mem[mod_words], 4 * mod_words);

   CT::unpoison(tmp_mem.data(), tmp_mem.size());

   BigInt r;
   r.swap_reg(tmp_mem);
   return r;
}
} // namespace backends

/*
Sets result to a^-1 * 2^k mod a
with n <= k <= 2n
Returns k

"The Montgomery Modular Inverse - Revisited" Çetin Koç, E. Savas
https://citeseerx.ist.psu.edu/viewdoc/citations?doi=10.1.1.75.8377

A const time implementation of this algorithm is described in
"Constant Time Modular Inversion" Joppe W. Bos
http://www.joppebos.com/files/CTInversion.pdf
*/
template <typename Backend, expression_template_option ExpressionTemplates>
std::size_t almost_montgomery_inverse(number<Backend, ExpressionTemplates>&       result,
                                      const number<Backend, ExpressionTemplates>& a,
                                      const number<Backend, ExpressionTemplates>& p)
{
   size_t k = 0;

   BigInt u = p, v = a, r = 0, s = 1;

   while (v > 0)
   {
      if (u.is_even())
      {
         u >>= 1;
         s <<= 1;
      }
      else if (v.is_even())
      {
         v >>= 1;
         r <<= 1;
      }
      else if (u > v)
      {
         u -= v;
         u >>= 1;
         r += s;
         s <<= 1;
      }
      else
      {
         v -= u;
         v >>= 1;
         s += r;
         r <<= 1;
      }

      ++k;
   }

   if (r >= p)
   {
      r -= p;
   }

   result = p - r;

   return k;
}

template <typename Backend, expression_template_option ExpressionTemplates>
number<Backend, ExpressionTemplates> normalized_montgomery_inverse(const number<Backend, ExpressionTemplates>& a, const number<Backend, ExpressionTemplates>& p)
{
   number<Backend, ExpressionTemplates> r;
   size_t k = almost_montgomery_inverse(r, a, p);

   for (size_t i = 0; i != k; ++i)
   {
      if (r.is_odd())
         r += p;
      r >>= 1;
   }

   return r;
}

template <typename Backend, expression_template_option ExpressionTemplates>
number<Backend, ExpressionTemplates> inverse_mod_pow2(const number<Backend, ExpressionTemplates>& a1, size_t k)
{
   typedef typename mpl::front<typename Backend::unsigned_types>::type ui_type;
   /*
   * From "A New Algorithm for Inversion mod p^k" by Çetin Kaya Koç
   * https://eprint.iacr.org/2017/411.pdf sections 5 and 7.
   */

   if (a1.is_even())
      return 0;

   number<Backend, ExpressionTemplates> a = a1;
   a.mask_bits(k);

   number<Backend, ExpressionTemplates> b = 1;
   number<Backend, ExpressionTemplates> X = 0;
   number<Backend, ExpressionTemplates> newb;

   const size_t a_words = a.sig_words();

   X.grow_to(round_up(k, sizeof(ui_type) * CHAR_BIT) / sizeof(ui_type) * CHAR_BIT);
   b.grow_to(a_words);

   /*
   Hide the exact value of k. k is anyway known to word length
   granularity because of the length of a, so no point in doing more
   than this.
   */
   const size_t iter = round_up(k, sizeof(ui_type) * CHAR_BIT);

   for (size_t i = 0; i != iter; ++i)
   {
      const bool b0 = b.get_bit(0);
      X.conditionally_set_bit(i, b0);
      newb = b - a;
      b.ct_cond_assign(b0, newb);
      b >>= 1;
   }

   X.mask_bits(k);
   X.const_time_unpoison();
   return X;
}

template <typename Backend, expression_template_option ExpressionTemplates>
number<Backend, ExpressionTemplates> inverse_mod(const number<Backend, ExpressionTemplates>& n, const number<Backend, ExpressionTemplates>& mod)
{
   if (mod.is_zero())
      throw BigInt::DivideByZero();
   if (mod.is_negative() || n.is_negative())
      throw Invalid_Argument("inverse_mod: arguments must be non-negative");
   if (n.is_zero() || (n.is_even() && mod.is_even()))
      return 0;

   if (mod.is_odd())
   {
      /*
      Fastpath for common case. This leaks information if n > mod
      but we don't guarantee const time behavior in that case.
      */
      if (n < mod)
         return inverse_mod_odd_modulus(n, mod);
      else
         return inverse_mod_odd_modulus(ct_modulo(n, mod), mod);
   }

   const size_t mod_lz = low_zero_bits(mod);
   BOOST_ASSERT(mod_lz > 0);
   const size_t mod_bits = mod.bits();
   BOOST_ASSERT(mod_bits > mod_lz);

   if (mod_lz == mod_bits - 1)
   {
      // In this case we are performing an inversion modulo 2^k
      return inverse_mod_pow2(n, mod_lz);
   }

   /*
   * In this case we are performing an inversion modulo 2^k*o for
   * some k > 1 and some odd (not necessarily prime) integer.
   * Compute the inversions modulo 2^k and modulo o, then combine them
   * using CRT, which is possible because 2^k and o are relatively prime.
   */

   const number<Backend, ExpressionTemplates> o      = mod >> mod_lz;
   const number<Backend, ExpressionTemplates> n_redc = ct_modulo(n, o);
   const number<Backend, ExpressionTemplates> inv_o  = inverse_mod_odd_modulus(n_redc, o);
   const number<Backend, ExpressionTemplates> inv_2k = inverse_mod_pow2(n, mod_lz);

   // No modular inverse in this case:
   if (inv_o == 0 || inv_2k == 0)
      return 0;

   const number<Backend, ExpressionTemplates> m2k = BigInt::power_of_2(mod_lz);
   // Compute the CRT parameter
   const number<Backend, ExpressionTemplates> c = inverse_mod_pow2(o, mod_lz);

   // Compute h = c*(inv_2k-inv_o) mod 2^k
   number<Backend, ExpressionTemplates>     h     = c * (inv_2k - inv_o);
   const bool h_neg = h.is_negative();
   h.set_sign(BigInt::Positive);
   h.mask_bits(mod_lz);
   const bool h_nonzero = h.is_nonzero();
   h.ct_cond_assign(h_nonzero && h_neg, m2k - h);

   // Return result inv_o + h * o
   h *= o;
   h += inv_o;
   return h;
}

template <typename Backend, expression_template_option ExpressionTemplates>
number<Backend, ExpressionTemplates> monty_inverse(const number<Backend, ExpressionTemplates>& a)
{
   if (a % 2 == 0)
   {
      throw std::invalid_argument("Monty_inverse only valid for odd integers");
   }

   /*
    * From "A New Algorithm for Inversion mod p^k" by Çetin Kaya Koç
    * https://eprint.iacr.org/2017/411.pdf sections 5 and 7.
    */

   limb_type b = 1;
   limb_type r = 0;

   for (size_t i = 0; i != sizeof(limb_type) * CHAR_BIT; ++i)
   {
      const limb_type bi = b % 2;
      r >>= 1;
      r += bi << (sizeof(limb_type) * CHAR_BIT - 1);

      b -= a * bi;
      b >>= 1;
   }

   // Now invert in addition space
   r = (~static_cast<limb_type>(0) - r) + 1;

   return r;
}
}
} // namespace boost::multiprecision

#endif
