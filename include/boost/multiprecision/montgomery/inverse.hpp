//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
// Copyright (c) 2018-2019 Alexey Moskvin
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef BOOST_MULTIPRECISION_INVERSE_HPP
#define BOOST_MULTIPRECISION_INVERSE_HPP

#include <boost/integer.hpp>
#include <boost/multiprecision/cpp_int/cpp_int_config.hpp>

#if defined(BOOST_HAS_INT128)
#define CRYPTO3_MP_WORD_BITS 64
#else
#define CRYPTO3_MP_WORD_BITS 32
#endif

namespace boost {
namespace multiprecision {

limb_type monty_inverse(limb_type a)
{
   const limb_type MP_WORD_MAX = ~static_cast<limb_type>(0);

   if (a % 2 == 0)
   {
      throw std::invalid_argument("monty_inverse only valid for odd integers");
   }

   /*
            * From "A New Algorithm for Inversion mod p^k" by Çetin Kaya Koç
            * https://eprint.iacr.org/2017/411.pdf sections 5 and 7.
            */

   limb_type b = 1;
   limb_type r = 0;

   for (size_t i = 0; i != CRYPTO3_MP_WORD_BITS; ++i)
   {
      const limb_type bi = b % 2;
      r >>= 1;
      r += bi << (CRYPTO3_MP_WORD_BITS - 1);

      b -= a * bi;
      b >>= 1;
   }

   // Now invert in addition space
   r = (MP_WORD_MAX - r) + 1;

   return r;
}
}
} // namespace boost::multiprecision
#endif //BOOST_MULTIPRECISION_INVERSE_H