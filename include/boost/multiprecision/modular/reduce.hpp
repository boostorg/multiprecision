//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_MONTGOMERY_INT_REDC_HPP
#define BOOST_MULTIPRECISION_MONTGOMERY_INT_REDC_HPP

#include <boost/container/vector.hpp>
#include <boost/multiprecision/cpp_int/montgomery_params.hpp>

namespace boost {
namespace multiprecision {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127) // conditional expression is constant
#endif


template <typename Backend>
inline void eval_redc(Backend &result, const montgomery_params<Backend> &mod)
{
   using default_ops::eval_lt;
   using default_ops::eval_multiply_add;

   typedef cpp_int_backend<Backend::limb_bits * 3, Backend::limb_bits * 3, unsigned_magnitude, unchecked, void> cpp_three_int_backend;
   typedef typename Backend::allocator_type alloc;

   const size_t    p_size = mod.p_words();
   const limb_type p_dash = mod.p_dash();
   const size_t    z_size = 2 * (mod.p_words() + 1);

   container::vector<limb_type, alloc> z(result.size(), 0);
   for (size_t i = 0; i < result.size(); ++i)
   {
      z[i] = result.limbs()[i];
   }

   if (result.size() < z_size)
   {
      result.resize(z_size, z_size);
      z.resize(z_size, 0);
   }

   cpp_three_int_backend w(z[0]);

   result.limbs()[0] = w.limbs()[0] * p_dash;

   eval_multiply_add(w, result.limbs()[0], mod.p().backend().limbs()[0]);
   eval_right_shift(w, Backend::limb_bits);

   for (size_t i = 1; i != p_size; ++i)
   {
      for (size_t j = 0; j < i; ++j)
      {
         eval_multiply_add(w, result.limbs()[j], mod.p().backend().limbs()[i - j]);
      }

      eval_add(w, z[i]);

      result.limbs()[i] = w.limbs()[0] * p_dash;

      eval_multiply_add(w, result.limbs()[i], mod.p().backend().limbs()[0]);

      eval_right_shift(w, Backend::limb_bits);
   }

   for (size_t i = 0; i != p_size; ++i)
   {
      for (size_t j = i + 1; j != p_size; ++j)
      {
         eval_multiply_add(w, result.limbs()[j], mod.p().backend().limbs()[p_size + i - j]);
      }

      eval_add(w, z[p_size + i]);

      result.limbs()[i] = w.limbs()[0];

      eval_right_shift(w, Backend::limb_bits);
   }

   eval_add(w, z[z_size - 1]);

   result.limbs()[p_size] = w.limbs()[0];
   result.limbs()[p_size + 1] = w.limbs()[1];

   if (result.size() != p_size + 1)
   {
      result.resize(p_size + 1, p_size + 1);
   }
   result.normalize();
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

}
} // namespace boost::multiprecision

#endif