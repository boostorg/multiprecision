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
inline void eval_mod_redc(Backend& result, const  Backend& mod)
{

   using default_ops::eval_add;
   using default_ops::eval_bit_set;
   using default_ops::eval_lt;
   using default_ops::eval_multiply;

   Backend mod2 = mod;
   eval_add(mod2, mod);

   if (result.size() < mod.size() || eval_lt(result, mod))
   {
      if (eval_lt(result, 0))
      {
         eval_add(result, mod);
         return;
      }
      return;
   }
   else if (eval_lt(result, mod2))
   {
      Backend t1(result);

      eval_right_shift(t1, (Backend::limb_bits * (mod.size() - 1)));
      {
         Backend p2;
         eval_bit_set(p2, 2 * Backend::limb_bits * mod.size());
         eval_divide(p2, mod);
         eval_multiply(t1, p2);
      }
      eval_right_shift(t1, (Backend::limb_bits * (mod.size() + 1)));

      eval_multiply(t1, mod);

      eval_mask_bits(t1, Backend::limb_bits * (mod.size() + 1));

      eval_subtract(t1, result, t1);

      if (eval_lt(t1, 0))
      {
         Backend p2;
         eval_bit_set(p2, Backend::limb_bits * (mod.size() + 1));
         eval_add(t1, p2);
      }

      eval_reduce_below(t1, mod);

      if (eval_lt(result, 0))
      {
         eval_subtract(t1, mod, t1);
      }

      result = t1;
      return;
   }
   else
   {
      // too big, fall back to normal division
      Backend t2;
      divide_unsigned_helper(&result, result, mod, t2);
      result = t2;
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
