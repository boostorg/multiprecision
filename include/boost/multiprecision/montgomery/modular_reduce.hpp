//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MONTGOMERY_INT_MOD_REDC_HPP
#define BOOST_MULTIPRECISION_MONTGOMERY_INT_MOD_REDC_HPP

#include <boost/multiprecision/montgomery/mask_bits.hpp>
#include <boost/multiprecision/montgomery/reduce_below.hpp>

namespace boost {
namespace multiprecision {
namespace backends {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127) // conditional expression is constant
#endif

template <typename Backend>
inline void mod_redc( number<Backend>& result, const  number<Backend>& mod)
{

   using default_ops::eval_add;
   using default_ops::eval_bit_set;
   using default_ops::eval_gt;
   using default_ops::eval_lt;
   using default_ops::eval_multiply;

   const size_t x_sw = result.backend().size();

   number<Backend> mod2 = mod;
   mod2 += mod;
   if (result.backend().size() < mod.backend().size() || (result < mod))
   {
      if (result < 0)
      {
         result += mod;
         return;
      } // make positive
      return;
   }
   else if (result < mod2)
   {
      number<Backend> t1 = result;


      eval_right_shift(t1.backend(), (Backend::limb_bits * (mod.backend().size() - 1)));
      //import_bits(t1, result.backend().limbs() + mod.backend().size() - 1, result.backend().limbs() + x_sw);
      {
         // Compute mu = floor(2^{2k} / m)
         number<Backend> p2;
         eval_bit_set(p2.backend(), 2 * Backend::limb_bits * mod.backend().size());
         p2 /= mod;
         t1 *= p2;
      }
      eval_right_shift(t1.backend(), (Backend::limb_bits * (mod.backend().size() + 1)));

      t1 *= mod;

      eval_mask_bits(t1.backend(), Backend::limb_bits * (mod.backend().size() + 1));

      t1 = abs(t1 - result);

      if (t1 < 0)
      {
         number<Backend> p2;
         eval_bit_set(p2.backend(), Backend::limb_bits * (mod.backend().size() + 1));
         t1 += p2;
      }

      eval_reduce_below(t1, mod);

      if (result < 0)
      {
         t1 -= mod;
      }

      result = t1;
      return;
   }
   else
   {
      // too big, fall back to normal division
      //divide_unsigned_helper(&result, result, mod, t2);
      result %= mod;
      return;
   }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

}
}
} // namespace boost::multiprecision::backends

#endif
